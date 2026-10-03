#include <ctime>
#include "airline.h"

A_Flight* flightHead = NULL;
A_Passenger* passengerHead = NULL;
A_Booking* bookingHead = NULL;
A_Feedback* feedbackHead = NULL;
A_WaitNode* front = NULL;
A_WaitNode* rear = NULL;
A_Route* A_routeHead = NULL;
int bookingCounter = 1;
int userCounter = 1;

A_Graph airGraph;
const int AIRPORT_COUNT = 10;

const string ADMIN_ID = "admin";
const string ADMIN_PASS = "airline@2026";
//=============================UI==================================
void setColor(int color)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void header(string title)
{
    setColor(11);
    cout << "\n============================================================\n";
    cout << "                " << title << endl;
    cout << "============================================================\n";
    setColor(7);
}
void success(string msg)
{
    setColor(10); cout << "[SUCCESS] " << msg << endl; setColor(7);
}

void error(string msg)
{
    setColor(12); cout << "[ERROR] " << msg << endl; setColor(7);
}

void info(string msg)
{
    setColor(14); cout << "[INFO] " << msg << endl; setColor(7);
}
//================= FLIGHT FUNCTIONS =================
A_Flight* findFlight(int id)
{
    A_Flight* f = flightHead;
    while(f != NULL)
    {
        if(f->id == id) return f;
        f = f->next;
    }
    return NULL;
}

void addFlight()
{
    A_Flight* f = new A_Flight();
    cout << "Enter Flight ID: ";
    cin >> f->id;
    if(findFlight(f->id))
    {
        error("Flight already exists!");
        delete f;
        return;
    }

    cout << "Flight Number (e.g. PK301): ";
    cin >> f->flightNum;
    cout << "Airline Name: ";
    cin >> ws;
    getline(cin, f->airline);

    showAirports();
    int src, dest;
    cout << "Select Source Airport Number: ";
    cin >> src;
    cout << "Select Destination Airport Number: ";
    cin >> dest;

    if(src < 0 || src >= AIRPORT_COUNT || dest < 0 || dest >= AIRPORT_COUNT || src == dest)
    {
        error("Invalid Selection!");
        delete f;
        return;
    }

    f->source = airGraph.airports[src];
    f->destination = airGraph.airports[dest];
    int tempParent[AIRPORT_COUNT];
    f->distance = A_dijkstra(src, dest,tempParent);
    if(f->distance == 99999)
    {
        error("No air route exists!");
        delete f;
        return;
    }

    f->price = f->distance * 12.5; // Flight rates are higher
    cout << "Aircraft Capacity (Seats): ";
    cin >> f->seats;
    if(f->seats <= 0)
    {
        error("Invalid seat count!");
        delete f;
        return;
    }

    f->bookedSeats = 0;
    f->status = "On Time";
    f->gate = "A-" + to_string(f->id % 12 + 1);
    f->next = NULL;

    if(flightHead == NULL)
    {
        flightHead = f;
    }
    else
    {
        A_Flight* temp = flightHead;
        while(temp->next != NULL)
        {
            temp = temp->next;
        }
        temp->next = f;
    }
    success("Flight Added to Schedule!");
}
void viewFlights()
{
    A_Flight* f = flightHead;

    header("CURRENT FLIGHT SCHEDULE");

    if(f == NULL)
    {
        error("No flights scheduled!");
        return;
    }

    while(f != NULL)
    {
        cout << "Flight ID      : "
             << f->id << endl;

        cout << "Flight Number  : "
             << f->flightNum << endl;

        cout << "Airline        : "
             << f->airline << endl;

        cout << "Route          : "
             << f->source
             << " -> "
             << f->destination << endl;

        cout << "Distance       : "
             << f->distance
             << " KM" << endl;

        cout << "Price          : "
             << f->price << endl;

        cout << "Seats          : "
             << f->bookedSeats
             << "/"
             << f->seats << endl;

        // FULL FLIGHT WARNING
        if(f->bookedSeats == f->seats)
        {
            setColor(12);

            cout << "Flight Status  : FULLY BOOKED\n";

            setColor(7);
        }

        cout << "Status         : "
             << f->status << endl;

        cout << "Gate           : "
             << f->gate << endl;

        setColor(13);

        cout << "------------------------------------------------------------\n";

        setColor(7);

        f = f->next;
    }
}
void deleteFlight()
{
    int id;
    header("DEACTIVATE FLIGHT");

    cout << "Enter Flight ID to Remove: ";
    cin >> id;

    A_Flight *curr = flightHead;
    A_Flight *prev = NULL;

    while(curr)
    {
        if(curr->id == id)
        {
            A_Booking* b = bookingHead;
            while(b)
            {
                if(b->flightId == id)
                    b->flightId = -1;
                b = b->next;
            }

            if(prev) prev->next = curr->next;
            else flightHead = curr->next;

            delete curr;

            success("Flight Cancelled Successfully!");
            return;
        }

        prev = curr;
        curr = curr->next;
    }

    error("Flight not found!");
}
void updateFlight()
{
    int id;
    cout << "Enter Flight ID: ";
    cin >> id;
    A_Flight* f = findFlight(id);
    if(!f)
    {
        error("Flight Not Found!");
        return;
    }
    cout << "Update Base Price: ";
    cin >> f->price;
    cout << "Update Seat Capacity: ";
    cin >> f->seats;
    success("Flight Updated!");
    while(f->bookedSeats < f->seats)
    {
        int before = f->bookedSeats;
        A_assignFromWaiting(id);
        if(before == f->bookedSeats)
        {
            break;
        }
    }
}

void A_assignFromWaiting(int flightId) {
    A_WaitNode *curr = front, *prev = NULL;

    while(curr) {
        if(curr->flightId == flightId) {

            A_Flight* f = findFlight(flightId);

            if(f && f->bookedSeats < f->seats) {

                A_Booking* b = new A_Booking();
                b->id = bookingCounter++;
                b->flightId = flightId;
                b->passengerId = curr->passengerId;
                b->travelClass = "Economy (W)";
                b->next = bookingHead;
                bookingHead = b;

                f->bookedSeats++;

                // SAFE DELETE
                if(prev) prev->next = curr->next;
                else front = curr->next;

                if(curr == rear) rear = prev;

                A_WaitNode* del = curr;
                curr = curr->next;
                delete del;

                A_saveWaitingList();
                return;
            }
        }

        prev = curr;
        curr = curr->next;
    }
}
// ================= PASSENGER FUNCTIONS =================
A_Passenger* A_addUser(string name, string phone, string passport)
{
    A_Passenger* p = new A_Passenger();
    p->id = userCounter++;
    p->name = name;
    p->phone = phone;
    p->passport = passport;
    p->next = passengerHead;
    passengerHead = p;
    return p;
}

A_Passenger* A_findPassenger(int id)
{
    A_Passenger* p = passengerHead;
    while(p)
    {
        if(p->id == id)
        {
            return p;
        }
        p = p->next;
    }
    return NULL;
}
A_Passenger* A_findUserByPhone(string phone)
{
    A_Passenger* p = passengerHead;

    while(p)
    {
        if(p->phone == phone)
        {
            return p;
        }

        p = p->next;
    }

    return NULL;
}
bool passportExists(string passport)
{
    A_Passenger* p = passengerHead;

    while(p)
    {
        if(p->passport == passport)
        {
            return true;
        }

        p = p->next;
    }

    return false;
}
// ================= BOOKING FUNCTIONS =================
bool A_isInWaiting(int pid, int fid)
{
    A_WaitNode* t = front;

    while(t)
    {
        if(t->passengerId == pid && t->flightId == fid)
            return true;
        t = t->next;
    }
    return false;
}

void A_addToWaiting(int pid, int fid)
{
    if(A_isInWaiting(pid, fid))
    {
        error("Already in waiting list!");
        return;
    }

    A_WaitNode* n = new A_WaitNode();
    n->passengerId = pid;
    n->flightId = fid;
    n->next = NULL;

    if(!front)
    {
        front = rear = n;
    }
    else
    {
        rear->next = n;
        rear = n;
    }

    A_saveWaitingList();
}
void A_viewWaitingList()
{
    if(!front)
    {
        error("Waiting List Empty!");
        return;
    }

    A_WaitNode* t = front;

    header("FLIGHT WAITING LIST");

    while(t)
    {
        setColor(13);
        cout << "------------------------------------------------------------\n";
        setColor(7);
        A_Passenger* p = A_findPassenger(t->passengerId);

        cout << "Passenger ID  : " << t->passengerId << endl;

        if(p)
            cout << "Name          : " << p->name << endl;

        cout << "Flight ID     : " << t->flightId << endl;

        setColor(13);
        cout << "------------------------------------------------------------\n";
        setColor(7);

        t = t->next;
    }
}
bool A_alreadyBooked(int pid, int fid)
{
    A_Booking* b = bookingHead;

    while(b)
    {
        if(b->passengerId == pid &&
           b->flightId == fid)
        {
            return true;
        }

        b = b->next;
    }

    return false;
}
void A_bookTicket(A_Passenger* user)
{
    int fid;

    cout << "Enter Flight ID: ";
    cin >> fid;

    A_Flight* f = findFlight(fid);

    if(!f)
    {
        error("Flight Not Found!");
        return;
    }

    // already booked check
    if(A_alreadyBooked(user->id, fid))
    {
        error("You already booked this flight!");
        return;
    }

    // seat available
    if(f->bookedSeats < f->seats)
    {
        A_Booking* b = new A_Booking();

        b->id = bookingCounter++;
        b->flightId = fid;
        b->passengerId = user->id;

        // ===================== TIME FIX (IMPORTANT) =====================
        time_t now = time(0);
        tm *ltm = localtime(&now);

        char buffer[30];
        strftime(buffer, sizeof(buffer),
                 "%Y-%m-%d %H:%M:%S", ltm);

        b->bookingTime = buffer;
        // ===============================================================

        int c;
        cout << "Select Class (1. Economy 2. Business): ";
        cin >> c;

        b->travelClass = (c == 2) ? "Business" : "Economy";

        // insert booking
        b->next = bookingHead;
        bookingHead = b;

        f->bookedSeats++;

        success("Ticket Booked Successfully!");

        printBoardingPass(user, f, b->id, b->travelClass);
    }
    else
    {
        A_addToWaiting(user->id, fid);
        info("Flight Full! Added to Waiting List.");
    }
}

void A_viewMyBookings(A_Passenger* user)
{
    A_Booking* b = bookingHead;
    bool found = false;

    header("MY BOOKINGS");

    while(b)
    {
        if(b->passengerId == user->id)
        {
            A_Flight* f = findFlight(b->flightId);
            setColor(13);
            cout << "------------------------------------------------------------\n";
            setColor(7);
            cout << "Booking ID     : " << b->id << endl;

            if(f)
            {
                cout << "Flight Number  : " << f->flightNum << endl;
                cout << "Route          : " << f->source << " -> " << f->destination << endl;
            }
            cout << "Class          : " << b->travelClass << endl;
            cout<<"Booking Time   : "<< b->bookingTime;
            setColor(13);
            cout << "\n------------------------------------------------------------\n";
            setColor(7);

            found = true;
        }
        b = b->next;
    }

    if(!found)
        error("No bookings found!");
}
void A_viewAllBookingsWithUsers()
{
    if(!bookingHead)
    {
        error("No bookings in system!");
        return;
    }

    A_Booking* b = bookingHead;

    header("MASTER BOOKING RECORD");

    while(b)
    {
        A_Passenger* p = A_findPassenger(b->passengerId);
        A_Flight* f = findFlight(b->flightId);
        setColor(13);
        cout << "------------------------------------------------------------\n";
        setColor(7);
        cout << "Booking ID     : " << b->id << endl;

        cout << "\n--- Passenger Info ---\n";
        if(p)
        {
            cout << "Name          : " << p->name << endl;
            cout << "Phone         : " << p->phone << endl;
        }

        cout << "\n--- Flight Info ---\n";
        if(f)
        {
            cout << "Flight No     : " << f->flightNum << endl;
            cout << "Route         : " << f->source << " -> " << f->destination << endl;
        }

        cout << "Class         : " << b->travelClass << endl;

        setColor(13);
        cout << "------------------------------------------------------------\n";
        setColor(7);

        b = b->next;
    }
}
void A_cancelBooking(A_Passenger* user)
{
    int bid;
    cout << "Enter Booking ID to Cancel: ";
    cin >> bid;
    A_Booking *curr = bookingHead, *prev = NULL;
    while(curr)
    {
        if(curr->id == bid && curr->passengerId == user->id)
        {
            A_Flight* f = findFlight(curr->flightId);
            if(prev) prev->next = curr->next;
            else bookingHead = curr->next;
            if(f && f->bookedSeats > 0)
            {
                f->bookedSeats--;
                A_assignFromWaiting(f->id);
            }
            delete curr;
            success("Ticket Cancelled Successfully!");
            return;
        }
        prev = curr; curr = curr->next;
    }
    error("Invalid Booking ID!");
}

// ================= FEEDBACK & REPORTS =================
void A_submitFeedback(A_Passenger* user)
{
    A_Feedback* f = new A_Feedback();
    f->userId = user->id;
    f->name = user->name;
    cout << "Message: ";
    cin >> ws;
    getline(cin, f->message);
    f->next = feedbackHead;
    feedbackHead = f;
    success("Feedback Sent!");
}

void A_viewFeedback()
{
    A_Feedback* f = feedbackHead;
    if(!f)
    {
        error("No feedback received.");
        return;
    }
    header("PASSENGER FEEDBACK");
    while(f)
    {
        cout << "User: " << f->name << " (ID: " << f->userId << ")\nMsg: " << f->message << endl;
        cout << "---------------------------\n";
        f = f->next;
    }
}
void A_advancedReport()
{
    header("AIRLINE ADVANCED ANALYTICS");

    int totalFlights = 0;
    int totalPassengers = 0;
    int totalBookings = 0;
    int waitingCount = 0;

    double totalRevenue = 0;

    A_Flight* mostBooked = NULL;

    // TOTAL FLIGHTS + REVENUE
    A_Flight* f = flightHead;

    while(f)
    {
        totalFlights++;

        // OCCUPANCY REPORT
        double occupancy =
        (double(f->bookedSeats) / f->seats) * 100;

        cout << "\n================================\n";

        cout << "Flight Number : "
             << f->flightNum << endl;

        cout << "Occupancy     : "
             << occupancy << "%" << endl;

        // BUSINESS / ECONOMY REVENUE
        A_Booking* tempBook = bookingHead;

        while(tempBook)
        {
            if(tempBook->flightId == f->id)
            {
                if(tempBook->travelClass == "Business")
                {
                    totalRevenue += f->price * 2;
                }
                else
                {
                    totalRevenue += f->price;
                }
            }

            tempBook = tempBook->next;
        }

        if(mostBooked == NULL ||
           f->bookedSeats > mostBooked->bookedSeats)
        {
            mostBooked = f;
        }

        f = f->next;
    }

    // TOTAL PASSENGERS
    A_Passenger* p = passengerHead;

    while(p)
    {
        totalPassengers++;
        p = p->next;
    }

    // TOTAL BOOKINGS
    A_Booking* b = bookingHead;

    while(b)
    {
        totalBookings++;
        b = b->next;
    }

    // WAITING LIST COUNT
    A_WaitNode* w = front;

    while(w)
    {
        waitingCount++;
        w = w->next;
    }

    cout << "\n================================\n";

    cout << "Total Flights       : "
         << totalFlights << endl;

    cout << "Total Passengers    : "
         << totalPassengers << endl;

    cout << "Total Bookings      : "
         << totalBookings << endl;

    cout << "Waiting Passengers  : "
         << waitingCount << endl;

    cout << "Total Revenue       : "
         << totalRevenue << " PKR" << endl;

    if(mostBooked)
    {
        cout << "Most Booked Flight  : "
             << mostBooked->flightNum
             << endl;
    }

    double avgBookings =
    (totalFlights > 0)
    ? double(totalBookings) / totalFlights
    : 0;

    cout << "Average Bookings/Flight : "
         << avgBookings << endl;
}
void printBoardingPass(A_Passenger* p, A_Flight* f, int bId, string cls)
{
    header("OFFICIAL BOARDING PASS");
    cout << left;
    setColor(13); cout << "------------------------------------------------------------\n"; setColor(7);
    cout << setw(20) << "Booking ID" << ": " << bId << endl;
    cout << setw(20) << "Passenger" << ": " << p->name << endl;
    cout << setw(20) << "Passport" << ": " << p->passport << endl;
    setColor(13); cout << "------------------------------------------------------------\n"; setColor(7);
    cout << setw(20) << "Flight No" << ": " << f->flightNum << " (" << f->airline << ")" << endl;
    cout << setw(20) << "From" << ": " << f->source << " -> " << f->destination << endl;
    cout << setw(20) << "Gate" << ": " << f->gate << " | Class: " << cls << endl;
    cout << setw(20) << "Price" << ": " << (cls == "Business" ? f->price * 2 : f->price) << endl;
    setColor(13); cout << "------------------------------------------------------------\n"; setColor(7);
    cout << "STATUS: CONFIRMED | PLEASE ARRIVE 2 HOURS BEFORE FLIGHT\n";
    cout << "============================================================\n";
}

// ================= GRAPH / ROUTE FUNCTIONS =================
void A_initializeGraph()
{
    for(int i=0; i<AIRPORT_COUNT; i++)
    {
        for(int j=0; j<AIRPORT_COUNT; j++)
        airGraph.adjMatrix[i][j] = (i==j ? 0 : 99999);
    }

    airGraph.airports[0] = "Karachi";
    airGraph.airports[1] = "Lahore";
    airGraph.airports[2] = "Islamabad";
    airGraph.airports[3] = "Dubai";
    airGraph.airports[4] = "London";
    airGraph.airports[5] = "New York";
    airGraph.airports[6] = "Peshawar";
    airGraph.airports[7] = "Quetta";
    airGraph.airports[8] = "Sialkot";
    airGraph.airports[9] = "Doha";

    airGraph.adjMatrix[0][1] = 1000; airGraph.adjMatrix[1][0] = 1000; // Karachi-Lahore
    airGraph.adjMatrix[0][2] = 1400; airGraph.adjMatrix[2][0] = 1400; // Karachi-Islamabad
    airGraph.adjMatrix[0][3] = 1200; airGraph.adjMatrix[3][0] = 1200; // Karachi-Dubai

    airGraph.adjMatrix[1][2] = 300;  airGraph.adjMatrix[2][1] = 300;  // Lahore-Islamabad
    airGraph.adjMatrix[1][8] = 200;  airGraph.adjMatrix[8][1] = 200;  // Lahore-Sialkot

    airGraph.adjMatrix[2][6] = 250;  airGraph.adjMatrix[6][2] = 250;  // Islamabad-Peshawar
    airGraph.adjMatrix[2][7] = 600;  airGraph.adjMatrix[7][2] = 600;  // Islamabad-Quetta

    airGraph.adjMatrix[3][4] = 5500; airGraph.adjMatrix[4][3] = 5500; // Dubai-London
    airGraph.adjMatrix[4][5] = 5600; airGraph.adjMatrix[5][4] = 5600; // London-New York

    airGraph.adjMatrix[3][9] = 800;  airGraph.adjMatrix[9][3] = 800;  // Dubai-Doha
    airGraph.adjMatrix[1][9] = 900;  airGraph.adjMatrix[9][1] = 900;  // Lahore-Doha
}

void showAirports()
{
    cout << "\n===== AIRPORT CODES =====\n";
    for(int i=0; i<10; i++) cout << i << ". " << airGraph.airports[i] << endl;
}

int A_getMinVertex(int dist[], bool visited[])
{
    int min = 99999, index = -1;
    for(int i=0; i<AIRPORT_COUNT; i++)
    {
        if(!visited[i] && dist[i] < min)
        {
            min = dist[i];
            index = i;
        }
    }
    return index;
}

int A_dijkstra(int src, int dest, int parent[])
{
    int dist[AIRPORT_COUNT];
    bool visited[AIRPORT_COUNT];

    // initialization
    for(int i = 0; i < AIRPORT_COUNT; i++)
    {
        dist[i] = 99999;
        visited[i] = false;
        parent[i] = -1;
    }

    dist[src] = 0;

    for(int i = 0; i < AIRPORT_COUNT - 1; i++)
    {
        int u = A_getMinVertex(dist, visited);
        if(u == -1) break;

        visited[u] = true;

        for(int v = 0; v < AIRPORT_COUNT; v++)
        {
            if(!visited[v] &&
               airGraph.adjMatrix[u][v] != 99999 &&
               dist[u] + airGraph.adjMatrix[u][v] < dist[v])
            {
                dist[v] = dist[u] + airGraph.adjMatrix[u][v];
                parent[v] = u;
            }
        }
    }

    return dist[dest];
}
void A_printPath(int parent[], int j)
{
    if(parent[j] == -1)
    {
        cout << airGraph.airports[j];
        return;
    }

    A_printPath(parent, parent[j]);
    cout << " -> " << airGraph.airports[j];
}
void addRoute()
{
    int u, v, d;
    showAirports();
    cout << "Source Index: ";
    cin >> u;
    cout << "Dest Index: ";
    cin >> v;
    cout << "Distance: ";
    cin >> d;
    if(u >= 0 && v >= 0 && u < 10 && v < 10)
    {
        airGraph.adjMatrix[u][v] = d;
        airGraph.adjMatrix[v][u] = d;
        A_Route* r = new A_Route();

        r->source = airGraph.airports[u];
        r->destination = airGraph.airports[v];
        r->distance = d;

        r->next = A_routeHead;

        A_routeHead = r;
        success("Route Added!");
    }
}

void removeRoute()
{
    int u, v;
    showAirports();
    cout << "Source: ";
    cin >> u;
    cout << "Dest: ";
    cin >> v;
    if(u >= 0 && v >= 0 && u < 10 && v < 10)
    {
        airGraph.adjMatrix[u][v] = 99999;
        airGraph.adjMatrix[v][u] = 99999;
        success("Route Removed!");
    }
}
void viewRoutes()
{
    header("ALL AIR ROUTES");

    bool found = false;

    for(int i = 0; i < AIRPORT_COUNT; i++)
    {
        for(int j = i + 1; j < AIRPORT_COUNT; j++)
        {
            if(airGraph.adjMatrix[i][j] != 99999 &&
               airGraph.adjMatrix[i][j] != 0)
            {
                cout << airGraph.airports[i]
                     << " <--> "
                     << airGraph.airports[j]
                     << " : "
                     << airGraph.adjMatrix[i][j]
                     << " KM" << endl;

                found = true;
            }
        }
    }

    if(!found)
    {
        error("No routes available!");
    }
}
void findFlightByRoute()
{
    string s, d;
    cout << "Enter Source: ";
    cin >> ws;
    getline(cin, s);
    cout << "Enter Dest: ";
    getline(cin, d);
    A_Flight* f = flightHead; bool found = false;
    while(f)
    {
        if(f->source == s && f->destination == d)
        {
            cout << f->flightNum << " | " << f->airline << " | Price: " << f->price << endl;
            found = true;
        }
        f = f->next;
    }
    if(!found) error("No direct flights!");
}
void searchByFlightNumber()
{
    string num;

    cout << "Enter Flight Number: ";
    cin >> num;

    A_Flight* f = flightHead;

    while(f)
    {
        if(f->flightNum == num)
        {
            header("FLIGHT FOUND");

            cout << "Flight Number : "
                 << f->flightNum << endl;

            cout << "Airline       : "
                 << f->airline << endl;

            cout << "Route         : "
                 << f->source
                 << " -> "
                 << f->destination << endl;

            cout << "Price         : "
                 << f->price << endl;

            cout << "Seats         : "
                 << f->bookedSeats
                 << "/"
                 << f->seats << endl;

            return;
        }

        f = f->next;
    }

    error("Flight not found!");
}
void updateFlightStatus()
{
    int id;
    cout << "Enter Flight ID: ";
    cin >> id;
    A_Flight* f = findFlight(id);
    if(!f)
    {
        error("Not Found!");
        return;
    }
    cout << "New Status (On Time / Delayed / Boarding): ";
    cin >> ws;
    getline(cin, f->status);
    success("Status Updated!");
}

void A_findShortestRoute()
{
    int s, d;
    showAirports();

    cout << "Source Index: ";
    cin >> s;

    cout << "Destination Index: ";
    cin >> d;

    int parent[AIRPORT_COUNT];

    int distance = A_dijkstra(s, d, parent);

    if(distance == 99999)
    {
        error("No route found!");
        return;
    }

    success("Shortest distance: " + to_string(distance) + " KM");

    cout << "\n===== SHORTEST ROUTE =====\n";
    A_printPath(parent, d);
    cout << endl;
}

// ================= CLEANUP =================
void A_cleanup()
{
    while(flightHead)
    {
        A_Flight* t = flightHead;
        flightHead = flightHead->next;
        delete t;
    }
    while(passengerHead)
    {
        A_Passenger* t = passengerHead;
        passengerHead = passengerHead->next;
        delete t;
    }
    while(bookingHead)
    {
        A_Booking* t = bookingHead;
        bookingHead = bookingHead->next;
        delete t;
    }
    while(feedbackHead)
    {
        A_Feedback* t = feedbackHead;
        feedbackHead = feedbackHead->next;
        delete t;
    }
    while(front)
    {
       A_WaitNode* t = front;
       front = front->next;
       delete t;
    }
}
bool A_adminLogin()
{
    string id, pass;
    int attempts = 3;

    while(attempts > 0)
    {
        header("    ADMIN LOGIN");

        cout << "Remaining Attempts: " << attempts << endl;

        cout << "Enter Admin ID: ";
        cin >> id;

        cout << "Enter Password: ";
        cin >> pass;

        if(id == ADMIN_ID && pass == ADMIN_PASS)
        {
            success("Login Successful!");
            return true;
        }
        else
        {
            attempts--;
            error("Invalid Credentials!");

            if(attempts > 0)
            {
                info("Try again. You still have " + to_string(attempts) + " attempt(s) left.");
            }
        }
    }

    error("Access denied. No attempts left.");
    return false;
}
// ================= FILE HANDLING =================
void A_saveFlights()
{
    ofstream f("AirlineFlights.txt");
    A_Flight* t = flightHead;

    while(t)
    {
        f << t->id << "\n"
          << t->flightNum << "\n"
          << t->airline << "\n"
          << t->source << "\n"
          << t->destination << "\n"
          << t->distance << "\n"
          << t->price << " "
          << t->seats << " "
          << t->bookedSeats << "\n"
          << t->status << "\n"
          << t->gate << "\n";

        t = t->next;
    }

    f.close();
}
void A_loadFlights()
{
    ifstream f("AirlineFlights.txt");
    if(!f)
    {
        return;
    }
    while(true)
    {
        A_Flight* t = new A_Flight();
        if(!(f >> t->id))
        {
            delete t;
            break;
        }
        f.ignore();
        getline(f, t->flightNum);
        getline(f, t->airline);
        getline(f, t->source);

        getline(f, t->destination);
        f >> t->distance >> t->price >> t->seats >> t->bookedSeats;
        f.ignore();
        getline(f, t->status);
        getline(f, t->gate);
        t->next = flightHead;
        flightHead = t;
    }
}
void A_saveUsers()
{
    ofstream f("AirlineUsers.txt");

    A_Passenger* p = passengerHead;

    while(p)
    {
        f << p->id << "\n"
          << p->name << "\n"
          << p->phone << "\n"
          << p->passport << "\n";

        p = p->next;
    }

    f.close();
}

void A_loadUsers()
{
    ifstream f("AirlineUsers.txt");
    if(!f)
    {
        return;
    }
    while(true)
    {
        A_Passenger* p = new A_Passenger();
        if(!(f >> p->id))
        {
            delete p;
            break;
        }
        f.ignore();
        getline(f, p->name);
        f >> p->phone;
        f.ignore();
        getline(f, p->passport);
        p->next = passengerHead;
        passengerHead = p;
        if(p->id >= userCounter)
        {
            userCounter = p->id + 1;
        }
    }
}
void A_saveBookings()
{
    ofstream f("AirlineBookings.txt");

    A_Booking* b = bookingHead;

    while(b)
    {
        f << b->id << "\n";
        f << b->flightId << "\n";
        f << b->passengerId << "\n";
        f << b->travelClass << "\n";
        f << b->bookingTime << "\n";
        b = b->next;
    }
}
void A_loadBookings()
{
    ifstream f("AirlineBookings.txt");

    if(!f)
        return;

    while(true)
    {
        A_Booking* b = new A_Booking();

        if(!(f >> b->id))
        {
            delete b;
            break;
        }

        f >> b->flightId;
        f >> b->passengerId;

        f.ignore();

        getline(f, b->travelClass);
        getline(f, b->bookingTime);
        b->next = bookingHead;
        bookingHead = b;

        if(b->id >= bookingCounter)
            bookingCounter = b->id + 1;
    }
}
void A_saveWaitingList()
{
    ofstream f("AirlineWaightings.txt");
    A_WaitNode* w = front;
    while(w)
    {
        f << w->passengerId << " " << w->flightId << "\n";
    w = w->next;
    }
}
void A_loadWaitingList()
{
    ifstream f("AirlineWaightings.txt");
    if(!f)
    {
        return;
    }
    while(true)
    {
        A_WaitNode* w = new A_WaitNode();

        if(!(f >> w->passengerId >> w->flightId))
        {
            delete w;
            break;
        }

        w->next = NULL;

        if(!front)
        {
            front = rear = w;
        }
        else
        {
            rear->next = w;
            rear = w;
        }
    }
}
void A_saveFeedback()
{
    ofstream f("AirlineServiceFeedbacks.txt");
    A_Feedback* t = feedbackHead;
    while(t)
    {
        f << t->userId << "\n"
        << t->name << "\n"
        << t->message << "\n";
    t = t->next;
    }
}
void A_loadFeedback()
{
    ifstream f("AirlineServiceFeedbacks.txt");
    if(!f)
    {
        return;
    }

    while(true)
    {
        A_Feedback* t = new A_Feedback();

        if(!(f >> t->userId))
        {
            delete t;
            break;
        }

        f.ignore();
        getline(f, t->name);
        getline(f, t->message);

        t->next = feedbackHead;
        feedbackHead = t;
    }
}
// ================= MENUS =================
void A_adminMenu()
{
    int c;

    do
    {
        setColor(9);
        header("ADMIN CONTROL PANEL");
        setColor(7);
        setColor(9);
        cout << " [1] Add Flight\n";
        cout << " [2] View Flights\n";
        cout << " [3] Delete Flight\n";
        cout << " [4] Update Flight\n";
        cout << " [5] Waiting List\n";
        cout << " [6] Feedback\n";
        cout << " [7] All Bookings\n";
        cout << " [8] Search Route\n";
        cout << " [9] Add Route\n";
        cout << "[10] Remove Route\n";
        cout << "[11] View All Routes\n";
        cout << "[12] Report Analysis\n";
        cout << "[13] Search By Flight Number\n";
        cout << " [0] Logout\n";
        setColor(7);
        cout << "\nEnter choice: ";
        cin >> c;

        switch(c)
        {
            case 1: addFlight(); break;
            case 2: viewFlights(); break;
            case 3: deleteFlight(); break;
            case 4: updateFlight(); break;
            case 5: A_viewWaitingList(); break;
            case 6: A_viewFeedback(); break;
            case 7: A_viewAllBookingsWithUsers(); break;
            case 8: findFlightByRoute(); break;
            case 9: addRoute(); break;
            case 10: removeRoute(); break;
            case 11: viewRoutes(); break;
            case 12: A_advancedReport(); break;
            case 13:searchByFlightNumber();break;
            case 0: cout << "Logging out...\n"; break;
            default: error("Invalid Choice!");
        }

    } while(c != 0);
}
void A_userMenu(A_Passenger* u)
{
    int c;

    do
    {
        setColor(10);
        header("PASSENGER DASHBOARD - " + u->name);
        setColor(7);
        setColor(9);
        cout << " [1] View Flights\n";
        cout << " [2] Search Flight\n";
        cout << " [3] Book Ticket\n";
        cout << " [4] My Bookings\n";
        cout << " [5] Cancel Booking\n";
        cout << " [6] Feedback\n";
        cout << " [7] Shortest Route\n";
        cout << " [0] Logout\n";
        setColor(7);
        cout << "\nEnter choice: ";
        cin >> c;

        switch(c)
        {
            case 1: viewFlights(); break;
            case 2: findFlightByRoute(); break;
            case 3: A_bookTicket(u); break;
            case 4: A_viewMyBookings(u); break;
            case 5: A_cancelBooking(u); break;
            case 6: A_submitFeedback(u); break;
            case 7: A_findShortestRoute(); break;
            case 0: cout << "Logging out...\n"; break;
            default: error("Invalid Choice!");
        }

    } while(c != 0);
}
void AirlineMenu()
{
    A_initializeGraph();

    A_loadFlights();
    A_loadUsers();
    A_loadBookings();
    A_loadWaitingList();
    A_loadFeedback();

    int choice;

    do
    {
        header("QUANTUM AIRLINE NETWORK");

        setColor(9);
        cout << "[1]. Admin Login\n";
        cout << "[2]. Passenger Login/Register\n";
        cout << "[0]. Exit\n";
        setColor(7);

        cout << "Choice: ";
        cin >> choice;

        // ================= ADMIN =================
        if(choice == 1)
        {
            if(A_adminLogin())
            {
                A_adminMenu();
            }
        }

        // ================= PASSENGER =================
        else if(choice == 2)
        {
            string n, p, ph;

            cout << "Name: ";
            cin >> ws;
            getline(cin, n);

            cout << "Passport: ";
            cin >> p;

            cout << "Phone: ";
            cin >> ph;

            // ================= PHONE VALIDATION =================
            if(ph.length() != 11)
            {
                error("Phone number must contain exactly 11 digits.");
                continue;
            }

            if(ph[0] != '0' || ph[1] != '3')
            {
                error("Phone number must start with 03.");
                continue;
            }

            bool valid = true;

            for(int i = 0; i < ph.length(); i++)
            {
                if(!isdigit(ph[i]))
                {
                    valid = false;
                    break;
                }
            }

            if(!valid)
            {
                error("Phone number must contain digits only.");
                continue;
            }

            // ================= CHECK EXISTING USER =================
            A_Passenger* temp = passengerHead;

            bool found = false;

            while(temp)
            {
                // LOGIN EXISTING USER
                if(temp->phone == ph &&
                   temp->passport == p)
                {
                    success("Login Successful!");

                    A_userMenu(temp);

                    found = true;
                    break;
                }

                temp = temp->next;
            }

            // ================= REGISTER NEW USER =================
            if(!found)
            {
                // CHECK IF PASSPORT ALREADY EXISTS
                if(passportExists(p))
                {
                    error("This passport is already registered with another phone number.");
                    continue;
                }

                // CREATE NEW ACCOUNT
                A_Passenger* u =
                A_addUser(n, ph, p);

                success("Registration Successful!");

                A_userMenu(u);
            }
        }

        // ================= EXIT =================
        else if(choice == 0)
        {
            success("Exiting Airline System...");
        }

        else
        {
            error("Invalid Choice!");
        }

    }
    while(choice != 0);

    // ================= SAVE DATA =================
    A_saveFlights();
    A_saveUsers();
    A_saveBookings();
    A_saveWaitingList();
    A_saveFeedback();

    // ================= CLEAN MEMORY =================
    A_cleanup();
}

