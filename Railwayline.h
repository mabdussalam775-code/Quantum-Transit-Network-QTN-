
#ifndef RAILWAYLINE_H
#define RAILWAYLINE_H

#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <windows.h>

using namespace std;

// ======================DATA STRUCTURES=========================
struct Train
{
    int id;
    string name;
    string source;
    string destination;
    int distance;
    double price;
    int seats;
    int bookedSeats;
    Train* next;
};
struct Passenger
{
    int id;
    string name;
    long long phone;
    Passenger* next;
};
struct Booking
{
    int id;
    int trainId;
    int passengerId;
    Booking* next;
};
struct WaitNode
{
    int passengerId;
    int trainId;
    WaitNode* next;
};
struct Feedback
{
    int userId;
    string name;
    string message;
    Feedback* next;
};
struct Graph
{
    string cities[10];
    int adjMatrix[10][10];
};
// ================= GLOBAL HEADS =================
extern Train* trainHeadR;
extern Passenger* passengerHeadR;
extern Booking* bookingHeadR;
extern Feedback* feedbackHeadR;

extern WaitNode* frontR;
extern WaitNode* rearR;

extern int bookingCounterR;
extern int userCounterR;

extern Graph railwayGraph;
extern const int CITY_COUNT;
extern const string ADMIN_PASSWORD ;
// =====================UI FUNCTIONS==================
void R_setColor(int color);
void R_header(string text);
void R_success(string text);
void R_error(string text);
void R_info(string text);
// ================= TRAIN FUNCTIONS =================
Train* findTrain(int id);
void addTrain();
void viewTrains();
void deleteTrain();
void updateTrain();
void assignFromWaiting(int trainId);
// ================= PASSENGER FUNCTIONS =================
Passenger* addUser(string name, long long phone);
Passenger* findPassenger(int id);
// ================= BOOKING FUNCTIONS =================
void addToWaiting(int pid, int tid);
void viewWaitingList();
void bookTicket(Passenger* user);
void viewMyBookings(Passenger* user);
void viewAllBookingsWithUsers();
void cancelBooking(Passenger* user);
// ================= FEEDBACK FUNCTIONS =================
void submitFeedback(Passenger* user);
void viewFeedback();
// ================= REPORT =================
void report();
// ================= TICKET =================
void printTicket(Passenger* user, Train* t, int bookingId);
// ================= FILE HANDLING - TRAINS =================
void saveTrains();
void loadTrains();
// ================= FILE HANDLING - USERS =================
void saveUsers();
void loadUsers();
// ================= FILE HANDLING - FEEDBACK =================
void saveFeedback();
void loadFeedback();
// ================= FILE HANDLING - BOOKINGS =================
void saveBookings();
void loadBookings();
//================== FILE HABDLING - WAITING LIST =============
void saveWaitingList();
void loadWaitingList();
//================== GRAPH APPLICATION ========================
void initializeGraph();
void showCities();
int dijkstra(int src, int dest);
int getMinVertex(int dist[], bool visited[]);
// ================= MENUS =================
void userMenu(Passenger* u);
void adminMenu();
void RailwayMenu();
//=============================================================
void findTrainByRoute();
void advancedReport();
void findShortestRoute();

Passenger* findUserByPhone(long long phone);

bool adminLogin();

void clearTrains();
void clearUsers();
void clearBookings();
void clearFeedback();
void cleanup();
#endif
