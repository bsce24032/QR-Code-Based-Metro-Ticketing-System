#pragma once
#include <iostream>
using namespace std;
#include "json.hpp"
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <string> 
#include <vector>
#include <sstream>  // Include this for stringstream to work

// color codes
#define RESET "\033[0m"
#define CYAN "\033[36m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define WHITE "\033[97m"

using json = nlohmann::json;

// forward declaration of PassengerData class
class PassengerData;

// Base abstract class
class PaymentMethodsBase {
public:
  // run-time polymorphism here
  // cannot create the object but pointer can be made

  // these are the declarations
  virtual void displayDetails() = 0;
  virtual bool makePayment() = 0;
  virtual ~PaymentMethodsBase() {}
  virtual double getAmount() const = 0;
  virtual void setAmount(double amt) = 0;
  virtual string getPaymentType() const = 0;
  virtual long long int getRefrence() const = 0;
  virtual PassengerData *getPassengerData() const = 0;
  virtual void setPassengerData(PassengerData *passenger) = 0;
  virtual void generateReceipt() = 0;
};

// PaymentRecipt class
class PaymentRecipt {
private:
  int transaction_id;
  string time_stamp;
  string status;
  // aggregation
  PaymentMethodsBase *payment;

public:
  PaymentRecipt(PaymentMethodsBase *pm);
  void setTransactionId(int id);
  void setTime(string time);
  void setStatus(string st);
  int getTransactionId();
  string getTime();
  string getStatus();
  void displayReceipt();
};

// PaymentMethods class
class PaymentMethods : public PaymentMethodsBase {
protected:
  string paymentMethodName;
  long long int reference;
  double amount;
  PassengerData *passenger;
  PaymentRecipt receipt;

public:
  PaymentMethods(const string &method, double amt);
  void setAmount(double amt) override;
  double getAmount() const override;
  void setReference(long long ref);
  long long int getRefrence() const override;
  PassengerData *getPassengerData() const override;
  void setPassengerData(PassengerData *p) override;
  void generateReceipt() override;
};

// JazzCash class
class JazzCash : public PaymentMethods {
private:
  double balance;

public:
  JazzCash();
  JazzCash(long long int number, double balance);
  JazzCash(double amo);
  void displayDetails() override;
  bool makePayment() override;
  void setBalance(double balance);
  double getBalance();
  string getPaymentType() const override;
};

// EasyPaisa class
class EasyPaisa : public PaymentMethods {
private:
  double balance;

public:
  EasyPaisa();
  EasyPaisa(long long int number, double balance);
  EasyPaisa(double amo);
  void displayDetails() override;
  bool makePayment() override;
  void setBalance(double balance);
  double getBalance();
  string getPaymentType() const override;
};

// Bank class
class Bank : public PaymentMethods {
private:
  double balance;

public:
  Bank();
  Bank(long long int account_number, double balance);
  Bank(double amo);
  void displayDetails() override;
  bool makePayment() override;
  void setBalance(double balance);
  double getBalance();
  string getPaymentType() const override;
};

// global variable
extern string filename;

// Helper functions
void writeIntoJsonFromPaymentMethods(const vector<unique_ptr<PaymentMethodsBase>> &customers);
void readFromJsonFromPaymentMethods();
void displayPaymentMethods();
void readDataJsonIntoVector(vector<PaymentMethodsBase *> &amnount_list);
double operator+(double total, const PaymentMethodsBase &payment);
double averageAndSumTheFileAmounts(double &sum_amounts, vector<PaymentMethodsBase *> &amount_list);

// Replace existing inline definitions with:
inline string getCurrentDateTime() {
  time_t now = time(nullptr);
  char buf[80];
  strftime(buf, sizeof(buf), "%d/%m/%Y %H:%M", localtime(&now));
  return string(buf);
}

inline int generateTransactionID() {
  static int counter = 100000;
  return ++counter;
}