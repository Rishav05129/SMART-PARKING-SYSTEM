#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstdlib>
using namespace std;
struct Location {
    int id;
    string name, address;
    int total, available;
};
static const string LOC_FILE = "locations.txt";
static vector<Location> locs;
int readInt(const string &prompt) {
    string s;
    while (true) {
        cout << prompt;
        if (!getline(cin, s)) exit(0);
        try {
            size_t pos;
            int v = stoi(s, &pos);
            if (pos == s.size()) return v;
        } catch (...) {}
        cout << "Invalid number, dobara try karo.\n";
    }
}
string readLine(const string &prompt) {
    string s;
    while (true) {
        cout << prompt;
        if (!getline(cin, s)) exit(0);
        if (!s.empty()) return s;
        cout << "Khali input allowed nahi hai.\n";
    }
}
void saveLocations() {
    ofstream f(LOC_FILE);
    for (auto &l : locs)
        f << l.id << '|' << l.name << '|' << l.address << '|' << l.total << '|' << l.available << '\n';
}
void loadLocations() {
    locs.clear();
    ifstream f(LOC_FILE);
    string line;
    while (getline(f, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string a, b, c, d, e;
        getline(ss, a, '|'); getline(ss, b, '|'); getline(ss, c, '|');
        getline(ss, d, '|'); getline(ss, e, '|');
        try {
            locs.push_back({stoi(a), b, c, stoi(d), stoi(e)});
        } catch (...) {}
    }
}
Location *findLocation(int id) {
    for (auto &l : locs)
        if (l.id == id) return &l;
    return nullptr;
}
bool hasFreeSlot(int locId) {
    Location *l = findLocation(locId);
    return l && l->available > 0;
}
bool vehicleEntered(int locId) {
    Location *l = findLocation(locId);
    if (!l || l->available <= 0) return false;
    l->available--;
    saveLocations();
    return true;
}
bool vehicleExited(int locId) {
    Location *l = findLocation(locId);
    if (!l || l->available >= l->total) return false;
    l->available++;
    saveLocations();
    return true;
}
int nextId() {
    int mx = 0;
    for (auto &l : locs) mx = max(mx, l.id);
    return mx + 1;
}
void viewLocations() {
    cout << "\nPARKING LOCATIONS\n";
    if (locs.empty()) {
        cout << "Koi location nahi hai.\n";
        return;
    }
    for (auto &l : locs) {
        cout << "\nLocation " << l.id << ": " << l.name << "\n"
             << "Address: " << l.address << "\n"
             << "Total Slots: " << l.total << "\n"
             << "Available Slots: " << l.available << "\n";
    }
}
void addLocation() {
    Location l;
    l.id = nextId();
    l.name = readLine("Location Name: ");
    l.address = readLine("Address: ");
    do {
        l.total = readInt("Total Slots: ");
        if (l.total <= 0) cout << "Slots 0 se zyada hone chahiye.\n";
    } while (l.total <= 0);
    l.available = l.total;
    locs.push_back(l);
    saveLocations();
    cout << "Location add ho gayi (ID " << l.id << ").\n";
}
void editLocation() {
    viewLocations();
    if (locs.empty()) return;
    Location *l = findLocation(readInt("\nEdit karne ke liye Location ID: "));
    if (!l) { cout << "Location nahi mili.\n"; return; }
    l->name = readLine("New Name: ");
    l->address = readLine("New Address: ");
    int occupied = l->total - l->available;
    int t;
    do {
        t = readInt("New Total Slots: ");
        if (t < occupied)
            cout << "Abhi " << occupied << " vehicles parked hain, total isse kam nahi ho sakta.\n";
    } while (t < occupied || t <= 0);
    l->total = t;
    l->available = t - occupied;
    saveLocations();
    cout << "Location update ho gayi.\n";
}
void deleteLocation() {
    viewLocations();
    if (locs.empty()) return;
    int id = readInt("\nDelete karne ke liye Location ID: ");
    for (size_t i = 0; i < locs.size(); i++) {
        if (locs[i].id == id) {
            if (locs[i].available != locs[i].total) {
                cout << "Is location par vehicles parked hain, delete nahi ho sakti.\n";
                return;
            }
            locs.erase(locs.begin() + i);
            saveLocations();
            cout << "Location delete ho gayi.\n";
            return;
        }
    }
    cout << "Location nahi mili.\n";
}
void manageLocations() {
    int ch;
    do {
        cout << "\n--- MANAGE PARKING LOCATIONS ---\n"
             << "1. View Locations\n2. Add Location\n3. Edit Location\n"
             << "4. Delete Location\n0. Back\n";
        ch = readInt("Enter Your Choice: ");
        switch (ch) {
            case 1: viewLocations(); break;
            case 2: addLocation(); break;
            case 3: editLocation(); break;
            case 4: deleteLocation(); break;
            case 0: break;
            default: cout << "Galat choice.\n";
        }
    } while (ch != 0);
}
void vehicleMenu() {
    int ch;
    do {
        cout << "\n--- VEHICLE ENTRY & EXIT ---\n"
             << "1. Vehicle Entry\n2. Vehicle Exit\n0. Back\n";
        ch = readInt("Enter Your Choice: ");
        if (ch == 1 || ch == 2) {
            int id = readInt("Location ID: ");
            if (ch == 1) {
                if (vehicleEntered(id))
                    cout << "Entry ho gayi. Available slots: " << findLocation(id)->available << "\n";
                else
                    cout << "Entry fail: location galat hai ya parking full hai.\n";
            } else {
                if (vehicleExited(id))
                    cout << "Exit ho gayi. Available slots: " << findLocation(id)->available << "\n";
                else
                    cout << "Exit fail: location galat hai ya koi vehicle parked nahi.\n";
            }
        }
    } while (ch != 0);
}
int main() {
    loadLocations();
    int ch;
    do {
        cout << "\n===== PARKING MANAGEMENT SYSTEM =====\n"
             << "1. Manage Parking Locations\n"
             << "2. Vehicle Entry & Exit Details\n"
             << "0. Exit\n";
        ch = readInt("Enter Your Choice: ");
        switch (ch) {
            case 1: manageLocations(); break;
            case 2: vehicleMenu(); break;
            case 0: cout << "Thank you!\n"; break;
            default: cout << "Galat choice.\n";
        }
    } while (ch != 0);
    return 0;
}
