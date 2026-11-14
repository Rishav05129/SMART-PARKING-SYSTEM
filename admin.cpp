#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <ctime>
#include <cmath>
#include <cstdlib>
using namespace std;
const int MAX = 500;
struct Vehicle {
    string name, mobile, vehicleNo;
    double estHours, actHours, normalFee, overtimeFee, totalFee;
    bool exited;
    int month, year;
    Vehicle *next;
};
Vehicle *head = NULL;
string adminUser, adminPass;
double normalRate = 500, overtimeRate = 250;
double expAmount[MAX];
int expMonth[MAX], expYear[MAX], expCount = 0;
double getNumber() {
    double x;
    while (!(cin >> x)) {
        if (cin.eof()) exit(0);
        cin.clear();
        cin.ignore(1000, '\n');
        cout << "Invalid input! Try again: ";
    }
    cin.ignore(1000, '\n');
    return x;
}
void getNow(int &month, int &year) {
    time_t now = time(0);
    tm *t = localtime(&now);
    month = t->tm_mon + 1;
    year = t->tm_year + 1900;
}
void saveSettings() {
    ofstream f("admin.txt");
    f << adminUser << endl << adminPass << endl << normalRate << endl << overtimeRate << endl;
}
void freeList() {
    while (head != NULL) {
        Vehicle *t = head;
        head = head->next;
        delete t;
    }
}
void loadVehicles() {
    freeList();
    ifstream f("vehicles.txt");
    Vehicle *last = NULL;
    while (true) {
        Vehicle *v = new Vehicle;
        if (!(f >> v->vehicleNo >> v->mobile >> v->estHours >> v->actHours >> v->normalFee >> v->overtimeFee >> v->totalFee >> v->exited >> v->month >> v->year)) {
            delete v;
            break;
        }
        f.ignore();
        getline(f, v->name);
        v->next = NULL;
        if (last == NULL) head = v;
        else last->next = v;
        last = v;
    }
}
void loadExpenses() {
    ifstream e("expenses.txt");
    while (expCount < MAX && e >> expMonth[expCount] >> expYear[expCount] >> expAmount[expCount])
        expCount++;
}
void loadSettings() {
    ifstream f("admin.txt");
    if (!f) {
        cout << "\n===== ADMIN REGISTRATION =====\nCreate Username: ";
        cin >> adminUser;
        cout << "Create Password: ";
        cin >> adminPass;
        saveSettings();
        cout << "\nAdmin Account Created Successfully!\n";
        return;
    }
    f >> adminUser >> adminPass >> normalRate >> overtimeRate;
}
bool adminLogin() {
    string user, pass;
    cout << "\n===== ADMIN LOGIN =====\nEnter Username: ";
    cin >> user;
    cout << "Enter Password: ";
    cin >> pass;
    if (user == adminUser && pass == adminPass) {
        cout << "\nLogin Successful!\n";
        return true;
    }
    cout << "\nInvalid Username or Password!\n";
    return false;
}
void changeRate(string title, double &rate) {
    cout << "\nCurrent " << title << " Rate: Rs. " << rate << "\nEnter New " << title << " Rate: ";
    double r = getNumber();
    if (r < 0) {
        cout << "Invalid Rate!\n";
        return;
    }
    rate = r;
    saveSettings();
    cout << title << " Rate Updated Successfully!\n";
}
void printVehicle(Vehicle *t) {
    cout << fixed << setprecision(2);
    cout << "\nCustomer Name: " << t->name << "\nMobile Number: " << t->mobile << "\nVehicle Number: " << t->vehicleNo << endl;
    cout << "Estimated Hours: " << t->estHours << endl;
    if (t->exited) {
        cout << "Actual Hours: " << t->actHours << "\nTotal Fees: Rs. " << t->totalFee << endl;
        cout << "Exit Month/Year: " << t->month << "/" << t->year << "\nStatus: Exited\n";
    } else
        cout << "Status: Currently Parked\n";
    cout << "------------------------------\n";
}
void searchVehicle() {
    string number;
    cout << "\nEnter Vehicle Number: ";
    cin >> number;
    for (Vehicle *t = head; t != NULL; t = t->next) {
        if (t->vehicleNo == number) {
            cout << "\n===== VEHICLE DETAILS =====";
            printVehicle(t);
            return;
        }
    }
    cout << "Vehicle Not Found!\n";
}
void displayRecords() {
    if (head == NULL) {
        cout << "\nNo Vehicle Records Found!\n";
        return;
    }
    cout << "\n===== ALL VEHICLE RECORDS =====\n";
    for (Vehicle *t = head; t != NULL; t = t->next)
        printVehicle(t);
}
double revenueOf(int year, int month) {
    double sum = 0;
    for (Vehicle *t = head; t != NULL; t = t->next)
        if (t->exited && t->year == year && (month == 0 || t->month == month))
            sum += t->totalFee;
    return sum;
}
double expenseOf(int year, int month) {
    double sum = 0;
    for (int i = 0; i < expCount; i++)
        if (expYear[i] == year && (month == 0 || expMonth[i] == month))
            sum += expAmount[i];
    return sum;
}
void totalRevenue() {
    double revenue = 0, spent = 0;
    for (Vehicle *t = head; t != NULL; t = t->next)
        if (t->exited) revenue += t->totalFee;
    for (int i = 0; i < expCount; i++)
        spent += expAmount[i];
    double net = revenue - spent;
    cout << fixed << setprecision(2);
    cout << "\nTotal Revenue : Rs. " << revenue << "\nTotal Expenses: Rs. " << spent << endl;
    cout << (net >= 0 ? "NET PROFIT    : Rs. " : "NET LOSS      : Rs. ") << fabs(net) << endl;
}
void drawGraph(string label[], double rev[], double spent[], int n) {
    double maxValue = 1, totalRev = 0, totalExp = 0;
    for (int i = 0; i < n; i++)
        if (fabs(rev[i] - spent[i]) > maxValue) maxValue = fabs(rev[i] - spent[i]);
    cout << fixed << setprecision(2) << "\n'#' = Profit     '=' = Loss\n\n";
    for (int i = 0; i < n; i++) {
        double p = rev[i] - spent[i];
        int len = (int)(fabs(p) / maxValue * 40);
        if (len == 0 && p != 0) len = 1;
        cout << label[i] << " | " << string(len, p >= 0 ? '#' : '=') << (len > 0 ? " " : "");
        cout << (p >= 0 ? "Profit Rs. " : "Loss Rs. ") << fabs(p) << endl;
        totalRev += rev[i];
        totalExp += spent[i];
    }
    double net = totalRev - totalExp;
    cout << "\nTotal Revenue: Rs. " << totalRev << "\nTotal Expense: Rs. " << totalExp << endl;
    cout << (net >= 0 ? "Total Profit : Rs. " : "Total Loss   : Rs. ") << fabs(net) << endl;
}
void monthlyGraph() {
    string names[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    double rev[12], spent[12];
    cout << "\nEnter Year (e.g. 2026): ";
    int year = (int)getNumber();
    for (int m = 0; m < 12; m++) {
        rev[m] = revenueOf(year, m + 1);
        spent[m] = expenseOf(year, m + 1);
    }
    cout << "\n===== MONTHLY PROFIT / LOSS GRAPH (" << year << ") =====";
    drawGraph(names, rev, spent, 12);
}
void yearlyGraph() {
    int month, thisYear;
    string names[5];
    double rev[5], spent[5];
    getNow(month, thisYear);
    for (int i = 0; i < 5; i++) {
        int year = thisYear - 4 + i;
        names[i] = to_string(year);
        rev[i] = revenueOf(year, 0);
        spent[i] = expenseOf(year, 0);
    }
    cout << "\n===== YEARLY PROFIT / LOSS GRAPH (last 5 years) =====";
    drawGraph(names, rev, spent, 5);
}
void adminPanel() {
    int choice;
    if (!adminLogin()) return;
    do {
        cout << "\n===== ADMIN PANEL =====\n1. Change Normal Parking Rate\n2. Change Overtime Parking Rate\n"
             << "3. View Vehicle Records\n4. Search Vehicle\n5. View Revenue and Profit/Loss\n"
             << "6. Monthly Profit/Loss Graph\n7. Yearly Profit/Loss Graph\n8. Logout\nEnter Your Choice: ";
        choice = (int)getNumber();
        loadVehicles();
        switch (choice) {
            case 1: changeRate("Normal", normalRate); break;
            case 2: changeRate("Overtime", overtimeRate); break;
            case 3: displayRecords(); break;
            case 4: searchVehicle(); break;
            case 5: totalRevenue(); break;
            case 6: monthlyGraph(); break;
            case 7: yearlyGraph(); break;
            case 8: cout << "Admin Logged Out!\n"; break;
            default: cout << "Invalid Choice!\n";
        }
    } while (choice != 8);
}
int main() {
    int choice;
    loadSettings();
    loadExpenses();
    cout << "\n===== PARKING MANAGEMENT SYSTEM (ADMIN) =====\n";
    do {
        cout << "\n1. Admin Login\n2. Exit Program\nEnter Your Choice: ";
        choice = (int)getNumber();
        switch (choice) {
            case 1: adminPanel(); break;
            case 2: cout << "Thank You!\n"; break;
            default: cout << "Invalid Choice!\n";
        }
    } while (choice != 2);
    freeList();
    return 0;
}
