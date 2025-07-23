#include "tickets-QRgen.h"

// ******************** Ticket Info Class    ***************************
int TicketInfo::ticketCount = 0;

TicketInfo::TicketInfo(): numberOfTickets(0),pricePerTicket(0),departure(),arrival(),passenger() {
    ticketCount++;
}

TicketInfo::TicketInfo(int n,double p,const Station& d,const Station& a,const PassengerData& pd): numberOfTickets(n),pricePerTicket(p),departure(d),arrival(a),passenger(pd){
    ticketCount++;
}

double TicketInfo::totalPrice() const { 
    return numberOfTickets*pricePerTicket; 
}

void TicketInfo::displayTicketInfo() {
    cout<<"Passenger: "<<passenger.getName()<<endl
        <<"From "<<departure.getStationName()
        <<" To "<<arrival.getStationName()<<endl
        <<"No. of Tickets = "<<numberOfTickets<<endl
        <<"Price Per Ticket = "<<pricePerTicket<<endl
        <<"Total Price = "<<totalPrice()<<endl<<endl;
}

// Getter
int TicketInfo::getNoOfTickects(){
    return numberOfTickets;
}
double TicketInfo::getPricePerTickect(){
    return pricePerTicket;
}
PassengerData TicketInfo::getPassenger(){
    return passenger;
}
Station TicketInfo::getArrivalStation(){
    return arrival;
}
Station TicketInfo::getDepartureStation(){
    return departure;
}

int TicketInfo::getTicketCount(){
    return ticketCount;
}

// Setter
void TicketInfo::setNoOfTickects(int no){
    numberOfTickets=no;
}
void TicketInfo::setPricePerTickect(double pri){
    pricePerTicket=pri;
}
void TicketInfo::setPassenger(PassengerData p){
    passenger=p;
}
void TicketInfo::setArrivalStation(Station a){
    arrival=a;
}
void TicketInfo::setDepartureStation(Station d){
    departure=d;
}

ostream& operator<<(ostream& out,TicketInfo& obj){
    out<<"Passenger: "<<obj.passenger.getName()<<endl
    <<"From "<<obj.departure.getStationName()
    <<" To "<<obj.arrival.getStationName()<<endl
    <<"No. of Tickets = "<<obj.numberOfTickets<<endl
    <<"Price Per Ticket = "<<obj.pricePerTicket<<endl
    <<"Total Price = "<<obj.totalPrice()<<endl<<endl;
    return out;
} 
istream& operator>>(istream& in, TicketInfo& obj){
    cout<<"Enter no. of tickets";
    in >> obj.numberOfTickets;
    cout<<"Enter price of tickets";
    in >> obj.pricePerTicket;
    return in;
}

bool TicketInfo::operator==( TicketInfo& o)  {
    return numberOfTickets==o.numberOfTickets && pricePerTicket==o.pricePerTicket &&
           departure.getStationCode()==o.departure.getStationCode() &&
           arrival.getStationCode()==o.arrival.getStationCode() &&
           passenger.getName()==o.passenger.getName();
}

// ******************** Ticket Booking Class ***************************
TicketBooking::TicketBooking():bookings() {}
void TicketBooking::bookTicket( TicketInfo& info){
    bookings.push_back(info);
    cout<<"[Booked via TicketInfo] "<<endl; 
    info.displayTicketInfo();
}
void TicketBooking::bookTicket(const PassengerData& p,const Station& dep,const Station& arr,int num){
    double price=30.0;
    TicketInfo info(num,price,dep,arr,p);
    bookings.push_back(info);
    cout<<"[Booked via params] "<<endl; 
    info.displayTicketInfo();
}
void TicketBooking::displayAllBookings(){
    cout<<"--- Display All Bookings ---"<<endl;
    for(auto& t:bookings){
        t.displayTicketInfo();
        cout<<endl;
    } 
}
void TicketBooking::process(){
    cout<<"Processing "<<bookings.size()<<" booking(s)..."<<endl;
}
const vector<TicketInfo>& TicketBooking::getBookings() const {
    return bookings; 
}

// ******************** Ticket Manager Class ***************************
// This class manages ticket bookings using a map for CNIC-based access.
// Add a ticket to the map
void TicketManager::bookTicket(TicketInfo& t) {
    int store = stoi(t.getPassenger().getCnic()); // Use CNIC as key
    if (ticketMap.find(store) != ticketMap.end()) {
        cout << "Ticket already booked for this CNIC.\n";
        return;
    }
    ticketMap[store] = t;
    cout << "Ticket booked successfully.\n";
}

// Display all tickets
void TicketManager::displayAllTickets() {
    if (ticketMap.empty()) {
        cout << "No tickets booked.\n";
        return;
    }
    for (auto& pair : ticketMap) {
        cout << "\n--- Ticket Info ---\n";
        pair.second.displayTicketInfo();
    }
}

// Search for a ticket
void TicketManager::searchTicket(int id) {
    auto it = ticketMap.find(id);
    if (it != ticketMap.end()) {
        cout << "Ticket Found:\n";
        it->second.displayTicketInfo();
    } else {
        cout << "Ticket ID not found.\n";
    }
}

// Cancel a ticket
void TicketManager::cancelTicket(int id) {
    if (ticketMap.erase(id)) {
        cout << "Ticket canceled.\n";
    } else {
        cout << "Ticket ID not found.\n";
    }
}


//****************** SaveTicketToFile Class ******************
SaveTicketToFile::SaveTicketToFile(TicketInfo& i):info(i){}
//Append‑safe save
void SaveTicketToFile::saveAsJSON(const string& filename) const {
    // 1) Read existing file into JSON array (or start new)
    json arr = json::array();
    ifstream myfile;
    myfile.open(filename);
    if (myfile.is_open()) {
        try {
            json existing = json::parse(myfile);       // read all 
            if (existing.is_array()) {
                arr = existing;
            }
        } catch (...) {
            // invalid or empty → start fresh array
        }
    }
    // if((info.totalPrice()) < 60){
        // 2) Append new ticket object                
        arr.push_back({
            {"Passenger Name: ", info.getPassenger().getName()},
            {"CNIC: ", info.getPassenger().getCnic()},
            {"Deparure Station: ",      info.getDepartureStation().getStationName()},
            {"Arrival Station: ",        info.getArrivalStation().getStationName()},
            {"No. of Tickets: ",       info.getNoOfTickects()},
            {"Total Price: ",     info.totalPrice()},
            {"Price Per Ticket: ",  info.getPricePerTickect()}
        });
    // }
    // else{
    //     cout<<"The total price is greater than 60 so this ticket is not save"<<endl;
    // }
    // 3) Write back full array                   
    ofstream out(filename);
    out << arr.dump(4);
}


//──────── Load all tickets ──────────
vector<TicketInfo> SaveTicketToFile::loadAllFromJSON(const string& filename) {
    ifstream in(filename);
    if (!in.is_open()){
        throw runtime_error("Cannot open "+filename);
    } 
    json arr = json::parse(in);                   // parse file  
    if (!arr.is_array()){
        throw runtime_error("Malformed JSON");
    } 
    vector<TicketInfo> result;
    for (auto& j : arr) {                         // iterate array  
        PassengerData p(j.at("Passenger Name: ").get<string>(), 0, j.at("CNIC: ").get<string>());
        Station d;
        d.setStationName(j.at("Deparure Station: ").get<string>());
        Station a;
        a.setStationName(j.at("Arrival Station: ").get<string>());
        TicketInfo t(j.at("No. of Tickets: ").get<int>(), j.at("Price Per Ticket: ").get<double>(), d, a, p);
        result.push_back(t);
    }
    return result;
}

//****************** TicketIDGenerator (Singleton) ******************
/// Returns and increments the next unique ticket ID.
/// This is a singleton class, so only one instance exists.
int TicketIDGenerator::nextID() {
    return currentID++;
}

//****************** QRChecker (Singleton) ******************
QRChecker* QRChecker::instance=nullptr;
QRChecker* QRChecker::getInstance(){
    if(!instance) instance=new QRChecker();
    return instance;
}
bool QRChecker::isValidJSON(const string& j){
    return !j.empty() && j.front()=='{' && j.back()=='}';
}


// ******************** QR Code Data Class  ***************************
QRCodeData::QRCodeData(){
    qrData="";
}
QRCodeData::QRCodeData(PassengerData& p, TicketInfo& t) {
    qrData = "{\"type\":\"QRCODE\",\"data\":{"
             "\"Name\":\"" + p.getName() + "\","
             "\"CNIC\":\"" + p.getCnic() + "\","
             "\"Departure\":\"" + t.getDepartureStation().getStationName() + "\","
             "\"Arrival\":\"" + t.getArrivalStation().getStationName() + "\""
             "}}";
}

void QRCodeData::setQrCodeData(PassengerData& p, TicketInfo& t) {
    qrData = "{\"type\":\"QRCODE\",\"data\":{"
             "\"Name\":\"" + p.getName() + "\","
             "\"CNIC\":\"" + p.getCnic() + "\","
             "\"Departure\":\"" + t.getDepartureStation().getStationName() + "\","
             "\"Arrival\":\"" + t.getArrivalStation().getStationName() + "\""
             "}}";
}
string QRCodeData::getQrCodeData(){
    return qrData;
}


// ******************** QR Gegeration Class  ***************************
QRGeneration::QRGeneration(QRCodeData& q){
    qrCode=q;
}
void QRGeneration::printQr(const QrCode &qr) {
    int size = qr.getSize();
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            cout << (qr.getModule(x, y) ? "██" : "  ");
        }
        cout << endl; 
    }
}

void QRGeneration::generateQR(){
    // 1. Define the text you want to encode into the QR code.
    // const char *text = qrCode.getQrCodeData().c_str();
    try {
        // 2. Generate the QR code using the qrcodegen library.
        QrCode qr = QrCode::encodeText(qrCode.getQrCodeData().c_str(), QrCode::Ecc::LOW);
        printQr(qr);
    } catch (const exception& e) {
        cerr << "QR code generation failed: " << e.what() << endl;
    }
    
}

void QRGeneration::saveQRCodePNG(const string& filename){
    // 1. Define the text you want to encode into the QR code.
    // const char *text = qrCode.getQrCodeData().c_str();

    // 2. Generate the QR code using the qrcodegen library.
    QrCode qr = QrCode::encodeText(qrCode.getQrCodeData().c_str(), QrCode::Ecc::HIGH);
    int qrSize = qr.getSize();

    // 3. Define scaling and border settings for the output image.
    int scale = 10;    // Number of pixels per QR module.
    int border = 4;    // Border size in terms of modules.
    int imgSize = (qrSize + 2 * border) * scale;

    // 4. Create a grayscale OpenCV image with a white background.
    cv::Mat qrImage(imgSize, imgSize, CV_8UC1, cv::Scalar(255));

    // 5. Draw the QR code on the image.
    // Iterate over each module of the QR code.
    for (int y = 0; y < qrSize; y++) {
        for (int x = 0; x < qrSize; x++) {
            if (qr.getModule(x, y)) {
                // Calculate top-left pixel coordinates for the current module.
                int x0 = (x + border) * scale;
                int y0 = (y + border) * scale;
                // Draw a filled rectangle for each dark module.
                cv::rectangle(qrImage, cv::Rect(x0, y0, scale, scale), cv::Scalar(0), cv::FILLED);
            }
        }
    }

    // 6. Save the generated QR code image as a PNG file.
    if(cv::imwrite(filename, qrImage))
        cout << "QR Code successfully saved as 'qrcode.png'" << endl;
    else
        cerr << "Error: Failed to save QR Code image." << endl;
}


//──────── Repository<T> ────────
template<typename T>
void Repository<T>::add(const T& item){ 
    items.push_back(item); 
}
template<typename T>
vector<T> Repository<T>::getAll() const{
    return items; 
}
// explicit instantiation:
template class Repository<TicketInfo>;