#include <iostream>
#include <string>
#include <iomanip>
using namespace std;

struct Parking {
    string name;
    int total;
    int available;
};

int main() {

    Parking p[40] = {
        {"City Mall",50,20},
        {"Metro Parking",30,15},
        {"Parade Ground",100,45},
        {"Tibetan Market",40,18},
        {"Ashley Hall",60,25},
        {"ISBT Parking",80,30},
        {"Railway Station",70,22},
        {"Clock Tower",50,12},
        {"Rajpur Road",60,35},
        {"Dharampur",50,20},
        {"Ballupur",40,15},
        {"Prem Nagar",50,25},
        {"Patel Nagar",60,30},
        {"GMS Road",50,20},
        {"EC Road",40,15},
        {"Dalanwala",30,10},
        {"Race Course",50,20},
        {"Transport Nagar",80,35},
        {"Saharanpur Road",60,25},
        {"Chakrata Road",50,20},
        {"Dilaram Chowk",40,18},
        {"Kishan Nagar",30,12},
        {"Raipur Road",60,25},
        {"Jogiwala",40,15},
        {"Mothrowala",30,10},
        {"Rispana Bridge",50,20},
        {"Prince Chowk",40,15},
        {"Darshan Lal Chowk",30,10},
        {"Bindal Bridge",40,15},
        {"Sahastradhara Road",60,25},
        {"Survey Chowk",40,18},
        {"Kabul House",30,12},
        {"Kanak Chowk",40,15},
        {"Rangers Ground",50,20},
        {"Niranjanpur",40,15},
        {"Selaqui",60,25},
        {"Sahastradhara Crossing",30,10},
        {"Old Bus Stand",50,20},
        {"Vasant Vihar",40,15},
        {"Subhash Road",30,12}
    };

    int choice;

    cout << "\n============================================\n";
    cout << "       DEHRADUN SMART PARKING SYSTEM\n";
    cout << "============================================\n";

    cout << "\n1. Vehicle Registration";
    cout << "\n2. Vehicle Exit";
    cout << "\n3. Smart Parking Dashboard";
    cout << "\n4. Exit";

    cout << "\n\nSelect Your Service: ";
    cin >> choice;

    if (choice == 3) {

        int totalSlots = 0;
        int availableSlots = 0;

        cout << "\n";
        cout << "============================================\n";
        cout << "          SMART PARKING DASHBOARD\n";
        cout << "              DEHRADUN CITY\n";
        cout << "============================================\n";

        for (int i = 0; i < 40; i++) {

            int occupied = p[i].total - p[i].available;

            totalSlots += p[i].total;
            availableSlots += p[i].available;

            double percentage =
                (p[i].available * 100.0) / p[i].total;

            string status;

            if (p[i].available == 0)
                status = "FULL";
            else if (percentage < 30)
                status = "LIMITED";
            else
                status = "AVAILABLE";

            cout << "\n--------------------------------------------\n";

            cout << "PARKING ID       : DD-" << setw(3)
                 << setfill('0') << i + 1 << setfill(' ') << endl;

            cout << "LOCATION         : " << p[i].name << endl;

            cout << "TOTAL CAPACITY   : " << p[i].total << endl;

            cout << "OCCUPIED SLOTS   : " << occupied << endl;

            cout << "FREE SLOTS       : " << p[i].available << endl;

            cout << "AVAILABILITY     : "
                 << fixed << setprecision(1)
                 << percentage << "%" << endl;

            cout << "PARKING STATUS   : " << status << endl;
        }

        cout << "\n============================================\n";
        cout << "          CITY PARKING SUMMARY\n";
        cout << "============================================\n";

        cout << "TOTAL LOCATIONS   : 40\n";
        cout << "TOTAL SLOTS       : " << totalSlots << endl;
        cout << "OCCUPIED SLOTS    : "
             << totalSlots - availableSlots << endl;
        cout << "AVAILABLE SLOTS   : " << availableSlots << endl;

        cout << "============================================\n";
        cout << "       DEHRADUN SMART PARKING SYSTEM\n";
        cout << "============================================\n";
    }

    else if (choice == 1) {
        cout << "\nVehicle Registration Selected!\n";
    }

    else if (choice == 2) {
        cout << "\nVehicle Exit Selected!\n";
    }

    else if (choice == 4) {
        cout << "\nThank You For Using Smart Parking!\n";
    }

    else {
        cout << "\nInvalid Service Selection!\n";
    }

    return 0;
}