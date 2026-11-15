#include <string>
using namespace std;
const double NORMAL_RATE = 50;
double overtimeRate;
struct FeeNode
{
    string vehicleNo;
    double bookedHours, actualHours;
    double onlineFee, overtimeHours;
    double overtimeFee, totalFee;
    FeeNode *next;
};
FeeNode *feeHead = NULL;
double calculateOnlineFee(double bookedHours)
{
    return bookedHours * NORMAL_RATE;
}
double calculateOvertimeHours(double bookedHours, double actualHours)
{
    if (actualHours > bookedHours)
        return actualHours - bookedHours;
    return 0;
}
double calculateOvertimeFee(double overtimeHours)
{
    return overtimeHours * overtimeRate;
}
double calculateTotalFee(double bookedHours, double actualHours)
{
    double overtimeHours = calculateOvertimeHours(bookedHours, actualHours);
    return calculateOnlineFee(bookedHours) + calculateOvertimeFee(overtimeHours);
}
FeeNode *addFeeRecord(string vehicleNo, double bookedHours, double actualHours)
{
    FeeNode *newNode = new FeeNode;
    newNode->vehicleNo = vehicleNo;
    newNode->bookedHours = bookedHours;
    newNode->actualHours = actualHours;
    newNode->onlineFee = calculateOnlineFee(bookedHours);
    newNode->overtimeHours = calculateOvertimeHours(bookedHours, actualHours);
    newNode->overtimeFee = calculateOvertimeFee(newNode->overtimeHours);
    newNode->totalFee = newNode->onlineFee + newNode->overtimeFee;
    newNode->next = feeHead;
    feeHead = newNode;
    return newNode;
}
FeeNode *findFeeRecord(string vehicleNo)
{
    FeeNode *temp = feeHead;
    while (temp != NULL)
    {
        if (temp->vehicleNo == vehicleNo)
            return temp;
        temp = temp->next;
    }
    return NULL;
}
bool removeFeeRecord(string vehicleNo)
{
    FeeNode *temp = feeHead;
    FeeNode *prev = NULL;
    while (temp != NULL)
    {
        if (temp->vehicleNo == vehicleNo)
        {
            if (prev == NULL)
                feeHead = temp->next;
            else
                prev->next = temp->next;
            delete temp;
            return true;
        }
        prev = temp;
        temp = temp->next;
    }
    return false;
}
double totalCollection()
{
    double sum = 0;
    FeeNode *temp = feeHead;
    while (temp != NULL)
    {
        sum += temp->totalFee;
        temp = temp->next;
    }
    return sum;
}
void clearFeeRecords()
{
    while (feeHead != NULL)
    {
        FeeNode *temp = feeHead;
        feeHead = feeHead->next;
        delete temp;
    }
}
