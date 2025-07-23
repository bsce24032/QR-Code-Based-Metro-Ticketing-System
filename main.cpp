#include "passengers-staff.h"
#include "payments.h"
#include "stations-metro.h"
#include "tickets-QRgen.h"
#include "QRdecode.h"
#include <iostream>
#include <vector>
#include <memory>
#include <limits>
#include <iomanip>

using namespace std;

// Define the static member
vector<PassengerData> PassengerData::totalpassengers;

// Global objects for system management
MetroStation lahoreMetro;
SaveStationIntoFile stationSaver;
TicketBooking ticketBooking;
TicketManager ticketManager;

// QR Decoding related objects
QrDecode qrDecoder;
JsonViewerChecker jsonChecker;
Feedback feedbackSystem;
FeedbackReporter feedbackReporter(&feedbackSystem);
AccessMetro accessMetro(&feedbackReporter);

// Color codes for better UI
#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
// #define WHITE "\033[37m"
#define BOLD "\033[1m"

// Utility functions
void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void pauseScreen() {
    cout << "\n" << YELLOW << "Press Enter to continue..." << RESET;
    cin.ignore();
    cin.get();
}

void printHeader(const string& title) {
    cout << BOLD << CYAN << "\n" << string(60, '=') << RESET << endl;
    cout << BOLD << CYAN << "           " << title << RESET << endl;
    cout << BOLD << CYAN << string(60, '=') << RESET << endl;
}

void printSubHeader(const string& subtitle) {
    cout << BOLD << YELLOW << "\n--- " << subtitle << " ---" << RESET << endl;
}

// Input validation functions
int getValidInteger(const string& prompt) {
    int value;
    while (true) {
        cout << prompt;
        if (cin >> value) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        } else {
            cout << RED << "Invalid input! Please enter a valid number." << RESET << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
}

double getValidDouble(const string& prompt) {
    double value;
    while (true) {
        cout << prompt;
        if (cin >> value) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        } else {
            cout << RED << "Invalid input! Please enter a valid number." << RESET << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
}

string getValidString(const string& prompt) {
    string value;
    cout << prompt;
    getline(cin, value);
    return value;
}

// Menu display functions
void displayMainMenu() {
    clearScreen();
    printHeader("QR Code Based Metro Ticketing System");
    cout << GREEN << "\n1. " << WHITE << "Passenger Management" << RESET << endl;
    cout << GREEN << "2. " << WHITE << "Station Management" << RESET << endl;
    cout << GREEN << "3. " << WHITE << "Ticket Booking" << RESET << endl;
    cout << GREEN << "4. " << WHITE << "Payment Processing" << RESET << endl;
    cout << GREEN << "5. " << WHITE << "QR Code Generation" << RESET << endl;
    cout << GREEN << "6. " << WHITE << "QR Code Decoding & Validation" << RESET << endl;
    cout << GREEN << "7. " << WHITE << "Feedback System" << RESET << endl;
    cout << GREEN << "8. " << WHITE << "Admin Panel" << RESET << endl;
    cout << GREEN << "9. " << WHITE << "Staff Management" << RESET << endl;
    cout << GREEN << "10. " << WHITE << "System Reports" << RESET << endl;
    cout << RED << "0. " << WHITE << "Exit System" << RESET << endl;
    cout << CYAN << "\nEnter your choice: " << RESET;
}

// Passenger Management Functions
void addPassenger() {
    printSubHeader("Add New Passenger");
    
    string name = getValidString("Enter passenger name: ");
    int age = getValidInteger("Enter passenger age: ");
    string cnic = getValidString("Enter passenger CNIC: ");
    
    PassengerData passenger(name, age, cnic);
    passenger.addPassenger(passenger);
    
    cout << GREEN << "\n✓ Passenger added successfully!" << RESET << endl;
    pauseScreen();
}

void viewAllPassengers() {
    printSubHeader("All Registered Passengers");
    
    vector<PassengerData> passengers = PassengerData::getVector();
    if (passengers.empty()) {
        cout << YELLOW << "No passengers registered yet." << RESET << endl;
    } else {
        cout << left << setw(20) << "Name" << setw(10) << "Age" << setw(15) << "CNIC" << endl;
        cout << string(45, '-') << endl;
        
        for (const auto& p : passengers) {
            cout << left << setw(20) << p.getName() 
                 << setw(10) << p.getAge() 
                 << setw(15) << p.getCnic() << endl;
        }
    }
    pauseScreen();
}

void savePassengerData() {
    printSubHeader("Save Passenger Data");
    PassengerData::passengerDataJson();
    pauseScreen();
}

void passengerManagement() {
    int choice;
    do {
        clearScreen();
        printHeader("PASSENGER MANAGEMENT");
        cout << GREEN << "1. " << WHITE << "Add New Passenger" << RESET << endl;
        cout << GREEN << "2. " << WHITE << "View All Passengers" << RESET << endl;
        cout << GREEN << "3. " << WHITE << "Save Passenger Data" << RESET << endl;
        cout << RED << "0. " << WHITE << "Back to Main Menu" << RESET << endl;
        
        choice = getValidInteger("\nEnter your choice: ");
        
        switch (choice) {
            case 1: addPassenger(); break;
            case 2: viewAllPassengers(); break;
            case 3: savePassengerData(); break;
            case 0: break;
            default: 
                cout << RED << "Invalid choice! Please try again." << RESET << endl;
                pauseScreen();
        }
    } while (choice != 0);
}

// Station Management Functions
void initializeStations() {
    printSubHeader("Initialize Metro Stations");
    lahoreMetro = MetroStation(&stationSaver);
    lahoreMetro.setMetroName("Lahore Metro");
    lahoreMetro.initializeLahoreMetroStations("stations.json");
    
    // Set metro station for QR decoder
    qrDecoder.setMetroStation(&lahoreMetro);
    
    cout << GREEN << "\n✓ Lahore Metro stations initialized successfully!" << RESET << endl;
    pauseScreen();
}

void viewAllStations() {
    printSubHeader("All Metro Stations");
    
    vector<Station> stations = lahoreMetro.returnStations();
    if (stations.empty()) {
        cout << YELLOW << "No stations loaded. Please initialize stations first." << RESET << endl;
    } else {
        cout << left << setw(25) << "Station Name" << setw(10) << "Code" 
             << setw(10) << "Price" << setw(12) << "Distance" << endl;
        cout << string(57, '-') << endl;
        
        for (const auto& s : stations) {
            cout << left << setw(25) << s.getStationName()
                 << setw(10) << s.getStationCode()
                 << setw(10) << s.getPrice()
                 << setw(12) << s.getDistance() << endl;
        }
    }
    pauseScreen();
}

void addNewStation() {
    printSubHeader("Add New Station");
    
    string name = getValidString("Enter station name: ");
    string code = getValidString("Enter station code: ");
    int price = getValidInteger("Enter station price: ");
    int distance = getValidInteger("Enter station distance: ");
    
    Station newStation(name, code, price, distance);
    lahoreMetro.addStation(newStation);
    
    cout << GREEN << "\n✓ Station added successfully!" << RESET << endl;
    pauseScreen();
}

void saveStationData() {
    printSubHeader("Save Station Data");
    lahoreMetro.saveStationsToFile("lahore_stations.json");
    pauseScreen();
}

void stationManagement() {
    int choice;
    do {
        clearScreen();
        printHeader("STATION MANAGEMENT");
        cout << GREEN << "1. " << WHITE << "Initialize Default Stations" << RESET << endl;
        cout << GREEN << "2. " << WHITE << "View All Stations" << RESET << endl;
        cout << GREEN << "3. " << WHITE << "Add New Station" << RESET << endl;
        cout << GREEN << "4. " << WHITE << "Save Station Data" << RESET << endl;
        cout << RED << "0. " << WHITE << "Back to Main Menu" << RESET << endl;
        
        choice = getValidInteger("\nEnter your choice: ");
        
        switch (choice) {
            case 1: initializeStations(); break;
            case 2: viewAllStations(); break;
            case 3: addNewStation(); break;
            case 4: saveStationData(); break;
            case 0: break;
            default: 
                cout << RED << "Invalid choice! Please try again." << RESET << endl;
                pauseScreen();
        }
    } while (choice != 0);
}

// Ticket Booking Functions
Station selectStation(const string& prompt) {
    vector<Station> stations = lahoreMetro.returnStations();
    if (stations.empty()) {
        cout << RED << "No stations available. Please initialize stations first." << RESET << endl;
        return Station();
    }
    
    cout << "\n" << prompt << endl;
    cout << string(50, '-') << endl;
    
    for (size_t i = 0; i < stations.size(); i++) {
        cout << (i + 1) << ". " << stations[i].getStationName() 
             << " (" << stations[i].getStationCode() << ")" << endl;
    }
    
    int choice = getValidInteger("\nSelect station number: ");
    if (choice > 0 && choice <= static_cast<int>(stations.size())) {
        return stations[choice - 1];
    }
    
    cout << RED << "Invalid selection!" << RESET << endl;
    return Station();
}

PassengerData selectPassenger() {
    vector<PassengerData> passengers = PassengerData::getVector();
    if (passengers.empty()) {
        cout << RED << "No passengers available. Please add passengers first." << RESET << endl;
        return PassengerData();
    }
    
    cout << "\nSelect Passenger:" << endl;
    cout << string(40, '-') << endl;
    
    for (size_t i = 0; i < passengers.size(); i++) {
        cout << (i + 1) << ". " << passengers[i].getName() 
             << " (CNIC: " << passengers[i].getCnic() << ")" << endl;
    }
    
    int choice = getValidInteger("\nSelect passenger number: ");
    if (choice > 0 && choice <= static_cast<int>(passengers.size())) {
        return passengers[choice - 1];
    }
    
    cout << RED << "Invalid selection!" << RESET << endl;
    return PassengerData();
}

void bookTicket() {
    printSubHeader("Book New Ticket");
    
    PassengerData passenger = selectPassenger();
    if (passenger.getName().empty()) {
        pauseScreen();
        return;
    }
    
    Station departure = selectStation("Select Departure Station:");
    if (departure.getStationName().empty()) {
        pauseScreen();
        return;
    }
    
    Station arrival = selectStation("Select Arrival Station:");
    if (arrival.getStationName().empty()) {
        pauseScreen();
        return;
    }
    
    if (departure.getStationCode() == arrival.getStationCode()) {
        cout << RED << "Departure and arrival stations cannot be the same!" << RESET << endl;
        pauseScreen();
        return;
    }
    
    int numTickets = getValidInteger("Enter number of tickets: ");
    
    // Book ticket using both methods
    ticketBooking.bookTicket(passenger, departure, arrival, numTickets);
    
    // Create ticket info for manager
    TicketInfo ticketInfo(numTickets, 30.0, departure, arrival, passenger);
    ticketManager.bookTicket(ticketInfo);
    
    // Save ticket to file
    SaveTicketToFile saver(ticketInfo);
    saver.saveAsJSON("tickets.json");
    
    cout << GREEN << "\n✓ Ticket booked successfully!" << RESET << endl;
    pauseScreen();
}

void viewMyTickets() {
    printSubHeader("My Tickets");
    
    string cnic = getValidString("Enter your CNIC to view tickets: ");
    try {
        int cnicInt = stoi(cnic);
        ticketManager.searchTicket(cnicInt);
    } catch (const exception& e) {
        cout << RED << "Invalid CNIC format!" << RESET << endl;
    }
    
    pauseScreen();
}

void viewAllBookings() {
    printSubHeader("All Ticket Bookings");
    ticketBooking.displayAllBookings();
    ticketManager.displayAllTickets();
    pauseScreen();
}

void cancelTicket() {
    printSubHeader("Cancel Ticket");
    
    string cnic = getValidString("Enter CNIC for ticket cancellation: ");
    try {
        int cnicInt = stoi(cnic);
        ticketManager.cancelTicket(cnicInt);
    } catch (const exception& e) {
        cout << RED << "Invalid CNIC format!" << RESET << endl;
    }
    
    pauseScreen();
}

void ticketManagementSystem() {
    int choice;
    do {
        clearScreen();
        printHeader("TICKET BOOKING SYSTEM");
        cout << GREEN << "1. " << WHITE << "Book New Ticket" << RESET << endl;
        cout << GREEN << "2. " << WHITE << "View My Tickets" << RESET << endl;
        cout << GREEN << "3. " << WHITE << "View All Bookings" << RESET << endl;
        cout << GREEN << "4. " << WHITE << "Cancel Ticket" << RESET << endl;
        cout << RED << "0. " << WHITE << "Back to Main Menu" << RESET << endl;
        
        choice = getValidInteger("\nEnter your choice: ");
        
        switch (choice) {
            case 1: bookTicket(); break;
            case 2: viewMyTickets(); break;
            case 3: viewAllBookings(); break;
            case 4: cancelTicket(); break;
            case 0: break;
            default: 
                cout << RED << "Invalid choice! Please try again." << RESET << endl;
                pauseScreen();
        }
    } while (choice != 0);
}

// Payment Processing Functions
unique_ptr<PaymentMethodsBase> createPaymentMethod(int choice) {
    switch (choice) {
        case 1: {
            long long number = getValidInteger("Enter JazzCash number: ");
            double balance = getValidDouble("Enter account balance: ");
            return make_unique<JazzCash>(number, balance);
        }
        case 2: {
            long long number = getValidInteger("Enter EasyPaisa number: ");
            double balance = getValidDouble("Enter account balance: ");
            return make_unique<EasyPaisa>(number, balance);
        }
        case 3: {
            long long account = getValidInteger("Enter bank account number: ");
            double balance = getValidDouble("Enter account balance: ");
            return make_unique<Bank>(account, balance);
        }
        default:
            return nullptr;
    }
}

void processPayment() {
    printSubHeader("Process Payment");
    
    vector<PassengerData> passengers = PassengerData::getVector();
    if (passengers.empty()) {
        cout << RED << "No passengers available for payment processing." << RESET << endl;
        pauseScreen();
        return;
    }
    
    PassengerData passenger = selectPassenger();
    if (passenger.getName().empty()) {
        pauseScreen();
        return;
    }
    
    double amount = getValidDouble("Enter payment amount: ");
    
    displayPaymentMethods();
    int paymentChoice = getValidInteger("Select payment method: ");
    
    auto paymentMethod = createPaymentMethod(paymentChoice);
    if (!paymentMethod) {
        cout << RED << "Invalid payment method selected!" << RESET << endl;
        pauseScreen();
        return;
    }
    
    paymentMethod->setAmount(amount);
    paymentMethod->setPassengerData(&passenger);
    
    cout << "\nPayment Details:" << endl;
    paymentMethod->displayDetails();
    
    if (paymentMethod->makePayment()) {
        paymentMethod->generateReceipt();
        
        // Save payment to file
        vector<unique_ptr<PaymentMethodsBase>> payments;
        payments.push_back(std::move(paymentMethod));
        writeIntoJsonFromPaymentMethods(payments);
        
        cout << GREEN << "\n✓ Payment processed successfully!" << RESET << endl;
    } else {
        cout << RED << "\n✗ Payment failed!" << RESET << endl;
    }
    
    pauseScreen();
}

void viewPaymentHistory() {
    printSubHeader("Payment History");
    readFromJsonFromPaymentMethods();
    pauseScreen();
}

void calculatePaymentStats() {
    printSubHeader("Payment Statistics");
    
    vector<PaymentMethodsBase*> payments;
    readDataJsonIntoVector(payments);
    
    if (payments.empty()) {
        cout << YELLOW << "No payment data available." << RESET << endl;
    } else {
        double totalSum = 0.0;
        double average = averageAndSumTheFileAmounts(totalSum, payments);
        
        cout << "Total Payments: " << payments.size() << endl;
        cout << "Total Amount: $" << fixed << setprecision(2) << totalSum << endl;
        cout << "Average Amount: $" << fixed << setprecision(2) << average << endl;
    }
    
    // Clean up
    for (auto* payment : payments) {
        delete payment;
    }
    
    pauseScreen();
}

void paymentProcessing() {
    int choice;
    do {
        clearScreen();
        printHeader("PAYMENT PROCESSING");
        cout << GREEN << "1. " << WHITE << "Process New Payment" << RESET << endl;
        cout << GREEN << "2. " << WHITE << "View Payment History" << RESET << endl;
        cout << GREEN << "3. " << WHITE << "Payment Statistics" << RESET << endl;
        cout << RED << "0. " << WHITE << "Back to Main Menu" << RESET << endl;
        
        choice = getValidInteger("\nEnter your choice: ");
        
        switch (choice) {
            case 1: processPayment(); break;
            case 2: viewPaymentHistory(); break;
            case 3: calculatePaymentStats(); break;
            case 0: break;
            default: 
                cout << RED << "Invalid choice! Please try again." << RESET << endl;
                pauseScreen();
        }
    } while (choice != 0);
}

// QR Code Generation Functions
void generateQRCode() {
    printSubHeader("Generate QR Code");
    
    try {
        vector<TicketInfo> tickets = SaveTicketToFile::loadAllFromJSON("tickets.json");
        if (tickets.empty()) {
            cout << YELLOW << "No tickets found. Please book a ticket first." << RESET << endl;
            pauseScreen();
            return;
        }
        
        // Display available tickets
        cout << "Available Tickets:" << endl;
        for (size_t i = 0; i < tickets.size(); i++) {
            cout << (i + 1) << ". " << tickets[i].getPassenger().getName() 
                 << " - " << tickets[i].getDepartureStation().getStationName()
                 << " to " << tickets[i].getArrivalStation().getStationName() << endl;
        }
        
        int choice = getValidInteger("Select ticket for QR generation: ");
        if (choice < 1 || choice > static_cast<int>(tickets.size())) {
            cout << RED << "Invalid selection!" << RESET << endl;
            pauseScreen();
            return;
        }
        
        TicketInfo& selectedTicket = tickets[choice - 1];
        PassengerData passenger = selectedTicket.getPassenger();
        
        // Generate QR code
        QRCodeData qrData(passenger, selectedTicket);
        QRGeneration qrGen(qrData);
        
        cout << "\nQR Code Data: " << qrData.getQrCodeData() << endl;
        cout << "\nGenerating QR Code..." << endl;
        
        qrGen.generateQR();
        
        char saveChoice;
        cout << "\nSave QR code as PNG image? (y/n): ";
        cin >> saveChoice;
        
        if (saveChoice == 'y' || saveChoice == 'Y') {
            qrGen.saveQRCodePNG("qrcode.png");
        }
        
        cout << GREEN << "\n✓ QR Code generated successfully!" << RESET << endl;
        
    } catch (const exception& e) {
        cout << RED << "Error generating QR code: " << e.what() << RESET << endl;
    }
    
    pauseScreen();
}

void validateQRCode() {
    printSubHeader("Validate QR Code");
    
    string qrData = getValidString("Enter QR code data to validate: ");
    
    QRChecker* checker = QRChecker::getInstance();
    if (checker->isValidJSON(qrData)) {
        cout << GREEN << "✓ QR Code is valid!" << RESET << endl;
    } else {
        cout << RED << "✗ QR Code is invalid!" << RESET << endl;
    }
    
    pauseScreen();
}

void qrCodeGeneration() {
    int choice;
    do {
        clearScreen();
        printHeader("QR CODE GENERATION");
        cout << GREEN << "1. " << WHITE << "Generate QR Code" << RESET << endl;
        cout << GREEN << "2. " << WHITE << "Validate QR Code" << RESET << endl;
        cout << RED << "0. " << WHITE << "Back to Main Menu" << RESET << endl;
        
        choice = getValidInteger("\nEnter your choice: ");
        
        switch (choice) {
            case 1: generateQRCode(); break;
            case 2: validateQRCode(); break;
            case 0: break;
            default: 
                cout << RED << "Invalid choice! Please try again." << RESET << endl;
                pauseScreen();
        }
    } while (choice != 0);
}

// QR Code Decoding Functions
void scanAndDecodeQR() {
    printSubHeader("Scan and Decode QR Code");
    
    cout << YELLOW << "Starting camera for QR code scanning..." << RESET << endl;
    cout << CYAN << "Make sure you have a QR code ready to scan." << RESET << endl;
    
    // Call the Python script to scan QR code
    qrDecoder.callScript();
    
    pauseScreen();
}

void readDecodedQR() {
    printSubHeader("Read Decoded QR Data");
    
    qrDecoder.readJsonOutput();
    
    pauseScreen();
}

void validateDecodedTicket() {
    printSubHeader("Validate Decoded Ticket");
    
    if (lahoreMetro.returnStations().empty()) {
        cout << RED << "No stations loaded. Please initialize stations first." << RESET << endl;
        pauseScreen();
        return;
    }
    
    // First read the decoded data
    qrDecoder.readJsonOutput();
    
    // Then validate against stations
    qrDecoder.validateTicketWithStations();
    
    pauseScreen();
}

void advancedQRValidation() {
    printSubHeader("Advanced QR Validation");
    
    // Load passengers from file
    jsonChecker.loadPassengersFromFile("passenger_data.json");
    
    // Read decoded JSON
    jsonChecker.readDecodedJson();
    
    // Validate passenger name
    jsonChecker.validate();
    
    cout << "\nValidated passenger name: " << jsonChecker.getValidatedName() << endl;
    
    pauseScreen();
}

void manualPassengerValidation() {
    printSubHeader("Manual Passenger Validation");
    
    jsonChecker.loadPassengersFromFile("passenger_data.json");
    
    string name = getValidString("Enter passenger name: ");
    int age = getValidInteger("Enter passenger age: ");
    
    if (jsonChecker.validatePassenger(name, age)) {
        cout << GREEN << "✓ Passenger validated successfully!" << RESET << endl;
        cout << "Validated name: " << jsonChecker.getValidatedName() << endl;
    } else {
        cout << RED << "✗ Passenger validation failed!" << RESET << endl;
    }
    
    pauseScreen();
}

void qrCodeDecoding() {
    int choice;
    do {
        clearScreen();
        printHeader("QR CODE DECODING & VALIDATION");
        cout << GREEN << "1. " << WHITE << "Scan and Decode QR Code" << RESET << endl;
        cout << GREEN << "2. " << WHITE << "Read Decoded QR Data" << RESET << endl;
        cout << GREEN << "3. " << WHITE << "Validate Decoded Ticket" << RESET << endl;
        cout << GREEN << "4. " << WHITE << "Advanced QR Validation" << RESET << endl;
        cout << GREEN << "5. " << WHITE << "Manual Passenger Validation" << RESET << endl;
        cout << RED << "0. " << WHITE << "Back to Main Menu" << RESET << endl;
        
        choice = getValidInteger("\nEnter your choice: ");
        
        switch (choice) {
            case 1: scanAndDecodeQR(); break;
            case 2: readDecodedQR(); break;
            case 3: validateDecodedTicket(); break;
            case 4: advancedQRValidation(); break;
            case 5: manualPassengerValidation(); break;
            case 0: break;
            default: 
                cout << RED << "Invalid choice! Please try again." << RESET << endl;
                pauseScreen();
        }
    } while (choice != 0);
}

// Feedback System Functions
void collectFeedback() {
    printSubHeader("Collect Passenger Feedback");
    
    accessMetro.welcome();
    accessMetro.collectAndSaveFeedback();
    
    cout << GREEN << "\n✓ Feedback collected successfully!" << RESET << endl;
    pauseScreen();
}

void viewStationFeedback() {
    printSubHeader("View Station Feedback");
    
    string stationName = getValidString("Enter station name: ");
    accessMetro.reportStationFeedback(stationName);
    
    pauseScreen();
}

void viewAllFeedbacks() {
    printSubHeader("All Station Feedbacks");
    
    accessMetro.printAllStationFeedbacks();
    
    pauseScreen();
}

void saveFeedbackToFile() {
    printSubHeader("Save Feedback to File");
    
    accessMetro.saveFeedbacksToJson("feedback.json");
    
    pauseScreen();
}

void myfeedbackSystem() {
    int choice;
    do {
        clearScreen();
        printHeader("FEEDBACK SYSTEM");
        cout << GREEN << "1. " << WHITE << "Collect Passenger Feedback" << RESET << endl;
        cout << GREEN << "2. " << WHITE << "View Station Feedback" << RESET << endl;
        cout << GREEN << "3. " << WHITE << "View All Feedbacks" << RESET << endl;
        cout << GREEN << "4. " << WHITE << "Save Feedback to File" << RESET << endl;
        cout << RED << "0. " << WHITE << "Back to Main Menu" << RESET << endl;
        
        choice = getValidInteger("\nEnter your choice: ");
        
        switch (choice) {
            case 1: collectFeedback(); break;
            case 2: viewStationFeedback(); break;
            case 3: viewAllFeedbacks(); break;
            case 4: saveFeedbackToFile(); break;
            case 0: break;
            default: 
                cout << RED << "Invalid choice! Please try again." << RESET << endl;
                pauseScreen();
        }
    } while (choice != 0);
}

// Admin Panel Functions
void adminPanel() {
    printSubHeader("Admin Panel");
    
    string name = getValidString("Enter admin name: ");
    int age = getValidInteger("Enter admin age: ");
    string cnic = getValidString("Enter admin CNIC: ");
    int accountNumber = getValidInteger("Enter admin account number: ");
    string adminName = getValidString("Enter admin username: ");
    
    Admin admin(name, age, cnic, accountNumber, adminName);
    
    cout << "\nAdmin Account Details:" << endl;
    admin.adminAccountOutput();
    admin.adminBalance();
    
    cout << "\n" << CYAN << "Fetching passenger payment accounts..." << RESET << endl;
    admin.passengerPaymentAccount();
    
    pauseScreen();
}

// Staff Management Functions
void addStaff() {
    printSubHeader("Add Staff Member");
    
    string name = getValidString("Enter staff name: ");
    int age = getValidInteger("Enter staff age: ");
    string department = getValidString("Enter department: ");
    int salary = getValidInteger("Enter salary: ");
    
    Staff staff(name, age, department, salary);
    Staff staffManager; // To manage staff
    staffManager.addStaff(staff);
    
    cout << GREEN << "\n✓ Staff member added successfully!" << RESET << endl;
    pauseScreen;
}

void staffManagement() {
    int choice;
    do {
        clearScreen();
        printHeader("STAFF MANAGEMENT");
        cout << GREEN << "1. " << WHITE << "Add Staff Member" << RESET << endl;
        cout << RED << "0. " << WHITE << "Back to Main Menu" << RESET << endl;
        
        choice = getValidInteger("\nEnter your choice: ");
        
        switch (choice) {
            case 1: addStaff(); break;
            case 0: break;
            default: 
                cout << RED << "Invalid choice! Please try again." << RESET << endl;
                pauseScreen();
        }
    } while (choice != 0);
}

// System Reports Functions
void systemReports() {
    clearScreen();
    printHeader("SYSTEM REPORTS");
    
    cout << BOLD << "System Statistics:" << RESET << endl;
    cout << "Total Tickets Created: " << TicketInfo::getTicketCount() << endl;
    cout << "Total Passengers: " << PassengerData::getVector().size() << endl;
    cout << "Total Stations: " << lahoreMetro.returnStations().size() << endl;
    
    cout << "\nNext Ticket ID: " << TicketIDGenerator::getInstance().nextID() << endl;
    
    pauseScreen();
}

// Main function
int main() {
    
    cout << BOLD << GREEN << "Welcome to Lahore Metro Transit System!" << RESET << endl;
    cout << YELLOW << "Initializing system..." << RESET << endl;
    
    // Initialize global objects
    lahoreMetro = MetroStation(&stationSaver);
    lahoreMetro.setMetroName("Lahore Metro");
    qrDecoder.setMetroStation(&lahoreMetro);
    
    int choice;
    do {
        displayMainMenu();
        choice = getValidInteger("");
        
        switch (choice) {
            case 1: passengerManagement(); break;
            case 2: stationManagement(); break;
            case 3: ticketManagementSystem(); break;
            case 4: paymentProcessing(); break;
            case 5: qrCodeGeneration(); break;
            case 6: qrCodeDecoding(); break;
            case 7: myfeedbackSystem(); break;
            case 8: adminPanel(); break;
            case 9: staffManagement(); break;
            case 10: systemReports(); break;
            case 0: 
                cout << BOLD << GREEN << "\nThank you for using Lahore Metro Transit System!" << RESET << endl;
                cout << YELLOW << "Goodbye!" << RESET << endl;
                break;
            default: 
                cout << RED << "Invalid choice! Please try again." << RESET << endl;
                pauseScreen();
        }
    } while (choice != 0);
    
    return 0;
}

// g++ -std=c++17 main.cpp payments.cpp tickets-QRgen.cpp qrcodegen.cpp -o metro $(pkg-config --cflags --libs opencv4)