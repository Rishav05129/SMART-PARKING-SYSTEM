#include <iostream>
#include <string>
#include <list>
#include <fstream>
#include <iomanip>
using namespace std;

const int TOTAL_SLOTS = 100;
        // car = 40
        // EV = 20
        // Scooty = 20
        // Bike = 20
const string DATA_FILE = "parking_data.txt";

            // Class : Stores details of vehicles.
class Vehicle {
private:
    string name;
    string type;
    string number;
    int slot;

public:
    Vehicle(string n, string t, string num, int s) {           // constructor 
        name = n;
        type = t;
        number = num;
        slot = s;
    }

    string getName(){
        return name;
    }

    string getType(){
        return type;
    }

    string getNumber(){
        return number;
    }

    int getSlot(){
        return slot;
    }

    void displayDetails() const {           // all details 
        cout << "Name: " << name << '\n';
        cout << "Vehicle Type: " << type << '\n';
        cout << "Vehicle Number: " << number << '\n';
        cout << "Parking Slot: " << slot << '\n';
    }
};

            // Class: Manages parking slots and parked vehicles.
class ParkingSystem {
private:
    bool parkingSlots[TOTAL_SLOTS];
    list<Vehicle> vehicles;


    void readData(){            //file se data read karna
    ifstream file(DATA_FILE);

    int slot;
    string name, type, number;

    while(file >> slot >> quoted(name) >> quoted(type) >> quoted(number)){
        int start, end;
        if(getSlotRange(type, start, end) && slot >= start && slot <= end && !parkingSlots[slot - 1] && !vehicleExists(number)){
            Vehicle v(name, type, number, slot);
            vehicles.push_back(v);
            parkingSlots[slot - 1] = true;
        }
    }
    file.close();
}


    void saveData() {       //file me data write 
        ofstream file(DATA_FILE);

        for(Vehicle& v : vehicles) {
            file << v.getSlot() << ' '
                 << quoted(v.getName()) << ' '
                 << quoted(v.getType()) << ' '
                 << quoted(v.getNumber()) << '\n';
        }
    }


    int findFreeSlot() {        //free slot check karna 
        for(int i = 0; i < TOTAL_SLOTS; i++) {
            if (!parkingSlots[i]) {
                return i;
            }
        }
        return -1;
    }


    bool getSlotRange(string type, int &start, int &end){          // slot numbers 
    if(type == "Car"){
        start = 1;
        end = 40;
    }
    else if(type == "EV") {
        start = 41;
        end = 60;
    }
    else if(type == "Scooty") {
        start = 61;
        end = 80;
    }
    else if(type == "Bike") {
        start = 81;
        end = 100;
    }
    else{
        return false;
    }
    return true;
}

int findFreeSlot(string type) {
    int start, end;
    if(!getSlotRange(type, start, end)) {
        return -1;
    }
    for(int i=start; i<=end; i++){
        if(!parkingSlots[i - 1]){
            return i;
        }
    }
    return -1;
}


    bool vehicleExists(string number) {         //check vehicle exists our parking or not 
        for (Vehicle& v : vehicles) {
            if (v.getNumber() == number) {
                return true;
            }
        }
        return false;
    }


    void vehicleEntry() {           // vehicle entry 
        string name, type, number;
        int vehicle_type;

        cout << "\n--- VEHICLE ENTRY ---\n";
        cout << "Enter your name: ";
        getline(cin >> ws, name);

        cout << "\n1. Car\n2. EV\n3. Bike\n4. Scooty\n";
        cout << "Choose vehicle type: ";
        cin >> vehicle_type;

        switch (vehicle_type){
            case 1: type = "Car"; break;
            case 2: type = "EV"; break;
            case 3: type = "Bike"; break;
            case 4: type = "Scooty"; break;
            default:
                cout << "Invalid vehicle type!\n";
                return;
        }

        cout << "Enter vehicle number: ";
        cin >> number;

        if(vehicleExists(number)) {
            cout << "This vehicle is already parked!\n";
            return;
        }

        int assignedSlot = findFreeSlot(type);
        if (assignedSlot == -1) {
            cout << type <<"Parking is full!\n";
            return;
        }

        Vehicle v(name, type, number, assignedSlot);

        parkingSlots[assignedSlot-1] = true;
        vehicles.push_back(v);
        saveData();

        cout << "\nVehicle entry successful!\n";
        v.displayDetails();
    }


    void vehicleExit() {            // vehicle exit 
        string number;

        cout << "\n--- VEHICLE EXIT ---\n";
        cout << "Enter vehicle number: ";
        cin >> number;

        for (auto it = vehicles.begin(); it != vehicles.end(); it++) {
            if (it->getNumber() == number) {
                it->displayDetails();
                parkingSlots[it->getSlot() - 1] = false;
                vehicles.erase(it);
                saveData();

                cout << "Vehicle exit successful!\n";
                cout << "Parking slot released.\n";
                return;
            }
        }
        cout << "Vehicle not found!\n";
    }


    void showParking(){         // show parking status 
    int car = 0, bike = 0, ev = 0, scooty = 0;

    for(Vehicle& v : vehicles) {
        if (v.getType() == "Car")
            car++;
        else if (v.getType() == "Bike")
            bike++;
        else if (v.getType() == "EV")
            ev++;
        else if (v.getType() == "Scooty")
            scooty++;
    }

    cout << "\n.... PARKING AVAILABILITY ....\n";
    cout << "Occupied Car: " << car<< "/40" << " :  Available: " << 40-car << '\n';
    cout << "Occupied EV: " << ev << "/20" << " :  Available: " << 20-ev << '\n';
    cout << "Occupied Scooty: " << scooty << "/20" << " :  Available: " << 20-scooty << '\n';
    cout << "Occupied Bike: " << bike << "/20"<< " :  Available: " << 20-bike << '\n';

    cout << "\nTotal Available: " << TOTAL_SLOTS - vehicles.size() << "/100\n";
}


void searchVehicle() {          // vehicle search by number or name 
    string search;

    cout << "\nEnter vehicle number / Name: ";
    getline(cin >> ws, search);

    bool found = false;
    for(Vehicle& v : vehicles){
        if(v.getNumber() == search || v.getName() == search) {
            cout << "\nVehicle found!\n";
            v.displayDetails();
            found = true;
        }
    }
    if(!found){
        cout << "Vehicle not found!\n";
    }
}

public:
    ParkingSystem() {           // construstor 
        for (int i = 0; i < TOTAL_SLOTS; i++) {
            parkingSlots[i] = false;
        }
        readData();
    }

    void run() {            // object call this. main work 
        int choice;
        do {
            cout << "\n--- SMART PARKING MANAGEMENT ---\n";
            cout << "1. Vehicle Entry\n";
            cout << "2. Vehicle Exit\n";
            cout << "3. Show Parking Availability\n";
            cout << "4. Search Vehicle\n";
            cout << "0. Exit\n";
            cout << "Enter your choice: ";
            cin >> choice;

            switch (choice){
                case 1: vehicleEntry(); break;
                case 2: vehicleExit(); break;
                case 3: showParking(); break;
                case 4: searchVehicle(); break;
                case 0: saveData();
                    cout << "Program exited successfully.\n"; break;
                default: cout << "Invalid choice!\n";
            }
        } while (choice != 0);
    }
};

int main() {
    ParkingSystem obj;
    obj.run();
    return 0;
}
