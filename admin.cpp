#include <iostream>
#include <string>
using namespace std;
#define MAX 10
struct Vehicle {
    string number;
    string type;
};
struct Slot {
    int id;
    bool occupied;
    Vehicle vehicle;
};

Slot slots[MAX];
void viewSlots() {
    cout << "\n===== PARKING SLOTS =====\n";
    for (int i = 0; i < MAX; i++) {
        cout << "Slot " << slots[i].id << " : ";
        if (slots[i].occupied)
            cout << "Occupied - " << slots[i].vehicle.number << endl;
        else
            cout << "Available\n";
    }
}
void viewVehicles() {
    cout << "\n===== PARKED VEHICLES =====\n";
    bool found = false;
    for (int i = 0; i < MAX; i++) {
        if (slots[i].occupied) {
            cout << "Vehicle: " << slots[i].vehicle.number
                 << " | Type: " << slots[i].vehicle.type
                 << " | Slot: " << slots[i].id << endl;
            found = true;
        }
    }
    if (!found)
        cout << "No vehicles parked.\n";
}
void adminMenu() {
    int choice;
    while (true) {
        cout << "\n========== ADMIN PANEL ==========\n";
        cout << "1. View Parking Slots\n";
        cout << "2. View Parked Vehicles\n";
        cout << "3. Logout\n";
        cout << "Enter Choice: ";
        cin >> choice;
        switch (choice) {
        case 1:
            viewSlots();
            break;
        case 2:
            viewVehicles();
            break;
        case 3:
            cout << "Admin Logged Out.\n";
            return;
        default:
            cout << "Invalid Choice!\n";
        }
    }
}
void adminLogin() {
    string username, password;
    cout << "\n========== ADMIN LOGIN ==========\n";
    cout << "Username: ";
    cin >> username;
    cout << "Password: ";
    cin >> password;
    if (username == "admin" && password == "1234") {
        cout << "\nLogin Successful!\n";
        adminMenu();
    }
    else {
        cout << "\nInvalid Username or Password!\n";
    }
}
int main() {
    for (int i = 0; i < MAX; i++) {
        slots[i].id = i + 1;
        slots[i].occupied = false;
    }
    slots[0].occupied = true;
    slots[0].vehicle.number = "UK07AB1234";
    slots[0].vehicle.type = "Car";
    slots[2].occupied = true;
    slots[2].vehicle.number = "UK08CD5678";
    slots[2].vehicle.type = "Bike";
    adminLogin();
    return 0;
}
