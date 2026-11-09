#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
using namespace std;
struct Vehicle
{
    string name, mobile, vehicleNo;
    double estimatedHours, actualHours;
    double normalFee, overtimeHours, overtimeFee, totalFee;
    bool exited;
    Vehicle *next;
};
Vehicle *head = NULL;
string adminUsername, adminPassword;
double normalRate = 50, overtimeRate = 50;
void saveSettings()
{
    ofstream file("admin.txt");
    file << adminUsername << endl;
    file << adminPassword << endl;
    file << normalRate << endl;
    file << overtimeRate << endl;
    file.close();
}
void adminRegistration()
{
    cout << "\n===== ADMIN REGISTRATION =====\n";
    cout << "Create Username: ";
    cin >> adminUsername;
    cout << "Create Password: ";
    cin >> adminPassword;
    saveSettings();
    cout << "\nAdmin Account Created Successfully!\n";
}
void loadSettings()
{
    ifstream file("admin.txt");
    if (!file)
    {
        adminRegistration();
        return;
    }
    file >> adminUsername >> adminPassword >> normalRate >> overtimeRate;
    file.close();
}
bool adminLogin()
{
    string username, password;
    cout << "\n===== ADMIN LOGIN =====\n";
    cout << "Enter Username: ";
    cin >> username;
    cout << "Enter Password: ";
    cin >> password;
    if (username == adminUsername && password == adminPassword)
    {
        cout << "\nLogin Successful!\n";
        return true;
    }
    cout << "\nInvalid Username or Password!\n";
    return false;
}
void changeNormalRate()
{
    double rate;
    cout << "\nCurrent Normal Rate: Rs. " << normalRate << endl;
    cout << "Enter New Normal Rate: ";
    cin >> rate;
    if (rate < 0)
    {
        cout << "Invalid Rate!\n";
        return;
    }
    normalRate = rate;
    saveSettings();
    cout << "Normal Rate Updated Successfully!\n";
}
void changeOvertimeRate()
{
    double rate;
    cout << "\nCurrent Overtime Rate: Rs. " << overtimeRate << endl;
    cout << "Enter New Overtime Rate: ";
    cin >> rate;
    if (rate < 0)
    {
        cout << "Invalid Rate!\n";
        return;
    }
    overtimeRate = rate;
    saveSettings();
    cout << "Overtime Rate Updated Successfully!\n";
}
void vehicleEntry()
{
    Vehicle *newNode = new Vehicle;
    cout << "\n===== VEHICLE ENTRY =====\n";
    cin.ignore();
    cout << "Enter Customer Name: ";
    getline(cin, newNode->name);
    cout << "Enter Mobile Number: ";
    cin >> newNode->mobile;
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
    cout << "\nVehicle Entry Successful!\n";
}
void vehicleExit()
{
    string number;
    cout << "\nEnter Vehicle Number: ";
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
                cout << "Invalid Parking Duration!\n";
                return;
            }
            double normalHours;
            if (temp->actualHours < temp->estimatedHours)
                normalHours = temp->actualHours;
            else
                normalHours = temp->estimatedHours;
            temp->normalFee = normalHours * normalRate;
            if (temp->actualHours > temp->estimatedHours)
            {
                temp->overtimeHours = temp->actualHours - temp->estimatedHours;
                temp->overtimeFee = temp->overtimeHours * overtimeRate;
            }
            temp->totalFee = temp->normalFee + temp->overtimeFee;
            temp->exited = true;
            cout << fixed << setprecision(2);
            cout << "\n===== FEES COLLECTION DETAILS =====\n";
            cout << "Customer Name: " << temp->name << endl;
            cout << "Mobile Number: " << temp->mobile << endl;
            cout << "Vehicle Number: " << temp->vehicleNo << endl;
            cout << "Estimated Hours: " << temp->estimatedHours << endl;
            cout << "Actual Hours: " << temp->actualHours << endl;
            cout << "Normal Parking Fees: Rs. " << temp->normalFee << endl;
            cout << "Overtime Hours: " << temp->overtimeHours << endl;
            cout << "Overtime Charges: Rs. " << temp->overtimeFee << endl;
            cout << "----------------------------------\n";
            cout << "TOTAL REVENUE: Rs. " << temp->totalFee << endl;
            cout << "==================================\n";
            return;
        }
        temp = temp->next;
    }
    cout << "Vehicle Not Found or Already Exited!\n";
}
void searchVehicle()
{
    string number;
    cout << "\nEnter Vehicle Number: ";
    cin >> number;
    Vehicle *temp = head;
    while (temp != NULL)
    {
        if (temp->vehicleNo == number)
        {
            cout << "\n===== VEHICLE DETAILS =====\n";
            cout << "Customer Name: " << temp->name << endl;
            cout << "Mobile Number: " << temp->mobile << endl;
            cout << "Vehicle Number: " << temp->vehicleNo << endl;
            cout << "Estimated Hours: " << temp->estimatedHours << endl;
            if (temp->exited)
            {
                cout << "Actual Hours: " << temp->actualHours << endl;
                cout << "Total Fees: Rs. " << temp->totalFee << endl;
                cout << "Status: Exited\n";
            }
            else
                cout << "Status: Currently Parked\n";
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
        cout << "\nNo Vehicle Records Available!\n";
        return;
    }
    cout << "\n===== ALL VEHICLE RECORDS =====\n";
    while (temp != NULL)
    {
        cout << "\nCustomer Name: " << temp->name << endl;
        cout << "Mobile Number: " << temp->mobile << endl;
        cout << "Vehicle Number: " << temp->vehicleNo << endl;
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
            cout << "Status: Currently Parked\n";
        cout << "------------------------------\n";
        temp = temp->next;
    }
}
void totalRevenue()
{
    double revenue = 0;
    Vehicle *temp = head;
    while (temp != NULL)
    {
        if (temp->exited)
            revenue += temp->totalFee;
        temp = temp->next;
    }
    cout << fixed << setprecision(2);
    cout << "\nTotal Revenue: Rs. " << revenue << endl;
}
void displayParkedVehicles()
{
    Vehicle *temp = head;
    bool found = false;
    cout << "\n===== CURRENTLY PARKED VEHICLES =====\n";
    while (temp != NULL)
    {
        if (!temp->exited)
        {
            found = true;
            cout << "\nCustomer Name: " << temp->name << endl;
            cout << "Mobile Number: " << temp->mobile << endl;
            cout << "Vehicle Number: " << temp->vehicleNo << endl;
            cout << "Estimated Hours: " << temp->estimatedHours << endl;
        }
        temp = temp->next;
    }
    if (!found)
        cout << "No Vehicles Currently Parked!\n";
}
void adminPanel()
{
    int choice;
    if (!adminLogin())
        return;
    do
    {
        cout << "\n===== ADMIN PANEL =====\n";
        cout << "1. Change Normal Parking Rate\n";
        cout << "2. Change Overtime Parking Rate\n";
        cout << "3. View All Vehicle Records\n";
        cout << "4. Search Vehicle\n";
        cout << "5. View Total Revenue\n";
        cout << "6. View Currently Parked Vehicles\n";
        cout << "7. Logout\n";
        cout << "Enter Your Choice: ";
        cin >> choice;
        switch (choice)
        {
            case 1: changeNormalRate(); break;
            case 2: changeOvertimeRate(); break;
            case 3: displayRecords(); break;
            case 4: searchVehicle(); break;
            case 5: totalRevenue(); break;
            case 6: displayParkedVehicles(); break;
            case 7: cout << "Admin Logged Out!\n"; break;
            default: cout << "Invalid Choice!\n";
        }
    } while (choice != 7);
}
int main()
{
    int choice;
    loadSettings();
    cout << "\n===== PARKING MANAGEMENT SYSTEM =====\n";
    do
    {
        cout << "\n1. Vehicle Entry\n";
        cout << "2. Vehicle Exit and Fee Collection\n";
        cout << "3. Admin Login\n";
        cout << "4. Exit Program\n";
        cout << "Enter Your Choice: ";
        cin >> choice;
        switch (choice)
        {
            case 1: vehicleEntry(); break;
            case 2: vehicleExit(); break;
            case 3: adminPanel(); break;
            case 4: cout << "Thank You!\n"; break;
            default: cout << "Invalid Choice!\n";
        }
    } while (choice != 4);
    while (head != NULL)
    {
        Vehicle *temp = head;
        head = head->next;
        delete temp;
    }
    return 0;
}
