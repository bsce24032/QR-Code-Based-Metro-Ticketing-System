#ifndef BSCE24035_H
#define BSCE24035_H
// #pragma once 
#include "passengers-staff.h"
#include "stations-metro.h"

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include "json.hpp"

#include "qrcodegen.hpp"
#include <opencv2/opencv.hpp>  // Ignore this error if you istall the opencv in wsl/linux using command:  sudo apt-get update and sudo apt-get install libopencv-dev


using namespace qrcodegen;

using namespace std;
using json = nlohmann::json;


// ******************** Ticket Info Class    ***************************
// holds one booking’s info
class TicketInfo {
private:
    int numberOfTickets;
    double pricePerTicket;
    Station departure;
    Station arrival;
    PassengerData passenger;
    static int ticketCount;
public:
    TicketInfo();
    TicketInfo(int num, double price, const Station& dep, const Station& arr, const PassengerData& p);
    virtual ~TicketInfo() {}

    // compute total
    virtual double totalPrice() const;
    // display details
    virtual void displayTicketInfo();

    int getNoOfTickects();
    double getPricePerTickect();
    PassengerData getPassenger();
    Station getArrivalStation();
    Station getDepartureStation();

    static int getTicketCount();

    void setNoOfTickects(int no);
    void setPricePerTickect(double pri);
    void setPassenger(PassengerData p);
    void setArrivalStation(Station a);
    void setDepartureStation(Station d);

    friend ostream& operator<<(ostream& out, TicketInfo& obj); 
    friend istream& operator>>(istream& in, TicketInfo& obj); 

    // equality for testing
    bool operator==( TicketInfo& o);
};


//********************** Compile-time polymorphism: function overloading **********************
// Base/ Abstract class for runtime polymorphism
class TicketProcessor {
public:
    virtual void process() = 0;
    virtual ~TicketProcessor() {}
};


// ******************** Ticket Booking Class ***************************
// Inherits runtime polymorphism
class TicketBooking : public TicketProcessor {
    vector<TicketInfo> bookings;  // aggregation
public:
    TicketBooking();

    // compile-time polymorphism: two overloads
    void bookTicket(TicketInfo& info);    // overload
    void bookTicket(const PassengerData& p, const Station& dep, const Station& arr, int numTickets);  // overload

    void displayAllBookings() ;
    void process() override;               // runtime polymorphism

    // expose bookings for external use
    const vector<TicketInfo>& getBookings() const;
};

// ******************** Ticket Manager Class ***************************
class TicketManager {
private:
    map<int, TicketInfo> ticketMap; // key: CNIC, value: TicketInfo

public:
    // Add a ticket to the map
    void bookTicket(TicketInfo& t);
    // Display all tickets
    void displayAllTickets();
    // Search for a ticket
    void searchTicket(int id);
    // Cancel a ticket
    void cancelTicket(int id);
};

//********************** SaveTicketToFile **********************
// Composition with TicketInfo
class SaveTicketToFile {
    TicketInfo& info;
public:
    SaveTicketToFile(TicketInfo& info);
    void saveAsJSON(const string& filename) const;  
    // now returns all tickets saved in file
    static vector<TicketInfo> loadAllFromJSON(const string& filename);
};

//********************** Singleton: TicketIDGenerator **********************
// Thread-safe singleton for generating unique ticket IDs
class TicketIDGenerator {
private:
    // private constructor prevents external instantiation
    TicketIDGenerator() : currentID(100) {}  
    int currentID;

public:
    // static method to access the singleton instance
    static TicketIDGenerator& getInstance() {
        static TicketIDGenerator instance;
        return instance;
    }

    // delete copy/move
    TicketIDGenerator(const TicketIDGenerator&) = delete;
    TicketIDGenerator& operator=(const TicketIDGenerator&) = delete;

    // return next unique ID
    int nextID();
};


//********************** Singleton: QRChecker **********************
class QRChecker {
    static QRChecker* instance;
    QRChecker() {}
public:
    static QRChecker* getInstance();
    bool isValidJSON(const string& json);
};


// ******************** QR Code Data Class  ***************************
class QRCodeData{
private:
    string qrData;
public:
    QRCodeData();
    QRCodeData(PassengerData& p, TicketInfo& t);
    void setQrCodeData(PassengerData& p, TicketInfo& t);
    string getQrCodeData();
};


// ******************** QR Gegeration Class  ***************************
// Composition with TicketInfo
class QRGeneration {
    QRCodeData qrCode;
public:
    QRGeneration(QRCodeData& q);
    void printQr(const QrCode &qr);
    void generateQR();
    void saveQRCodePNG(const string& filename);
};

//──────────── Template class: Repository ────────────
template<typename T>
class Repository {
    vector<T> items;
public:
    void add(const T& item);
    vector<T> getAll() const;
};

// Explicit instantiation for TicketInfo
extern template class Repository<TicketInfo>;

#endif