#include <iostream>
using namespace std;
class Parking
{
public:
    string vehicle;
    int slot, hours;
    float fee;
    void input()
    {
        cout << "Enter Vehicle Number: ";
        cin >> vehicle;
        cout << "Enter Slot Number: ";
        cin >> slot;
        cout << "Enter Parking Hours: ";
        cin >> hours;
    }
    void calculateFee()
    {
        fee = 30 + (hours - 1) * 20;
        if (hours <= 0)
            fee = 0;
    }
    void display()
    {
        cout << "\n===== PARKING DETAILS =====\n";
        cout << "Vehicle: " << vehicle << endl;
        cout << "Slot: " << slot << endl;
        cout << "Hours: " << hours << endl;
        cout << "Fee: Rs. " << fee << endl;
    }
};

int main()
{
    Parking p;
    cout << "===== SMART PARKING =====\n";
    p.input();
    p.calculateFee();
    p.display();
    return 0;
}
