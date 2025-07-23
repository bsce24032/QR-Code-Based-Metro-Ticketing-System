#pragma once

#include "stations-metro.h" // MetroStation, Station definitions
#include "passengers-staff.h" // PassengerData
#include "tickets-QRgen.h" // Possibly Identity/Person
#include "payments.h" // Staff/Admin
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include "json.hpp"
#include <iomanip>

using json = nlohmann::json;
using namespace std;

/* 
=======================================
1. INHERITANCE + COMPOSITION
QrDecode uses COMPOSITION: It 'has-a' MetroStation object pointer
We own this pointer and use it to validate ticket stations
=======================================
*/
class QrDecode {
protected:
    MetroStation* metrostation;  // Composition: QrDecode *has-a* MetroStation
    json decoded;                // Stores decoded QR JSON content

public:
    void callScript() {
#ifdef _WIN32
    // On Windows: assume 'python' is on PATH
    const char* command = "python QRscanner.py";
    FILE* pipe = _popen(command, "r");
#else
    // On Unix (Linux/macOS): call python3
    const char* command = "python3 QRscanner.py";
    FILE* pipe = popen(command, "r");
#endif

    if (!pipe) {
        cerr << "❌ Failed to run Python script!" << endl;
        return;
    }

    // Capture and display script output
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        cout << buffer;
    }

    // Clean up the pipe correctly for each OS
#ifdef _WIN32
    _pclose(pipe);
#else
    pclose(pipe);
#endif
}



    // Read decoded QR code JSON from file
    void readJsonOutput() {
        ifstream file("decoded.json");
        if (!file.is_open()) {
            cerr << "Could not open decoded.json file." << endl;
            return;
        }

        try {
            file >> decoded;
            cout << "\n--- Parsed QR JSON ---\n";
            cout << "Name: " << decoded["Name"] << "\n";
            cout << "CNIC: " << decoded["CNIC"] << "\n";
            cout << "Departure: " << decoded["Departure"] << "\n";
            cout << "Arrival: " << decoded["Arrival"] << "\n";
        } catch (const exception& e) {
            cerr << "Error parsing decoded JSON: " << e.what() << endl;
        }
    }

    // Validate ticket against station codes
    void validateTicketWithStations() {
        if (decoded.empty()) {
            cerr << "No QR data found to validate." << endl;
            return;
        }

        string depCode = decoded["Departure"];
        string arrCode = decoded["Arrival"];

        bool depFound = false, arrFound = false;

        // Validate codes by iterating over stations
        for (const Station& s : metrostation->returnStations()) {
            if (s.getStationCode() == depCode) depFound = true;
            if (s.getStationCode() == arrCode) arrFound = true;
        }

        if (depFound && arrFound)
            cout << "✅ Ticket is valid. Both stations exist.\n";
        else
            cerr << "❌ Invalid ticket. Station(s) not found.\n";
    }

    // COMPOSITION: Set the station object
    void setMetroStation(MetroStation* m) {
        metrostation = m;
    }

    // Getter for decoded QR JSON
    json getDecoded() const {
        return decoded;
    }
};

/*
=======================================
2. POLYMORPHISM via INTERFACE
Validator is an abstract class that enforces implementation of validation logic.
=======================================
*/
class Validator {
public:
    virtual void readDecodedJson() = 0;
    virtual void validate() = 0;
    virtual ~Validator() {}
};

/*
=======================================
3. INHERITANCE + POLYMORPHISM
JsonViewerChecker inherits from both Validator (interface) and QrDecode (base class)
=======================================
*/
class JsonViewerChecker : public Validator, public QrDecode {
private:
    vector<PassengerData> passengers;  // Store passenger objects
    json passengerData;                // Store original passenger JSON
    string validatedName;

public:
    // Load decoded QR JSON
    void readDecodedJson() override {
        ifstream inFile("decoded.json");
        if (!inFile.is_open()) {
            cerr << "❌ Could not open decoded.json" << endl;
            return;
        }

        try {
            inFile >> decoded;
            cout << "✅ QR JSON loaded\n";
        } catch (const exception& e) {
            cerr << "❌ JSON parsing error: " << e.what() << endl;
        }
    }

    // Load passengers from file and populate vector
    void loadPassengersFromFile(const string& filename = "passenger_data.json") {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "❌ Could not open passenger data file\n";
            return;
        }

        try {
            json data;
            file >> data;
            passengerData = data;

            // Convert each entry into PassengerData object
            for (const auto& entry : data) {
                string name = entry.value("Name", "");
                string cnic = entry.value("CNIC", "");
                int age = 0;

                // Handle age string/number formats
                if (entry["Age"].is_string()) {
                    age = stoi(entry["Age"].get<string>());
                } else if (entry["Age"].is_number()) {
                    age = entry["Age"].get<int>();
                } else {
                    cerr << "⚠️ Invalid age format.\n";
                    continue;
                }

                passengers.emplace_back(name, age, cnic);
            }

            cout << "✅ Passengers loaded.\n";
        } catch (const exception& e) {
            cerr << "❌ Error loading passenger data: " << e.what() << endl;
        }
    }

    // Validate QR name against passenger list
    void validate() override {
        if (decoded.empty() || passengers.empty()) {
            cerr << "❌ Missing data for validation.\n";
            return;
        }

        string name = decoded["Name"];
        bool found = false;

        for (const auto& p : passengers) {
            if (p.getName() == name) {
                validatedName = name;
                cout << "✅ Name matched.\n";
                found = true;
                break;
            }
        }

        if (!found) {
            cout << "❌ Name not found.\n";
        }
    }

    string getValidatedName() const { return validatedName; }
    json getPassengerData() const { return passengerData; }

    // Extra name-age validator
    bool validatePassenger(const string& inputName, int inputAge) {
        for (const auto& p : passengers) {
            if (toLower(p.getName()) == toLower(inputName) && p.getAge() == inputAge) {
                validatedName = p.getName();
                return true;
            }
        }
        return false;
    }

private:
    string toLower(const string& str) const {
        string lowered = str;
        transform(lowered.begin(), lowered.end(), lowered.begin(), ::tolower);
        return lowered;
    }
};

/*
=======================================
4. AGGREGATION: FeedbackReporter uses a pointer to Feedback
We don’t own it; just use it.
=======================================
*/
class Feedback {
private:
    map<string, vector<pair<int, string>>> stationRatings;

public:
    bool validatePassenger(const string& name, int age) {
        return age >= 18;
    }

    void addFeedback(const string& stationName, int rating, const string& comment) {
        stationRatings[stationName].emplace_back(rating, comment);
    }

    double getAverageRating(const string& stationName) {
        auto it = stationRatings.find(stationName);
        if (it != stationRatings.end() && !it->second.empty()) {
            double total = 0;
            for (const auto& [r, _] : it->second) total += r;
            return total / it->second.size();
        }
        return 0.0;
    }

    void printRatings() {
        for (const auto& [station, ratings] : stationRatings) {
            cout << "Station: " << station << ", Avg Rating: " << getAverageRating(station) << endl;
        }
    }

    void saveToJson(const string& filename) {
        json existing;
        ifstream inFile(filename);
        if (inFile.is_open()) {
            try { inFile >> existing; } 
            catch (...) { cerr << "⚠️ Couldn't parse existing file.\n"; }
        }

        for (const auto& [station, entries] : stationRatings) {
            for (const auto& [rating, comment] : entries) {
                existing[station].push_back({ rating, comment });
            }
        }

        ofstream outFile(filename);
        outFile << setw(4) << existing;
        cout << "✅ Feedback saved.\n";
    }
};

/*
=======================================
5. AGGREGATION CONTINUES
FeedbackReporter aggregates Feedback class (pointer)
=======================================
*/
class FeedbackReporter {
private:
    Feedback* feedbackPtr;

public:
    FeedbackReporter(Feedback* fb) : feedbackPtr(fb) {}

    void reportForStation(const string& stationName) {
        double avg = feedbackPtr->getAverageRating(stationName);
        if (avg == 0.0) cout << "No ratings for " << stationName << "\n";
        else cout << "Avg rating for " << stationName << ": " << avg << "\n";
    }

    void printAllRatings() {
        feedbackPtr->printRatings();
    }

    void addFeedbackForStation(const string& stationName, int rating, const string& comment) {
        feedbackPtr->addFeedback(stationName, rating, comment);
    }

    Feedback* returnPointer() {
        return feedbackPtr;
    }
};

/*
=======================================
6. ASSOCIATION: AccessMetro uses FeedbackReporter loosely
This is association (no lifetime management)
=======================================
*/
class AccessMetro {
private:
    FeedbackReporter* reporter;

public:
    AccessMetro(FeedbackReporter* fbReporter) : reporter(fbReporter) {}

    void welcome() {
        cout << "===== Metro Feedback System =====\n";
    }

    void collectAndSaveFeedback() {
        string name, station, comment;
        int age, rating;

        cout << "Enter your name: ";
        cin >> name;
        cout << "Enter your age: ";
        cin >> age;

        if (!reporter->returnPointer()->validatePassenger(name, age)) {
            cout << "Invalid passenger.\n";
            return;
        }

        cout << "Enter station name: ";
        cin.ignore();
        getline(cin, station);
        cout << "Enter rating (1-5): ";
        cin >> rating;
        cin.ignore();
        cout << "Enter comment: ";
        getline(cin, comment);

        reporter->addFeedbackForStation(station, rating, comment);
    }

    void saveFeedbacksToJson(const string& filename) {
        reporter->returnPointer()->saveToJson(filename);
    }

    void reportStationFeedback(const string& stationName) {
        reporter->reportForStation(stationName);
    }

    void printAllStationFeedbacks() {
        reporter->printAllRatings();
    }
};

/*
=======================================
7. SINGLETON + OPERATOR OVERLOADING
=======================================
*/
class Author {
private:
    vector<string> authors;
    static Author* instance;

    Author() {
        authors = { "Shahmeer", "Umer", "Kashif", "Shawaiz", "Arham" };
    }

public:
    Author(const Author&) = delete;
    Author& operator=(const Author&) = delete;

    static Author* getInstance() {
        if (!instance) instance = new Author();
        return instance;
    }

    void addAuthor(const string& name) {
        authors.push_back(name);
    }

    // OPERATOR OVERLOADING
    friend ostream& operator<<(ostream& os, const Author& a) {
        os << "📚 Authors:\n";
        for (const auto& name : a.authors) {
            os << "- " << name << "\n";
        }
        return os;
    }
};

// Define static singleton pointer
Author* Author::instance = nullptr;

/*
=======================================
8. TEMPLATE CLASS + EXCEPTION HANDLING
Passenger searcher that searches by name or age using template
=======================================
*/
template <typename T>
class PassengerSearcher {
public:
    void search(T key) {
        try {
            ifstream file("passenger_data.json");
            if (!file) throw runtime_error("❌ Cannot open passenger_data.json");

            json data;
            file >> data;
            bool found = false;

            if constexpr (is_same_v<T, string>) {
                for (const auto& p : data) {
                    if (p["Name"] == key) {
                        printPassenger(p);
                        found = true;
                    }
                }
            } else if constexpr (is_same_v<T, int>) {
                for (const auto& p : data) {
                    if (p["Age"] == key) {
                        printPassenger(p);
                        found = true;
                    }
                }
            } else {
                throw invalid_argument("Unsupported search type.");
            }

            if (!found) cout << "❗ No matching passenger found.\n";

        } catch (const exception& e) {
            cerr << "❌ Error: " << e.what() << "\n";
        }
    }

private:
    void printPassenger(const json& p) {
        cout << "👤 Name: " << p["Name"]
             << ", Age: " << p["Age"]
             << ", CNIC: " << p["CNIC"] << "\n";
    }
};