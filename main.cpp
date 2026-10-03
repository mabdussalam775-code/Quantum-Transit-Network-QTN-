#include <iostream>
#include <ctime>
#include <cstdlib>

#include "airline.h"
#include "RailwayLine.h"
#include "busService.h"

using namespace std;

// Function declaration
void BusServiceMenu();

// ================= HUMAN VERIFICATION =================
bool humanVerification()
{
    srand(time(0));

    int attempts = 3;

    while(attempts--)
    {
        int num1 = rand() % 20 + 1;
        int num2 = rand() % 20 + 1;

        int answer;

        B_setColor(14);

        cout << "\n============================================================\n";
        cout << "                 HUMAN VERIFICATION SYSTEM                  \n";
        cout << "============================================================\n";

        B_setColor(7);

        cout << "Attempts Left: " << attempts + 1 << endl;
        cout << "\nSolve the following question:\n";
        cout << num1 << " + " << num2 << " = ";
        cin >> answer;

        if(answer == (num1 + num2))
        {
            B_setColor(10);
            cout << "\nVerification Successful Welcome in QTN!\n";
            B_setColor(7);

            return true;
        }
        else
        {
            B_setColor(12);
            cout << "\nWrong Answer!\n";
            B_setColor(7);
        }
    }

    return false;
}

// ================= MAIN FUNCTION =================
int main()
{
    if(!humanVerification())
    {
        B_setColor(12);

        cout << "------------------------------------------------------------\n";
        cout << "                    ACCESS DENIED                           \n";
        cout << "------------------------------------------------------------\n";

        B_setColor(7);

        return 0;
    }

    int choice;

    do
    {
        B_setColor(11);

        cout << "\n============================================================\n";
        cout << "################## QUANTUM TRANSIT NETWORK #################\n";
        cout << "============================================================\n";

        B_setColor(7);

        R_setColor(10);

        cout << "===========================\n";
        cout << "         MAIN MENU         \n";
        cout << "===========================\n";

        cout << "[1]. Airline System\n";
        cout << "[2]. Railway System\n";
        cout << "[3]. Bus Reservation System\n";
        cout << "[0]. Exit\n";

        R_setColor(7);

        cout << "Enter choice: ";
        cin >> choice;

        // Input Validation
        if(cin.fail())
        {
            cin.clear();
            cin.ignore(1000, '\n');

            B_setColor(12);
            cout << "\nInvalid input! Please enter a number.\n";
            B_setColor(7);

            continue;
        }
        switch(choice)
        {
            case 1:
                AirlineMenu();
                break;

            case 2:
                RailwayMenu();
                break;

            case 3:
                BusServiceMenu();
                break;

            case 0:
                B_setColor(10);
                cout << "------------------------------------------------------------\n";
                cout << "                PROGRAM ENDED SUCCESSFULLY                  \n";
                cout << "------------------------------------------------------------\n";

                B_setColor(7);
                break;

            default:
                B_setColor(12);
                cout << "\nInvalid Choice! Try again.\n";
                B_setColor(7);
        }

    } while(choice != 0);

    return 0;
}
