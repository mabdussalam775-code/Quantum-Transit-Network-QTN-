#ifndef BUSSERVICE_H
#define BUSSERVICE_H

#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <windows.h>

using namespace std;

// ================= STRUCTS =================
struct Bus
{
    int id;
    string name;
    string source;
    string destination;
    double price;
    int seats;
    int bookedSeats;
    Bus* next;
};

struct BusPassenger
{
    int id;
    string name;
    long long phone;
    BusPassenger* next;
};

struct BusBooking {
    int id;
    int busId;
    int passengerId;
    BusBooking* next;
};

struct BusWaitNode
{
    int passengerId;
    int busId;
    BusWaitNode* next;
};

struct BusFeedback
{
    int userId;
    string name;
    string message;
    BusFeedback* next;
};

struct BusRoute
{
    string source;
    string destination;
    int distance;
    BusRoute* next;
};

// ================= GLOBAL VARIABLES =================
extern Bus* busHead;
extern BusPassenger* busPassengerHead;
extern BusBooking* busBookingHead;
extern BusFeedback* busFeedbackHead;
extern BusWaitNode* busFront;
extern BusWaitNode* busRear;
extern BusRoute* routeHead;

extern int busBookingCounter;
extern int busUserCounter;

// ================= FUNCTION DECLARATIONS =================

// UI
void B_setColor(int color);
void B_header(string title);
void B_success(string msg);
void B_error(string msg);
void B_info(string msg);

// Bus
Bus* findBus(int id);
void addBus();
void viewBuses();
void deleteBus();
void updateBus();

// Passenger
BusPassenger* addBusUser(string name, long long phone);
BusPassenger* findBusPassenger(int id);
BusPassenger* findBusUserByPhone(long long phone);

// Booking
void bookBusTicket(BusPassenger* user);
void cancelBusBooking(BusPassenger* user);
void viewMyBusBookings(BusPassenger* user);

// Waiting
void enqueueWaiting(int pid, int bid);
void B_viewWaitingList();

// Routes
void addBusRoute();
void viewBusRoutes();
void removeBusRoute();

// Reports
void B_advancedReport();

// Feedback
void submitBusFeedback(BusPassenger* user);
void viewBusFeedback();
void assignFromBusWaiting(int busId);
void promoteOneFromBusWaiting(int busId);
void dequeueWaiting();
void findBusByRoute();
void printBusTicket(BusPassenger* user, Bus* b, int bookingId);
void B_viewAllBookingsWithUsers();
bool B_alreadyBooked(int pid, int bid);

// File handling
void B_loadBusData();
void B_saveBusData();

void B_loadBusUsers();
void B_saveBusUsers();

void B_loadBusBookings();
void B_saveBusBookings();

void B_loadBusWaitingList();
void B_saveBusWaitingList();

void B_loadBusFeedback();
void B_saveBusFeedback();

void B_loadRoutes();
void B_saveRoutes();

// Menus
void busAdminMenu();
void busUserMenu(BusPassenger* u);
void BusServiceMenu();
// Auth
bool busAdminLogin();

// Cleanup
void cleanupBusSystem();

#endif
