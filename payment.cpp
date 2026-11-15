#include <algorithm>
#include <cctype>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <limits>
#include <queue>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>
using namespace std;

const int SLOTS = 5;
const double BIKE_RATE = 20, CAR_RATE = 50, OVERTIME_MULT = 1.5;

struct Ticket { string type; int slot, hours; time_t entry; bool active; };
struct Txn { int id; string vehicle, type, mode, status; double amount; };

priority_queue<int, vector<int>, greater<int>> bikeSlots, carSlots;  // min-heap: smallest free slot first
unordered_map<string, Ticket> tickets;       // vehicle -> ticket
unordered_map<string, vector<int>> history;  // vehicle -> transaction indexes
vector<Txn> txns;

// ---------- input helpers ----------
int readInt(const string &p) {
    int x;
    cout << p;
    while (!(cin >> x)) {
        if (cin.eof()) exit(0);
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid! Enter a number: ";
    }
    return x;
}

string readWord(const string &p, bool upper) {
    string s;
    cout << p;
    if (!(cin >> s)) exit(0);
    for (char &c : s) c = upper ? toupper(c) : tolower(c);
    return s;
}

double rateOf(const string &type) { return type == "bike" ? BIKE_RATE : CAR_RATE; }

// ---------- billing ----------
void showTxn(const Txn &t) {
    cout << "\nTxn " << t.id << " | " << t.vehicle << " | " << t.type
         << " | Rs. " << t.amount << " | " << (t.mode.empty() ? "-" : t.mode)
         << " | " << t.status << "\n";
}

// UPI ID sahi format mein hai ya nahi (jaise name@upi)
bool validUPI(const string &id) {
    size_t at = id.find('@');
    if (at == string::npos || at < 2) return false;           // @ se pehle naam
    if (id.find('@', at + 1) != string::npos) return false;   // @ sirf ek baar
    if (id.size() - at - 1 < 2) return false;                 // @ ke baad bank
    for (char c : id)
        if (!isalnum((unsigned char)c) && c != '@' && c != '.' && c != '_' && c != '-') return false;
    return true;
}

void pay(int i) {
    int c = readInt("Payment mode (1.UPI 2.Card): ");
    if (c < 1 || c > 2) { cout << "Invalid mode. Kept Pending.\n"; return; }

    if (c == 1) {
        txns[i].mode = "UPI";
        string upi = readWord("Enter your UPI ID (e.g. name@upi): ", false);
        if (!validUPI(upi)) {
            txns[i].status = "Failed";
            cout << "Invalid UPI ID!\nPayment Status: Failed\n";
            return;
        }
        cout << "Payment request of Rs. " << txns[i].amount << " sent to " << upi << "\n";
        cout << "Processing payment...\n";
        cout.flush();
        this_thread::sleep_for(chrono::seconds(1));
        txns[i].status = "Successful";   // valid UPI ID dene pe payment ho gayi
    } else {
        txns[i].mode = "Card";
        cout << "Card payment (simulation)\nProcessing payment...\n";
        cout.flush();
        this_thread::sleep_for(chrono::seconds(1));
        txns[i].status = (rand() % 10 < 8) ? "Successful" : "Failed";  // 80% success
    }
    cout << "Payment Status: " << txns[i].status << "\n";
}

void newTxn(const string &v, const string &type, double amount) {
    txns.push_back({(int)txns.size() + 101, v, type, "", "Pending", amount});
    int i = txns.size() - 1;
    history[v].push_back(i);
    showTxn(txns[i]);
    pay(i);
}

// ---------- features ----------
void entry() {
    string v = readWord("\nVehicle No (no spaces): ", true);
    if (tickets.count(v) && tickets[v].active) { cout << "Already parked!\n"; return; }

    string type = readWord("Type (bike/car): ", false);
    if (type != "bike" && type != "car") { cout << "Invalid type.\n"; return; }

    int hours = readInt("Booked hours: ");
    if (hours <= 0) { cout << "Hours must be > 0.\n"; return; }

    auto &q = (type == "bike") ? bikeSlots : carSlots;
    if (q.empty()) { cout << "No slot available!\n"; return; }
    int slot = q.top();
    q.pop();

    tickets[v] = {type, slot, hours, time(nullptr), true};
    cout << "\n--- TICKET --- " << v << " | " << type << " | Slot " << slot
         << " | " << hours << " hr x Rs. " << rateOf(type) << "\n";
    newTxn(v, "Parking", hours * rateOf(type));
}

void exitVehicle() {
    string v = readWord("\nVehicle No: ", true);
    if (!tickets.count(v) || !tickets[v].active) { cout << "Vehicle not found!\n"; return; }
    Ticket &t = tickets[v];

    int mins = readInt("Actual parked minutes (-1 = use real time): ");
    if (mins < 0) mins = (int)ceil(difftime(time(nullptr), t.entry) / 60.0);

    int extra = max(0, mins - t.hours * 60);
    ((t.type == "bike") ? bikeSlots : carSlots).push(t.slot);  // free the slot
    t.active = false;

    cout << "Parked " << mins << " min, booked " << t.hours * 60 << " min.\n";
    if (extra > 0) {
        int extraHours = (extra + 59) / 60;  // round up
        cout << "Overtime: " << extra << " min -> " << extraHours << " hr\n";
        newTxn(v, "Overtime", extraHours * rateOf(t.type) * OVERTIME_MULT);
    } else {
        cout << "No overtime.\n";
    }
    cout << "Slot released.\n";
}

void viewSlots() {
    cout << "\nFree bike slots: " << bikeSlots.size() << "/" << SLOTS
         << " | Free car slots: " << carSlots.size() << "/" << SLOTS << "\n";
    cout << "Parked vehicles:\n";
    for (auto &p : tickets)
        if (p.second.active)
            cout << "  " << p.first << " | " << p.second.type << " | Slot " << p.second.slot << "\n";
}

void pendingDues() {
    bool found = false;
    for (size_t i = 0; i < txns.size(); i++) {
        if (txns[i].status == "Pending" || txns[i].status == "Failed") {
            found = true;
            showTxn(txns[i]);
            string s = readWord("Retry payment? (y/n): ", false);
            if (s[0] == 'y') pay(i);
        }
    }
    if (!found) cout << "No pending dues.\n";
}

void searchVehicle() {
    string v = readWord("\nVehicle No: ", true);
    if (!tickets.count(v)) { cout << "Not found.\n"; return; }
    Ticket &t = tickets[v];
    cout << v << " | " << t.type << " | Slot " << t.slot << " | "
         << (t.active ? "Parked" : "Exited") << "\n";
    for (int i : history[v]) showTxn(txns[i]);
}

void allHistory() {
    double total = 0;
    for (auto &t : txns) {
        showTxn(t);
        if (t.status == "Successful") total += t.amount;
    }
    cout << "\nTotal Collection: Rs. " << total << "\n";
}

int main() {
    srand((unsigned)time(nullptr));
    cout << fixed << setprecision(2);
    for (int i = 1; i <= SLOTS; i++) { bikeSlots.push(i); carSlots.push(i); }

    int ch;
    do {
        cout << "\n===== PAYMENT SYSTEM =====\n"
             << "1.Entry  2.Exit  3.Slots  4.Pending Dues  5.Search  6.History  7.Quit\n";
        ch = readInt("Choice: ");
        switch (ch) {
            case 1: entry(); break;
            case 2: exitVehicle(); break;
            case 3: viewSlots(); break;
            case 4: pendingDues(); break;
            case 5: searchVehicle(); break;
            case 6: allHistory(); break;
            case 7: cout << "Thank you!\n"; break;
            default: cout << "Invalid choice!\n";
        }
    } while (ch != 7);
}
