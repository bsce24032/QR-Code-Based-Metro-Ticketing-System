#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
// #include <filesystem>
#include "json.hpp"

using namespace std;
using json = nlohmann::json;
// namespace fs = std::filesystem;

// -------------------- Class: PricePerStation --------------------
// This class encapsulates price and distance info for a station.
// ✔️ OOP Concept: Encapsulation
class PricePerStation {
private:
    int price = 0;
    int distance = 0;

public:
    void setPrice(int p) { price = p; }
    int getPrice() const { return price; }

    void setDistance(int d) { distance = d; }
    int getDistance() const { return distance; }
};

// -------------------- Class: Station --------------------
// This class represents a station and inherits price/distance from PricePerStation.
// ✔️ OOP Concepts: Inheritance, Encapsulation, Abstraction
class Station : public PricePerStation {
private:
    string stationname;
    string stationcode;

public:
    Station() = default;

    // Constructor to initialize a full station
    Station(const string& name, const string& code, int price, int distance) {
        setStationName(name);
        setStationCode(code);
        setPrice(price);         // inherited
        setDistance(distance);   // inherited
    }

    // ✔️ Copy constructor
    Station(const Station& other) {
        stationname = other.stationname;
        stationcode = other.stationcode;
        setPrice(other.getPrice());
        setDistance(other.getDistance());
    }

    // Setters and getters
    void setStationName(const string& name) { stationname = name; }
    void setStationCode(const string& code) { stationcode = code; }

    string getStationName() const { return stationname; }
    string getStationCode() const { return stationcode; }
};

// -------------------- Class: SaveStationIntoFile --------------------
// This class handles writing station data to JSON files.
// ✔️ OOP Concepts: Abstraction, Encapsulation
class SaveStationIntoFile {
private:
    json stationsave = json::array(); // Store station data before writing

public:
    // Saves unique stations to JSON array
    void saveStationtoJSON(const vector<Station>& stations) {
        for (const auto& station : stations) {
            json stationObj;
            stationObj["station_name"] = station.getStationName();
            stationObj["station_code"] = station.getStationCode();
            stationObj["price"] = station.getPrice();
            stationObj["distance"] = station.getDistance();

            // Avoid saving duplicates
            bool alreadyExists = false;
            for (const auto& savedStation : stationsave) {
                if (savedStation["station_code"] == station.getStationCode()) {
                    alreadyExists = true;
                    break;
                }
            }

            if (!alreadyExists) {
                stationsave.push_back(stationObj);
            }
        }
    }

    // Writes saved stations to a JSON file
    void writeToFile(const string& filename) {
        ofstream file(filename);
        if (file.is_open()) {
            file << stationsave.dump(4); // Pretty print with indentation
            file.close();
            cout << "✅ Station data written to " << filename << endl;
        } else {
            cerr << "❌ Error opening file for writing!" << endl;
        }
    }

    // Returns all station data as JSON
    json getAllStationJson() const {
        return stationsave;
    }

    // Checks if a file is missing or empty
    bool isFileEmptyOrNonExistent(const string& filename) {
        // if (!fs::exists(filename)) return true;
        ifstream file(filename);
        return file.peek() == ifstream::traits_type::eof();
    }
};

// -------------------- Class: MetroStation --------------------
// Represents a full metro system containing multiple stations.
// ✔️ OOP Concepts: Association, Composition, Encapsulation
class MetroStation {
private:
    SaveStationIntoFile* save;         // ✔️ Association: uses SaveStationIntoFile (but doesn't own it)
    string metroname;
    vector<Station> stations;          // ✔️ Composition: MetroStation owns these Station objects

public:
    MetroStation() = default;

    // Constructor to inject a SaveStationIntoFile dependency
    MetroStation(SaveStationIntoFile* saveObj) : save(saveObj) {}

    // Setters and Getters
    void setMetroName(const string& name) { metroname = name; }
    string getMetroName() const { return metroname; }

    // Add a new station to the metro
    void addStation(const Station& station) {
        stations.push_back(station);
    }

    // Get all added stations
    vector<Station> returnStations() const {
        return stations;
    }

    // Save stations using associated SaveStationIntoFile
    void saveStationsToFile(const string& filename) {
        if (save) {
            save->saveStationtoJSON(stations);
            save->writeToFile(filename);
        }
    }

    // Return all station data as JSON
    json getStationJson() const {
        return save ? save->getAllStationJson() : json::array();
    }

    // Initialize hardcoded Lahore Metro stations if file is missing/empty
    void initializeLahoreMetroStations(const string& filename) {
        if (!save->isFileEmptyOrNonExistent(filename)) {
            loadStationsFromFile(filename); // Load instead of creating new
            return;
        }

        // Hardcoded names for demo
        vector<string> names = {
            "Ali Town", "Thokar Niaz Baig", "Canal View", "Wahdat Road", "Awan Town",
            "Sabzazar", "Shahnoor", "Bund Road", "Samanabad", "Gulshan-e-Ravi",
            "Chauburji", "Lake Road", "GPO", "Lakshmi Chowk", "Mayo Gardens",
            "Sultanpura", "UET", "Baghbanpura", "Shalimar Gardens", "Dera Gujran"
        };

        for (size_t i = 0; i < names.size(); ++i) {
            string code = "LHR" + to_string(i + 1);
            int price = 30;
            int distance = (i * 3);
            Station s(names[i], code, price, distance);
            addStation(s);
        }

        saveStationsToFile(filename); // Save created data
    }

    // Load station data from a file
    void loadStationsFromFile(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "❌ Error opening file for reading!" << endl;
            return;
        }

        json stationData;
        file >> stationData;

        for (const auto& stationObj : stationData) {
            string name = stationObj["station_name"];
            string code = stationObj["station_code"];
            int price = stationObj["price"];
            int distance = stationObj["distance"];

            Station s(name, code, price, distance);
            addStation(s);
        }

        file.close();
        cout << "📥 Stations loaded from " << filename << endl;
    }

    // Load stations (alias for reuse)
    void initializeStationsFromFile(const string& filename) {
        loadStationsFromFile(filename);
    }

    // Find station name from code (used in QR validation, etc.)
    string getStationNameByCode(const string& code) {
        for (const Station& s : returnStations()) {
            if (s.getStationCode() == code) {
                return s.getStationName();
            }
        }
        return "";
    }
};