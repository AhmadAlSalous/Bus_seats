#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>

using namespace std;

class Seat {
public:
    int row;
    char col;
    string seatType;
    bool available;
    string passenger;

    //i will create a deafult constructor
    Seat() : row(0), col('A'), seatType("Passenger"), available(true), passenger("") {}

    // another parameterized constructor
    Seat(int r, char c, string type = "Passenger", bool avail = true)
            : row(r), col(c), seatType(type), available(avail), passenger("") {}
};

class Passenger {
public:
    string name;
    string passengerType;

    Passenger(string n, string type) : name(n), passengerType(type) {}
};

class SeatingPlan {
private:
    int rows;
    int cols;
    vector<vector<Seat>> seats;
    vector<vector<vector<string>>> history;

    void initializeSeats() {
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                seats[i][j] = Seat(i + 1, 'A' + j);
            }
        }
        //  i will initialize these seats as occupied and they are for the driver and co-driver
        seats[0][0] = Seat(1, 'A', "Driver", false);
        seats[0][1] = Seat(1, 'B', "Co-Driver", false);
        saveToHistory();
    }

    void saveToHistory() {
        vector<vector<string>> currentPlan;
        for (auto& row : seats) {
            vector<string> rowData;
            for (auto& seat : row) {
                rowData.push_back(seat.passenger);
            }
            currentPlan.push_back(rowData);
        }
        if (history.size() >= 5) {
            history.erase(history.begin());
        }
        history.push_back(currentPlan);
    }

    bool canSeatPassenger(Seat& seat, Passenger& passenger) {
        // a method tp check if passenger type is Child and seat is next to a door
        if (passenger.passengerType == "Child" &&
            ((seat.row == 4 && (seat.col == 'C' || seat.col == 'D')) ||
             (seat.row == 6 && (seat.col == 'C' || seat.col == 'D')) ||
             (seat.row == 8 && (seat.col == 'C' || seat.col == 'D')))) {
            return false; //  notallow  seat child next to the door
        }



        return true; // Allow seating for other passengers
    }

public:
    SeatingPlan(int r = 8, int c = 4) : rows(r), cols(c), seats(r, vector<Seat>(c)) {
        initializeSeats();
    }

    string allocateSeat(string seatNumber, Passenger passenger) {
        int row = seatNumber[0] - '0' - 1;
        char col = seatNumber[1];

        if (row >= 0 && row < rows && col >= 'A' && col < 'A' + cols) {
            int colIndex = col - 'A';
            Seat& seat = seats[row][colIndex];
            if (seat.available && canSeatPassenger(seat, passenger)) {
                seat.available = false;
                seat.passenger = passenger.name;
                saveToHistory();
                ostringstream oss;
                oss << "Seat " << seatNumber << " allocated to " << passenger.name;
                return oss.str();
            } else if (!seat.available) {
                return "Seat is already booked.";
            } else {
                return "Seat not suitable for the passenger type.";
            }
        }
        return "Seat number is invalid.";
    }

    void autoAllocateSeats(vector<Passenger>& passengers) {
        for (Passenger& passenger : passengers) {
            bool allocated = false;
            for (int i = 0; i < rows && !allocated; ++i) {
                for (int j = 0; j < cols && !allocated; ++j) {
                    Seat& seat = seats[i][j];
                    if (seat.available && canSeatPassenger(seat, passenger)) {
                        seat.available = false;
                        seat.passenger = passenger.name;
                        allocated = true;
                    }
                }
            }
        }
        saveToHistory();
    }

    void displaySeatingPlan() {
        cout << "Seating Plan (O: Available, X: Unavailable):" << endl;
        for (auto& row : seats) {
            for (auto& seat : row) {
                cout << (seat.available ? 'O' : 'X') << ' ';
            }
            cout << endl;
        }
    }

    void displayAvailableSeats() {
        cout << "Available Seats:" << endl;
        for (auto& row : seats) {
            for (auto& seat : row) {
                if (seat.available) {
                    cout << seat.row << seat.col << " ";
                }
            }
            cout << endl;
        }
    }

    void resetSeatingPlan() {
        for (auto& row : seats) {
            for (auto& seat : row) {
                seat.available = true;
                seat.passenger = "";
            }
        }
        initializeSeats();
    }

    void saveToFile(string filename = "seating_plan.txt") {
        ofstream file(filename);
        if (!file.is_open()) {
            cerr << "Error: Unable to open file " << filename << " for writing." << endl;
            return;
        }

        for (const auto& plan : history) {
            for (const auto& row : plan) {
                for (const auto& seat : row) {
                    file << setw(10) << (seat.empty() ? "Empty" : seat) << ' ';
                }
                file << endl;
            }
            file << endl;
        }

        file.close();
    }


    void loadFromFile(string filename = "seating_plan.txt") {
        ifstream file(filename);
        if (file.is_open()) {
            history.clear();
            vector<vector<string>> currentPlan;
            string line;
            while (getline(file, line)) {
                if (line.empty()) {
                    if (!currentPlan.empty()) {
                        history.push_back(currentPlan);
                        currentPlan.clear();
                    }
                } else {
                    vector<string> rowData;
                    istringstream iss(line);
                    string seat;
                    while (iss >> seat) {
                        rowData.push_back(seat == "Empty" ? "" : seat);
                    }
                    currentPlan.push_back(rowData);
                }
            }
            if (!currentPlan.empty()) {
                history.push_back(currentPlan);
            }
            file.close();
        }
    }

    void retrieveSeatingPlan(int planNumber) {
        if (planNumber >= 0 && planNumber < history.size()) {
            for (int i = 0; i < rows; ++i) {
                for (int j = 0; j < cols; ++j) {
                    seats[i][j].passenger = history[planNumber][i][j];
                    seats[i][j].available = history[planNumber][i][j].empty();
                }
            }
        } else {
            cout << "Invalid plan number" << endl;
        }
    }

    void displaySeatingAllocation() {
        for (auto& row : seats) {
            for (auto& seat : row) {
                if (!seat.available) {
                    cout << "Passenger: " << seat.passenger << " is seated at Row " << seat.row << ", Column " << seat.col << endl;
                }
            }
        }
    }

    void displayRules() {
        cout << "Seating Allocation Rules:" << endl;
        cout << "1. Families and ladies must first be seated in the Family & Lady’s section." << endl;
        cout << "2. Children must be safely seated away from 'Door' positions." << endl;
        cout << "3. Male passengers must not be seated in the Family & Lady’s section." << endl;
        cout << "4. The seating plan is computer-generated and does not depend on user inputs or selection." << endl;
    }

    int getHistorySize() const {
        return history.size();
    }
};

int main() {
    SeatingPlan spa;
    spa.loadFromFile();

    vector<Passenger> passengers = {
        Passenger("ABD", "Male"),
        Passenger("Elisa", "Lady"),
        Passenger("Ali", "Child"),
        Passenger("Ben", "Family"),
        Passenger("Ahmad" ,"Child" ),
        Passenger("Adrian", "Male"),
        Passenger("Anam", "Male"),
        Passenger("John Doe", "Male"),
        Passenger("Abdulla K", "Male"),
        Passenger("Khaled M", "Male"),
        Passenger("Ismael", "Male"),
        Passenger("Yarah", "Lady"),
        Passenger("Martin", "Child"),
        Passenger("Rosie", "Child"),

    };

    spa.autoAllocateSeats(passengers);

    spa.displayRules();

    char choice;
    while (true) {
        cout << "\nMenu:" << endl;
        cout << "D - Display seating allocation" << endl;
        cout << "V - View available seats" << endl;
        cout << "B - Book a seat" << endl;
        cout << "R - Reset seating plan" << endl;
        cout << "L - Load a previous plan" << endl;
        cout << "Q - Quit" << endl;
        cout << "Enter your choice: ";
        cin >> choice;
        cout << endl;

        if (choice == 'D' || choice == 'd') {
            spa.displaySeatingAllocation();
        } else if (choice == 'V'|| choice == 'v') {
            spa.displayAvailableSeats();
        } else if (choice == 'B' || choice == 'b') {
            string seatNumber, passengerName, passengerType;
            cout << "Enter seat number to book (e.g., 1A): ";
            cin >> seatNumber;
            cout << "Enter passenger name: ";
            cin >> passengerName;
            cout << "Enter passenger type (Family, Lady, Male, Child): ";
            cin >> passengerType;
            cout << spa.allocateSeat(seatNumber, Passenger(passengerName, passengerType)) << endl;
        } else if (choice == 'R' || choice == 'r') {
            spa.resetSeatingPlan();
            cout << "Seating plan has been reset." << endl;
        } else if (choice == 'L' || choice == 'l') {
            int planNumber;
            cout << "Enter plan number to load (0 to " << spa.getHistorySize() - 1 << "): ";
            cin >> planNumber;
            spa.retrieveSeatingPlan(planNumber);
            cout << "Loaded seating plan " << planNumber << endl;
        } else if (choice == 'Q' || choice == 'q') {
            break;
        }
    }

    // Save the current state
    spa.saveToFile();

    return 0;
}

