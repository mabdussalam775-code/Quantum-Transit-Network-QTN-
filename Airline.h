#ifndef AIRLINE_H
#define AIRLINE_H

#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <windows.h>

using namespace std;
//================= DATA STRUCTURES OF AIRLINE==================
struct A_Flight
{
    int id;
    string flightNum;
    string airline;
    string source;
    string destination;
    int distance;
    double price;
    int seats;
    int bookedSeats;
    string status;
    string gate; // Extra: Gate assignment
    A_Flight* next;
};

struct A_Passenger
{
    int id;
    string name;
    string phone;
    string passport; // Extra: Passport info
    A_Passenger* next;
};

struct A_Booking
{
    int id;
    int flightId;
    int passengerId;
    string travelClass;// Extra: Economy/Business
    string bookingTime;
    A_Booking* next;
};

struct A_WaitNode
{
    int passengerId;
    int flightId;
    A_WaitNode* next;
};

struct A_Feedback
{
    int userId;
    string name;
    string message;
    A_Feedback* next;
};
struct A_Route
{
    string source;
    string destination;
    int distance;

    A_Route* next;
};
struct A_Graph
{
    string airports[10];
    int adjMatrix[10][10];
};

// ================= GLOBAL HEADS =================
extern A_Flight* flightHead;
extern A_Passenger* passengerHead;
extern A_Booking* bookingHead;
extern A_Feedback* feedbackHead;
extern A_WaitNode* front;
extern A_WaitNode* rear;

extern A_Route* A_routeHead;

extern int bookingCounter;
extern int userCounter;

extern A_Graph airGraph;
extern const int AIRPORT_COUNT;

extern const string ADMIN_ID;
extern const string ADMIN_PASS;

//==============================UI================================
void setColor(int color);
void header(string text);
void success(string text);
void error(string text);
void info(string text);
// ================= FLIGHT MANAGEMENT FUNCTIONS =================
A_Flight* findFlight(int id);
void addFlight();
void viewFlights();
void deleteFlight();
void updateFlight();
void updateFlightStatus();
void A_assignFromWaiting(int flightId);
// ================= PASSENGER MANAGEMENT FUNCTIONS =================
A_Passenger* A_addUser(string name, string phone, string passport);
A_Passenger* A_findPassenger(int id);
A_Passenger* A_findUserByPhone(string phone);
bool passportExists(string passport);
// ================= BOOKING MANAGEMENT FUNCTIONS =================
void A_bookTicket(A_Passenger* user);
void A_viewMyBookings(A_Passenger* user);
void A_viewAllBookingsWithUsers();
void A_cancelBooking(A_Passenger* user);
// ================= WAITING LIST FUNCTIONS =================
void A_addToWaiting(int pid, int fid);
void A_viewWaitingList();
// ================= FEEDBACK SYSTEM FUNCTIONS =================
void A_submitFeedback(A_Passenger* user);
void A_viewFeedback();
// ================= REPORTS & ANALYTICS =================
void A_report();
void A_advancedReport();
// ================= BOARDING PASS FUNCTIONS =================
void printBoardingPass(A_Passenger* user, A_Flight* f, int bId, string cls);
// ================= GRAPH / ROUTE SYSTEM =================
void A_initializeGraph();
void showAirports();
int A_dijkstra(int src, int dest, int parent[]);
int A_getMinVertex(int dist[], bool visited[]);
void addRoute();
void removeRoute();
void findFlightByRoute();
void searchByFlightNumber();
void A_findShortestRoute();
void A_printPath(int parent[], int j);
// ================= FILE HANDLING SYSTEM =================
void A_saveFlights();
void A_loadFlights();

void A_saveUsers();
void A_loadUsers();

void A_saveBookings();
void A_loadBookings();

void A_saveFeedback();
void A_loadFeedback();

void A_saveWaitingList();
void A_loadWaitingList();
// ================= SYSTEM CLEANUP =================
void A_cleanup();
// ================= MENU SYSTEM =================
void A_adminMenu();
bool A_adminLogin();
void A_userMenu(A_Passenger* u);
void AirlineMenu();
//====================================================

#endif
