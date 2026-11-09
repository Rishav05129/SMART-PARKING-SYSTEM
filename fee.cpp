#include <iostream>
#include <string>
#include <iomanip>
using namespace std;

struct Vehicle
{
    string vehicleNo;
    double estimatedHours, actualHours;
    double normalFee, overtimeHours;
    double overtimeFee, totalFee;
    bool exited;
    Vehicle *next;
};

Vehicle *head = NULL;
const double NORMAL_RATE = 50;
double overtimeRate;

void vehicleEntry()
{
    Vehicle *newNode = new Vehicle;

    cout << "Enter Vehicle Number: ";
    cin >> newNode->vehicleNo;

    Vehicle *temp = head;

    while (temp != NULL)
    {
        if (temp->vehicleNo == newNode->vehicleNo && !temp->exited)
        {
            cout << "Vehicle Already Parked!\n";
            delete newNode;
            return;
        }
        temp = temp->next;
    }

    cout << "Enter Estimated Parking Hours: ";
    cin >> newNode->estimatedHours;

    if (newNode->estimatedHours <= 0)
    {
        cout << "Invalid Parking Duration!\n";
        delete newNode;
        return;
    }

    newNode->actualHours = 0;
    newNode->normalFee = 0;
    newNode->overtimeHours = 0;
    newNode->overtimeFee = 0;
    newNode->totalFee = 0;
    newNode->exited = false;

    newNode->next = head;
    head = newNode;

    cout << "Vehicle Entry Successful!\n";
}

void vehicleExit()
{
    string number;

    cout << "Enter Vehicle Number: ";
    cin >> number;

    Vehicle *temp = head;

    while (temp != NULL)
    {
        if (temp->vehicleNo == number && !temp->exited)
        {
            cout << "Enter Actual Parking Hours: ";
            cin >> temp->actualHours;

            if (temp->actualHours <= 0)
            {
                cout << "Invalid Duration!\n";
                return;
            }

            double normalHours = min(temp->actualHours, temp->estimatedHours);

            temp->normalFee = normalHours * NORMAL_RATE;

            if (temp->actualHours > temp->estimatedHours)
            {
                temp->overtimeHours = temp->actualHours - temp->estimatedHours;
                temp->overtimeFee = temp->overtimeHours * overtimeRate;
            }

            temp->totalFee = temp->normalFee + temp->overtimeFee;
            temp->exited = true;

            cout << fixed << setprecision(2);
            cout << "\n========== FEES COLLECTION DETAILS ==========\n";
            cout << "Vehicle No: " << temp->vehicleNo << endl;
            cout << "Estimated Parking: " << temp->estimatedHours << " Hours\n";
            cout << "Actual Parking: " << temp->actualHours << " Hours\n";
            cout << "Normal Parking Fees: Rs. " << temp->normalFee << endl;
            cout << "Overtime Duration: " << temp->overtimeHours << " Hours\n";
            cout << "Overtime Rate: Rs. " << overtimeRate << "/hour\n";
            cout << "Overtime Charges: Rs. " << temp->overtimeFee << endl;
            cout << "Total Fees: Rs. " << temp->totalFee << endl;
            cout << "=============================================\n";

            return;
        }

        temp = temp->next;
    }

    cout << "Vehicle Not Found!\n";
}

void searchVehicle()
{
    string number;

    cout << "Enter Vehicle Number: ";
    cin >> number;

    Vehicle *temp = head;

    while (temp != NULL)
    {
        if (temp->vehicleNo == number)
        {
            cout << "\nVehicle Number: " << temp->vehicleNo << endl;
            cout << "Estimated Hours: " << temp->estimatedHours << endl;

            if (temp->exited)
            {
                cout << "Status: Exited\n";
                cout << "Total Fees: Rs. " << temp->totalFee << endl;
            }
            else
            {
                cout << "Status: Currently Parked\n";
            }

            return;
        }
        temp = temp->next;
    }

    cout << "Vehicle Not Found!\n";
}

void displayRecords()
{
    Vehicle *temp = head;

    if (temp == NULL)
    {
        cout << "No Vehicle Records Available!\n";
        return;
    }

    while (temp != NULL)
    {
        cout << "\nVehicle No: " << temp->vehicleNo << endl;
        cout << "Estimated Hours: " << temp->estimatedHours << endl;

        if (temp->exited)
        {
            cout << "Actual Hours: " << temp->actualHours << endl;
            cout << "Normal Fees: Rs. " << temp->normalFee << endl;
            cout << "Overtime Hours: " << temp->overtimeHours << endl;
            cout << "Overtime Fees: Rs. " << temp->overtimeFee << endl;
            cout << "Total Fees: Rs. " << temp->totalFee << endl;
            cout << "Status: Exited\n";
        }
        else
        {
            cout << "Status: Currently Parked\n";
        }

        temp = temp->next;
    }
}

int main()
{
    int choice;

    cout << "PARKING MANAGEMENT SYSTEM\n";
    cout << "Normal Parking Rate: Rs. 50/hour\n";
    cout << "Enter Overtime Rate per Hour: Rs. ";
    cin >> overtimeRate;

    if (overtimeRate < 0)
    {
        cout << "Invalid Overtime Rate!\n";
        return 0;
    }

    do
    {
        cout << "\n1. Vehicle Entry\n";
        cout << "2. Vehicle Exit and Fee Collection\n";
        cout << "3. Search Vehicle\n";
        cout << "4. Display All Records\n";
        cout << "5. Exit\n";
        cout << "Enter Your Choice: ";
        cin >> choice;

        switch (choice)
        {
            case 1:
                vehicleEntry();
                break;
            case 2:
                vehicleExit();
                break;
            case 3:
                searchVehicle();
                break;
            case 4:
                displayRecords();
                break;
            case 5:
                cout << "Thank You!\n";
                break;
            default:
                cout << "Invalid Choice!\n";
        }
    }
    while (choice != 5);

    while (head != NULL)
    {
        Vehicle *temp = head;
        head = head->next;
        delete temp;
    }

    return 0;
}