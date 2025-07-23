
#include "payments.h"
#include "passengers-staff.h"

// global varaible decalartion
string filename = "payments.json";
//..........................................................................................

// Payment methods class

PaymentMethods::PaymentMethods(const string &method, double amt)
    : paymentMethodName(method), amount(amt), receipt(this) {}

void PaymentMethods::setAmount(double amt) { amount = amt; }

double PaymentMethods::getAmount() const { return amount; }

void PaymentMethods ::setReference(long long ref) { reference = ref; }

long long int PaymentMethods ::getRefrence() const { return reference; }

void PaymentMethods::generateReceipt() {
  receipt.setStatus("Verified");
  receipt.setTime(getCurrentDateTime());
  receipt.setTransactionId(generateTransactionID());
  receipt.displayReceipt();
}

// from kashif module
PassengerData *PaymentMethods::getPassengerData() const { return passenger; }

void PaymentMethods::setPassengerData(PassengerData *p) { passenger = p; }

//.........................................................................................
// JazzCash class

void JazzCash::displayDetails() {
  cout << "JazzCash Number: " << getRefrence() << endl;
  cout << "Balance: " << balance << endl;
}

bool JazzCash::makePayment() {
  if (balance > getAmount()) {
    cout << "\t\t\tPayment is Verified" << endl;
    balance = balance - getAmount();
   // cout<<balance<<endl;
    return true;
  } else {
    cout << "\t\t\tNot Sufficient Amount" << endl;
    return false;
  }
}

JazzCash::JazzCash() : PaymentMethods("JazzCash", 0.0) {
  setReference(0);
  balance = 0;
}

JazzCash::JazzCash(long long int number, double bal)
    : PaymentMethods("JazzCash", 0.0) {
  setReference(number);
  balance = bal;
}

JazzCash::JazzCash(double amo) : PaymentMethods("JazzCash", 0.0) {
  this->amount = amo;
}

void JazzCash::setBalance(double balance) { this->balance = balance; }

double JazzCash::getBalance() { return balance; }

string JazzCash ::getPaymentType() const { return "JazzCash"; }

//.........................................................................................
// EasyPaisa class

void EasyPaisa::displayDetails() {
  cout << "EasyPaisa Number: " << getRefrence() << endl;
  cout << "Balance: " << balance << endl;
}

bool EasyPaisa::makePayment() {
  if (balance > getAmount()) {
    cout << "\t\t\tPayment is Verified" << endl;
    balance = balance - getAmount();
    //cout<<balance<<endl;
    return true;
  } else {
    cout << "\t\t\tNot Sufficient Amount" << endl;
    return false;
  }
}

EasyPaisa::EasyPaisa() : PaymentMethods("EasyPaisa", 0.0) {
  setReference(0);
  balance = 0;
}

EasyPaisa::EasyPaisa(long long int number, double bal)
    : PaymentMethods("EasyPaisa", 0.0) {
  setReference(number);
  balance = bal;
}

EasyPaisa::EasyPaisa(double amo) : PaymentMethods("EasyPaisa", 0.0) {
  amount = amo;
}

void EasyPaisa::setBalance(double balance) { this->balance = balance; }

double EasyPaisa::getBalance() { return balance; }

string EasyPaisa ::getPaymentType() const { return "EasyPaisa"; }

//.........................................................................................
// Bank class

void Bank::displayDetails() {
  cout << "Bank Account Number: " << getRefrence() << endl;
  cout << "Balance: " << balance << endl;
}

bool Bank::makePayment() {
  if (balance > getAmount()) {
    cout << "\t\t\tPayment is Verified" << endl;
    balance = balance - getAmount();
   // cout<<balance<<endl;
    return true;
  } else {
    cout << "\t\t\tNot Sufficient Amount" << endl;
    return false;
  }
}

Bank::Bank() : PaymentMethods("Bank", 0.0) {
  setReference(0.0);
  balance = 0;
}

Bank::Bank(long long int account_number, double bal)
    : PaymentMethods("Bank", 0.0) {
  setReference(account_number);
  balance = bal;
}

Bank::Bank(double amo) : PaymentMethods("Bank", 0.0) { amount = amo; }

void Bank::setBalance(double balance) { this->balance = balance; }

double Bank::getBalance() { return balance; }

string Bank ::getPaymentType() const { return "Bank Transfer"; }

//.........................................................................................

// class for payment receipt

PaymentRecipt::PaymentRecipt(PaymentMethodsBase *pm) { payment = pm; };

void PaymentRecipt::setTransactionId(int id) { transaction_id = id; }

void PaymentRecipt ::setTime(string time) { time_stamp = time; }

void PaymentRecipt::setStatus(string st) { status = st; }

int PaymentRecipt ::getTransactionId() { return transaction_id; }

string PaymentRecipt ::getTime() { return time_stamp; }

string PaymentRecipt ::getStatus() { return status; }

void PaymentRecipt::displayReceipt() {
  cout << CYAN << "\t\t\tGenerating online Receipt\n" << RESET << endl;

  if (!payment || !payment->getPassengerData()) {
      cerr << YELLOW << "\t\t\tError: Passenger data is missing.\n" << RESET;
      return;
  }

  cout << GREEN << "\t\t\tName: " << WHITE
       << payment->getPassengerData()->getName() << endl;
  cout << GREEN << "\t\t\tAge: " << WHITE
       << payment->getPassengerData()->getAge() << endl;
  cout << GREEN << "\t\t\tTransaction ID: " << WHITE << transaction_id << endl;
  cout << GREEN << "\t\t\tTime: " << WHITE << getTime() << endl;
  cout << GREEN << "\t\t\tAmount: " << WHITE << payment->getAmount() << endl;
  cout << GREEN << "\t\t\tStatus: " << WHITE << status << RESET << endl;
}

//................................................................................................
// Functions for File Handling

void writeIntoJsonFromPaymentMethods(const vector<unique_ptr<PaymentMethodsBase>> &customers) {
  //first reading the file and getting that current data in it so we can overwrite it
  fstream file;
  json backup_data;
  file.open(filename, ios::in);
  if (file.is_open()) {
    // error handling
    try {
      {
        file >> backup_data;
        if (!backup_data.is_array()) {
          backup_data = json::array();
        }
      }
    } catch (const std::exception &e) {
      backup_data = json::array();
    }
    file.close();
  } else {
    backup_data = json::array();
  }

  for (const auto &value : customers) {
    json j;
    j["Name"] = value->getPassengerData()->getName();
    j["Age"] = value->getPassengerData()->getAge();
    j["Payment Type"] = value->getPaymentType();
    j["Number"] = value->getRefrence();
    j["Amount"] = value->getAmount();
    j["Status"] = "Paid";
    backup_data.push_back(j);
  }

  file.open(filename, ios::out);
  if (file.is_open()) {
    file << backup_data.dump(4);
    file.close();
    cout << "\t\t\tData written into file" << endl;
  } else {
    cout << "\t\t\tFile failed to open" << endl;
  }
}

//...............................................................................................

void readFromJsonFromPaymentMethods() {
  cout << "\t\t\tData of payment methods file is being displayed..." << endl;
  cout << endl;

  fstream file;
  file.open(filename, ios::in);
  json read_data;
  if (file.is_open()) {
    file >> read_data;
  } else {
    cout << "\t\t\tFile not open" << endl;
  }
  file.close();

  for (auto &value : read_data) {
    cout << "\t\t\tName: " << value["Name"] << endl;
    cout << "\t\t\tAge: " << value["Age"] << endl;
    cout << "\t\t\tPayment Method: " << value["Payment Type"] << endl;
    cout << "\t\t\tNumber: " << value["Number"] << endl;
    cout << "\t\t\tAmount Paid: " << value["Amount"] << endl;
    cout << "\t\t\tStatus: " << "Paid" << endl;
    cout << endl;
  }
}

//.............................................................................................................

void displayPaymentMethods() {
  cout << "\t\t\tAvailable Payment Methods:\n";
  cout << "\t\t\t1. JazzCash\n";
  cout << "\t\t\t2. Bank\n";
  cout << "\t\t\t3. EasyPaisa\n";
}

//..............................................................................................................

double operator+(double total, const PaymentMethodsBase &payment) {
  total = 0.0;
  total = payment.getAmount() + total;
  return total;
}

//................................................................................................................

void readDataJsonIntoVector(vector<PaymentMethodsBase *> &amnount_list) {
  json amount_data;
  fstream file;
  file.open(filename, ios::in);

  if (!file.is_open()) {
    cerr << "\t\t\tError opening file: " << filename << endl;
    return;
  }

  try {
    file >> amount_data;
  } catch (const json::exception &e) {
    cerr << "\t\t\tJSON parsing error: " << e.what() << endl;
    file.close();
    return;
  }

  for (const auto &properties : amount_data) {
    if (!properties.contains("Payment Type") ||
        !properties.contains("Amount")) {
      cerr << "\t\t\tMissing required fields in JSON" << endl;
      continue;
    }

    string type = properties["Payment Type"];
    double amount = properties["Amount"];

    if (type == "JazzCash") {
      amnount_list.push_back(new JazzCash(amount));
    } else if (type == "EasyPaisa") {
      amnount_list.push_back(new EasyPaisa(amount));
    } else if (type == "Bank Transfer") {
      amnount_list.push_back(new Bank(amount));
    }
  }
  file.close();
}

//................................................................................................

double averageAndSumTheFileAmounts(double &sum_amounts,
                                   vector<PaymentMethodsBase *> &amount_list) {
  int count = 0;
  sum_amounts = 0.0;
  for (auto &amount : amount_list) {
    sum_amounts = sum_amounts + (amount->getAmount());
    count++;
  }
  return sum_amounts / count;
}
