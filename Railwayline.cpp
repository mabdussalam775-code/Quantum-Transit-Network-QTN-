#include "RailwayLine.h"

Train* trainHeadR = NULL;
Passenger* passengerHeadR = NULL;
Booking* bookingHeadR = NULL;
Feedback* feedbackHeadR = NULL;

WaitNode* frontR = NULL;
WaitNode* rearR = NULL;

int bookingCounterR = 1;
int userCounterR = 1;

Graph railwayGraph;
const int CITY_COUNT = 10;
const string ADMIN_PASSWORD = "railway@2026";

//=============================UI==================================
void R_setColor(int color)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void R_header(string title)
{
    R_setColor(11);
    cout << "\n============================================================\n";
    cout << "                " << title << endl;
    cout << "============================================================\n";
    R_setColor(7);
}
void R_success(string msg)
{
    R_setColor(10); cout << "[SUCCESS] " << msg << endl;  R_setColor(7);
}

void R_error(string msg)
{
    R_setColor(12); cout << "[ERROR] " << msg << endl;  R_setColor(7);
}

void R_info(string msg)
{
    R_setColor(14); cout << "[INFO] " << msg << endl;  R_setColor(7);
}
Train* findTrain(int id)
{
    Train* t = trainHeadR;
    while(t!=NULL)
    {
        if(t->id == id)
        {
            return t;
        }
        t = t->next;
    }
    return NULL;
}
void addTrain()
{
    Train* t = new Train();

    cout<<"Enter Train ID: ";
    cin>>t->id;

    if(findTrain(t->id))
    {
        R_error("Train already exists!");
        delete t;
        return;
    }

    cout<<"Train Name: ";
    cin>>ws;
    getline(cin,t->name);

    showCities();

    int src,dest;

    cout<<"Select Source City Number: ";
    cin>>src;
    cout<<"Select Destination City Number: ";
    cin>>dest;

    if(src < 0 || src >= CITY_COUNT || dest < 0 || dest >= CITY_COUNT)
    {
        R_error("Invalid City Selection!");
        delete t;
        return;
    }
    if(src == dest)
    {
        R_error("Source and Destination cannot be same!");
        delete t;
        return;
    }
    t->source = railwayGraph.cities[src];
    t->destination = railwayGraph.cities[dest];
    t->distance = dijkstra(src, dest);

    if(t->distance == 99999)
    {
        R_error("No Route Exists Between Cities!");
        delete t;
        return;
    }
    t->price = t->distance * 5;

    cout<<"Total Seats: ";
    cin>>t->seats;

    //NEW: seat validation
    if(t->seats <= 0)
    {
        R_error("Invalid seat count!");
        delete t;
        return;
    }

    t->bookedSeats = 0;

    t->next = NULL;

    // INSERTION AT END
    if(trainHeadR==NULL)
    {
        trainHeadR = t;
    }
    else
    {
        Train* temp = trainHeadR;
        while(temp->next!=NULL)
        {
            temp = temp->next;
        }
        temp->next = t;
    }

    R_success("Train Added Successfully!");
}
void viewTrains()
{
    Train* t = trainHeadR;

    R_header("AVAILABLE TRAINS");

    if(t == NULL)
    {
        R_error("No trains available!");
        return;
    }

    while(t != NULL)
    {
        cout << "Train ID   : " << t->id << endl;
        cout << "Name       : " << t->name << endl;
        cout << "Source      : " << t->source << endl;
        cout << "Destination : " << t->destination << endl;
        cout << "Distance    : "<< t->distance<< " KM" << endl;
        cout << "Price      : " << t->price << endl;
        cout << "Seats      : " << t->bookedSeats << "/" << t->seats << endl;
        R_setColor(13);
        cout<<"------------------------------------------------------------\n";
        R_setColor(7);

        t = t->next;
    }
}
void deleteTrain()
{
    int id;
    R_header("DELETE DEACTIVATION");
    cout << "Enter Train ID to Deactivate: ";
    cin>>id;

    Train* t = trainHeadR;
    Train* prev = NULL;

    while(t!=NULL)
    {
        if(t->id == id)
        {
            if(prev!=NULL)
            {
                prev->next = t->next;
            }
            else
            {
                trainHeadR = t->next;
            }

            delete t;
            cout<<"Train Deactivated Successfully!\n";
            return;
        }
        prev = t;
        t = t->next;
    }

    cout<<"Train is Not Found!\n";
}
void assignFromWaiting(int trainId)
{
    if(frontR==NULL)
    {
       return;
    }

    WaitNode* temp = frontR;
    WaitNode* prev = NULL;

    while(temp!=NULL)
    {
        if(temp->trainId == trainId)
        {
            Train* t = findTrain(trainId);

            if(t!=NULL && t->bookedSeats < t->seats)
            {
                Booking* b = new Booking();
                b->id = bookingCounterR++;
                b->trainId = trainId;
                b->passengerId = temp->passengerId;

                b->next = bookingHeadR;
                bookingHeadR = b;

                t->bookedSeats++;

                cout<<"Waiting passenger allocated seat!\n";

                // REMOVE from queue
                if(prev!=NULL)
                {
                    prev->next = temp->next;
                }
                else
                {
                    frontR = temp->next;
                }

                if(temp == rearR)
                {
                    rearR = prev;
                }

                delete temp;
                saveWaitingList();
                return;
            }
        }
        prev = temp;
        temp = temp->next;
    }
}
void updateTrain()
{
    int id;
    cout<<"Enter Train ID: ";
    cin>>id;

    Train* t = findTrain(id);

    if(t==NULL)
    {
        cout<<"Not Found!\n";
        return;
    }

    t->price = t->distance * 6;
    cout<<"New Seats: ";
    cin >> t->seats;

    cout<<"Updated Successfully!\n";
    while(t->bookedSeats < t->seats)
    {

         int before = t->bookedSeats;

         assignFromWaiting(id);

         if(before == t->bookedSeats)
         break;
    }
}
// ================= PASSENGER =================
Passenger* addUser(string name, long long phone)
{
    Passenger* p = new Passenger();

    p->id = userCounterR++;
    p->name = name;
    p->phone = phone;

    p->next = passengerHeadR;
    passengerHeadR = p;

    return p;
}
Passenger* findPassenger(int id)
{
    Passenger* p = passengerHeadR;

    while(p!=NULL)
    {
        if(p->id == id)
        {
           return p;
        }
        p = p->next;
    }

    return NULL;
}
// ================= BOOKING =================
void addToWaiting(int pid, int tid)
{
    WaitNode* n = new WaitNode();
    n->passengerId = pid;
    n->trainId = tid;
    n->next = NULL;

    if(frontR==NULL)
    {
        frontR = rearR = n;
    }
    else
    {
        rearR->next = n;
        rearR = n;
    }
    saveWaitingList();
}
void viewWaitingList()
{
    if(frontR==NULL)
    {
        cout<<"No passengers in waiting list!\n";
        return;
    }

    WaitNode* temp = frontR;

    cout<<"\n===== WAITING LIST =====\n";

    while(temp!=NULL)
    {
        Passenger* p = findPassenger(temp->passengerId);

        cout<<"Passenger ID: "<<temp->passengerId;

        if(p!=NULL)
        {
            cout<<" | Name: "<<p->name;
        }
        else
        {
            cout<<" | Name: Not Found";
        }
        cout<<" | Train ID: " <<temp->trainId<<endl;

        temp = temp->next;
    }
}
void bookTicket(Passenger* user)
{
    int id;
    cout<<"Enter Train ID: ";
    cin>>id;

    Train* t = findTrain(id);

    if(t==NULL)
    {
        cout<<"Train Not Found!\n";
        return;
    }
    Booking* temp = bookingHeadR;

   while(temp)
   {
      if(temp->passengerId == user->id &&temp->trainId == id)
       {
        R_error("You already booked this train!");
        return;
       }

    temp = temp->next;
}
    if(t->bookedSeats < t->seats)
    {
        Booking* b = new Booking();
        b->id = bookingCounterR++;
        b->trainId = id;
        b->passengerId = user->id;

        b->next = bookingHeadR;
        bookingHeadR = b;

        t->bookedSeats++;

        printTicket(user, t, b->id);
    }
    else
    {
        addToWaiting(user->id, id);
        cout<<"Added to Waiting List!\n";
    }
}
void viewMyBookings(Passenger* user)
{
    Booking* b = bookingHeadR;
    bool found = false;

    while(b!=NULL)
    {
        if(b->passengerId == user->id)
        {
            cout<<"----------------------\n";
            cout<<"Booking ID: "<<b->id<<"\n Train ID: "<<b->trainId<<endl;
            cout<<"----------------------\n";
            found = true;
        }
        b = b->next;
    }
    if(found==false)
    {
        cout << "No Bookings!\n";
    }
}
void viewAllBookingsWithUsers()
{
    if (bookingHeadR == NULL)
    {
        R_error("No bookings found!");
        return;
    }

    Booking* b = bookingHeadR;

    R_header("ALL BOOKINGS WITH USER DETAILS");

    while (b != NULL)
    {
        Passenger* p = findPassenger(b->passengerId);
        Train* t = findTrain(b->trainId);

        R_setColor(13);
        cout<<"------------------------------------------------------------\n";
        R_setColor(7);
        R_info("Booking ID: " + to_string(b->id));
        R_setColor(13);
        cout<<"------------------------------------------------------------\n";
        R_setColor(7);

        // Passenger Section
        cout << "[ Passenger Info ]\n";
        if (p != NULL)
        {
            cout << left << setw(20) << "Passenger ID:" << p->id << endl;
            cout << left << setw(20) << "Name:" << p->name << endl;
            cout << left << setw(20) << "Phone:" << p->phone << endl;
        }
        else
        {
            R_error("Passenger Not Found");
        }

        // Train Section
        cout << "\n[ Train Info ]\n";
        if (t != NULL)
        {
            cout << left << setw(20) << "Train ID:" << t->id << endl;
            cout << left << setw(20) << "Train Name:" << t->name << endl;
            cout << left << setw(20)<< "Source:"<< t->source << endl;
            cout << left << setw(20)<< "Destination:"<< t->destination << endl;
            cout << left << setw(20)<< "Distance:"<< t->distance<< " KM" << endl;
        }
        else
        {
            R_error("Train Not Found");
        }

        R_setColor(13);
        cout<<"------------------------------------------------------------\n";
        R_setColor(7);

        b = b->next;
    }

    R_success("All bookings displayed successfully!");
}
// ================= FEEDBACK =================
void submitFeedback(Passenger* user)
{
    Feedback* f = new Feedback();

    f->userId = user->id;
    f->name = user->name;

    cout<<"Enter Feedback: ";
    cin>>ws;
    getline(cin, f->message);

    f->next = feedbackHeadR;
    feedbackHeadR = f;

    cout<<"Feedback Submitted!\n";
}
void cancelBooking(Passenger* user)
{
    int bid;
    cout<<"Enter Booking ID to Cancel: ";
    cin>>bid;

    Booking* curr = bookingHeadR;
    Booking* prev = NULL;

    while (curr != NULL)
    {
        if (curr->id == bid && curr->passengerId == user->id)
        {
            Train* t = findTrain(curr->trainId);

            // remove booking node
            if (prev != NULL)
                prev->next = curr->next;
            else
                bookingHeadR = curr->next;

            cout<<"Booking Cancelled Successfully!\n";

            if (t != NULL && t->bookedSeats > 0)
            {
                t->bookedSeats--;
                // try to assign waiting passenger
                assignFromWaiting(t->id);
            }
            delete curr;
            return;
        }

        prev = curr;
        curr = curr->next;
    }

    cout<<"Booking Not Found or Not Owned by You!\n";
}
void viewFeedback()
{
    Feedback* f = feedbackHeadR;

    if(f==NULL)
    {
        cout<<"No Feedback!\n";
        return;
    }

    while(f!=NULL)
    {
        cout<<f->userId<<" | "
            <<f->name<<" | "
            <<f->message<< endl;

        f = f->next;
    }
}
//=====================TICKET PRINTING====================
void printTicket(Passenger* user, Train* t, int bookingId)
{
    R_header("OFFICIAL TRAIN TICKET");

    cout << left;

    cout << setw(20) << "Booking ID" << ": " << bookingId << endl;
    cout << setw(20) << "Passenger ID" << ": " << user->id << endl;
    cout << setw(20) << "Name" << ": " << user->name << endl;
    cout << setw(20) << "Phone" << ": " << user->phone << endl;

    R_setColor(13);
    cout<<"------------------------------------------------------------\n";
    R_setColor(7);

    cout << setw(20) << "Train ID" << ": " << t->id << endl;
    cout << setw(20) << "Train Name" << ": " << t->name << endl;
    cout << setw(20)<< "Source"<< ": "<< t->source<< endl;
    cout << setw(20)<< "Destination"<< ": "<< t->destination<< endl;
    cout << setw(20)<< "Distance"<< ": "<< t->distance<< " KM"<< endl;
    cout << setw(20) << "Price" << ": " << t->price << endl;

    R_setColor(13);
    cout<<"------------------------------------------------------------\n";
    R_setColor(7);

    cout << "STATUS: CONFIRMED " << endl;
    cout << "Have a Safe Journey " << endl;
    R_setColor(13);
    cout << "============================================================\n";
    R_setColor(7);
}
//================== GRAPH =========================
void initializeGraph()
{
    // ===== CITY NAMES =====

    railwayGraph.cities[0] = "Lahore";
    railwayGraph.cities[1] = "Karachi";
    railwayGraph.cities[2] = "Islamabad";
    railwayGraph.cities[3] = "Rawalpindi";
    railwayGraph.cities[4] = "Faisalabad";
    railwayGraph.cities[5] = "Multan";
    railwayGraph.cities[6] = "Peshawar";
    railwayGraph.cities[7] = "Quetta";
    railwayGraph.cities[8] = "Sialkot";
    railwayGraph.cities[9] = "Gujranwala";
    // ===== INITIALIZE MATRIX =====

    for(int i=0; i<CITY_COUNT; i++)
    {
        for(int j=0; j<CITY_COUNT; j++)
        {
            if(i == j)
            {
                railwayGraph.adjMatrix[i][j] = 0;
            }
            else
            {
                railwayGraph.adjMatrix[i][j] = 99999;
            }
        }
    }

// ================= CONNECTIONS =================
// Lahore Connections
railwayGraph.adjMatrix[0][1] = 320;
railwayGraph.adjMatrix[1][0] = 320;

railwayGraph.adjMatrix[0][2] = 380;
railwayGraph.adjMatrix[2][0] = 380;

railwayGraph.adjMatrix[0][4] = 180;
railwayGraph.adjMatrix[4][0] = 180;

railwayGraph.adjMatrix[0][8] = 120;
railwayGraph.adjMatrix[8][0] = 120;

railwayGraph.adjMatrix[0][9] = 80;
railwayGraph.adjMatrix[9][0] = 80;

// Karachi Connections
railwayGraph.adjMatrix[1][5] = 450;
railwayGraph.adjMatrix[5][1] = 450;

railwayGraph.adjMatrix[1][7] = 600;
railwayGraph.adjMatrix[7][1] = 600;

// Islamabad / Rawalpindi
railwayGraph.adjMatrix[2][3] = 20;
railwayGraph.adjMatrix[3][2] = 20;

railwayGraph.adjMatrix[2][6] = 180;
railwayGraph.adjMatrix[6][2] = 180;

// Faisalabad Connections
railwayGraph.adjMatrix[4][5] = 240;
railwayGraph.adjMatrix[5][4] = 240;

railwayGraph.adjMatrix[4][9] = 90;
railwayGraph.adjMatrix[9][4] = 90;

// Multan Connections
railwayGraph.adjMatrix[5][7] = 550;
railwayGraph.adjMatrix[7][5] = 550;

// Peshawar Connections
railwayGraph.adjMatrix[6][3] = 170;
railwayGraph.adjMatrix[3][6] = 170;

// Sialkot / Gujranwala
railwayGraph.adjMatrix[8][9] = 30;
railwayGraph.adjMatrix[9][8] = 30;

// Extra Important Links
railwayGraph.adjMatrix[2][5] = 300;
railwayGraph.adjMatrix[5][2] = 300;

railwayGraph.adjMatrix[0][5] = 340;
railwayGraph.adjMatrix[5][0] = 340;

railwayGraph.adjMatrix[3][4] = 220;
railwayGraph.adjMatrix[4][3] = 220;

railwayGraph.adjMatrix[6][7] = 500;
railwayGraph.adjMatrix[7][6] = 500;

railwayGraph.adjMatrix[8][4] = 150;
railwayGraph.adjMatrix[4][8] = 150;
}
void showCities()
{
    cout << "\n===== PAKISTAN CITIES =====\n";

    for(int i=0; i<CITY_COUNT; i++)
    {
        cout << i << ". "
             << railwayGraph.cities[i]
             << endl;
    }
}
int getMinVertex(int dist[], bool visited[])
{
    int min = 99999;
    int index = -1;

    for(int i=0; i<CITY_COUNT; i++)
    {
        if(!visited[i] && dist[i] < min)
        {
            min = dist[i];
            index = i;
        }
    }

    return index;
}
int dijkstra(int src, int dest)
{
    const int INF = 99999;

    int parent[CITY_COUNT];
    int dist[CITY_COUNT];
    bool visited[CITY_COUNT];

    for(int i = 0; i < CITY_COUNT; i++)
    {
        dist[i] = INF;
        visited[i] = false;
        parent[i] = -1;
    }

    dist[src] = 0;

    for(int count = 0; count < CITY_COUNT - 1; count++)
    {
        int u = getMinVertex(dist, visited);

        if(u == -1)
            break;

        visited[u] = true;

        // IMPORTANT SAFETY CHECK
        if(dist[u] == INF)
            continue;

        for(int v = 0; v < CITY_COUNT; v++)
        {
            if(!visited[v] &&
               railwayGraph.adjMatrix[u][v] != INF &&
               dist[u] + railwayGraph.adjMatrix[u][v] < dist[v])
            {
                dist[v] = dist[u] + railwayGraph.adjMatrix[u][v];
                parent[v] = u;
            }
        }
    }

    // ================= PATH PRINT =================
    cout << "\nShortest Path: ";

    if(dist[dest] == INF)
    {
        cout << "NO PATH EXISTS\n";
        return INF;
    }

    int path[CITY_COUNT];
    int pathCount = 0;

    int current = dest;

    while(current != -1)
    {
        path[pathCount++] = current;
        current = parent[current];
    }

    for(int i = pathCount - 1; i >= 0; i--)
    {
        cout << railwayGraph.cities[path[i]];
        if(i != 0) cout << " -> ";
    }

    cout << endl;

    return dist[dest];
}
void findTrainByRoute()
{
    string src, dest;
    bool found = false;

    cout<<"Enter Source City: ";
    cin>>ws;
    getline(cin, src);

    cout<<"Enter Destination City: ";
    getline(cin, dest);

    Train* t = trainHeadR;

    R_header("SEARCH RESULTS");

    while(t != NULL)
    {
        if(t->source == src && t->destination == dest)
        {
            cout << "Train ID: " << t->id << endl;
            cout << "Name    : " << t->name << endl;
            cout << "Price   : " << t->price << endl;
            cout << "Seats   : " << t->bookedSeats << "/" << t->seats << endl;
            cout << "---------------------------------\n";

            found = true;
        }
        t = t->next;
    }

    if(!found)
        R_error("No Train Found for this Route!");
}
void advancedReport()
{
    int totalBookings = 0;
    double revenue = 0;

    Train* t = trainHeadR;

    while(t != NULL)
    {
        revenue += t->bookedSeats * t->price;
        t = t->next;
    }

    Booking* b = bookingHeadR;

    while(b != NULL)
    {
        totalBookings++;
        b = b->next;
    }

    R_header("SYSTEM ANALYTICS REPORT");

    cout << "Total Bookings  : " << totalBookings << endl;
    cout << "Total Revenue   : " << revenue << " PKR" << endl;

    R_success("Report Generated Successfully!");
}
void findShortestRoute()
{
    int src, dest;

    R_header("FIND SHORTEST ROUTE (DIJKSTRA)");

    showCities();

    cout<<"Enter Source City Index: ";
    cin>>src;

    cout<<"Enter Destination City Index: ";
    cin>>dest;

    if(src < 0 || dest < 0 || src >= CITY_COUNT || dest >= CITY_COUNT)
    {
        R_error("Invalid City Selection!");
        return;
    }

    int distance = dijkstra(src, dest);

    if(distance == 99999)
    {
        R_error("No route found between selected cities!");
    }
    else
    {
        R_success("Shortest Route Calculated Successfully!");
        cout << "Total Distance: " << distance << " KM" << endl;
    }
}
Passenger* findUserByPhone(long long phone)
{
    Passenger* p = passengerHeadR;
    while(p)
    {
        if(p->phone == phone)
            return p;
        p = p->next;
    }
    return NULL;
}
bool adminLogin()
{
    string pass;
    int attempts = 3;

    while(attempts > 0)
    {
        cout << "Enter Admin Password: ";
        cin >> pass;

        if(pass == ADMIN_PASSWORD)
        {
            R_success("Login Successful!");
            return true;
        }

        attempts--;
        R_error("Wrong Password!");
        cout<<"Attempts left: "<<attempts << endl;
    }

    return false;
}
//==================CleanUp Functions ================
void clearTrains()
{
    while(trainHeadR)
    {
        Train* t = trainHeadR;
        trainHeadR = trainHeadR->next;
        delete t;
    }
}

void clearUsers()
{
    while(passengerHeadR)
    {
        Passenger* p = passengerHeadR;
        passengerHeadR = passengerHeadR->next;
        delete p;
    }
}

void clearBookings()
{
    while(bookingHeadR)
    {
        Booking* b = bookingHeadR;
        bookingHeadR = bookingHeadR->next;
        delete b;
    }
}

void clearFeedback()
{
    while(feedbackHeadR)
    {
        Feedback* f = feedbackHeadR;
        feedbackHeadR = feedbackHeadR->next;
        delete f;
    }
}
void cleanup()
{
    clearTrains();
    clearUsers();
    clearBookings();
    clearFeedback();
}
// ================= FILE HANDLING =================
// ==================== TRAINS =====================
void saveTrains()
{
    ofstream file("RailwayLineTrains.txt");
    Train* t = trainHeadR;

    while(t!=NULL)
    {
        file<<t->id<<"\n"
            <<t->name<<"\n"
            <<t->source<<"\n"
            <<t->destination<<"\n"
            <<t->distance<<"\n"
            <<t->price<<" "<< t->seats<<" "<<t->bookedSeats<<"\n";
        t = t->next;
    }
}
void loadTrains()
{
    clearTrains();

    ifstream file("RailwayLineTrains.txt");
    if(!file) return;

    while(true)
    {
        Train* t = new Train();

        if(!(file >> t->id))
        {
            delete t;
            break;
        }

        file.ignore();
        getline(file, t->name);
        getline(file, t->source);
        getline(file, t->destination);

            file >> t->distance
                 >> t->price
                 >> t->seats
                 >> t->bookedSeats;
        file.ignore();

        t->next = trainHeadR;
        trainHeadR = t;
    }
}
// ==================== USERS =====================
void saveUsers()
{
    ofstream file("RailwayLineUsers.txt");
    Passenger* p = passengerHeadR;

    while(p)
    {
        file << p->id << "\n"
             << p->name << "\n"
             << p->phone << "\n";
        p = p->next;
    }
}
void loadUsers()
{
    clearUsers();

    ifstream file("RailwayLineUsers.txt");
    if(!file) return;

    while(true)
    {
        Passenger* p = new Passenger();

        if(!(file >> p->id))
        {
            delete p;
            break;
        }

        file.ignore();
        getline(file, p->name);
        file >> p->phone;
        file.ignore();

        p->next = passengerHeadR;
        passengerHeadR = p;

        if(p->id >= userCounterR)
            userCounterR = p->id + 1;
    }
}
//================= FEEDBACK ======================
void saveFeedback()
{
    ofstream file("RailwayServiceFeedbacks.txt");
    Feedback* f = feedbackHeadR;

    while(f)
    {
        file << f->userId << "\n"
             << f->name << "\n"
             << f->message << "\n";
        f = f->next;
    }
}
void loadFeedback()
{
    ifstream file("RailwayServiceFeedbacks.txt");
    if(!file) return;

    while(true)
    {
        Feedback* f = new Feedback();

        if(!(file >> f->userId)) break;
        file.ignore();

        getline(file, f->name);
        getline(file, f->message);

        f->next = feedbackHeadR;
        feedbackHeadR = f;
    }
}
//===================BOOKINGS================
void saveBookings()
{
    ofstream file("RailwayBookings.txt");

    Booking* b = bookingHeadR;

    while(b)
    {
        file << b->id << " "
             << b->trainId << " "
             << b->passengerId << "\n";

        b = b->next;
    }

    file.close();
}
void loadBookings()
{
    clearBookings();

    ifstream file("RailwayBookings.txt");
    if(!file) return;

    int id, trainId, passengerId;

    bookingCounterR = 1;

    while(file >> id >> trainId >> passengerId)
    {
        Booking* b = new Booking();

        b->id = id;
        b->trainId = trainId;
        b->passengerId = passengerId;

        b->next = bookingHeadR;
        bookingHeadR = b;

        if(id >= bookingCounterR)
            bookingCounterR = id + 1;
    }
}
//================== WAITING LIST ===================
void saveWaitingList()
{
    ofstream file("RailwayWaightings.txt");

    WaitNode* temp = frontR;

    while(temp)
    {
        file << temp->passengerId
             << " "
             << temp->trainId
             << "\n";

        temp = temp->next;
    }

    file.close();
}
void loadWaitingList()
{
    ifstream file("RailwayWaightings.txt");

    if(!file)
    {
        return;
    }

    int pid, tid;

    while(file >> pid >> tid)
    {
        WaitNode* n = new WaitNode();

        n->passengerId = pid;
        n->trainId = tid;

        n->next = NULL;

        if(frontR == NULL)
        {
            frontR = rearR = n;
        }
        else
        {
            rearR->next = n;
            rearR = n;
        }
    }

    file.close();
}
// ================= MENUS =================
void userMenu(Passenger* u)
{
    int ch;

    do
    {
        R_setColor(10);
        R_header("PASSENGER DASHBOARD - " + u->name);
        R_setColor(7);

        R_setColor(11);
        cout << "\n[1]. View Trains\n";
        cout << "[2]. Book Ticket\n";
        cout << "[3]. My Bookings\n";
        cout << "[4]. Cancel Booking\n";
        cout << "[5]. Submit Feedback\n";
        cout << "[6]. Find Shortest Route (Graph)\n";
        cout << "[0]. Exit\n";
        R_setColor(7);

        cout << "Enter choice: ";
        cin >> ch;

        switch(ch)
        {
            case 1:
                viewTrains();
                break;

            case 2:
                viewTrains();
                bookTicket(u);
                break;

            case 3:
                viewMyBookings(u);
                break;

            case 4:
                cancelBooking(u);
                break;

            case 5:
                submitFeedback(u);
                break;

            case 6:
                findShortestRoute();
                break;

            case 0:
                cout << "Logging out...\n";
                break;

            default:
                R_error("Invalid Choice!");
        }

    } while(ch != 0);
}
void adminMenu()
{
    int ch;

    do
    {
        R_setColor(9);
        R_header("RAILWAY ADMIN CONTROL PANEL");
        R_setColor(7);

        R_setColor(13);
        cout << "[1]. Add Train\n";
        cout << "[2]. View Trains\n";
        cout << "[3]. Delete Train\n";
        cout << "[4]. Update Train\n";
        cout << "[5]. View Waiting\n";
        cout << "[6]. View Feedback\n";
        cout << "[7]. View All Bookings\n";
        cout << "[8]. Find Train by Route\n";
        cout << "[9]. Advanced Report\n";
        cout << "[0]. Exit\n";
        R_setColor(7);

        cout<<"Enter choice: ";
        cin>>ch;

        if (ch == 1) addTrain();
        else if (ch == 2) viewTrains();
        else if (ch == 3) deleteTrain();
        else if (ch == 4) updateTrain();
        else if (ch == 5) viewWaitingList();
        else if (ch == 6) viewFeedback();
        else if (ch == 7) viewAllBookingsWithUsers();
        else if(ch == 8) findTrainByRoute();
        else if(ch == 9) advancedReport();

    } while (ch != 0);
}
// ================= MAIN =================

void RailwayMenu()
{
    initializeGraph();
    loadWaitingList();
    loadTrains();
    loadUsers();
    loadFeedback();
    loadBookings();

    // Ensure bookingCounter is always correct even if file is corrupted
    Booking* temp = bookingHeadR;
    int maxId = 0;

    while(temp != NULL)
    {
        if(temp->id > maxId)
            maxId = temp->id;

        temp = temp->next;
    }

    bookingCounterR = maxId + 1;

    // ================= USER INTERFACE =================
    int choice;

    while(true)
    {
         R_header("QUANTUM RAILWAY NETWORK");

        R_setColor(9);
        cout << "[1]. Admin Panel\n";
        cout << "[2]. User Panel\n";
        cout << "[0]. Exit\n";
        R_setColor(7);

        cout << "Enter choice: ";
        cin >> choice;

        if(choice == 1)
        {
             if(adminLogin())
             {
                adminMenu();
             }
             else
             {
                R_error("Wrong Password! Access Denied.");
             }
        }
        else if(choice == 2)
        {
            string name;
            long long phone;

            cout << "Enter Name: ";
            cin >> ws;
            getline(cin, name);

            cout << "Enter Phone: ";
            cin >> phone;

            Passenger* u;
            Passenger* existing = findUserByPhone(phone);

            if(existing != NULL)
                u = existing;
            else
                u = addUser(name, phone);

            userMenu(u);
        }
        else if(choice == 0)
        {
            cout << "Exiting system...\n";
            break;
        }
        else
        {
            R_error("Invalid choice!");
        }

        // ================= AUTO SAVE =================
        saveTrains();
        saveUsers();
        saveFeedback();
        saveBookings();
        saveWaitingList();
    }
    // ================= CLEANUP MEMORY =================
    cleanup();
    cout << "System closed safely. Memory cleaned.\n";

}

