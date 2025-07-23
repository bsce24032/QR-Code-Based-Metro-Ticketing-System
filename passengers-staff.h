#pragma once
#include<iostream>
#include<string>
#include<vector>
#include<fstream>
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

// -------------------- Identity Class --------------------
// Represents CNIC (national identity) data
// Used by Person class through composition (has-a relationship)
class Identity {
private:
    string cnic;  // Encapsulation: CNIC is private

public:
    Identity() {}
    Identity(string cnic) : cnic(cnic) {}

    // Getter and Setter (Encapsulation)
    string getCnic() const { return cnic; }
    void setCnic(string a) { cnic = a; }
};

// -------------------- Person Class (Base Class) --------------------
// Represents a general person with name and age
// Composed with Identity class (Composition)
class Person {
protected:
    Identity identity;  // Composition: Person "has-an" Identity
    string name;
    int age;

public:
    Person() {}
    Person(string n, int a) : name(n), age(a) {}

    // Encapsulated access
    void setName(string n) { name = n; }
    string getName() const { return name; }

    void setAge(int a) { age = a; }
    int getAge() const { return age; }

    void setCnic(const string& cnic) {
        identity.setCnic(cnic);
    }
};

// -------------------- PassengerData Class --------------------
// Inherits from Person (Inheritance)
// Stores and handles multiple passenger records
class PassengerData : public Person {
protected:
    // Static vector shared among all instances (Aggregation)
    static vector<PassengerData> totalpassengers;

public:
    PassengerData() {}

    PassengerData(string name, int age, string cnic) : Person(name, age) {
        this->name = name;
        this->age = age;
        identity.setCnic(cnic);
    }

    // Aggregation: Adding object to global passenger list
    void addPassenger(const PassengerData& p) {
        totalpassengers.push_back(p);
    }

    // Static method to access shared passenger list
    static vector<PassengerData> getVector() {
        return totalpassengers;
    }

    // Writes all passenger data (existing + current) to JSON
    static void passengerDataJson() {
        json passengerdata = json::array();

        // Load existing data
        ifstream infile("passenger.json");
        if (infile.is_open()) {
            try {
                infile >> passengerdata;
            } catch (...) {
                cerr << "\t\t\tError reading existing passenger data. Starting fresh.\n";
            }
            infile.close();
        }

        // Add new passengers
        for (const PassengerData& p : totalpassengers) {
            json entry;
            entry["Name"] = p.name;
            entry["Age"] = p.age;
            entry["CNIC"] = p.identity.getCnic();
            passengerdata.push_back(entry);
        }

        // Save updated data
        ofstream outfile("passenger_data.json");
        if (outfile.is_open()) {
            outfile << passengerdata.dump(4);
            outfile.close();
            cout << "\t\t\tPassenger data successfully updated in passenger_data.json\n";
        } else {
            cerr << "\t\t\tFailed to open passenger_data.json for writing.\n";
        }
    }

    // Encapsulated getter for CNIC
    string getCnic() const {
        return identity.getCnic();
    }
};

// // Define the static member outside the class
// vector<PassengerData> PassengerData::totalpassengers;

// -------------------- Staff Class --------------------
// Inherits from Person
// Aggregates multiple staff members
class Staff : public Person {
protected:
    vector<Staff> totalstaff;  // Aggregation: maintains a list of staff
    string department;
    int salary;

public:
    Staff() {}
    Staff(string name, int age, string department, int salary)
        : Person(name, age), department(department), salary(salary) {}

    void addStaff(const Staff& s) {
        totalstaff.push_back(s);
    }

    vector<Staff> returnStaff() {
        return totalstaff;
    }
};

// -------------------- Admin Class --------------------
// Inherits from Person
// Handles account and payment data
// Demonstrates association with file/JSON resources
class Admin : public Person {
private:
    int accountnumber;
    string adminname;
    int totalbalance;

public:
    Admin() : Person(), accountnumber(0), adminname(""), totalbalance(0) {}

    Admin(string name, int age, string cnic, int accNum, string adminName)
        : Person(name, age), accountnumber(accNum), adminname(adminName) {
        identity.setCnic(cnic);
    }

    void adminAccountOutput() const {
        cout << "Admin Name: " << adminname << endl;
        cout << "Account Number: " << accountnumber << endl;
    }

    void adminBalance() const {
        cout << "Total Balance " << totalbalance << endl;
    }

    // Reads and displays payment details from a JSON file
    // Association: Admin class uses file resources (not owned)
    void passengerPaymentAccount() const {
        cout << "\n\t\t\tFetching Passenger Payment Accounts..." << endl;

        fstream file("Payment_Method.json", ios::in);
        if (!file.is_open()) {
            cerr << "\t\t\tUnable to open payment file." << endl;
            return;
        }

        json payment_data;
        try {
            file >> payment_data;
        } catch (const json::parse_error& e) {
            cerr << "\t\t\tJSON parsing error: " << e.what() << endl;
            file.close();
            return;
        }
        file.close();

        if (payment_data.empty()) {
            cout << "\t\t\tNo payment records found." << endl;
            return;
        }

        for (const auto& p : payment_data) {
            cout << "\n";
            if (p.contains("Name")) cout << "\t\t\tName: " << p["Name"] << endl;
            if (p.contains("Age")) cout << "\t\t\tAge: " << p["Age"] << endl;
            if (p.contains("Payment Type")) cout << "\t\t\tPayment Method: " << p["Payment Type"] << endl;
            if (p.contains("Number")) cout << "\t\t\tNumber: " << p["Number"] << endl;
            if (p.contains("Amount")) cout << "\t\t\tAmount Paid: " << p["Amount"] << endl;
            if (p.contains("Status")) cout << "\t\t\tStatus: " << p["Status"] << endl;
        }
    }
};