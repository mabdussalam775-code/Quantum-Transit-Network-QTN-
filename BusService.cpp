#include "busService.h"

// ================= GLOBAL DEFINITIONS =================
Bus* busHead = NULL;
BusPassenger* busPassengerHead = NULL;
BusBooking* busBookingHead = NULL;
BusFeedback* busFeedbackHead = NULL;
BusWaitNode* busFront = NULL;
BusWaitNode* busRear = NULL;
BusRoute* routeHead = NULL;

int busBookingCounter = 1;
int busUserCounter = 1;

const string ADMIN_NAME = "admin";
const string ADMIN_PASSWORD = "bus@2026";

// ================= UI / SYSTEM =================
void B_setColor(int color)
 {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void B_header(string title)
{
    B_setColor(11);
    cout << "\n============================================================\n";
    cout << "                " << title << endl;
    cout << "============================================================\n";
    B_setColor(7);
}

void B_success(string msg) {
    B_setColor(10);
    cout << "[SUCCESS] " << msg << endl;
    B_setColor(7);
}

void B_error(string msg) {
    B_setColor(12);
    cout << "[ERROR] " << msg << endl;
    B_setColor(7);
}

void B_info(string msg) {
    B_setColor(14);
    cout << "[INFO] " << msg << endl;
    B_setColor(7);
}

// ================= BUS FUNCTIONS =================
Bus* findBus(int id)
{
    Bus* temp = busHead;
    while (temp != NULL)
    {
        if (temp->id == id) return temp;
        temp = temp->next;
    }
    return NULL;
}

void addBus()
 {
    Bus* b = new Bus();
    cout << "Enter Bus ID: ";
    cin >> b->id;
    if (findBus(b->id))
    {
        B_error("Bus ID already exists!");
        delete b;
        return;
    }
    cout<<"Bus Name: ";
    cin >> ws;
    getline(cin, b->name);
    cout<<"Source: ";
    getline(cin, b->source);
    cout<<"Destination: ";
    getline(cin, b->destination);
    cout<<"Ticket Price: ";
    cin >> b->price;
    cout<<"Total Seats: ";
    cin >> b->seats;
    b->bookedSeats = 0;
    b->next = busHead;
    busHead = b;
    B_success("Bus added successfully!");
}
void viewBuses()
{
    B_header("AVAILABLE BUSES");

    Bus* temp = busHead;

    if(temp == NULL)
    {
        B_info("No buses in system.");
        return;
    }

    while(temp != NULL)
    {
        B_setColor(11);
        cout << "\n--------------------------------------------------------\n";
        B_setColor(7);

        cout << left;

        cout << setw(20) << "Bus ID"
             << ": " << temp->id << endl;

        cout << setw(20) << "Bus Name"
             << ": " << temp->name << endl;

        cout << setw(20) << "Route"
             << ": " << temp->source
             << " -> "
             << temp->destination << endl;

        cout << setw(20) << "Ticket Price"
             << ": PKR " << temp->price << endl;

        cout << setw(20) << "Booked Seats"
             << ": "
             << temp->bookedSeats
             << "/"
             << temp->seats << endl;

        // Seat Status
        if(temp->bookedSeats == temp->seats)
        {
            B_setColor(12);
            cout << setw(20) << "Status"
                 << ": FULLY BOOKED" << endl;
            B_setColor(7);
        }
        else
        {
            B_setColor(10);
            cout << setw(20) << "Status"
                 << ": Available" << endl;
            B_setColor(7);
        }

        B_setColor(11);
        cout << "\n--------------------------------------------------------\n";
        B_setColor(7);

        temp = temp->next;
    }
}
void deleteBus()
{
    int id;

    cout << "Enter Bus ID to remove: ";
    cin >> id;

    Bus *curr = busHead, *prev = NULL;

    while(curr!=NULL)
    {
        if(curr->id == id)
        {
            // REMOVE BUS FROM LIST

            if(prev)
            {
                prev->next = curr->next;
            }
            else
            {
                busHead = curr->next;
            }

            // REMOVE RELATED BOOKINGS

            BusBooking* bk = busBookingHead;
            BusBooking* prevBk = NULL;

            while(bk!=NULL)
            {
                if(bk->busId == id)
                {
                    if(prevBk)
                    {
                        prevBk->next = bk->next;
                    }
                    else
                    {
                        busBookingHead = bk->next;
                    }

                    BusBooking* del = bk;
                    bk = bk->next;

                    delete del;
                }
                else
                {
                    prevBk = bk;
                    bk = bk->next;
                }
            }

            // REMOVE WAITING LIST ENTRIES

            BusWaitNode* w = busFront;
            BusWaitNode* prevW = NULL;

            while(w!=NULL)
            {
                if(w->busId == id)
                {
                    if(prevW)
                    {
                        prevW->next = w->next;
                    }
                    else
                    {
                        busFront = w->next;
                    }

                    if(w == busRear)
                    {
                        busRear = prevW;
                    }

                    BusWaitNode* del = w;
                    w = w->next;

                    delete del;
                }
                else
                {
                    prevW = w;
                    w = w->next;
                }
            }

            // DELETE BUS

            delete curr;

            B_success("Bus removed from system.");

            return;
        }

        prev = curr;
        curr = curr->next;
    }

    B_error("Bus not found.");
}
void updateBus()
{
    int id;
    cout << "Enter Bus ID to update: ";
    cin >> id;
    Bus* b = findBus(id);
    if (b)
    {
        cout << "New Price: ";
        cin >> b->price;
        cout << "New Total Capacity: ";
        cin >> b->seats;
        B_success("Bus details updated.");
        assignFromBusWaiting(id);
    } else B_error("Bus not found.");
}

void assignFromBusWaiting(int busId)
{
    Bus* b = findBus(busId);

    if(b==NULL)
    {
        return;
    }

    BusWaitNode* curr = busFront;
    BusWaitNode* prev = NULL;

   while(curr!=NULL)
   {
        if(b->bookedSeats >= b->seats)
        {
            break;
        }
        if(curr->busId == busId)
        {
            // CREATE BOOKING FROM WAITING USER

            BusBooking* bk = new BusBooking();

            bk->id = busBookingCounter++;
            bk->busId = busId;
            bk->passengerId = curr->passengerId;
            bk->next = busBookingHead;

            busBookingHead = bk;

            b->bookedSeats++;

            B_info("Waiting passenger promoted to confirmed booking.");

            // REMOVE FROM QUEUE

            BusWaitNode* del = curr;

            if(prev)
            {
                prev->next = curr->next;
            }
            else
            {
                busFront = curr->next;
            }

            if(curr == busRear)
            {
                busRear = prev;
            }

            curr = curr->next;

            delete del;

            // continue checking next available seat
        }
        else
        {
            prev = curr;
            curr = curr->next;
        }
    }

    if(busFront == NULL)
    {
        busRear = NULL;
    }
}
void promoteOneFromBusWaiting(int busId)
{
    Bus* b = findBus(busId);
    if(!b) return;

    if(busFront == NULL) return;

    BusWaitNode* curr = busFront;
    BusWaitNode* prev = NULL;

    while(curr)
    {
        if(curr->busId == busId)
        {
            // CREATE BOOKING
            BusBooking* bk = new BusBooking();

            bk->id = busBookingCounter++;
            bk->busId = busId;
            bk->passengerId = curr->passengerId;
            bk->next = busBookingHead;

            busBookingHead = bk;

            b->bookedSeats++;

            B_info("Waiting passenger promoted after cancellation.");

            // REMOVE NODE FROM QUEUE
            if(prev)
                prev->next = curr->next;
            else
                busFront = curr->next;

            if(curr == busRear)
                busRear = prev;

            delete curr;

            if(busFront == NULL)
                busRear = NULL;

            return; //ONLY ONE PROMOTION
        }

        prev = curr;
        curr = curr->next;
    }
}
// ================= PASSENGER FUNCTIONS =================
BusPassenger* addBusUser(string name, long long phone)
{
    BusPassenger* p = new BusPassenger();
    p->id = busUserCounter++;
    p->name = name;
    p->phone = phone;
    p->next = busPassengerHead;
    busPassengerHead = p;
    return p;
}
BusPassenger* findBusPassenger(int id)
{
    BusPassenger* temp = busPassengerHead;
    while (temp) {
        if (temp->id == id) return temp;
        temp = temp->next;
    }
    return NULL;
}

BusPassenger* findBusUserByPhone(long long phone) {
    BusPassenger* temp = busPassengerHead;
    while (temp) {
        if (temp->phone == phone) return temp;
        temp = temp->next;
    }
    return NULL;
}
// ================= BOOKING FUNCTIONS =================
void enqueueWaiting(int pid, int bid)
{
    BusWaitNode* n = new BusWaitNode();
    n->passengerId = pid;
    n->busId = bid;
    n->next = NULL;

    if(busFront == NULL)
        busFront = busRear = n;
    else
    {
        busRear->next = n;
        busRear = n;
    }
}
void dequeueWaiting()
{
    if(busFront == NULL)
    {
        return;
    }

    BusWaitNode* temp = busFront;

    busFront = busFront->next;

    if(busFront == NULL)
    {
        busRear = NULL;
    }

    delete temp;
}
void B_viewWaitingList()
{
    B_header("BUS WAITING LIST");

    if(busFront == NULL)
    {
        B_info("Waiting list empty.");
        return;
    }

    BusWaitNode* temp = busFront;

    while(temp)
    {
        BusPassenger* p = findBusPassenger(temp->passengerId);
        Bus* b = findBus(temp->busId);

        cout << "Passenger ID : " << temp->passengerId << endl;
        cout << "Passenger    : " << (p ? p->name : "N/A") << endl;
        cout << "Bus          : " << (b ? b->name : "N/A") << endl;
        cout << "-----------------------------------\n";

        temp = temp->next;
    }
}
void findBusByRoute()
{
    string src, dest;

    cout << "Enter Source: ";
    cin >> ws;
    getline(cin, src);

    cout << "Enter Destination: ";
    getline(cin, dest);

    Bus* temp = busHead;

    bool found = false;

    while(temp)
    {
        if(temp->source == src &&
           temp->destination == dest)
        {
            cout << "\nBus ID      : " << temp->id << endl;
            cout << "Bus Name    : " << temp->name << endl;
            cout << "Route       : " << temp->source
                 << " -> "
                 << temp->destination << endl;
            cout << "Price       : " << temp->price << endl;
            cout << "Seats       : "
                 << temp->bookedSeats
                 << "/"
                 << temp->seats << endl;

            found = true;
        }

        temp = temp->next;
    }

    if(!found)
    {
        B_error("No buses found.");
    }
}
bool B_alreadyBooked(int pid, int bid)
{
    BusBooking* bk = busBookingHead;

    while(bk)
    {
        if(bk->passengerId == pid && bk->busId == bid)
        {
            return true;
        }
        bk = bk->next;
    }

    return false;
}
void bookBusTicket(BusPassenger* user)
{
    int bid;

    viewBuses();

    cout << "Enter Bus ID to book: ";
    cin >> bid;

    Bus* b = findBus(bid);

    if(!b)
    {
        B_error("Invalid Bus ID.");
        return;
    }

    //IMPROVEMENT 3 — DUPLICATE BOOKING CHECK
    if(B_alreadyBooked(user->id, bid))
    {
        B_error("You already booked this bus.");
        return;
    }

    if(b->bookedSeats < b->seats)
    {
        BusBooking* bk = new BusBooking();

        bk->id = busBookingCounter++;
        bk->busId = bid;
        bk->passengerId = user->id;
        bk->next = busBookingHead;

        busBookingHead = bk;

        b->bookedSeats++;

        printBusTicket(user, b, bk->id);

        B_success("Booking confirmed!");
    }
    else
    {
        enqueueWaiting(user->id, bid);

        B_info("Bus full. Added to waiting list.");
    }
}
void cancelBusBooking(BusPassenger* user)
{
    int id;

    cout << "Enter Booking ID to cancel: ";
    cin >> id;

    BusBooking *curr = busBookingHead, *prev = NULL;

    while(curr)
    {
        if(curr->id == id && curr->passengerId == user->id)
        {
            Bus* b = findBus(curr->busId);

            if(b)
            {
                b->bookedSeats--;
            }

            int busId = curr->busId;

            // REMOVE BOOKING NODE
            if(prev)
                prev->next = curr->next;
            else
                busBookingHead = curr->next;

            delete curr;

            B_success("Booking cancelled.");

            //STRICT CONTROLLED PROMOTION
            promoteOneFromBusWaiting(busId);

            return;
        }

        prev = curr;
        curr = curr->next;
    }

    B_error("Booking not found.");
}
void printBusTicket(BusPassenger* user, Bus* b, int bookingId) {
    cout << "\n========== BUS TICKET ==========\n";
    cout << "Booking ID : " << bookingId << endl;
    cout << "Passenger  : " << user->name << endl;
    cout << "Phone      : " << user->phone << endl;
    cout << "Bus Name   : " << b->name << endl;
    cout << "Route      : " << b->source << " -> " << b->destination << endl;
    cout << "Price      : " << b->price << endl;
    cout << "================================\n\n";
}
void viewMyBusBookings(BusPassenger* user)
{
    B_header("YOUR BOOKINGS");
    BusBooking* bk = busBookingHead;
    bool found = false;
    while (bk)
    {
        if (bk->passengerId == user->id)
        {
            Bus* b = findBus(bk->busId);
            cout << "Booking ID: " << bk->id << " | Bus: " << (b ? b->name : "N/A")
                 << " | Route: " << (b ? b->source + " to " + b->destination : "N/A") << endl;
            found = true;
        }
        bk = bk->next;
    }
    if (!found) B_info("No bookings found.");
}
void B_viewAllBookingsWithUsers()
{
    B_header("ALL BOOKINGS");

    BusBooking* bk = busBookingHead;

    if(bk == NULL)
    {
        B_info("No bookings found.");
        return;
    }

    while(bk)
    {
        BusPassenger* p = findBusPassenger(bk->passengerId);

        Bus* b = findBus(bk->busId);

        cout << "Booking ID : " << bk->id << endl;

        cout << "Passenger  : "
             << (p ? p->name : "N/A") << endl;

        cout << "Phone      : "
             << (p ? p->phone : 0) << endl;

        cout << "Bus        : "
             << (b ? b->name : "N/A") << endl;

        cout << "Route      : "
             << (b ? b->source : "N/A")
             << " -> "
             << (b ? b->destination : "N/A")
             << endl;

        cout << "-----------------------------------\n";

        bk = bk->next;
    }
}

//================================ROUTES===========================
void addBusRoute()
{
    BusRoute* r = new BusRoute();

    cout << "Enter Source: ";
    cin >> ws;
    getline(cin, r->source);

    cout << "Enter Destination: ";
    getline(cin, r->destination);

    cout << "Enter Distance: ";
    cin >> r->distance;

    r->next = routeHead;

    routeHead = r;

    B_success("Route added successfully.");
}
void viewBusRoutes()
{
    B_header("ALL ROUTES");

    BusRoute* temp = routeHead;

    if(temp == NULL)
    {
        B_info("No routes found.");
        return;
    }

    while(temp!=NULL)
    {
        cout << temp->source
             << " -> "
             << temp->destination
             << " | Distance: "
             << temp->distance
             << " KM"
             << endl;

        temp = temp->next;
    }
}
void removeBusRoute()
{
    string src, dest;

    cout << "Enter Source: ";
    cin >> ws;
    getline(cin, src);

    cout << "Enter Destination: ";
    getline(cin, dest);

    BusRoute* curr = routeHead;
    BusRoute* prev = NULL;

    while(curr)
    {
        if(curr->source == src &&curr->destination == dest)
        {
            if(prev)
            {
                prev->next = curr->next;
            }
            else
            {
                routeHead = curr->next;
            }

            delete curr;

            B_success("Route removed.");

            return;
        }

        prev = curr;
        curr = curr->next;
    }

    B_error("Route not found.");
}
//===========================REPORT===========================
void B_advancedReport()
{
    B_header("ADVANCED REPORT");

    int totalBuses = 0;
    int totalUsers = 0;
    int totalBookings = 0;
    double revenue = 0;

    Bus* b = busHead;

    while(b)
    {
        totalBuses++;

        revenue += b->bookedSeats * b->price;

        b = b->next;
    }

    BusPassenger* p = busPassengerHead;

    while(p)
    {
        totalUsers++;

        p = p->next;
    }

    BusBooking* bk = busBookingHead;

    while(bk)
    {
        totalBookings++;

        bk = bk->next;
    }

    cout << "Total Buses      : "
         << totalBuses << endl;

    cout << "Total Users      : "
         << totalUsers << endl;

    cout << "Total Bookings   : "
         << totalBookings << endl;

    cout << "Total Revenue    : "
         << revenue << endl;
}

// ================= FEEDBACK FUNCTIONS =================
void submitBusFeedback(BusPassenger* user)
{
    BusFeedback* f = new BusFeedback();
    f->userId = user->id;
    f->name = user->name;
    cout << "Enter your feedback: ";
    cin >> ws;
    getline(cin, f->message);
    f->next = busFeedbackHead;
    busFeedbackHead = f;
    B_success("Feedback submitted!");
}

void viewBusFeedback()
{
    B_header("USER FEEDBACK");
    BusFeedback* f = busFeedbackHead;
    while (f!=NULL)
    {
        cout<<f->name<<" (ID: "<< f->userId<<"): "<<f->message<<endl;
        f = f->next;
    }
}

// ================= FILE HANDLING =================
void B_saveBusData()
{
    ofstream out("BusData.txt");
    Bus* b = busHead;
    while (b!=NULL)
    {
        out<< b->id<<endl
        <<b->name<<endl
        <<b->source<<endl
        <<b->destination<<endl
        <<b->price<<endl
        <<b->seats<<endl
        <<b->bookedSeats<<endl;
        b = b->next;
    }
    out.close();
}

void B_loadBusData()
{
    ifstream in("BusData.txt");
    if (!in) return;
    while (true)
    {
        Bus* b = new Bus();
        if (!(in >> b->id))
        {
            delete b;
            break;
        }
        in>>ws;
        getline(in, b->name);
        getline(in, b->source);
        getline(in, b->destination);
        in>>b->price>>b->seats>>b->bookedSeats;
        b->next = busHead;
        busHead = b;
    }
    in.close();
}

void B_saveBusUsers()
{
    ofstream out("BusUsers.txt");
    BusPassenger* p = busPassengerHead;
    while (p!=NULL)
    {
        out << p->id << endl << p->name << endl << p->phone << endl;
        p = p->next;
    }
    out.close();
}

void B_loadBusUsers()
{
    ifstream in("BusUsers.txt");
    if (!in) return;
    while (true)
    {
        BusPassenger* p = new BusPassenger();
        if (!(in >> p->id))
        {
            delete p;
             break;
        }
        in >> ws; getline(in, p->name);
        in >> p->phone;
        p->next = busPassengerHead;
        busPassengerHead = p;
        if (p->id >= busUserCounter)
        {
            busUserCounter = p->id + 1;
        }
    }
    in.close();
}

void B_saveBusBookings()
{
    ofstream out("BusBookings.txt");
    BusBooking* bk = busBookingHead;
    while (bk!=NULL)
    {
        out << bk->id << " " << bk->busId << " " << bk->passengerId << endl;
        bk = bk->next;
    }
    out.close();
}

void B_loadBusBookings()
{
    ifstream in("BusBookings.txt");
    if (!in) return;
    int id, bid, pid;
    while (in >> id >> bid >> pid)
    {
        BusBooking* bk = new BusBooking();
        bk->id = id;
        bk->busId = bid;
        bk->passengerId = pid;
        bk->next = busBookingHead;
        busBookingHead = bk;
        if (id >= busBookingCounter)
        {
            busBookingCounter = id + 1;
        }
    }
    in.close();
}

void B_saveBusWaitingList()
{
    ofstream out("BusWaightings.txt");
    BusWaitNode* temp = busFront;
    while (temp!=NULL)
    {
        out << temp->passengerId << " " << temp->busId << endl;
        temp = temp->next;
    }
    out.close();
}

void B_loadBusWaitingList()
{
    ifstream in("BusWaightings.txt");
    int pid, bid;

    while(in >> pid >> bid)
    {
        enqueueWaiting(pid, bid);
    }
}
void B_saveBusFeedback()
{
    ofstream out("BusServiceFeedbacks.txt");
    BusFeedback* f = busFeedbackHead;
    while (f!=NULL)
    {
        out << f->userId << endl << f->name << endl << f->message << endl;
        f = f->next;
    }
    out.close();
}

void B_loadBusFeedback()
{
    ifstream in("BusServiceFeedbacks.txt");
    if (!in) return;
    while (true)
    {
        BusFeedback* f = new BusFeedback();
        if (!(in >> f->userId))
        {
            delete f;
            break;
        }
        in >> ws;
        getline(in, f->name);
        getline(in, f->message);
        f->next = busFeedbackHead;
        busFeedbackHead = f;
    }
}
void B_saveRoutes()
{
    ofstream out("BusRoutes.txt");

    BusRoute* temp = routeHead;

    while(temp!=NULL)
    {
        out << temp->source << endl;
        out << temp->destination << endl;
        out << temp->distance << endl;

        temp = temp->next;
    }

    out.close();
}
void B_loadRoutes()
{
    ifstream in("BusRoutes.txt");

    if(!in)
    {
        return;
    }

    while(!in.eof())
    {
        BusRoute* r = new BusRoute();

        if(!getline(in, r->source))
        {
            delete r;
            break;
        }

        getline(in, r->destination);

        in >> r->distance;

        in.ignore();

        r->next = routeHead;

        routeHead = r;
    }

    in.close();
}
// ================= MENUS =================
void busAdminMenu()
{
    int ch;

    do
    {
        B_header("BUS ADMIN CONTROL PANEL");
        B_setColor(9);
        cout << "[1]. Add Bus\n";
        cout << "[2]. View Buses\n";
        cout << "[3]. Delete Bus\n";
        cout << "[4]. Update Bus\n";
        cout << "[5]. View Waiting List\n";
        cout << "[6]. View Feedback\n";
        cout << "[7]. View All Bookings\n";
        cout << "[8]. Search Bus By Route\n";
        cout << "[9]. Add Route\n";
        cout << "[10]. Remove Route\n";
        cout << "[11]. View Routes\n";
        cout << "[12]. Advanced Report\n";
        cout << "[0]. Exit\n";
        B_setColor(7);
        cout << "Choice: ";
        cin >> ch;

        switch(ch)
        {
            case 1:
                addBus();
                break;

            case 2:
                viewBuses();
                break;

            case 3:
                deleteBus();
                break;

            case 4:
                updateBus();
                break;

            case 5:
                B_viewWaitingList();
                break;

            case 6:
                viewBusFeedback();
                break;

            case 7:
                B_viewAllBookingsWithUsers();
                break;

            case 8:
                findBusByRoute();
                break;

            case 9:
                addBusRoute();
                break;

            case 10:
                removeBusRoute();
                break;

            case 11:
                viewBusRoutes();
                break;

            case 12:
                B_advancedReport();
                break;
        }

    } while(ch != 0);
}
void busUserMenu(BusPassenger* u)
{
    int ch;

    do
    {
        B_header("PASSENGER DASHBOARD - " + u->name);
        B_setColor(9);
        cout << "[1]. View Buses\n";
        cout << "[2]. Search Bus\n";
        cout << "[3]. Book Ticket\n";
        cout << "[4]. My Bookings\n";
        cout << "[5]. Cancel Booking\n";
        cout << "[6]. Feedback\n";
        cout << "[7]. View Routes\n";
        cout << "[0]. Logout\n";
        B_setColor(7);
        cout << "Choice: ";

        cin >> ch;

        switch(ch)
        {
            case 1:
                viewBuses();
                break;

            case 2:
                findBusByRoute();
                break;

            case 3:
                bookBusTicket(u);
                break;

            case 4:
                viewMyBusBookings(u);
                break;

            case 5:
                cancelBusBooking(u);
                break;

            case 6:
                submitBusFeedback(u);
                break;

            case 7:
                viewBusRoutes();
                break;
        }

    } while(ch != 0);
}
bool busAdminLogin()
{
    string name, password;

    cout<<"Enter Admin Name: ";
    cin>>name;

    cout<<"Enter Admin Password: ";
    cin>>password;

    return (name == ADMIN_NAME && password == ADMIN_PASSWORD);
}
void cleanupBusSystem()
{
    while(busHead)
    {
        Bus* t = busHead;
        busHead = busHead->next;
        delete t;
    }

    while(busPassengerHead)
    {
        BusPassenger* t = busPassengerHead;
        busPassengerHead = busPassengerHead->next;
        delete t;
    }

    while(busBookingHead)
    {
        BusBooking* t = busBookingHead;
        busBookingHead = busBookingHead->next;
        delete t;
    }

    while(busFeedbackHead)
    {
        BusFeedback* t = busFeedbackHead;
        busFeedbackHead = busFeedbackHead->next;
        delete t;
    }

    while(busFront)
    {
        BusWaitNode* t = busFront;
        busFront = busFront->next;
        delete t;
    }

    while(routeHead)
    {
        BusRoute* t = routeHead;
        routeHead = routeHead->next;
        delete t;
    }
}
void  BusServiceMenu()
{
    B_loadBusData();
    B_loadBusUsers();
    B_loadBusBookings();
    B_loadBusFeedback();
    B_loadBusWaitingList();
    B_loadRoutes();

    int choice;
    while (true)
    {
        B_header("QUANTUM BUS RESERVATION NETWORK");
        cout << "[1]. Admin Login\n[2]. User Login/Register\n[0]. Exit\nChoice: ";
        cin >> choice;
        if (choice == 1)
        {
            if (busAdminLogin())
            {
                busAdminMenu();
            }
            else B_error("Invalid Password!");
        }
        else if (choice == 2)
        {
            string name;
            long long phone;
            cout << "Enter Phone: ";
            cin >> phone;
            BusPassenger* u = findBusUserByPhone(phone);
            if (!u)
            {
                cout << "Register Name: "; cin >> ws; getline(cin, name);
                u = addBusUser(name, phone);
            }
            busUserMenu(u);
        }
        else if (choice == 0)
        {
            break;
        }

        B_saveBusData();
        B_saveBusUsers();
        B_saveBusBookings();
        B_saveBusFeedback();
        B_saveBusWaitingList();
        B_saveRoutes();
    }
    cleanupBusSystem();
}
