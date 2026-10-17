
#include <iostream>
#include <fstream>
#include <string>
using namespace std;
const int MAX = 100;
struct Parking {
    int id;
    string name, location;
    int totalSlots, availableSlots;
};
Parking parkings[MAX];
int countParking = 0;
string lowerText(string s) {
    for (int i = 0; i < s.length(); i++) {
        if (s[i] >= 'A' && s[i] <= 'Z')
            s[i] = s[i] + 32;
    }
    return s;
}
int toNumber(string s) {
    int n = 0;
    for (int i = 0; i < s.length(); i++) {
        if (s[i] < '0' || s[i] > '9')
            return -1;
        n = n * 10 + (s[i] - '0');
    }
    return n;
}
void saveParkings() {
    ofstream file("parking.txt");
    for (int i = 0; i < countParking; i++)
        file << parkings[i].id << "|" << parkings[i].name << "|" << parkings[i].location << "|" << parkings[i].totalSlots << "|" << parkings[i].availableSlots << "\n";
    file.close();
}
void loadParkings() {
    countParking = 0;
    ifstream file("parking.txt");
    string line;
    while (countParking < MAX && getline(file, line)) {
        size_t a = line.find('|');
        size_t b = line.find('|', a + 1);
        size_t c = line.find('|', b + 1);
        size_t d = line.find('|', c + 1);
        if (a == string::npos || b == string::npos || c == string::npos || d == string::npos)
            continue;
        Parking p;
        p.id = toNumber(line.substr(0, a));
        p.name = line.substr(a + 1, b - a - 1);
        p.location = line.substr(b + 1, c - b - 1);
        p.totalSlots = toNumber(line.substr(c + 1, d - c - 1));
        p.availableSlots = toNumber(line.substr(d + 1));
        if (p.id < 0 || p.totalSlots < 0 || p.availableSlots < 0 || p.availableSlots > p.totalSlots)
            continue;
        bool duplicate = false;
        for (int i = 0; i < countParking; i++) {
            if (parkings[i].id == p.id) {
                duplicate = true;
                break;
            }
        }
        if (!duplicate)
            parkings[countParking++] = p;
    }
    file.close();
}
void setupAdmin() {
    ifstream check("admin.txt");
    if (check.good()) {
        check.close();
        return;
    }
    check.close();
    string user, pass;
    cout << "Create Admin Username: ";
    cin >> user;
    cout << "Create Admin Password: ";
    cin >> pass;
    ofstream file("admin.txt");
    file << user << "\n" << pass;
    file.close();
    cout << "Admin account created successfully!\n";
}
bool login() {
    setupAdmin();
    string user, pass, savedUser, savedPass;
    ifstream file("admin.txt");
    if (!file) {
        cout << "Cannot open admin.txt!\n";
        return false;
    }
    getline(file, savedUser);
    getline(file, savedPass);
    file.close();
    cout << "\n===== ADMIN LOGIN =====\nUsername: ";
    cin >> user;
    cout << "Password: ";
    cin >> pass;
    if (user == savedUser && pass == savedPass) {
        cout << "Login successful!\n";
        return true;
    }
    cout << "Invalid username or password!\n";
    return false;
}
void showAllParkings() {
    if (countParking == 0) {
        cout << "No parking records found.\n";
        return;
    }
    cout << "\n===== PARKING DETAILS =====\n";
    for (int i = 0; i < countParking; i++)
        cout << "ID: " << parkings[i].id << " | Name: " << parkings[i].name << " | Location: " << parkings[i].location << " | Total Slots: " << parkings[i].totalSlots << " | Available Slots: " << parkings[i].availableSlots << "\n";
}
void searchCarDetails() {
    string number, line;
    cout << "Enter vehicle number: ";
    cin >> number;
    ifstream file("vehicle.txt");
    if (!file) {
        cout << "Vehicle records are not available.\n";
        return;
    }
    bool found = false;
    while (getline(file, line)) {
        size_t a = line.find('|');
        if (a == string::npos)
            continue;
        size_t b = line.find('|', a + 1);
        if (b == string::npos)
            continue;
        string vehicleNo = line.substr(0, a);
        if (lowerText(vehicleNo) == lowerText(number)) {
            cout << "Vehicle Number: " << vehicleNo << "\nVehicle Type: " << line.substr(a + 1, b - a - 1) << "\nParking ID: " << line.substr(b + 1) << "\n";
            found = true;
            break;
        }
    }
    file.close();
    if (!found)
        cout << "Vehicle not found.\n";
}
void viewParkings() {
    int choice;
    do {
        cout << "\n===== VIEW PARKING =====\n1. View All Parking Details\n2. Search Vehicle Details\n3. Back\nEnter choice: ";
        cin >> choice;
        if (choice == 1)
            showAllParkings();
        else if (choice == 2)
            searchCarDetails();
        else if (choice != 3)
            cout << "Invalid choice!\n";
    } while (choice != 3);
}
void updateParking() {
    int id, available;
    cout << "Enter Parking ID: ";
    cin >> id;
    for (int i = 0; i < countParking; i++) {
        if (parkings[i].id == id) {
            cout << "Parking: " << parkings[i].name << "\nTotal Slots: " << parkings[i].totalSlots << "\nAvailable Slots: " << parkings[i].availableSlots << "\nEnter new available slots: ";
            cin >> available;
            if (available < 0 || available > parkings[i].totalSlots) {
                cout << "Invalid slot count!\n";
                return;
            }
            parkings[i].availableSlots = available;
            saveParkings();
            cout << "Available slots updated successfully!\n";
            return;
        }
    }
    cout << "Parking ID not found.\n";
}
void manageDuplicates() {
    bool found = false;
    for (int i = 0; i < countParking; i++) {
        for (int j = 0; j < i; j++) {
            bool sameId = parkings[i].id == parkings[j].id;
            bool sameNameLocation = lowerText(parkings[i].name) == lowerText(parkings[j].name) && lowerText(parkings[i].location) == lowerText(parkings[j].location);
            if (sameId || sameNameLocation) {
                cout << "Possible duplicate: " << parkings[i].name << " | " << parkings[i].location << "\n";
                found = true;
                break;
            }
        }
    }
    if (!found)
        cout << "No duplicate parking records found.\n";
}
int main() {
    if (!login())
        return 0;
    loadParkings();
    int choice;
    do {
        cout << "\n===== ADMIN PANEL =====\n1. View Parking\n2. Update Available Slots\n3. Check Duplicates\n4. Exit\nEnter choice: ";
        cin >> choice;
        switch (choice) {
            case 1: viewParkings(); break;
            case 2: updateParking(); break;
            case 3: manageDuplicates(); break;
            case 4: cout << "Exiting Admin Panel...\n"; break;
            default: cout << "Invalid choice!\n";
        }
    } while (choice != 4);
    return 0;
}
