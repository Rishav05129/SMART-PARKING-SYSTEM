#include <iostream>
#include <string>
using namespace std;

int main()
{
    string vehicle[5] = {"UK07AB1234", "UK07CD5678", "", "UK07EF9012", ""};
    bool occupied[5] = {true, true, false, true, false};

    int choice;

    cout << "===== SMART PARKING =====\n";
    cout << "1. Search Vehicle\n";
    cout << "2. Parking Report\n";
    cout << "Enter Choice: ";
    cin >> choice;

    if (choice == 1)
    {
        string search;
        bool found = false;

        cout << "Enter Vehicle Number: ";
        cin >> search;

        for (int i = 0; i < 5; i++)
        {
            if (occupied[i] && vehicle[i] == search)
            {
                cout << "\nVehicle Found!\n";
                cout << "Vehicle Number: " << vehicle[i] << endl;
                cout << "Slot Number: " << i + 1 << endl;
                found = true;
                break;
            }
        }

        if (!found)
            cout << "Vehicle Not Found!\n";
    }

    else if (choice == 2)
    {
        int occupiedSlots = 0;

        cout << "\n===== PARKING REPORT =====\n";

        for (int i = 0; i < 5; i++)
        {
            if (occupied[i])
            {
                cout << "Slot " << i + 1
                     << " : Occupied - "
                     << vehicle[i] << endl;
                occupiedSlots++;
            }
            else
            {
                cout << "Slot " << i + 1
                     << " : Available" << endl;
            }
        }

        cout << "\nTotal Slots: 5";
        cout << "\nOccupied Slots: " << occupiedSlots;
        cout << "\nAvailable Slots: "
             << 5 - occupiedSlots << endl;
    }

    else
    {
        cout << "Invalid Choice!";
    }

    return 0;
}