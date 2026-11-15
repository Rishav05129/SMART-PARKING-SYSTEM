#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <winsock2.h>
#include <ws2tcpip.h>
typedef SOCKET sock_t;
#define CLOSESOCK closesocket
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
typedef int sock_t;
#define CLOSESOCK close
#endif
#include <algorithm>
#include <cctype>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
using namespace std;
static const string LOC_FILE = "locations.txt";
static const string VEH_FILE = "vehicles.txt";
static const string TXN_FILE = "txns.txt";
static const string EXP_FILE = "expenses.txt";
static const string ADM_FILE = "admin.txt";
struct Location {
    int id;
    string name, address;
    int total, available;
};
struct Record {
    int locId, slot;
    string name, mobile, vehicleNo, type;
    int estHours;
    double actHours, normalFee, overtimeFee;
    bool exited;
    time_t entryTime, exitTime;
    int month, year;
};
struct Txn {
    int id;
    string vehicleNo, kind, mode, status;
    double amount;
    int month, year;
};
struct Expense {
    int month, year;
    double amount;
    string note;
};
static vector<Location> locs;
static vector<Record> recs;
static vector<Txn> txns;
static vector<Expense> exps;
static string adminUser, adminPass;
static double normalRate = 50;
static double overtimeRate = 75;
string toUpper(string s) { for (char &c : s) c = (char)toupper((unsigned char)c); return s; }
string toLower(string s) { for (char &c : s) c = (char)tolower((unsigned char)c); return s; }
string trim(const string &s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == string::npos) return "";
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}
vector<string> split(const string &s, char d) {
    vector<string> r;
    string cur;
    for (char c : s) {
        if (c == d) { r.push_back(cur); cur.clear(); }
        else cur += c;
    }
    r.push_back(cur);
    return r;
}
string noPipe(string s) { for (char &c : s) if (c == '|') c = ' '; return s; }
int readInt(const string &prompt) {
    string s;
    while (true) {
        cout << prompt;
        if (!getline(cin, s)) exit(0);
        s = trim(s);
        try {
            size_t pos;
            int v = stoi(s, &pos);
            if (pos == s.size()) return v;
        } catch (...) {}
        cout << "Invalid number! Try again.\n";
    }
}
double readDouble(const string &prompt) {
    string s;
    while (true) {
        cout << prompt;
        if (!getline(cin, s)) exit(0);
        s = trim(s);
        try {
            size_t pos;
            double v = stod(s, &pos);
            if (pos == s.size()) return v;
        } catch (...) {}
        cout << "Invalid number! Try again.\n";
    }
}
string readLine(const string &prompt) {
    string s;
    while (true) {
        cout << prompt;
        if (!getline(cin, s)) exit(0);
        s = noPipe(trim(s));
        if (!s.empty()) return s;
        cout << "Empty input not allowed.\n";
    }
}
string readWord(const string &prompt) {
    string s;
    while (true) {
        cout << prompt;
        if (!getline(cin, s)) exit(0);
        s = trim(s);
        if (!s.empty() && s.find_first_of(" \t|") == string::npos) return s;
        cout << "Enter a single word without spaces.\n";
    }
}
bool validMobile(const string &m) {
    if (m.size() != 10) return false;
    for (char c : m) if (!isdigit((unsigned char)c)) return false;
    return true;
}
bool validUPI(const string &id) {
    size_t at = id.find('@');
    if (at == string::npos || at < 2) return false;
    if (id.find('@', at + 1) != string::npos) return false;
    if (id.size() - at - 1 < 2) return false;
    for (char c : id)
        if (!isalnum((unsigned char)c) && c != '@' && c != '.' && c != '_' && c != '-') return false;
    return true;
}
void today(int &m, int &y) {
    time_t t = time(nullptr);
    tm *lt = localtime(&t);
    m = lt->tm_mon + 1;
    y = lt->tm_year + 1900;
}
void saveSettings() {
    ofstream f(ADM_FILE);
    f << adminUser << '\n' << adminPass << '\n' << normalRate << '\n' << overtimeRate << '\n';
}
void saveLocations() {
    ofstream f(LOC_FILE);
    for (auto &l : locs)
        f << l.id << '|' << l.name << '|' << l.address << '|' << l.total << '|' << l.available << '\n';
}
void saveRecords() {
    ofstream f(VEH_FILE);
    f << fixed << setprecision(2);
    for (auto &r : recs)
        f << r.locId << '|' << r.slot << '|' << r.name << '|' << r.mobile << '|' << r.vehicleNo << '|'
          << r.type << '|' << r.estHours << '|' << r.actHours << '|' << r.normalFee << '|'
          << r.overtimeFee << '|' << (r.exited ? 1 : 0) << '|' << (long long)r.entryTime << '|'
          << (long long)r.exitTime << '|' << r.month << '|' << r.year << '\n';
}
void saveTxns() {
    ofstream f(TXN_FILE);
    f << fixed << setprecision(2);
    for (auto &t : txns)
        f << t.id << '|' << t.vehicleNo << '|' << t.kind << '|' << t.amount << '|' << t.mode << '|'
          << t.status << '|' << t.month << '|' << t.year << '\n';
}
void saveExpenses() {
    ofstream f(EXP_FILE);
    f << fixed << setprecision(2);
    for (auto &e : exps)
        f << e.month << '|' << e.year << '|' << e.amount << '|' << e.note << '\n';
}
void loadLocations() {
    locs.clear();
    ifstream f(LOC_FILE);
    string line;
    while (getline(f, line)) {
        line = trim(line);
        if (line.empty()) continue;
        auto p = split(line, '|');
        if (p.size() < 5) continue;
        try {
            Location l{stoi(p[0]), p[1], p[2], stoi(p[3]), stoi(p[4])};
            if (l.total <= 0 || l.available < 0 || l.available > l.total) continue;
            locs.push_back(l);
        } catch (...) {}
    }
}
void loadRecords() {
    recs.clear();
    ifstream f(VEH_FILE);
    string line;
    while (getline(f, line)) {
        line = trim(line);
        if (line.empty()) continue;
        auto p = split(line, '|');
        if (p.size() < 15) continue;
        try {
            Record r;
            r.locId = stoi(p[0]);
            r.slot = stoi(p[1]);
            r.name = p[2];
            r.mobile = p[3];
            r.vehicleNo = p[4];
            r.type = p[5];
            r.estHours = stoi(p[6]);
            r.actHours = stod(p[7]);
            r.normalFee = stod(p[8]);
            r.overtimeFee = stod(p[9]);
            r.exited = stoi(p[10]) != 0;
            r.entryTime = (time_t)stoll(p[11]);
            r.exitTime = (time_t)stoll(p[12]);
            r.month = stoi(p[13]);
            r.year = stoi(p[14]);
            recs.push_back(r);
        } catch (...) {}
    }
}
void loadTxns() {
    txns.clear();
    ifstream f(TXN_FILE);
    string line;
    while (getline(f, line)) {
        line = trim(line);
        if (line.empty()) continue;
        auto p = split(line, '|');
        if (p.size() < 8) continue;
        try {
            Txn t;
            t.id = stoi(p[0]);
            t.vehicleNo = p[1];
            t.kind = p[2];
            t.amount = stod(p[3]);
            t.mode = p[4];
            t.status = p[5];
            t.month = stoi(p[6]);
            t.year = stoi(p[7]);
            txns.push_back(t);
        } catch (...) {}
    }
}
void loadExpenses() {
    exps.clear();
    ifstream f(EXP_FILE);
    string line;
    while (getline(f, line)) {
        line = trim(line);
        if (line.empty()) continue;
        auto p = split(line, '|');
        if (p.size() < 3) continue;
        try {
            Expense e;
            e.month = stoi(p[0]);
            e.year = stoi(p[1]);
            e.amount = stod(p[2]);
            e.note = p.size() > 3 ? p[3] : "";
            exps.push_back(e);
        } catch (...) {}
    }
}
void loadSettings() {
    ifstream f(ADM_FILE);
    string u, p, n, o;
    if (f && getline(f, u) && getline(f, p) && !trim(u).empty() && !trim(p).empty()) {
        adminUser = trim(u);
        adminPass = trim(p);
        if (getline(f, n)) { try { normalRate = stod(n); } catch (...) {} }
        if (getline(f, o)) { try { overtimeRate = stod(o); } catch (...) {} }
        return;
    }
    cout << "ADMIN REGISTRATION\n";
    adminUser = readWord("Create Username: ");
    adminPass = readWord("Create Password: ");
    saveSettings();
    cout << "Admin Account Created Successfully!\n";
}
Location *findLocation(int id) {
    for (auto &l : locs)
        if (l.id == id) return &l;
    return nullptr;
}
string locName(int id) {
    Location *l = findLocation(id);
    return l ? l->name : "(Deleted Location)";
}
int nextLocId() {
    int mx = 0;
    for (auto &l : locs) mx = max(mx, l.id);
    return mx + 1;
}
int activeCountAt(int locId) {
    int c = 0;
    for (auto &r : recs)
        if (!r.exited && r.locId == locId) c++;
    return c;
}
int maxSlotUsed(int locId) {
    int m = 0;
    for (auto &r : recs)
        if (!r.exited && r.locId == locId) m = max(m, r.slot);
    return m;
}
int findFreeSlot(const Location &l) {
    vector<bool> used(l.total + 2, false);
    for (auto &r : recs)
        if (!r.exited && r.locId == l.id && r.slot >= 1 && r.slot <= l.total)
            used[r.slot] = true;
    for (int i = 1; i <= l.total; i++)
        if (!used[i]) return i;
    return -1;
}
int findActive(const string &no) {
    for (size_t i = 0; i < recs.size(); i++)
        if (!recs[i].exited && recs[i].vehicleNo == no) return (int)i;
    return -1;
}
void viewLocations() {
    cout << "PARKING LOCATIONS\n";
    if (locs.empty()) {
        cout << "No locations found.\n";
        return;
    }
    for (auto &l : locs)
        cout << "Location " << l.id << ": " << l.name
             << "\nAddress: " << l.address
             << "\nTotal Slots: " << l.total
             << "\nAvailable Slots: " << l.available << "\n";
}
void addLocation() {
    Location l;
    l.id = nextLocId();
    l.name = readLine("Location Name: ");
    l.address = readLine("Address: ");
    do {
        l.total = readInt("Total Slots: ");
        if (l.total <= 0) cout << "Slots must be greater than 0.\n";
    } while (l.total <= 0);
    l.available = l.total;
    locs.push_back(l);
    saveLocations();
    cout << "Location added (ID " << l.id << ").\n";
}
void editLocation() {
    viewLocations();
    if (locs.empty()) return;
    Location *l = findLocation(readInt("Location ID to edit: "));
    if (!l) { cout << "Location not found.\n"; return; }
    l->name = readLine("New Name: ");
    l->address = readLine("New Address: ");
    int occupied = max(l->total - l->available, activeCountAt(l->id));
    int minTotal = max(occupied, maxSlotUsed(l->id));
    int t;
    do {
        t = readInt("New Total Slots: ");
        if (t < minTotal)
            cout << "Total cannot be less than " << minTotal << " (vehicles currently parked).\n";
        else if (t <= 0)
            cout << "Slots must be greater than 0.\n";
    } while (t < minTotal || t <= 0);
    l->total = t;
    l->available = t - occupied;
    saveLocations();
    cout << "Location updated.\n";
}
void deleteLocation() {
    viewLocations();
    if (locs.empty()) return;
    int id = readInt("Location ID to delete: ");
    for (size_t i = 0; i < locs.size(); i++) {
        if (locs[i].id == id) {
            if (locs[i].available != locs[i].total || activeCountAt(id) > 0) {
                cout << "Vehicles are parked at this location, cannot delete.\n";
                return;
            }
            locs.erase(locs.begin() + i);
            saveLocations();
            cout << "Location deleted.\n";
            return;
        }
    }
    cout << "Location not found.\n";
}
void manageLocations() {
    int ch;
    do {
        cout << "MANAGE PARKING LOCATIONS\n"
             << "1. View Locations\n2. Add Location\n3. Edit Location\n4. Delete Location\n0. Back\n";
        ch = readInt("Enter Your Choice: ");
        switch (ch) {
            case 1: viewLocations(); break;
            case 2: addLocation(); break;
            case 3: editLocation(); break;
            case 4: deleteLocation(); break;
            case 0: break;
            default: cout << "Invalid choice!\n";
        }
    } while (ch != 0);
}
void showTxn(const Txn &t) {
    cout << "Txn " << t.id << " | " << t.vehicleNo << " | " << t.kind
         << " | Rs. " << t.amount << " | " << (t.mode.empty() ? "-" : t.mode)
         << " | " << t.status << " | " << t.month << "/" << t.year << "\n";
}
void pay(size_t i) {
    cout << "Amount due: Rs. " << txns[i].amount << "\n";
    int c = readInt("Payment mode (1.UPI  2.Card  0.Pay Later): ");
    if (c == 1) {
        txns[i].mode = "UPI";
        string upi = toLower(readWord("Enter your UPI ID (e.g. name@upi): "));
        if (!validUPI(upi)) {
            txns[i].status = "Failed";
            cout << "Invalid UPI ID!\n";
        } else {
            cout << "Payment request of Rs. " << txns[i].amount << " sent to " << upi
                 << "\nProcessing payment...\n";
            cout.flush();
            this_thread::sleep_for(chrono::seconds(1));
            txns[i].status = "Successful";
        }
    } else if (c == 2) {
        txns[i].mode = "Card";
        cout << "Card payment (simulation)\nProcessing payment...\n";
        cout.flush();
        this_thread::sleep_for(chrono::seconds(1));
        txns[i].status = (rand() % 10 < 8) ? "Successful" : "Failed";
    } else {
        cout << (c == 0 ? "Payment kept Pending.\n" : "Invalid mode. Payment kept Pending.\n");
        return;
    }
    saveTxns();
    cout << "Payment Status: " << txns[i].status << "\n";
}
void newTxn(const string &vehicle, const string &kind, double amount) {
    int mx = 100;
    for (auto &t : txns) mx = max(mx, t.id);
    Txn t;
    t.id = mx + 1;
    t.vehicleNo = vehicle;
    t.kind = kind;
    t.mode = "";
    t.status = "Pending";
    t.amount = amount;
    today(t.month, t.year);
    txns.push_back(t);
    saveTxns();
    showTxn(txns.back());
    pay(txns.size() - 1);
}
void pendingDues() {
    string v = toUpper(readWord("Vehicle Number: "));
    bool found = false;
    for (size_t i = 0; i < txns.size(); i++) {
        if (txns[i].vehicleNo == v && (txns[i].status == "Pending" || txns[i].status == "Failed")) {
            found = true;
            showTxn(txns[i]);
            string s = toLower(readWord("Pay now? (y/n): "));
            if (s[0] == 'y') pay(i);
        }
    }
    if (!found) cout << "No pending dues for this vehicle.\n";
}
void allTransactions() {
    if (txns.empty()) { cout << "No transactions found.\n"; return; }
    double total = 0, pending = 0;
    cout << "ALL TRANSACTIONS\n";
    for (auto &t : txns) {
        showTxn(t);
        if (t.status == "Successful") total += t.amount;
        else pending += t.amount;
    }
    cout << "Total Collected : Rs. " << total << "\nPending/Failed  : Rs. " << pending << "\n";
}
void printRecord(const Record &r) {
    cout << "Customer Name  : " << r.name
         << "\nMobile Number  : " << r.mobile
         << "\nVehicle Number : " << r.vehicleNo
         << "\nVehicle Type   : " << r.type
         << "\nLocation       : " << locName(r.locId)
         << "\nParking Slot   : " << r.slot
         << "\nBooked Hours   : " << r.estHours << "\n";
    if (r.exited) {
        cout << "Actual Hours   : " << r.actHours
             << "\nNormal Fee     : Rs. " << r.normalFee
             << "\nOvertime Fee   : Rs. " << r.overtimeFee
             << "\nTotal Fees     : Rs. " << r.normalFee + r.overtimeFee
             << "\nExit Month/Year: " << r.month << "/" << r.year
             << "\nStatus         : Exited\n";
    } else {
        cout << "Booking Fee    : Rs. " << r.normalFee << "\nStatus         : Currently Parked\n";
    }
}
void vehicleEntry() {
    cout << "VEHICLE ENTRY\n";
    if (locs.empty()) { cout << "No parking locations available.\n"; return; }
    viewLocations();
    Location *l = findLocation(readInt("Enter Location ID: "));
    if (!l) { cout << "Location not found.\n"; return; }
    if (l->available <= 0) { cout << "Parking is full at this location!\n"; return; }
    string no = toUpper(readWord("Vehicle Number (no spaces): "));
    if (findActive(no) >= 0) { cout << "This vehicle is already parked!\n"; return; }
    string name = readLine("Customer Name: ");
    string mobile;
    while (true) {
        mobile = readWord("Mobile Number (10 digits): ");
        if (validMobile(mobile)) break;
        cout << "Invalid mobile number!\n";
    }
    cout << "1. Car\n2. EV\n3. Bike\n4. Scooty\n";
    int vt = readInt("Choose vehicle type: ");
    string type;
    switch (vt) {
        case 1: type = "Car"; break;
        case 2: type = "EV"; break;
        case 3: type = "Bike"; break;
        case 4: type = "Scooty"; break;
        default: cout << "Invalid vehicle type!\n"; return;
    }
    int hours = readInt("Booked hours (1-72): ");
    if (hours < 1 || hours > 72) { cout << "Hours must be between 1 and 72.\n"; return; }
    int slot = findFreeSlot(*l);
    if (slot < 0) { cout << "No free slot available!\n"; return; }
    Record r;
    r.locId = l->id;
    r.slot = slot;
    r.name = name;
    r.mobile = mobile;
    r.vehicleNo = no;
    r.type = type;
    r.estHours = hours;
    r.actHours = 0;
    r.normalFee = hours * normalRate;
    r.overtimeFee = 0;
    r.exited = false;
    r.entryTime = time(nullptr);
    r.exitTime = 0;
    r.month = 0;
    r.year = 0;
    recs.push_back(r);
    l->available--;
    saveRecords();
    saveLocations();
    cout << "Vehicle entry successful!\nTICKET\n";
    printRecord(r);
    newTxn(no, "Parking", r.normalFee);
}
void vehicleExit() {
    cout << "VEHICLE EXIT\n";
    string no = toUpper(readWord("Vehicle Number: "));
    int idx = findActive(no);
    if (idx < 0) { cout << "Vehicle not found!\n"; return; }
    Record &r = recs[idx];
    int mins = readInt("Actual parked minutes (-1 = use real time): ");
    if (mins < 0) mins = max(1, (int)ceil(difftime(time(nullptr), r.entryTime) / 60.0));
    int bookedMins = r.estHours * 60;
    int extra = max(0, mins - bookedMins);
    int extraHours = (extra + 59) / 60;
    double otFee = extraHours * overtimeRate;
    r.exited = true;
    r.actHours = mins / 60.0;
    r.overtimeFee = otFee;
    r.exitTime = time(nullptr);
    today(r.month, r.year);
    Location *l = findLocation(r.locId);
    if (l && l->available < l->total) l->available++;
    saveRecords();
    saveLocations();
    cout << "Parked " << mins << " min, booked " << bookedMins << " min.\n";
    printRecord(r);
    cout << "Vehicle exit successful! Parking slot released.\n";
    for (auto &t : txns)
        if (t.vehicleNo == no && (t.status == "Pending" || t.status == "Failed")) {
            cout << "Note: You have unpaid dues (see 'Pending Dues' menu).\n";
            break;
        }
    if (extra > 0) {
        cout << "Overtime: " << extra << " min -> " << extraHours << " hr x Rs. " << overtimeRate << "\n";
        newTxn(no, "Overtime", otFee);
    } else {
        cout << "No overtime.\n";
    }
}
void searchVehicle(bool adminView) {
    string q = toLower(readLine("Enter Vehicle Number / Name: "));
    bool found = false;
    vector<string> shown;
    for (auto &r : recs) {
        if ((toLower(r.vehicleNo) == q || toLower(r.name) == q) && (adminView || !r.exited)) {
            if (!found) cout << "VEHICLE DETAILS\n";
            found = true;
            printRecord(r);
            if (find(shown.begin(), shown.end(), r.vehicleNo) == shown.end())
                shown.push_back(r.vehicleNo);
        }
    }
    if (!found) { cout << "Vehicle Not Found!\n"; return; }
    for (auto &v : shown) {
        bool head = false;
        for (auto &t : txns)
            if (t.vehicleNo == v) {
                if (!head) { cout << "Payments for " << v << ":\n"; head = true; }
                showTxn(t);
            }
    }
}
void displayRecords() {
    if (recs.empty()) { cout << "No Vehicle Records Found!\n"; return; }
    cout << "ALL VEHICLE RECORDS\n";
    for (auto &r : recs) printRecord(r);
}
double revenueOf(int year, int month) {
    double s = 0;
    for (auto &t : txns)
        if (t.status == "Successful" && t.year == year && t.month == month) s += t.amount;
    return s;
}
double expenseOf(int year, int month) {
    double s = 0;
    for (auto &e : exps)
        if (e.year == year && e.month == month) s += e.amount;
    return s;
}
void addExpense() {
    Expense e;
    e.month = readInt("Month (1-12): ");
    if (e.month < 1 || e.month > 12) { cout << "Invalid Month!\n"; return; }
    e.year = readInt("Year (e.g. 2026): ");
    if (e.year < 2000 || e.year > 2200) { cout << "Invalid Year!\n"; return; }
    e.amount = readDouble("Expense Amount (Rs.): ");
    if (e.amount <= 0) { cout << "Amount must be greater than 0.\n"; return; }
    e.note = readLine("Note (e.g. electricity, salary): ");
    exps.push_back(e);
    saveExpenses();
    cout << "Expense added.\n";
}
void totalRevenue() {
    double revenue = 0, spent = 0, pending = 0;
    for (auto &t : txns) {
        if (t.status == "Successful") revenue += t.amount;
        else pending += t.amount;
    }
    for (auto &e : exps) spent += e.amount;
    double net = revenue - spent;
    cout << fixed << setprecision(2);
    cout << "Total Revenue  : Rs. " << revenue
         << "\nTotal Expenses : Rs. " << spent
         << "\nUnpaid Dues    : Rs. " << pending << "\n";
    cout << (net >= 0 ? "NET PROFIT     : Rs. " : "NET LOSS       : Rs. ") << fabs(net) << "\n";
}
void monthlyProfitLoss() {
    static const char *names[12] = {"January", "February", "March", "April", "May", "June", "July",
                                    "August", "September", "October", "November", "December"};
    int month = readInt("Enter Month (1-12): ");
    if (month < 1 || month > 12) { cout << "Invalid Month!\n"; return; }
    int year = readInt("Enter Year (e.g. 2026): ");
    double revenue = revenueOf(year, month);
    double spent = expenseOf(year, month);
    double net = revenue - spent;
    cout << "" << names[month - 1] << " " << year << " REPORT\n"
         << "Revenue  : Rs. " << revenue << "\nExpenses : Rs. " << spent
         << "\n";
    if (net > 0) cout << "PROFIT   : Rs. " << net << "\n";
    else if (net < 0) cout << "LOSS     : Rs. " << fabs(net) << "\n";
    else cout << "No Profit, No Loss (Rs. 0.00)\n";
}
void changeRate(const string &title, double &rate) {
    cout << "Current " << title << " Rate: Rs. " << rate << " per hour\n";
    double r = readDouble("Enter New " + title + " Rate: ");
    if (r < 0) { cout << "Invalid Rate!\n"; return; }
    rate = r;
    saveSettings();
    cout << title << " Rate Updated Successfully!\n";
}
bool adminLogin() {
    cout << "ADMIN LOGIN\n";
    string u = readWord("Enter Username: ");
    string p = readWord("Enter Password: ");
    if (u == adminUser && p == adminPass) {
        cout << "Login Successful!\n";
        return true;
    }
    cout << "Invalid Username or Password!\n";
    return false;
}
void adminPanel() {
    if (!adminLogin()) return;
    int ch;
    do {
        cout << "ADMIN PANEL\n"
             << "1. Manage Parking Locations\n"
             << "2. Change Normal Parking Rate\n"
             << "3. Change Overtime Parking Rate\n"
             << "4. View Vehicle Records\n"
             << "5. Search Vehicle\n"
             << "6. View Revenue and Profit/Loss\n"
             << "7. Monthly Profit/Loss\n"
             << "8. Add Expense\n"
             << "9. Transaction History\n"
             << "0. Logout\n";
        ch = readInt("Enter Your Choice: ");
        switch (ch) {
            case 1: manageLocations(); break;
            case 2: changeRate("Normal", normalRate); break;
            case 3: changeRate("Overtime", overtimeRate); break;
            case 4: displayRecords(); break;
            case 5: searchVehicle(true); break;
            case 6: totalRevenue(); break;
            case 7: monthlyProfitLoss(); break;
            case 8: addExpense(); break;
            case 9: allTransactions(); break;
            case 0: cout << "Admin Logged Out!\n"; break;
            default: cout << "Invalid Choice!\n";
        }
    } while (ch != 0);
}
void userPanel() {
    int ch;
    do {
        cout << "VEHICLE PANEL\n"
             << "1. Vehicle Entry\n"
             << "2. Vehicle Exit\n"
             << "3. Parking Availability\n"
             << "4. Search Vehicle\n"
             << "5. Pending Dues\n"
             << "0. Back\n";
        ch = readInt("Enter Your Choice: ");
        switch (ch) {
            case 1: vehicleEntry(); break;
            case 2: vehicleExit(); break;
            case 3: viewLocations(); break;
            case 4: searchVehicle(false); break;
            case 5: pendingDues(); break;
            case 0: break;
            default: cout << "Invalid Choice!\n";
        }
    } while (ch != 0);
}
static string webToken;
static string urlDecode(const string &s) {
    string r;
    for (size_t i = 0; i < s.size(); i++) {
        if (s[i] == '+') r += ' ';
        else if (s[i] == '%' && i + 2 < s.size()) { r += (char)strtol(s.substr(i + 1, 2).c_str(), nullptr, 16); i += 2; }
        else r += s[i];
    }
    return r;
}
static string getParam(const string &data, const string &key) {
    for (auto &kv : split(data, '&')) {
        size_t e = kv.find('=');
        if (e != string::npos && kv.substr(0, e) == key) return urlDecode(kv.substr(e + 1));
    }
    return "";
}
static string jesc(const string &s) {
    string r;
    for (char c : s) {
        if (c == '"' || c == '\\') { r += '\\'; r += c; }
        else if ((unsigned char)c < 32) r += ' ';
        else r += c;
    }
    return r;
}
static string num(double d) { ostringstream o; o << fixed << setprecision(2) << d; return o.str(); }
static string jmsg(bool ok, const string &m, const string &extra = "") {
    return string("{\"ok\":") + (ok ? "true" : "false") + ",\"msg\":\"" + jesc(m) + "\"" + extra + "}";
}
static string txnJ(const Txn &t) {
    return "{\"id\":" + to_string(t.id) + ",\"vehicle\":\"" + jesc(t.vehicleNo) + "\",\"kind\":\"" + t.kind +
           "\",\"amount\":" + num(t.amount) + ",\"mode\":\"" + t.mode + "\",\"status\":\"" + t.status +
           "\",\"date\":\"" + to_string(t.month) + "/" + to_string(t.year) + "\"}";
}
static size_t webTxn(const string &v, const string &kind, double amt) {
    int mx = 100;
    for (auto &t : txns) mx = max(mx, t.id);
    Txn t;
    t.id = mx + 1; t.vehicleNo = v; t.kind = kind; t.mode = ""; t.status = "Pending"; t.amount = amt;
    today(t.month, t.year);
    txns.push_back(t);
    saveTxns();
    return txns.size() - 1;
}
static const char *WEB_PAGE = R"HTML(<!DOCTYPE html><html><head><meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1"><title>Smart Parking System</title>
<style>
:root{--bg:#070b18;--card:#111833;--card2:#18214a;--line:#27346a;--tx:#eef1ff;--mut:#8f9bc7;--pri:#7c5cff;--pri2:#22d3ee;--pk:#ec4899}
*{box-sizing:border-box}
body{margin:0;position:relative;overflow-x:hidden;font-family:'Segoe UI',system-ui,Arial,sans-serif;color:var(--tx);min-height:100vh;background:var(--bg)}
.orb{position:fixed;border-radius:50%;filter:blur(100px);opacity:.38;z-index:-1;animation:fl 16s ease-in-out infinite alternate}
.o1{width:520px;height:520px;background:#6d28d9;top:-160px;left:-120px}
.o2{width:460px;height:460px;background:#0891b2;top:30%;right:-140px;animation-delay:-5s}
.o3{width:420px;height:420px;background:#be185d;bottom:-160px;left:25%;animation-delay:-9s}
@keyframes fl{to{transform:translate(60px,50px) scale(1.15)}}
header{display:flex;align-items:center;justify-content:space-between;padding:14px 28px;background:rgba(7,11,24,.72);backdrop-filter:blur(14px);border-bottom:1px solid var(--line);position:sticky;top:0;z-index:20}
.brand{display:flex;align-items:center;gap:13px}
.logo{width:46px;height:46px;border-radius:14px;background:linear-gradient(135deg,var(--pri),var(--pri2));display:grid;place-items:center;font-size:25px;box-shadow:0 0 24px rgba(124,92,255,.7);animation:pu 3s ease-in-out infinite}
@keyframes pu{50%{box-shadow:0 0 40px rgba(34,211,238,.8)}}
.brand b{font-size:21px;display:block;background:linear-gradient(90deg,#fff,#a5b4fc,#67e8f9);-webkit-background-clip:text;background-clip:text;color:transparent}
.brand small{color:var(--mut);letter-spacing:1.5px;text-transform:uppercase;font-size:11px}
.live{font-size:13px;color:var(--mut);display:flex;align-items:center;gap:8px}
.live i{width:9px;height:9px;border-radius:50%;background:#22c55e;animation:bl 1.5s infinite}
@keyframes bl{0%{box-shadow:0 0 0 0 rgba(34,197,94,.7)}100%{box-shadow:0 0 0 10px rgba(34,197,94,0)}}
.road{height:22px;position:relative;overflow:hidden}
.road:before{content:"";position:absolute;left:0;right:0;top:50%;height:2px;background:repeating-linear-gradient(90deg,#34407a 0 18px,transparent 18px 36px)}
.road span{position:absolute;top:0;font-size:18px;animation:dr 14s linear infinite}
@keyframes dr{from{left:-40px}to{left:100%}}
nav{display:flex;gap:6px;margin:16px auto 6px;background:rgba(17,24,51,.8);padding:6px;border-radius:16px;width:max-content;border:1px solid var(--line);backdrop-filter:blur(8px)}
nav button{background:none;color:var(--mut);border:0;padding:11px 28px;border-radius:12px;cursor:pointer;font-size:15px;font-weight:600;transition:.25s}
nav button:hover{color:#fff}
nav button.on{background:linear-gradient(135deg,var(--pri),#4f46e5);color:#fff;box-shadow:0 8px 24px rgba(124,92,255,.55)}
main{max-width:1150px;margin:auto;padding:12px 22px 30px}
#v,#a{animation:fd .45s}
@keyframes fd{from{opacity:0;transform:translateY(10px)}}
.stats{display:grid;grid-template-columns:repeat(auto-fit,minmax(210px,1fr));gap:16px;margin:10px 0 14px}
.st{position:relative;overflow:hidden;padding:18px 20px;border-radius:18px;background:linear-gradient(145deg,rgba(124,92,255,.16),rgba(17,24,51,.8));border:1px solid var(--line);backdrop-filter:blur(8px);transition:.25s}
.st:hover{transform:translateY(-4px);border-color:var(--pri)}
.st small{color:var(--mut);text-transform:uppercase;letter-spacing:1px;font-size:11px;font-weight:700}
.st b{display:block;font-size:36px;margin-top:4px;background:linear-gradient(90deg,#fff,#67e8f9);-webkit-background-clip:text;background-clip:text;color:transparent}
.st i{position:absolute;right:16px;top:12px;font-size:30px;font-style:normal;opacity:.9}
.occ{padding:14px 20px;border-radius:16px;background:rgba(17,24,51,.8);border:1px solid var(--line);margin-bottom:22px;display:flex;align-items:center;gap:16px}
.occ span{color:var(--mut);font-size:13px;white-space:nowrap;min-width:110px}
.pbar{flex:1;height:10px;border-radius:10px;background:#0a1030;overflow:hidden;border:1px solid var(--line)}
.pbar div{height:100%;width:0;background:linear-gradient(90deg,var(--pri2),var(--pri),var(--pk));transition:width .9s;box-shadow:0 0 14px var(--pri)}
.grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(440px,1fr));gap:20px;align-items:start;margin-bottom:20px}
section{position:relative;background:linear-gradient(180deg,rgba(24,33,74,.92),rgba(17,24,51,.92));border:1px solid var(--line);border-radius:20px;padding:22px;box-shadow:0 14px 40px rgba(0,0,0,.4);margin-bottom:20px;transition:border-color .25s}
section:before{content:"";position:absolute;inset:0 0 auto 0;height:3px;border-radius:20px 20px 0 0;background:linear-gradient(90deg,var(--pri),var(--pri2),var(--pk))}
section:hover{border-color:#4b5bb0}
.grid section{margin-bottom:0}
h3{margin:0 0 16px;font-size:18px;display:flex;align-items:center;gap:10px}
h3 span{width:36px;height:36px;border-radius:11px;display:grid;place-items:center;background:linear-gradient(135deg,rgba(124,92,255,.35),rgba(34,211,238,.2));font-size:17px}
.r2{display:flex;gap:12px;flex-wrap:wrap}
.f{display:flex;flex-direction:column;gap:5px;margin-bottom:12px;flex:1;min-width:130px}
.f label{font-size:11px;color:var(--mut);font-weight:700;letter-spacing:.7px;text-transform:uppercase}
input,select{width:100%;background:#0a1030;color:var(--tx);border:1px solid var(--line);border-radius:11px;padding:12px;font-size:14px;outline:0;transition:.2s}
input:focus,select:focus{border-color:var(--pri2);box-shadow:0 0 0 3px rgba(34,211,238,.2),0 0 18px rgba(124,92,255,.25)}
input::placeholder{color:#5a6699}
button.b{background:linear-gradient(135deg,var(--pri),#4f46e5);color:#fff;border:0;border-radius:11px;padding:12px 22px;font-weight:700;font-size:14px;cursor:pointer;transition:.2s;margin:2px 8px 2px 0}
button.b:hover{transform:translateY(-2px);box-shadow:0 10px 26px rgba(124,92,255,.6)}
button.b:active{transform:scale(.97)}
button.g{background:#232e62}button.d{background:linear-gradient(135deg,#ef4444,#b91c1c)}
table{border-collapse:separate;border-spacing:0;width:100%;font-size:14px;margin-top:14px;border:1px solid var(--line);border-radius:12px;overflow:hidden}
th{background:#0a1030;color:var(--mut);font-size:11px;text-transform:uppercase;letter-spacing:.7px}
th,td{padding:10px 12px;text-align:left;border-bottom:1px solid var(--line)}
tr:last-child td{border-bottom:0}tr:hover td{background:rgba(124,92,255,.1)}
.bg{padding:3px 10px;border-radius:20px;font-size:12px;font-weight:700}
.bg.ok{background:rgba(34,197,94,.18);color:#86efac}.bg.wa{background:rgba(245,158,11,.18);color:#fcd34d}.bg.er{background:rgba(239,68,68,.18);color:#fca5a5}
pre{margin:12px 0 0;padding:12px 14px;border-radius:11px;white-space:pre-wrap;font-family:inherit;font-size:14px;line-height:1.55}
pre:empty{display:none}
pre.ok{background:rgba(34,197,94,.12);border:1px solid rgba(34,197,94,.4);color:#bbf7d0}
pre.err{background:rgba(239,68,68,.12);border:1px solid rgba(239,68,68,.4);color:#fecaca}
.sg{position:absolute;left:0;right:0;top:calc(100% + 6px);background:#0a1030;border:1px solid var(--line);border-radius:13px;z-index:10;overflow:hidden;box-shadow:0 18px 40px rgba(0,0,0,.6)}
.sg div{padding:12px 14px;cursor:pointer}.sg div:hover{background:rgba(124,92,255,.25)}
.chip{display:flex;gap:12px;flex-wrap:wrap;margin-top:16px}
.chip div{background:#0a1030;border:1px solid var(--line);border-radius:13px;padding:10px 16px;flex:1;min-width:140px}
.chip small{display:block;color:var(--mut);font-size:11px;text-transform:uppercase;letter-spacing:.6px}.chip b{font-size:18px}
.legend{display:flex;gap:18px;margin-top:16px;font-size:12px;color:var(--mut)}
.dot{display:inline-block;width:11px;height:11px;border-radius:3px;margin-right:6px;vertical-align:-1px}
.slots{display:grid;grid-template-columns:repeat(auto-fill,minmax(48px,1fr));gap:8px;margin-top:12px;max-height:250px;overflow:auto;padding:4px}
.sl{height:44px;border-radius:10px;display:grid;place-items:center;font-size:13px;font-weight:700;transition:.2s}
.sl:hover{transform:scale(1.12)}
.sl.f{background:rgba(34,197,94,.14);border:1px solid rgba(34,197,94,.55);color:#86efac}
.sl.o{background:rgba(239,68,68,.16);border:1px solid rgba(239,68,68,.55);font-size:19px}
#tw{position:fixed;right:20px;top:84px;z-index:50;display:flex;flex-direction:column;gap:10px}
.toast{padding:12px 18px;border-radius:12px;background:#141c3d;border:1px solid var(--line);box-shadow:0 12px 34px rgba(0,0,0,.55);animation:in .3s;max-width:340px;font-size:14px}
.toast.ok{border-left:4px solid #22c55e}.toast.err{border-left:4px solid #ef4444}
@keyframes in{from{opacity:0;transform:translateX(40px)}}
.mut{color:var(--mut)}.hide{display:none!important}
.login{max-width:480px;margin:30px auto}
footer{text-align:center;color:var(--mut);font-size:12px;padding-bottom:26px}
</style></head><body>
<div class="orb o1"></div><div class="orb o2"></div><div class="orb o3"></div><div id="tw"></div>
<header><div class="brand"><div class="logo">&#127359;</div><div><b>Smart Parking</b><small>Management System</small></div></div><div class="live"><i></i>Server Online</div></header>
<div class="road"><span>&#128663;</span></div>
<nav><button id="nv" class="on" onclick="tab('v')">&#128664; Vehicle Panel</button><button id="na" onclick="tab('a')">&#128274; Admin Panel</button></nav>
<main>
<div class="stats">
<div class="st"><i>&#128205;</i><small>Locations</small><b id="s1">0</b></div>
<div class="st"><i>&#127359;</i><small>Total Slots</small><b id="s2">0</b></div>
<div class="st"><i>&#9989;</i><small>Free Slots</small><b id="s3">0</b></div>
<div class="st"><i>&#128664;</i><small>Parked Now</small><b id="s4">0</b></div>
</div>
<div class="occ"><span id="pc">Loading...</span><div class="pbar"><div id="bar"></div></div></div>
<div id="v">
<section><h3><span>&#128269;</span>Find Parking Location</h3>
<div style="position:relative"><input id="q" placeholder="Search location by name or area..." oninput="sug()" onkeydown="if(event.key=='Enter')pickFirst()"><div id="sg" class="sg"></div></div><div id="sel"></div></section>
<div class="grid">
<section><h3><span>&#10133;</span>Vehicle Entry</h3><input type="hidden" id="loc">
<div class="r2"><div class="f"><label>Vehicle No</label><input id="vno" placeholder="UK07AB1234"></div><div class="f"><label>Customer Name</label><input id="nm" placeholder="Full name"></div></div>
<div class="r2"><div class="f"><label>Mobile</label><input id="mob" placeholder="10 digit number"></div><div class="f"><label>Type</label><select id="vt"><option>Car</option><option>EV</option><option>Bike</option><option>Scooty</option></select></div><div class="f"><label>Hours</label><input id="hrs" type="number" min="1" max="72" placeholder="1 - 72"></div></div>
<button class="b" onclick="entry()">Park Vehicle</button><pre id="r1"></pre></section>
<section><h3><span>&#128682;</span>Vehicle Exit</h3>
<div class="r2"><div class="f"><label>Vehicle No</label><input id="xno" placeholder="UK07AB1234"></div><div class="f"><label>Minutes (optional)</label><input id="xmin" type="number" placeholder="Blank = real time"></div></div>
<button class="b" onclick="exitV()">Exit Vehicle</button><pre id="r2"></pre></section>
<section><h3><span>&#128179;</span>Payment</h3>
<div class="r2"><div class="f"><label>Txn ID</label><input id="tid" placeholder="e.g. 101"></div><div class="f"><label>Mode</label><select id="pm" onchange="$('upi').parentNode.style.display=this.value=='UPI'?'':'none'"><option>UPI</option><option>Card</option></select></div></div>
<div class="f"><label>UPI ID</label><input id="upi" placeholder="name@upi"></div>
<button class="b" onclick="payNow()">Pay Now</button><pre id="r3"></pre></section>
<section><h3><span>&#9888;</span>Pending Dues</h3>
<div class="r2"><div class="f"><label>Vehicle No</label><input id="dno" placeholder="UK07AB1234"></div></div>
<button class="b g" onclick="dues()">Check Dues</button><div id="r4"></div></section>
</div></div>
<div id="a" class="hide">
<section id="lg" class="login"><h3><span>&#128274;</span>Admin Login</h3>
<div class="f"><label>Username</label><input id="au" placeholder="Username"></div><div class="f"><label>Password</label><input id="ap" type="password" placeholder="Password"></div>
<button class="b" onclick="login()">Login</button><pre id="r5"></pre></section>
<div id="ad" class="hide"><div class="grid">
<section><h3><span>&#128205;</span>Manage Locations</h3>
<div class="r2"><div class="f"><label>Name</label><input id="ln" placeholder="Location name"></div><div class="f"><label>Address</label><input id="la" placeholder="Address"></div></div>
<div class="r2"><div class="f"><label>Total Slots</label><input id="lt" type="number" placeholder="e.g. 60"></div><div class="f"><label>Location ID (edit / delete)</label><input id="li" type="number" placeholder="ID"></div></div>
<button class="b" onclick="act('/api/addloc',{name:$('ln').value,address:$('la').value,total:$('lt').value},'r6')">Add</button>
<button class="b g" onclick="act('/api/editloc',{id:$('li').value,name:$('ln').value,address:$('la').value,total:$('lt').value},'r6')">Edit</button>
<button class="b d" onclick="act('/api/delloc',{id:$('li').value},'r6')">Delete</button><pre id="r6"></pre></section>
<section><h3><span>&#128176;</span>Rates &amp; Expenses</h3>
<div class="r2"><div class="f"><label>Rate Type</label><select id="rk"><option value="normal">Normal</option><option value="overtime">Overtime</option></select></div><div class="f"><label>Rs. per hour</label><input id="rv" type="number" placeholder="e.g. 50"></div></div>
<button class="b" onclick="act('/api/rate',{kind:$('rk').value,value:$('rv').value},'r7')">Update Rate</button>
<hr style="border:0;border-top:1px solid var(--line);margin:16px 0">
<div class="r2"><div class="f"><label>Month</label><input id="em" type="number" placeholder="1-12"></div><div class="f"><label>Year</label><input id="ey" type="number" placeholder="2026"></div></div>
<div class="r2"><div class="f"><label>Amount</label><input id="ea" type="number" placeholder="Rs."></div><div class="f"><label>Note</label><input id="en" placeholder="Electricity, salary..."></div></div>
<button class="b" onclick="act('/api/expense',{month:$('em').value,year:$('ey').value,amount:$('ea').value,note:$('en').value},'r7')">Add Expense</button><pre id="r7"></pre></section>
</div>
<section><h3><span>&#128200;</span>Revenue / Profit &amp; Loss</h3>
<div class="r2"><div class="f"><label>Month (blank = all time)</label><input id="rm" type="number" placeholder="1-12"></div><div class="f"><label>Year</label><input id="ry" type="number" placeholder="2026"></div></div>
<button class="b" onclick="report()">Show Report</button><pre id="r8"></pre></section>
<section><h3><span>&#128203;</span>Records &amp; Transactions</h3>
<button class="b g" onclick="list('/api/locations','r9')">All Locations</button><button class="b g" onclick="list('/api/records','r9')">Vehicle Records</button><button class="b g" onclick="list('/api/txns','r9')">Transaction History</button><div id="r9" style="overflow-x:auto"></div></section>
</div></div>
</main>
<footer>Smart Parking Management System</footer>)HTML"
R"HTML(<script>
let T='';const $=i=>document.getElementById(i);
const esc=s=>String(s).replace(/&/g,'&amp;').replace(/</g,'&lt;').replace(/>/g,'&gt;');
const bd=v=>({Successful:'ok',Pending:'wa',Failed:'er',Parked:'wa',Exited:'ok'}[v]);
async function api(p,o={}){o.token=T;try{const r=await fetch(p,{method:'POST',headers:{'Content-Type':'application/x-www-form-urlencoded'},body:new URLSearchParams(o)});return await r.json();}catch(e){return{ok:false,msg:'Server error'}}}
function toast(m,ok){const t=document.createElement('div');t.className='toast '+(ok?'ok':'err');t.textContent=m.length>110?m.slice(0,110)+'...':m;$('tw').appendChild(t);setTimeout(()=>t.remove(),3500);}
function say(id,r){const e=$(id);e.textContent=r.msg;e.className=r.ok?'ok':'err';if(id!='r8')toast(r.msg,r.ok);}
function cnt(id,v){const e=$(id),s=+e.dataset.v||0;e.dataset.v=v;const t0=performance.now();(function f(t){const k=Math.min(1,(t-t0)/700);e.textContent=Math.round(s+(v-s)*k);if(k<1)requestAnimationFrame(f);})(t0);}
function tbl(a){if(!a.length)return'<p class="mut">No data found.</p>';const k=Object.keys(a[0]);return'<table><tr>'+k.map(x=>'<th>'+x+'</th>').join('')+'</tr>'+a.map(r=>'<tr>'+k.map(x=>'<td>'+(bd(r[x])?'<span class="bg '+bd(r[x])+'">'+r[x]+'</span>':esc(r[x]))+'</td>').join('')+'</tr>').join('')+'</table>';}
function tab(t){['v','a'].forEach(x=>{$(x).classList.toggle('hide',x!=t);$('n'+x).classList.toggle('on',x==t)});}
let L=[];
async function locs(){L=await api('/api/locations');const tot=L.reduce((a,l)=>a+l.total,0),fr=L.reduce((a,l)=>a+l.available,0),pc=tot?Math.round((tot-fr)*100/tot):0;cnt('s1',L.length);cnt('s2',tot);cnt('s3',fr);cnt('s4',tot-fr);$('bar').style.width=pc+'%';$('pc').textContent=tot?pc+'% occupied':'No locations yet';if($('loc').value)show($('loc').value);}
function match(){const v=$('q').value.trim().toLowerCase();if(!v)return[];return L.filter(l=>(l.name+' '+l.address).toLowerCase().includes(v)).slice(0,3);}
function sug(){$('loc').value='';$('sel').innerHTML='';const m=match();$('sg').innerHTML=$('q').value.trim()?(m.length?m.map(l=>'<div onmousedown="pick('+l.id+')">&#128205; '+esc(l.name)+' <span class="mut">- '+esc(l.address)+'</span></div>').join(''):'<div class="mut">No match found</div>'):'';}
function pick(id){const l=L.find(x=>x.id==id);if(!l)return;$('loc').value=id;$('q').value=l.name;$('sg').innerHTML='';show(id);}
function pickFirst(){const m=match();if(m.length)pick(m[0].id);}
async function show(id){const l=L.find(x=>x.id==id);if(!l){$('sel').innerHTML='';return;}const o=await api('/api/slots',{loc:id});if($('loc').value!=id)return;const n=Math.min(l.total,150);let h='<div class="chip"><div><small>Location</small><b>'+esc(l.name)+'</b></div><div><small>Address</small><b>'+esc(l.address)+'</b></div><div><small>Free Slots</small><b>'+l.available+' / '+l.total+'</b></div></div><div class="legend"><span><i class="dot" style="background:#22c55e"></i>Free</span><span><i class="dot" style="background:#ef4444"></i>Occupied</span></div><div class="slots">';for(let i=1;i<=n;i++){const u=Array.isArray(o)&&o.includes(i);h+='<div class="sl '+(u?'o':'f')+'" title="Slot '+i+(u?' - occupied':' - free')+'">'+(u?'&#128663;':i)+'</div>';}h+='</div>'+(l.total>n?'<p class="mut">+ '+(l.total-n)+' more slots</p>':'');$('sel').innerHTML=h;}
document.addEventListener('click',e=>{if(!e.target.closest('#q'))$('sg').innerHTML='';});
async function entry(){if(!$('loc').value){say('r1',{ok:false,msg:'Pehle location search karke select karo.'});return;}const r=await api('/api/entry',{loc:$('loc').value,vehicle:$('vno').value,name:$('nm').value,mobile:$('mob').value,type:$('vt').value,hours:$('hrs').value});
say('r1',r);if(r.ok){if(r.txn)$('tid').value=r.txn;['vno','nm','mob','hrs','q','loc'].forEach(i=>$(i).value='');$('vt').selectedIndex=0;$('sel').innerHTML='';}locs();}
async function exitV(){const r=await api('/api/exit',{vehicle:$('xno').value,mins:$('xmin').value});say('r2',r);if(r.ok){if(r.txn)$('tid').value=r.txn;$('xno').value='';$('xmin').value='';}locs();}
async function payNow(){const r=await api('/api/pay',{id:$('tid').value,mode:$('pm').value,upi:$('upi').value});say('r3',r);if(r.ok){$('tid').value='';$('upi').value='';}}
async function dues(){const r=await api('/api/dues',{vehicle:$('dno').value});$('r4').innerHTML=tbl(r);}
async function login(){const r=await api('/api/login',{user:$('au').value,pass:$('ap').value});say('r5',r);if(r.ok){T=r.token;$('lg').classList.add('hide');$('ad').classList.remove('hide');}}
async function act(p,o,out){const r=await api(p,o);say(out,r);locs();}
async function report(){const r=await api('/api/report',{month:$('rm').value,year:$('ry').value});say('r8',r);}
async function list(p,out){const r=await api(p);$(out).innerHTML=Array.isArray(r)?tbl(r):esc(r.msg);}
document.querySelectorAll('input').forEach(i=>{i.setAttribute('autocomplete',i.type=='password'?'new-password':'off');if(i.type!='hidden'){i.readOnly=true;i.addEventListener('focus',()=>i.readOnly=false);}});
locs();
</script></body></html>)HTML";
static string webRoute(const string &path, const string &d, string &ctype) {
    ctype = "application/json";
    auto P = [&](const string &k) { return getParam(d, k); };
    auto admin = [&]() { return !webToken.empty() && P("token") == webToken; };
    const string NOADMIN = jmsg(false, "Admin login required.");
    if (path == "/") { ctype = "text/html; charset=utf-8"; return WEB_PAGE; }
    if (path == "/api/locations") {
        string o = "[";
        for (size_t i = 0; i < locs.size(); i++) {
            auto &l = locs[i];
            if (i) o += ",";
            o += "{\"id\":" + to_string(l.id) + ",\"name\":\"" + jesc(l.name) + "\",\"address\":\"" + jesc(l.address) +
                 "\",\"total\":" + to_string(l.total) + ",\"available\":" + to_string(l.available) + "}";
        }
        return o + "]";
    }
    if (path == "/api/slots") {
        int id = atoi(P("loc").c_str());
        string o = "[";
        bool first = true;
        for (auto &r : recs)
            if (!r.exited && r.locId == id) { if (!first) o += ","; o += to_string(r.slot); first = false; }
        return o + "]";
    }
    if (path == "/api/entry") {
        Location *l = findLocation(atoi(P("loc").c_str()));
        if (!l) return jmsg(false, "Location not found.");
        if (l->available <= 0) return jmsg(false, "Parking is full at this location!");
        string no = toUpper(trim(P("vehicle")));
        if (no.empty() || no.find_first_of(" |") != string::npos) return jmsg(false, "Invalid vehicle number (no spaces).");
        if (findActive(no) >= 0) return jmsg(false, "This vehicle is already parked!");
        string name = noPipe(trim(P("name")));
        if (name.empty()) return jmsg(false, "Customer name required.");
        string mob = trim(P("mobile"));
        if (!validMobile(mob)) return jmsg(false, "Invalid mobile number!");
        string type = P("type");
        if (type != "Car" && type != "EV" && type != "Bike" && type != "Scooty") return jmsg(false, "Invalid vehicle type!");
        int h = atoi(P("hours").c_str());
        if (h < 1 || h > 72) return jmsg(false, "Hours must be between 1 and 72.");
        int slot = findFreeSlot(*l);
        if (slot < 0) return jmsg(false, "No free slot available!");
        Record r;
        r.locId = l->id; r.slot = slot; r.name = name; r.mobile = mob; r.vehicleNo = no; r.type = type;
        r.estHours = h; r.actHours = 0; r.normalFee = h * normalRate; r.overtimeFee = 0; r.exited = false;
        r.entryTime = time(nullptr); r.exitTime = 0; r.month = 0; r.year = 0;
        recs.push_back(r);
        l->available--;
        saveRecords(); saveLocations();
        size_t ti = webTxn(no, "Parking", r.normalFee);
        return jmsg(true, "Entry successful! Slot " + to_string(slot) + " at " + l->name + ". Fee Rs. " + num(r.normalFee) +
                              ". Txn ID " + to_string(txns[ti].id) + " - ab Payment section se pay karo.",
                    ",\"txn\":" + to_string(txns[ti].id));
    }
    if (path == "/api/exit") {
        string no = toUpper(trim(P("vehicle")));
        int idx = findActive(no);
        if (idx < 0) return jmsg(false, "Vehicle not found!");
        Record &r = recs[idx];
        string ms = trim(P("mins"));
        int mins = ms.empty() ? -1 : atoi(ms.c_str());
        if (mins < 0) mins = max(1, (int)ceil(difftime(time(nullptr), r.entryTime) / 60.0));
        int bookedMins = r.estHours * 60;
        int extra = max(0, mins - bookedMins);
        int extraHours = (extra + 59) / 60;
        double otFee = extraHours * overtimeRate;
        r.exited = true; r.actHours = mins / 60.0; r.overtimeFee = otFee; r.exitTime = time(nullptr);
        today(r.month, r.year);
        Location *l = findLocation(r.locId);
        if (l && l->available < l->total) l->available++;
        saveRecords(); saveLocations();
        string m = "Parked " + to_string(mins) + " min, booked " + to_string(bookedMins) + " min. Slot released.";
        if (extra > 0) {
            size_t ti = webTxn(no, "Overtime", otFee);
            m += " Overtime " + to_string(extraHours) + " hr = Rs. " + num(otFee) + ". Txn ID " + to_string(txns[ti].id);
            return jmsg(true, m, ",\"txn\":" + to_string(txns[ti].id));
        }
        return jmsg(true, m + " No overtime.");
    }
    if (path == "/api/pay") {
        int id = atoi(P("id").c_str());
        for (auto &t : txns) {
            if (t.id != id) continue;
            if (t.status == "Successful") return jmsg(false, "Already paid.");
            string mode = P("mode");
            if (mode == "UPI") {
                t.mode = "UPI";
                string upi = toLower(trim(P("upi")));
                t.status = validUPI(upi) ? "Successful" : "Failed";
                if (t.status == "Failed") { saveTxns(); return jmsg(false, "Invalid UPI ID! Payment Failed."); }
            } else if (mode == "Card") {
                t.mode = "Card";
                t.status = (rand() % 10 < 8) ? "Successful" : "Failed";
            } else return jmsg(false, "Invalid payment mode.");
            saveTxns();
            return jmsg(t.status == "Successful", "Payment of Rs. " + num(t.amount) + ": " + t.status);
        }
        return jmsg(false, "Transaction not found.");
    }
    if (path == "/api/dues") {
        string v = toUpper(trim(P("vehicle")));
        string o = "[";
        bool first = true;
        for (auto &t : txns)
            if (t.vehicleNo == v && (t.status == "Pending" || t.status == "Failed")) {
                if (!first) o += ",";
                o += txnJ(t); first = false;
            }
        return o + "]";
    }
    if (path == "/api/login") {
        if (P("user") == adminUser && P("pass") == adminPass) {
            if (webToken.empty()) webToken = to_string(rand()) + to_string(rand()) + to_string(time(nullptr));
            return jmsg(true, "Login Successful!", ",\"token\":\"" + webToken + "\"");
        }
        return jmsg(false, "Invalid Username or Password!");
    }
    if (path == "/api/addloc") {
        if (!admin()) return NOADMIN;
        Location l;
        l.id = nextLocId(); l.name = noPipe(trim(P("name"))); l.address = noPipe(trim(P("address")));
        l.total = atoi(P("total").c_str());
        if (l.name.empty() || l.address.empty()) return jmsg(false, "Name and address required.");
        if (l.total <= 0) return jmsg(false, "Slots must be greater than 0.");
        l.available = l.total;
        locs.push_back(l); saveLocations();
        return jmsg(true, "Location added (ID " + to_string(l.id) + ").");
    }
    if (path == "/api/editloc") {
        if (!admin()) return NOADMIN;
        Location *l = findLocation(atoi(P("id").c_str()));
        if (!l) return jmsg(false, "Location not found.");
        string n = noPipe(trim(P("name"))), a = noPipe(trim(P("address")));
        int t = atoi(P("total").c_str());
        int occupied = max(l->total - l->available, activeCountAt(l->id));
        int minTotal = max(occupied, maxSlotUsed(l->id));
        if (n.empty() || a.empty()) return jmsg(false, "Name and address required.");
        if (t <= 0 || t < minTotal) return jmsg(false, "Total cannot be less than " + to_string(max(1, minTotal)) + ".");
        l->name = n; l->address = a; l->total = t; l->available = t - occupied;
        saveLocations();
        return jmsg(true, "Location updated.");
    }
    if (path == "/api/delloc") {
        if (!admin()) return NOADMIN;
        int id = atoi(P("id").c_str());
        for (size_t i = 0; i < locs.size(); i++)
            if (locs[i].id == id) {
                if (locs[i].available != locs[i].total || activeCountAt(id) > 0)
                    return jmsg(false, "Vehicles are parked at this location, cannot delete.");
                locs.erase(locs.begin() + i); saveLocations();
                return jmsg(true, "Location deleted.");
            }
        return jmsg(false, "Location not found.");
    }
    if (path == "/api/rate") {
        if (!admin()) return NOADMIN;
        double v = atof(P("value").c_str());
        if (v < 0 || P("value").empty()) return jmsg(false, "Invalid Rate!");
        if (P("kind") == "overtime") overtimeRate = v; else normalRate = v;
        saveSettings();
        return jmsg(true, "Rate Updated Successfully!");
    }
    if (path == "/api/expense") {
        if (!admin()) return NOADMIN;
        Expense e;
        e.month = atoi(P("month").c_str()); e.year = atoi(P("year").c_str()); e.amount = atof(P("amount").c_str());
        e.note = noPipe(trim(P("note")));
        if (e.month < 1 || e.month > 12) return jmsg(false, "Invalid Month!");
        if (e.year < 2000 || e.year > 2200) return jmsg(false, "Invalid Year!");
        if (e.amount <= 0) return jmsg(false, "Amount must be greater than 0.");
        exps.push_back(e); saveExpenses();
        return jmsg(true, "Expense added.");
    }
    if (path == "/api/report") {
        if (!admin()) return NOADMIN;
        bool monthly = !trim(P("month")).empty();
        int m = atoi(P("month").c_str()), y = atoi(P("year").c_str());
        if (monthly && (m < 1 || m > 12 || y < 2000)) return jmsg(false, "Invalid month/year.");
        double rev = 0, sp = 0, pen = 0;
        for (auto &t : txns) {
            if (monthly && !(t.month == m && t.year == y)) continue;
            if (t.status == "Successful") rev += t.amount; else pen += t.amount;
        }
        for (auto &e : exps)
            if (!monthly || (e.month == m && e.year == y)) sp += e.amount;
        double net = rev - sp;
        string s = string(monthly ? to_string(m) + "/" + to_string(y) + " REPORT" : "TOTAL REPORT") +
                   "\nRevenue  : Rs. " + num(rev) + "\nExpenses : Rs. " + num(sp) + "\nUnpaid   : Rs. " + num(pen) +
                   "\n" + (net > 0 ? "PROFIT   : Rs. " : net < 0 ? "LOSS     : Rs. " : "No Profit, No Loss: Rs. ") + num(fabs(net));
        return jmsg(true, s);
    }
    if (path == "/api/txns") {
        if (!admin()) return NOADMIN;
        string o = "[";
        for (size_t i = 0; i < txns.size(); i++) { if (i) o += ","; o += txnJ(txns[i]); }
        return o + "]";
    }
    if (path == "/api/records") {
        if (!admin()) return NOADMIN;
        string o = "[";
        for (size_t i = 0; i < recs.size(); i++) {
            auto &r = recs[i];
            if (i) o += ",";
            o += "{\"vehicle\":\"" + jesc(r.vehicleNo) + "\",\"name\":\"" + jesc(r.name) + "\",\"mobile\":\"" + r.mobile +
                 "\",\"type\":\"" + r.type + "\",\"location\":\"" + jesc(locName(r.locId)) + "\",\"slot\":" + to_string(r.slot) +
                 ",\"bookedHrs\":" + to_string(r.estHours) + ",\"fee\":" + num(r.normalFee + r.overtimeFee) +
                 ",\"status\":\"" + (r.exited ? "Exited" : "Parked") + "\"}";
        }
        return o + "]";
    }
    ctype = "text/plain";
    return "Not found";
}
void startWebServer(int port = 8080) {
#ifdef _WIN32
    WSADATA w;
    WSAStartup(MAKEWORD(2, 2), &w);
#endif
    webToken.clear();
    sock_t s = socket(AF_INET, SOCK_STREAM, 0);
    int on = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (const char *)&on, sizeof(on));
    sockaddr_in a{};
    a.sin_family = AF_INET;
    a.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    a.sin_port = htons((unsigned short)port);
    if (::bind(s, (sockaddr *)&a, sizeof(a)) < 0 || listen(s, 10) < 0) {
        cout << "Port " << port << " already in use! Dusra port try karo.\n";
        CLOSESOCK(s);
        return;
    }
    string url = "http://localhost:" + to_string(port);
    cout << "Website chal rahi hai: " << url << "\n(Band karne ke liye Ctrl+C dabao)\n";
    cout.flush();
#ifdef _WIN32
    system(("start " + url).c_str());
#elif defined(__APPLE__)
    system(("open " + url).c_str());
#else
    system(("xdg-open " + url + " >/dev/null 2>&1 &").c_str());
#endif
    while (true) {
        sock_t c = accept(s, nullptr, nullptr);
        if (c == (sock_t)-1) continue;
        string req;
        char buf[4096];
        size_t hdrEnd = string::npos, need = 0;
        while (true) {
            int n = recv(c, buf, sizeof(buf), 0);
            if (n <= 0) break;
            req.append(buf, n);
            if (hdrEnd == string::npos) {
                hdrEnd = req.find("\r\n\r\n");
                if (hdrEnd != string::npos) {
                    string low = toLower(req.substr(0, hdrEnd));
                    size_t p = low.find("content-length:");
                    if (p != string::npos) need = (size_t)atol(low.c_str() + p + 15);
                }
            }
            if (hdrEnd != string::npos && req.size() >= hdrEnd + 4 + need) break;
        }
        if (hdrEnd != string::npos) {
            istringstream is(req);
            string method, target;
            is >> method >> target;
            string path = target, query;
            size_t q = target.find('?');
            if (q != string::npos) { path = target.substr(0, q); query = target.substr(q + 1); }
            string data = query + "&" + req.substr(hdrEnd + 4);
            string ctype;
            string body = webRoute(path, data, ctype);
            string resp = "HTTP/1.1 200 OK\r\nContent-Type: " + ctype + "\r\nContent-Length: " + to_string(body.size()) +
                          "\r\nConnection: close\r\n\r\n" + body;
            size_t sent = 0;
            while (sent < resp.size()) {
                int n = send(c, resp.data() + sent, (int)(resp.size() - sent), 0);
                if (n <= 0) break;
                sent += n;
            }
        }
        CLOSESOCK(c);
    }
}
int main() {
    srand((unsigned)time(nullptr));
    cout << fixed << setprecision(2);
    loadLocations();
    loadRecords();
    loadTxns();
    loadExpenses();
    loadSettings();
    startWebServer();
    return 0;
}
