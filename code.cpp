#include <iostream>
#include <vector>
#include <string>
#include <limits>
#include <fstream>
#include <iomanip>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <conio.h>
#include <windows.h>

using namespace std;

// Define constants
const int maxnode = 35;
const int MAX_USERS = 100;

// Structure to store the details of the rider
struct Rider {
    string rider_name;
    string taxi_number;
    string vehicle;
    float rider_rating;
    Rider* next;

    Rider(string name, string taxi, float rating, string veh)
        : rider_name(name), taxi_number(taxi), rider_rating(rating), vehicle(veh), next(nullptr) {}
};

// Structure to store the vertices
struct Node {
    int src;
    string src_name;
    int dest;
    string dest_name;
    float dist;
    Rider* riders; // to create a list of riders
    Node* next;

    Node(int s, string s_name, int d, string d_name, float distance)
        : src(s), src_name(s_name), dest(d), dest_name(d_name), dist(distance), riders(nullptr), next(nullptr) {}
};

// Creating a list
struct AdjacentList {
    Node* head;
};

// To create a graph
class Graph {
public:
    int no_vertices;
    AdjacentList* arr;

    Graph(int vertices) : no_vertices(vertices) {
        arr = new AdjacentList[no_vertices];
        for (int i = 0; i < no_vertices; i++) {
            arr[i].head = nullptr;
        }
    }

    ~Graph() {
        for (int i = 0; i < no_vertices; i++) {
            Node* current = arr[i].head;
            while (current) {
                Node* temp = current;
                current = current->next;
                delete temp;
            }
        }
        delete[] arr;
    }

    void insert_edge(int src, string src_name, int dest, string dest_name, float dist);
    void display_graph();
    void addrider(int vertex, string rider_name, string taxi_number, float rider_rating, string vehicle);
    int find_vertex_index(string vertex_name);
    float dijkstra(int src, int dest);
    void displayRidersSorted(int vertex, float distance, string userid, string src_name, string dest_name);
    void load_from_file();
    void printTopCustomers();
    void travelhistory(string userid);
    void rateRider(int vertex, string riderName);
    void read_rating_from_file();
    void initializeriderratings();
};

// User ID and passcode details
struct LoginDetails {
    string userid;
    string passcode;
};

// Structure to store the user ID and number of trips for the printing of top customers
struct UserEntry {
    string userid;
    int trips;
};

// Global variables
LoginDetails login[MAX_USERS];
int login_count = 0;

// Function prototypes
void pattern();
void pattern2();
void pattern3();
int authenticateUser (LoginDetails* users, int numUsers, string username, string password);
void getPassword(string& password);
void load_from_file(Graph& graph);
void quicksort(Rider* arr, int low, int high);
int partition(Rider* arr, int low, int high);
float calculate_cost(char vehicle_type, float total_distance);
void print_path(int parent[], int j, Graph& graph);
void print_solution(float dist[], int parent[], int src, int dest, Graph& graph);
int min_distance(float dist[], int spt_set[]);

// Function to display the welcome pattern
void pattern() {
    cout << " _    _ _____ _     _____ ________  ___ _____     _____ _____      \n";
    cout << "| |  | |  __| |   /  __ \\  _  |  \\/  ||  ___|   |   _|  _  |     \n";
    cout << "| |  | | |__ | |   | /  \\/ | | | .  . || |__       | | | | | |     \n";
    cout << "| |\\| |  __|| |   | |   | | | | |\\/| ||  __|      | | | | | |     \n";
    cout << "\\  /\\  / || || \\/\\ \\/ / |  | || |__      | | \\ \\_/ /     \n";
    cout << " \\/  \\/\\/\\/\\/\\/\\|  ||\\/      \\/  \\/      \n";
}

// Function to display the thank you pattern
void pattern2() {
    cout << " _____ _   _   ___ _   _  _   __   __   _______ _   _ \n";
    cout << "|_   _| | | | / _ \\ | \\ | || | / /   \\ \\ / /  _  | | | |\n";
    cout << "  | | | || |/ /\\ \\| \\| || |/ /     \\ V /| | | | | | |\n";
    cout << "  | | |  _  ||  _  || . ` ||    \\      \\ / | | | | | | |\n";
    cout << "  | | | | | || | | || |\\  || |\\  \\     | | \\ \\/ / || |\n";
    cout << "  \\/ \\| |/\\| |/\\| \\/\\| \\/     \\/  \\/ \\/\n";
}

// Function to display the vehicle pattern
void pattern3() {
    cout << " ______          \n";
    cout << "/|||\\`.      \n";
    cout << "(   _    _ _\\     \n";
    cout << "=`-()--()-'       \n";
    cout << "\n";
    cout << " ,                    \n";
    cout << ".-/c-.,::              \n";
    cout << "()'==()               \n";
}

// Function to authenticate user
int authenticateUser (LoginDetails* users, int numUsers, string username, string password) {
    for (int i = 0; i < numUsers; ++i) {
        if (users[i].userid == username && users[i].passcode == password) {
            return i;
        }
    }

}

// Function to get password input without displaying it
void getPassword(string& password) {
    char ch;
    password.clear();
    while (true) {
        ch = getch();
        if (ch == 13) { // Enter key
            break;
        } else if (ch == 8 && !password.empty()) { // Backspace key
            password.pop_back();
            cout << "\b \b"; // Move the cursor back and erase the character
        } else {
            password.push_back(ch);
            cout << "*";
        }
    }
    cout << endl;
}

// Function to load user details from file
void load_from_file(Graph& graph) {
    ifstream file("LOGIN_DET.txt");
    if (!file) {
        cout << "File Not found\n";
        return;
    }
    while (file >> login[login_count].userid >> login[login_count].passcode) {
        login_count++;
    }
    file.close();
}

// Function to calculate the cost of a ride based on the vehicle type
float calculate_cost(char vehicle_type, float total_distance) {
    float min_fare, rate_per_km;
    switch (vehicle_type) {
        case 'A': min_fare = 25; rate_per_km = 13; break;
        case 'B': min_fare = 20; rate_per_km = 10; break;
        case 'C': min_fare = 30; rate_per_km = 16; break;
        case 'S': min_fare = 40; rate_per_km = 20; break;
        case 'T': min_fare = 50; rate_per_km = 25; break;
        default: cout << "Invalid vehicle type.\n"; return -1;
    }
    return min_fare + rate_per_km * (total_distance - 2);
}

// Function to display the graph
void Graph::display_graph() {
    cout << "Location            |    | Destination        \n";
    cout << "--------------------|    |---------------------\n";
    for (int i = 0; i < no_vertices; i++) {
        Node* temp = arr[i].head;
        cout << "\n" << setw(19) << temp->src_name << " |   |";
        while (temp != nullptr) {
            cout << temp->dest_name << "(" << temp->dist << ")";
            temp = temp->next;
            if (temp != nullptr) {
                cout << ", ";
            }
        }
        cout << "\n";
    }
}

// Function to insert edges into the graph
void Graph::insert_edge(int src, string src_name, int dest, string dest_name, float dist) {
    Node* newnode_src = new Node(src, src_name, dest, dest_name, dist);
    newnode_src->next = arr[src].head;
    arr[src].head = newnode_src;

    Node* newnode_dest = new Node(dest, dest_name, src, src_name, dist);
    newnode_dest->next = arr[dest].head;
    arr[dest].head = newnode_dest;
}

// Function to add a rider to a specific vertex in the graph
void Graph::addrider(int vertex, string rider_name, string taxi_number, float rider_rating, string vehicle) {
    Rider* newrider = new Rider(rider_name, taxi_number, rider_rating, vehicle);
    Node* current_node = arr[vertex].head;

    if (current_node->riders == nullptr) {
        current_node->riders = newrider;
    } else {
        Rider* last = current_node->riders;
        while (last->next != nullptr) {
            last = last->next;
        }
        last->next = newrider;
    }
}

// Function to find the vertex index of a specific vertex name
int Graph::find_vertex_index(string vertex_name) {
    for (int i = 0; i < no_vertices; i++) {
        Node* temp = arr[i].head;
        while (temp != nullptr) {
            if (temp->src_name == vertex_name) {
                return i;
            }
            temp = temp->next;
        }
    }
    return -1; // Vertex not found
}

// Function to find the index with the minimum distance used in Dijkstra's algorithm
int min_distance(float dist[], int spt_set[]) {
    float min = numeric_limits<float>::max();
    int min_index;

    for (int v = 0; v < maxnode; v++) {
        if (spt_set[v] == 0 && dist[v] <= min) {
            min = dist[v];
            min_index = v;
        }
    }
    return min_index;
}

// Function to print the path of the user given source and destination
void print_path(int parent[], int j, Graph& graph) {
    if (parent[j] == -1) {
        cout << graph.arr[j].head->src_name << " ";
        return;
    }
    print_path(parent, parent[j], graph);
    cout << "--> " << graph.arr[j].head->src_name << " ";
}

// Function to print the solution of Dijkstra's algorithm
void print_solution(float dist[], int parent[], int src, int dest, Graph& graph) {
    cout << "-----------------------------------------------------------------------\n";
    cout << "| Your Path from " << graph.arr[src].head->src_name << " to " << graph.arr[dest].head->src_name << " will be:\n";
    cout << "| Path: ";
    print_path(parent, dest, graph);
    cout << "\n| And the total Distance you will be travelling is: " << dist[dest] << "KM \n";
    cout << "-----------------------------------------------------------------------\n";
}

// Function to find the shortest path from the source to the destination using Dijkstra's algorithm
float Graph::dijkstra(int src, int dest) {
    float dist[maxnode];
    int spt_set[maxnode];
    int parent[maxnode];

    for (int i = 0; i < maxnode; i++) {
        dist[i] = numeric_limits<float>::max();
        spt_set[i] = 0;
        parent[i] = -1;
    }

    dist[src] = 0;

    for (int count = 0; count < no_vertices - 1; count++) {
        int u = min_distance(dist, spt_set);
        spt_set[u] = 1;

        for (Node* temp = arr[u].head; temp != nullptr; temp = temp->next) {
            int v = temp->dest;
            if (!spt_set[v] && dist[u] != numeric_limits<float>::max() && dist[u] + temp->dist < dist[v]) {
                dist[v] = dist[u] + temp->dist;
                parent[v] = u;
            }
        }
    }

    print_solution(dist, parent, src, dest, *this);
    return dist[dest];
}

// Function to display riders sorted by ratings
void Graph::displayRidersSorted(int vertex, float distance, string userid, string src_name, string dest_name) {
    Rider* riders = arr[vertex].head->riders;
    vector<Rider> riderList;

    while (riders != nullptr) {
        riderList.push_back(*riders);
        riders = riders->next;
    }

    sort(riderList.begin(), riderList.end(), [](const Rider& a, const Rider& b) {
        return a.rider_rating > b.rider_rating;
    });

    cout << "\nRiders available at " << arr[vertex].head->src_name << " (Sorted by Ratings):\n";
    cout << setw(20) << "Rider Name" << setw(15) << "Taxi Number" << setw(15) << "Rider Ratings" << setw(15) << "Vehicle Type" << endl;

    for (const auto& rider : riderList) {
        cout << setw(20) << rider.rider_name
             << setw(15) << rider.taxi_number
             << setw(15) << fixed << setprecision(2) << rider.rider_rating
             << setw(15) << rider.vehicle << endl;
        Sleep(1000);
    }

    // Vehicle selection and cost calculation
    char vehicle;
    float A = calculate_cost('A', distance);
    float C = calculate_cost('C', distance);
    float S = calculate_cost('S', distance);
    float T = calculate_cost('T', distance);
    float B = calculate_cost('B', distance);

    cout << "\n\nSelect your vehicle type:";
    cout << "\n1.AUTO      Total cost will be : " << A;
    cout << "\n2.CAR       Total cost will be : " << C;
    cout << "\n3.SEDAN     Total cost will be : " << S;
    cout << "\n4.TRAVELLER Total cost will be : " << T;
    cout << "\n5.BIKE      Total cost will be : " << B;

    int choice;
    cin >> choice;

    switch (choice) {
        case 1: vehicle = 'A'; break;
        case 2: vehicle = 'C'; break;
        case 3: vehicle = 'S'; break;
        case 4: vehicle = 'T'; break;
        case 5: vehicle = 'B'; break;
        default: cout << "Invalid choice\n"; return;
    }

    string ridername, vehicleno;
    bool riderFound = false;

    for (const auto& rider : riderList) {
        if (rider.vehicle[0] == vehicle) {
            ridername = rider.rider_name;
            vehicleno = rider.taxi_number;
            riderFound = true;
            break;
        }
    }

    cout << "\n\nConnecting to the rider..............";
    Sleep(4000);
    system("cls");

    if (riderFound) {
        cout << "\n|-------------------------------------------\n";
        cout << "|Rider name: " << ridername << "\n";
        cout << "|Vehicle type: " << vehicle << "\n";
        cout << "|Vehicle number: " << vehicleno << "\n\n";
        cout << "|-------------------------------------------\n";

        float totalCost = (vehicle == 'A') ? A : (vehicle == 'C') ? C : (vehicle == 'S') ? S : (vehicle == 'T') ? T : B;
        cout << "\n|-----------------------------------------------\n";
        cout << "|Your total amount will be " << totalCost << "\n";
        cout << "|Please pay the amount in cash or online\n";
        cout << "|Your " << vehicle << " will be arriving shortly\n";
        cout << "|-----------------------------------------------\n";
        pattern3();
        Sleep(7000);
        cout << "\n\nYour " << vehicle << " has Arrived at your location\n";

        // Rating process
        int choiceRating;
        cout << "\nWould you like to rate " << ridername << "?\n";
        cout << "1.YES\n2.Maybe sometime later\n";
        cin >> choiceRating;

        if (choiceRating == 1) {
            rateRider(vertex, ridername);
        }

        // Save travel history
        ofstream fp("travel_history.txt", ios::app);
        if (fp.is_open()) {
            fp << setw(20) << userid << setw(30) << src_name << setw(30) << dest_name
               << setw(20) << ridername << setw(20) << vehicle << setw(20) << vehicleno << setw(10) << 0 << endl;
            fp.close();
        } else {
            cout << "Error opening travel history file.\n";
        }
    } else {
        cout << "No rider found with the selected vehicle type.\n";
    }
}

// Function to read ratings from file and update the graph
void Graph::read_rating_from_file() {
    ifstream file("RIDER_RATINGS.txt");
    if (file.is_open()) {
        string name;
        float rating;
        while (file >> name >> rating) {
            for (int i = 0; i < no_vertices; i++) {
                Rider* tempRider = arr[i].head->riders;
                while (tempRider != nullptr) {
                    if ( strcmp(tempRider->rider_name.c_str(), name.c_str()) == 0) {
                        tempRider->rider_rating = rating;
                        break;
                    }
                    tempRider = tempRider->next;
                }
            }
        }
        file.close();
    } else {
        cout << "Error opening file for reading.\n";
    }
}

// Function to initialize rider ratings from the file
void Graph::initializeriderratings() {
    read_rating_from_file();
}

// Function to rate a rider
void Graph::rateRider(int vertex, string riderName) {
    float userRating;
    Rider* riders = arr[vertex].head->riders;
    Rider* tempRider = riders;

    while (tempRider != nullptr && tempRider->rider_name != riderName) {
        tempRider = tempRider->next;
    }

    if (tempRider != nullptr) {
        cout << "Enter your rating (0-5) for " << riderName << ": ";
        cin >> userRating;

        tempRider->rider_rating = (tempRider->rider_rating + userRating) / 2;

        ofstream file("RIDER_RATINGS.txt", ios::app);
        if (file.is_open()) {
            file << tempRider->rider_name << " " << fixed << setprecision(2) << tempRider->rider_rating << endl;
            file.close();
            cout << "Thank you for rating " << riderName << "!\n";
        } else {
            cout << "Error opening file.\n";
        }
    } else {
        cout << "Rider not found at " << arr[vertex].head->src_name << endl;
    }
}

// Function to print travel history for a user
void Graph::travelhistory(string userid) {
    ifstream file("travel_history.txt");
    if (!file) {
        cout << "Unable to open the file\n";
        return;
    }

    string record[7];
    cout << setw(20) << "Username" << setw(30) << "Source Location" << setw(30) << "Destination Location"
         << setw(20) << "Selected Rider" << setw(20) << "Vehicle Type" << setw(20) << "Vehicleno" << setw(10) << "Rating Given" << endl;

    while (file >> record[0] >> record[1] >> record[2] >> record[3] >> record[4] >> record[5] >> record[6]) {
        if (record[0] == userid) {
            cout << setw(20) << record[0] << setw(30) << record[1] << setw(30) << record[2]
                 << setw(20) << record[3] << setw(20) << record[4] << setw(20) << record[5]
                 << setw(10) << record[6] << endl;
        }
    }
    file.close();
}

// Function to print top customers based on the number of trips
void Graph::printTopCustomers() {
    UserEntry userTrips[MAX_USERS] = {};
    memset(userTrips, 0, sizeof(userTrips));

    ifstream file("travel_history.txt");
    if (!file) {
        cout << "Unable to open the file\n";
        return;
    }

    string entry[7];
    while (file >> entry[0] >> entry[1] >> entry[2] >> entry[3] >> entry[4] >> entry[5] >> entry[6]) {
        bool found = false;
        for (int i = 0; i < MAX_USERS; i++) {
            if (userTrips[i].userid == entry[0]) {
                userTrips[i].trips++;
                found = true;
                break;
            } else if (userTrips[i].userid.empty()) {
                userTrips[i].userid = entry[0];
                userTrips[i].trips = 1;
                found = true;
                break;
            }
        }
        if (!found) {
            cout << "Too many users. Increase MAX_USERS.\n";
            return;
        }
    }

    cout << "Top 5 Customers:\n";
    for (int j = 0; j < 5; j++) {
        int maxTrips = 0;
        string topCustomer;

        for (int i = 0; i < MAX_USERS; i++) {
            if (!userTrips[i].userid.empty() && userTrips[i].trips > maxTrips) {
                maxTrips = userTrips[i].trips;
                topCustomer = userTrips[i].userid;
            }
        }

        if (maxTrips > 0) {
            cout << "User  ID " << topCustomer << " with " << maxTrips << " trips.\n";
            for (int i = 0; i < MAX_USERS; i++) {
                if (userTrips[i].userid == topCustomer) {
                    userTrips[i].trips = 0; // Set the trips of the top customer to 0 to find the next one
                    break;
                }
            }
        } else {
            cout << "No more users with travel history found.\n";
            break;
        }
    }
}

// Main function
int main() {
    system("cls");
    int no_vertices = 35; // Number of vertices
    Graph graph(no_vertices); // Creates a graph
    Graph load_from_file();
    pattern();


 // Inserting rider information

 graph.insert_edge(0, "MysoreRailwayStation", 1, "SayyajiRaoRoad", 4);
    graph.insert_edge(1, "SayyajiRaoRoad", 2, "MysoreZoo", 1);
    graph.insert_edge(2, "MysoreZoo", 3, "KaranjiLake", 1);
    graph.insert_edge(2, "MysoreZoo", 4, "BanniMantap", 2.5);
    graph.insert_edge(2, "MysoreZoo", 5, "Jayalakshmipuram", 3);
    graph.insert_edge(2, "MysoreZoo", 6, "Vontikoppal", 3);
    graph.insert_edge(3, "KaranjiLake", 4, "BanniMantap", 1);
    graph.insert_edge(4, "BanniMantap", 5, "Jayalakshmipuram", 2);
    graph.insert_edge(5, "Jayalakshmipuram", 6, "Vontikoppal", 2.6);
    graph.insert_edge(5, "Jayalakshmipuram", 7, "Nazarbad", 1.7);
    graph.insert_edge(5, "Jayalakshmipuram", 8, "Hunsur", 1.6);
    graph.insert_edge(5, "Jayalakshmipuram", 9, "KRS", 1.8);
    graph.insert_edge(5, "Jayalakshmipuram", 10, "Srirampura", 1.6);
    graph.insert_edge(8, "Hunsur", 11, "Nazarbad", 2.3);
    graph.insert_edge(8, "Hunsur", 12, "ChamundiHill", 2);
    graph.insert_edge(11, "Nazarbad", 13, "Krishnamurthypuram", 2);
    graph.insert_edge(11, "Nazarbad", 14, "RamaswamyCircle", 2);
    graph.insert_edge(12, "ChamundiHill", 11, "Nazarbad", 1.6);
    graph.insert_edge(12, "ChamundiHill", 14, "RamaswamyCircle", 2);
    graph.insert_edge(12, "ChamundiHill", 15, "Gokulam", 2.3);
    graph.insert_edge(12, "ChamundiHill", 16, "MandiMahal", 3);
    graph.insert_edge(13, "Krishnamurthypuram", 17, "Vontikoppal", 1);
    graph.insert_edge(13, "Krishnamurthypuram", 14, "RamaswamyCircle", 2.5);
    graph.insert_edge(14, "RamaswamyCircle", 17, "Vontikoppal", 1.8);
    graph.insert_edge(14, "RamaswamyCircle", 18, "MysorePalace", 2.4);
    graph.insert_edge(14, "RamaswamyCircle", 19, "Saraswathipuram", 2.8);
    graph.insert_edge(14, "RamaswamyCircle", 15, "Gokulam", 2);
    graph.insert_edge(15, "Gokulam", 19, "Saraswathipuram", 2.2);
    graph.insert_edge(15, "Gokulam", 20, "Srirampura", 2.2);
    graph.insert_edge(15, "Gokulam", 21, "Bannimantap", 1.7);
    graph.insert_edge(16, "MandiMahal", 21, "Bannimantap", 2);
    graph.insert_edge(16, "MandiMahal", 22, "MysoreUniversity", 8);
    graph.insert_edge(17, "Vontikoppal", 18, "MysorePalace", 3);
    graph.insert_edge(18, "MysorePalace", 23, "KaranjiLake", 1.8);
    graph.insert_edge(18, "MysorePalace", 19, "Saraswathipuram", 0.8);
    graph.insert_edge(19, "Saraswathipuram", 24, "RamaswamyCircle", 3.1);
    graph.insert_edge(19, "Saraswathipuram", 25, "SayyajiRaoRoad", 1.6);
    graph.insert_edge(19, "Saraswathipuram", 20, "Srirampura", 0.75);
    graph.insert_edge(20, "Srirampura", 25, "SayyajiRaoRoad", 1);
    graph.insert_edge(20, "Srirampura", 26, "Gokulam", 1.5);
    graph.insert_edge(20, "Srirampura", 27, "MysorePalace", 0.5);
    graph.insert_edge(20, "Srirampura", 28, "MandiMahal", 3.7);
    graph.insert_edge(20, "Srirampura", 21, "Bannimantap", 1.7);
    graph.insert_edge(21, "Bannimantap", 29, "MysoreUniversity", 3.7);
    graph.insert_edge(21, "Bannimantap", 30, "Sainikpuri", 4);
    graph.insert_edge(22, "MysoreUniversity", 30, "Sainikpuri", 1.3);
    graph.insert_edge(23, "GandhiSquare", 24, "RamaswamyCircle", 3.2);
    graph.insert_edge(24, "RamaswamyCircle", 25, "SayyajiRaoRoad", 3);
    graph.insert_edge(24, "RamaswamyCircle", 26, "Gokulam", 2.3);
    graph.insert_edge(24, "RamaswamyCircle", 31, "ChamarajaCircle", 1.8);
    graph.insert_edge(25, "SayyajiRaoRoad", 26, "Gokulam", 0.5);
    graph.insert_edge(26, "Gokulam", 31, "ChamarajaCircle", 2);
    graph.insert_edge(26, "Gokulam", 28, "MandiMahal", 2.2);
    graph.insert_edge(27, "SayyajiRaoRoad", 28, "MandiMahal", 3.8);
    graph.insert_edge(28, "MandiMahal", 32, "MysoreUniversity", 4);
    graph.insert_edge(28, "MandiMahal", 29, "MysoreUniversity", 4);
    graph.insert_edge(29, "MysoreUniversity", 30, "Sainikpuri", 2);
    graph.insert_edge(31, "ChamarajaCircle", 33, "ChurchStreet", 5.7);
    graph.insert_edge(31, "ChamarajaCircle", 32, "MysoreUniversity", 4);
    graph.insert_edge(32, "MysoreUniversity", 33, "ChurchStreet", 4);
    graph.insert_edge(33, "ChurchStreet", 34, "IITMysore", 3.5);



// Inserting rider information

// SayyajiRaoRoad
graph.addrider(0, "Ravi", "KA22HF6552", 4.5, "AUTO");
graph.addrider(0, "Nikhil", "KA22HF6245", 4.9, "CAR");
graph.addrider(0, "Chandan", "KA22HF6598", 4.2, "SEDAN");
graph.addrider(0, "Anitha", "KA22HF6752", 4.1, "TRAVELLER");
graph.addrider(0, "Shankar", "KA22HF6587", 4.6, "BIKE");

// PalaceRoad
graph.addrider(1, "Yash", "KA01AB1234", 4.8, "CAR");
graph.addrider(1, "Priya", "KA01BC5678", 4.5, "AUTO");
graph.addrider(1, "Arvind", "KA01XY9876", 4.7, "BIKE");
graph.addrider(1, "Nandini", "KA01DE3456", 4.2, "SEDAN");
graph.addrider(1, "Rachna", "KA01FG6789", 4.9, "TRAVELLER");

// ChamundiHills
graph.addrider(2, "Vikas", "KA02 AB1234", 4.6, "BIKE");
graph.addrider(2, "Aishwarya", "KA02BC5678", 4.4, "AUTO");
graph.addrider(2, "Suresh", "KA02XY9876", 4.8, "CAR");
graph.addrider(2, "Tejaswi", "KA02DE3456", 4.3, "SEDAN");
graph.addrider(2, "Radhika", "KA02FG6789", 4.7, "TRAVELLER");
graph.addrider(2, "Kiran", "KA02AB6789", 4.8, "TRAVELLER");
graph.addrider(2, "Aarav", "KA02CD1234", 4.5, "AUTO");
graph.addrider(2, "Swathi", "KA02EF5678", 4.4, "SEDAN");
graph.addrider(2, "Anushka", "KA02GH9876", 4.9, "BIKE");

// Srirampura
graph.addrider(3, "Krishna", "KA03AB1234", 4.5, "SEDAN");
graph.addrider(3, "Geetha", "KA03BC5678", 4.9, "CAR");
graph.addrider(3, "Rakesh", "KA03XY9876", 4.2, "BIKE");
graph.addrider(3, "Sowmya", "KA03DE3456", 4.7, "TRAVELLER");
graph.addrider(3, "Manju", "KA03FG6789", 4.4, "AUTO");

// RamaswamyCircle
graph.addrider(5, "Vinod", "KA05AB1234", 4.6, "BIKE");
graph.addrider(5, "Ranjani", "KA05BC5678", 4.4, "AUTO");
graph.addrider(5, "Naveen", "KA05XY9876", 4.8, "CAR");
graph.addrider(5, "Sunitha", "KA05DE3456", 4.3, "SEDAN");
graph.addrider(5, "Manju", "KA05FG6789", 4.7, "TRAVELLER");
graph.addrider(5, "Prashanth", "KA05AB6789", 4.7, "CAR");
graph.addrider(5, "Sushma", "KA05CD1234", 4.6, "BIKE");
graph.addrider(5, "Dinesh", "KA05EF5678", 4.8, "TRAVELLER");
graph.addrider(5, "Radha", "KA05GH9876", 4.5, "AUTO");

// RaniRoad
graph.addrider(6, "Aditya", "KA06AB1234", 4.5, "SEDAN");
graph.addrider(6, "Srinidhi", "KA06BC5678", 4.9, "CAR");
graph.addrider(6, "Kavya", "KA06XY9876", 4.2, "BIKE");
graph.addrider(6, "Pavani", "KA06DE3456", 4.7, "TRAVELLER");
graph.addrider(6, "Harsha", "KA06FG6789", 4.4, "AUTO");

// Gokulam
graph.addrider(7, "Raghu", "KA07AB1234", 4.8, "CAR");
graph.addrider(7, "Kavitha", "KA07BC5678", 4.5, "AUTO");
graph.addrider(7, "Manoj", "KA07XY9876", 4.7, "BIKE");
graph.addrider(7, "Veena", "KA07DE3456", 4.2, "SEDAN");
graph.addrider(7, "Harsha", "KA07FG6789", 4.9, "TRAVELLER");

// MahadevPura
graph.addrider(8, "Bhavana", "KA08AB1234", 4.6, "BIKE");
graph.addrider(8, "Ravi", "KA08BC5678", 4.4, "AUTO");
graph.addrider(8, "Kavita", "KA08XY9876", 4.8, "CAR");
graph.addrider(8, "Tejas", "KA08DE3456", 4.3, "SEDAN");
graph.addrider(8, "Sowmya", "KA08FG6789", 4.7, "TRAVELLER");

// BanniMantap
graph.addrider(9, "Vidya", "KA09AB1234", 4.5, "SEDAN");
graph.addrider(9, "Amit", "KA09BC5678", 4.9, "CAR");
graph.addrider(9, "Kiran", "KA09XY9876", 4.2, "BIKE");
graph.addrider(9, "Aravind", "KA09DE3456", 4.7, "TRAVELLER");
graph.addrider(9, "Shilpa", "KA09FG6789", 4.4, "AUTO");

// ChikkenHalli
graph.addrider(10, "Aarav", "KA10AB1234", 4.8, "CAR");
graph.addrider(10, "Manju", "KA10BC5678", 4.5, "AUTO");
graph.addrider(10, "Vijay", "KA10XY9876", 4.7, "BIKE");
graph.addrider(10, "Pradeep", "KA10DE3456", 4.2, "SEDAN");
graph.addrider(10, "Ravi", "KA10FG6789", 4.9, "TRAVELLER");

// Kodigehalli
graph.addrider(11, "Amit", "KA11AB1234", 4.6, "BIKE");
graph.addrider(11, "Sangeeta", "KA11BC5678", 4.4, "AUTO");
graph.addrider(11, "Ananya", "KA11XY9876", 4.8, "CAR");
graph.addrider(11, "Sowmya", "KA11DE3456", 4.3, "SEDAN");
graph.addrider(11, "Nikhil", "KA11FG6789", 4.7, "TRAVELLER");

// Brindavan Gardens
graph.addrider(12, "Deepika", "KA12AB1234", 4.5, "SEDAN");
graph.addrider(12, "Neelam", "KA12BC5678", 4.9, "CAR");
graph.addrider(12, "Ravi", "KA12XY9876", 4.2, "BIKE");
graph.addrider(12, "Ashok", "KA12DE3456", 4.7, "TRAVELLER");
graph.addrider(12, "Priya", "KA12FG6789", 4.4, "AUTO");

// KaranjiLake
graph.addrider(13, "Arvind", "KA13AB1234", 4.8, "CAR");
graph.addrider(13, "Vishal", "KA13BC5678", 4.5, "AUTO");
graph.addrider(13, "Pavithra", "KA13XY9876", 4.7, "BIKE");
graph.addrider(13, "Sanjay", "KA13DE3456", 4.2, "SEDAN");
graph.addrider(13, "Rahul", "KA13FG6789", 4.9, "TRAVELLER");

// Mueseum
graph.addrider(14, "Ravi", "KA14AB1234", 4.6, "BIKE");
graph.addrider(14, "Shankar", "KA14BC5678", 4.4, "AUTO");
graph.addrider(14, "Rama", "KA14XY9876", 4.8, "CAR");
graph.addrider(14, "Teja", "KA14DE3456", 4.3, "SEDAN");
graph.addrider(14, "Yogesh", "KA14FG6789", 4.7, "TRAVELLER");

// DevarajaMarket
graph.addrider(15, "Ravi", "KA15AB1234", 4.5, "SEDAN");
graph.addrider(15, "Sneha", "KA15BC5678", 4.9, "CAR");
graph.addrider(15, "Suman", "KA15XY9876", 4.2, "BIKE");
graph.addrider(15, "Aarthi", "KA15DE3456", 4.7, "TRAVELLER");
graph.addrider(15, "Rakesh", "KA15FG6789", 4.4, "AUTO");

// Ranganathittu
graph.addrider(16, "Arvind", "KA16AB1234", 4.8, "CAR");
graph.addrider(16, "Asha", "KA16BC5678", 4.5, "AUTO");
graph.addrider(16, "Sundar", "KA16XY9876", 4.7, "BIKE");
graph.addrider(16, "Chandana", "KA16DE3456", 4.2, "SEDAN");
graph.addrider(16, "Raghav", "KA16FG6789", 4.9, "TRAVELLER");

// Raidurg
graph.addrider(17, "Suresh", "KA17AB1234", 4.6, "BIKE");
graph.addrider(17, "Vineet", "KA17BC5678", 4.4, "AUTO");
graph.addrider(17, "Kiran", "KA17XY9876", 4.8, "CAR");
graph.addrider(17, "Savita", "KA17DE3456", 4.3, "SEDAN");
graph.addrider(17, "Radhika", "KA17FG6789", 4.7, "TRAVELLER");

// Srinivas colony
graph.addrider(18, "Vikram", "KA18AB1234", 4.8, "CAR");
graph.addrider(18, "Rajesh", "KA18BC5678", 4.5, "AUTO");
graph.addrider(18, "Akshay", "KA18XY9876", 4.7, "BIKE");
graph.addrider(18, "Anita", "KA18DE3456", 4.2, "SEDAN");
graph.addrider(18, "Vijay", "KA18FG6789", 4.9, "TRAVELLER");

// Jayanagar
graph.addrider(19, "Sushma", "KA19AB1234", 4.6, "BIKE");
graph.addrider(19, "Manoj", "KA19BC5678", 4.4, "AUTO");
graph.addrider(19, "Srinivas", "KA19XY9876", 4.8, "CAR");
graph.addrider(19, "Ananya", "KA19DE3456", 4.3, "SEDAN");
graph.addrider(19, "Vikram", "KA19FG6789", 4.7, "TRAVELLER");

// Chunchanakatte
graph.addrider(20, "Avinash", "KA20AB1234", 4.5, "SEDAN");
graph.addrider(20, "Nandini", "KA20BC5678", 4.9, "CAR");
graph.addrider(20, "Krishna", "KA20XY9876", 4.2, "BIKE");
graph.addrider(20, "Neha", "KA20DE3456", 4.7, "TRAVELLER");
graph.addrider(20, "Bhavana", "KA20FG6789", 4.4, "AUTO");

// CBT
graph.addrider(21, "Ravi Kumar", "KA22HF6552", 4.5, "AUTO");
graph.addrider(21, "Nikhil Shetty", "KA22HF6245", 4.9, "CAR");
graph.addrider(21, "Chandan Naidu", "KA22HF6598", 4.2, "SEDAN");
graph.addrider(21, "Anitha Reddy", "KA22HF6752", 4.1, "TRAVELLER");
graph.addrider(21, "Shankar Babu", "KA22HF6587", 4.6, "BIKE");

// Chamrajpet
graph.addrider(22, "Yash Ramesh", "KA01AB1234", 4.8, "CAR");
graph.addrider(22, "Priya Desai", "KA01BC5678", 4.5, "AUTO");
graph.addrider(22, "Arvind Bhat", "KA01XY9876", 4.7, "BIKE");
graph.addrider(22, "Nandini Patil", "KA01DE3456", 4.2, "SEDAN");
graph.addrider(22, "Rachna Rao", "KA01FG6789", 4.9, "TRAVELLER");

// Mysorezoo
graph.addrider(23, "Vikas Prasad", "KA02 AB1234", 4.6, "BIKE");
graph.addrider(23, "Aishwarya Gowda", "KA02BC5678", 4.4, "AUTO");
graph.addrider(23, "Suresh Kiran", "KA02XY9876", 4.8, "CAR");
graph.addrider(23, "Tejaswi Shenoy", "KA02DE3456", 4.3, "SEDAN");
graph.addrider(23, "Radhika Kamath", "KA02FG6789", 4.7, "TRAVELLER");
graph.addrider(23, "Kiran Hegde", "KA02AB6789", 4.8, "TRAVELLER");
graph.addrider(23, "Aarav Kulkarni", "KA02CD1234", 4.5, "AUTO");
graph.addrider(23, "Swathi R", "KA02EF5678", 4.4, "SEDAN");
graph.addrider(23, "Anushka M", "KA02GH9876", 4.9, "BIKE");

// ArtGallery
graph.addrider(24, "Krishna Murthy", "KA03AB1234", 4.5, "SEDAN");
graph.addrider(24, "Geetha K", "KA03BC5678", 4.9, "CAR");
graph.addrider(24, "Rakesh Raj", "KA03XY9876", 4.2, "BIKE");
graph.addrider(24, "Sowmya J", "KA03DE3456", 4.7, "TRAVELLER");
graph.addrider(24, "Manju Nair", "KA03FG6789", 4.4, "AUTO");

//  Lalitha Mahal Palace
graph.addrider(25, "Vinod B", "KA05AB1234", 4.6, "BIKE");
graph.addrider(25, "Ranjani Prabhu", "KA05BC5678", 4.4, "AUTO");
graph.addrider(25, "Naveen K", "KA05XY9876", 4.8, "CAR");
graph.addrider(25, "Sunitha V", "KA05DE3456", 4.3, "SEDAN");
graph.addrider(25, "Manju Reddy", "KA05FG6789", 4.7, "TRAVELLER");
graph.addrider(25, "Prashanth T", "KA05AB6789", 4.7, "CAR");
graph.addrider(25, "Sushma N", "KA05CD1234", 4.6, "BIKE");
graph.addrider(25, "Dinesh K", "KA05EF5678", 4.8, "TRAVELLER");
graph.addrider(25, "Radha J", "KA05GH9876", 4.5, "AUTO");

//  KRS Dam
graph.addrider(26, "Aditya Shetty", "KA06AB1234", 4.5, "SEDAN");
graph.addrider(26, "Srinidhi G", "KA06BC5678", 4.9, "CAR");
graph.addrider(26, "Kavya Prasad", "KA06XY9876", 4.2, "BIKE");
graph.addrider(26, "Pavani T", "KA06DE3456", 4.7, "TRAVELLER");
graph.addrider(26, "Harsha S", "KA06FG6789", 4.4, "AUTO");

//  Mall of Mysore
graph.addrider(27, "Raghu R", "KA07AB1234", 4.8, "CAR");
graph.addrider(27, "Kavitha V", "KA07BC5678", 4.5, "AUTO");
graph.addrider(27, "Manoj Bhat", "KA07XY9876", 4.7, "BIKE");
graph.addrider(27, "Veena P", "KA07DE3456", 4.2, "SEDAN");
graph.addrider(27, "Harsha B", "KA07FG6789", 4.9, "TRAVELLER");

// Mahadevpur
graph.addrider(28, "Bhavana K", "KA08AB1234", 4.6, "BIKE");
graph.addrider(28, "Ravi V", "KA08BC5678", 4.4, "AUTO");
graph.addrider(28, "Kavita R", "KA08XY9876", 4.8, "CAR");
graph.addrider(28, "Tejas Hegde", "KA08DE3456", 4.3, "SEDAN");
graph.addrider(28, "Sowmya P", "KA08FG6789", 4.7, "TRAVELLER");

// University of Mysore
graph.addrider(29, "Vidya S", "KA09AB1234", 4.5, "SEDAN");
graph.addrider(29, "Amit Kumar", "KA09BC5678", 4.9, "CAR");
graph.addrider(29, "Kiran J", "KA09XY9876", 4.2, "BIKE");
graph.addrider(29, "Aravind T", "KA09DE3456", 4.7, "TRAVELLER");
graph.addrider(29, "Shilpa B", "KA09FG6789", 4.4, "AUTO");

// Jayachamarajendra College
graph.addrider(30, "Aarav Reddy", "KA10AB1234", 4.8, "CAR");
graph.addrider(30, "Manju Prabhu", "KA10BC5678", 4.5, "AUTO");
graph.addrider(30, "Vijay M", "KA10XY9876", 4.7, "BIKE");
graph.addrider(30, "Pradeep S", "KA10DE3456", 4.2, "SEDAN");
graph.addrider(30, "Ravi Kiran", "KA10FG6789", 4.9, "TRAVELLER");

// Sayyaji Rao Road
graph.addrider(31, "Amit R", "KA11AB1234", 4.6, "BIKE");
graph.addrider(31, "Sangeeta S", "KA11BC5678", 4.4, "AUTO");
graph.addrider(31, "Ananya N", "KA11XY9876", 4.8, "CAR");
graph.addrider(31, "Sowmya M", "KA11DE3456", 4.3, "SEDAN");
graph.addrider(31, "Nikhil B", "KA11FG6789", 4.7, "TRAVELLER");

// Ashoka Road
graph.addrider(32, "Deepika M", "KA12AB1234", 4.5, "SEDAN");
graph.addrider(32, "Neelam Bhat", "KA12BC5678", 4.9, "CAR");
graph.addrider(32, "Ravi Reddy", "KA12XY9876", 4.2, "BIKE");
graph.addrider(32, "Ashok G", "KA12DE3456", 4.7, "TRAVELLER");
graph.addrider(32, "Priya Hegde", "KA12FG6789", 4.4, "AUTO");

// M.G. Road
graph.addrider(33, "Arvind S", "KA13AB1234", 4.8, "CAR");
graph.addrider(33, "Vishal K", "KA13BC5678", 4.5, "AUTO");
graph.addrider(33, "Pavithra S", "KA13XY9876", 4.7, "BIKE");


// Initialize rider ratings
graph.initializeriderratings();

// Main loop for user interaction
int login_choice = 0;
string userid, passcode;
int display = 0;

while (display != 3) {
    cout << "1.Display the graph\n2.Continue to Travel ease\n";
    cin >> display;
    switch (display) {
        case 1:
            graph.display_graph();
            break;
        case 2:
            system("cls");
            pattern();

            while (login_choice != 3) {
                cout << "\n1. Login\n2. Register\n3. Exit\n";
                cin >> login_choice;

                switch (login_choice) {
                    case 1:
                        Graph load_from_file();
                        cout << "Enter your user_id: ";
                        cin >> userid;

                        cout << "Enter your passcode: ";
                        getPassword(passcode);

                        if (authenticateUser (login, login_count, userid, passcode) != -1) {
                            system("cls");
                            cout << "Authentication successful\n";
                            cout << "Welcome " << userid << "\n";

                            int auth_choice = 0;
                            while (auth_choice != 2) {
                                cout << "\n1. Book a ride\n2. Exit\n";
                                cin >> auth_choice;

                                switch (auth_choice) {
                                    case 1:
                                        system("cls");
                                        string src_name, dest_name;
                                        cout << "Enter current location: ";
                                        cin >> src_name;

                                        cout << "Enter destination location: ";
                                        cin >> dest_name;

                                        int src = graph.find_vertex_index(src_name);
                                        int dest = graph.find_vertex_index(dest_name);

                                        if (src == -1 || dest == -1) {
                                            cout << "Invalid source or destination location.\n";
                                            break;
                                        } else {
                                            system("cls");
                                            float distance = graph.dijkstra(src, dest);
                                            Sleep(4000);
                                            graph.displayRidersSorted(src, distance, userid, src_name, dest_name);
                                            Sleep(4000);
                                            system("cls");
                                        }
                                        break;
                                }
                            }
                        } else {
                            cout << "Authentication failed: Please check your login credentials or register yourself\n";
                        }
                        break;

                    case 2:
                        cout << "Enter your user_id: ";
                        cin >> userid;

                        for (int b = 0; b < login_count; b++) {
                            if (login[b].userid == userid) {
                                cout << "\nUsername already taken\nPlease enter a different username:\n";
                                Sleep(500);
                                cout << "Enter your user_id: ";
                                cin >> userid;
                                break;
                            }
                        }

                        cout << "Enter your passcode: ";
                        cin >> passcode;

                        for (int a = 0; a < login_count; a++) {
                            if (login[a].userid == userid && login[a].passcode == passcode) {
                                cout << "User  already exists\n";
                                break;
                            }
                        }

                        std::ofstream fp("LOGIN_DET.txt", ios::app);
                        if (fp.is_open()) {
                            fp << userid << "\t" << passcode << endl;
                            fp.close();
                        } else {
                            cout << "File Not found\n";
                        }


                }
            }
    }
}
}
