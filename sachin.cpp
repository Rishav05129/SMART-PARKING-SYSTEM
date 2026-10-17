#include <iostream>
#include <string>
using namespace std;

const int TOTAL_SLOTS = 10;

string names[TOTAL_SLOTS];
string vehicle_types[TOTAL_SLOTS];
string vehicle_numbers[TOTAL_SLOTS];

bool occupied[TOTAL_SLOTS] = {false};

void vehicle_entry_book();
void vehicle_exit();
void show_parking();
void search_vehicle();

int main(){
    int choice = -1;
    while(choice!=0){
        cout<<"\n....SMART PARKING MANAGEMENT....\n";
        cout<<"1: Vehicle Entry & Slot Booking \n";
        cout<<"2: Vehicle Exit\n";
        cout<<"3: Show Parking Availability\n";
        cout<<"4: Search Vehicle\n";
        cout<<"0: Exit\n";

        cout << "Enter your choice: ";
        cin >> choice;
        switch(choice){
            case 1: vehicle_entry_book(); break;
            case 2: vehicle_exit(); break;
            case 3: show_parking(); break;
            case 4: search_vehicle(); break;
            case 0: cout<<"Program exited successfully...\n"; break;
            default: cout<<"Invalid choice.. Try again.\n";
        }
    }
    return 0;
}


void search_vehicle(){
    string search_no;
    bool found = false;
    cout<< "Enter vehicle number: ";
    cin>>search_no;

    for(int i=0; i<TOTAL_SLOTS; i++){
        if(occupied[i] && vehicle_numbers[i] == search_no){
            cout<<"Vehicle found!\n";
            cout<<"Name: "<< names[i]<<endl;
            cout<<"Type: "<< vehicle_types[i]<<endl;
            cout<<"Vehicle Number: "<< vehicle_numbers[i]<<endl;
            cout<<"Slot: " << i+1 <<endl;
            found = true;
            }
        }
        if(!found){
            cout << "Vehicle not found in parking.\n";
            }
    }

void show_parking(){    
    int available = 0;

    for(int i=0; i<TOTAL_SLOTS; i++){
        cout<<"Slot " << i+1 <<": ";
        if(occupied[i]){
            cout<<"Occupied by "<< vehicle_numbers[i] << endl;
            } 
            else{
                cout<<"Available\n";
                available++;
               }
            }
            cout<<"Available slots: "<< available <<"/" << TOTAL_SLOTS <<endl;
}

        //slot available function 
bool isSlotAvailable(int slot){
    for(int i=0; i < TOTAL_SLOTS; i++){
        if(occupied[i] && i+1 == slot){
            return false;
        }
    }
    return true;
}

void vehicle_entry_book(){
    int choice;
    string name, vehicle_type, vehicle_no;

    cout<<"\n... VEHICLE ENTRY ...\n";
    cout<<"Enter your name: ";
    getline(cin >> ws, name);

    cout<<"\nChoose vehicle type:\n";
    cout<<"1: Car\n2: EVs (Electric Vehicles)\n3: Bike\n4: Scooty\n";
    cout<< "Enter your choice: ";
    cin>>choice;

    switch(choice){
        case 1: vehicle_type = "Car"; break;
        case 2: vehicle_type = "EVs (Electric Vehicle)"; break;
        case 3: vehicle_type = "Bike"; break;
        case 4: vehicle_type = "Scooty"; break;
        default: cout<<"Invalid vehicle type!!\n";
        return;
    }

    cout<<"Enter vehicle number: ";
    cin>>vehicle_no;

            // Check whether the vehicle is already parked
    for(int i=0; i< TOTAL_SLOTS; i++){
        if(occupied[i] && vehicle_numbers[i] == vehicle_no){
            cout<<"This vehicle is already parked!\n";
            return;
        }
    }

    int available_slot = -1;
    for(int i=1; i <= TOTAL_SLOTS; i++){
        if(isSlotAvailable(i)){
        available_slot = i;
        break;
        }
    }

    if(available_slot==-1){
        cout<<"No slot is available for this time!!\n";
        return;
    }

            // Find the first available slot
    for (int i=0; i < TOTAL_SLOTS; i++) {
        if(!occupied[i]){
            names[i] = name;
            vehicle_types[i] = vehicle_type;
            vehicle_numbers[i] = vehicle_no;
            occupied[i] = true;

            cout<<"\nVehicle entry successful....\n";
            cout<<"Name: "<< names[i] <<endl;
            cout<<"Vehicle Type: "<< vehicle_types[i] <<endl;
            cout<<"Vehicle Number: "<< vehicle_numbers[i] <<endl;
            cout<<"Assigned Slot: "<< i+1 <<endl;

            return;
        }
        else{
        cout<<"Parking is full! NO SLOT is available.";
        }
    }
}

void vehicle_exit(){
    string re_enter_vehicle_no;
    int slot = -1;

    cout<<"\n... VEHICLE EXIT ...\n";
    cout<<"Enter vehicle number: ";
    cin>>re_enter_vehicle_no;
            // Search for the parked vehicle
    for(int i=0; i< TOTAL_SLOTS; i++){
        if(occupied[i] && vehicle_numbers[i] == re_enter_vehicle_no){
            slot = i;
            break;
        }
    }

    if(slot==-1){
        cout<<"Vehicle not found in parking!\n";
        return;
    }

    cout<<"\nVehicle found!\n";
    cout<<"Name: "<< names[slot] <<endl;
    cout<<"Vehicle Type: "<< vehicle_types[slot] <<endl;
    cout<<"Vehicle Number: "<< vehicle_numbers[slot] <<endl;
    cout<<"Parking Slot: "<< slot + 1 <<endl;

    int hours;
    const int rate = 100;

    cout<<"Enter parking duration in hours: ";
    cin>>hours;
    if(hours<=0){
        cout << "Invalid parking duration!\n";
        return;
    }

    int fee = hours*rate;
    cout<<"Parking Fee: Rs. " << fee <<endl;

            // Release the slot
    occupied[slot] = false;
    names[slot] = "";
    vehicle_types[slot] = "";
    vehicle_numbers[slot] = "";

    cout<<"Vehicle exit successful!"<<endl;
    cout<<"Parking slot released."<<endl;
}
