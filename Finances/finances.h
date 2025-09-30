#pragma once
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <random>
#include <stdexcept> 
#include <fstream>
#include "sqlite3.h"

using namespace std;

// Date
struct Date {
    int day{ 0 };
    int month{ 0 };
    int year{ 0 };

    void SetMonthYear(int m, int y) {};
    void SetMonthYear() {};
    void SetFullDate(int d, int m, int y) {};
    void SetFullDate() {};

    friend ostream& operator<<(ostream& os, const Date& d) {};
};

// CardNumber
struct CardNumber {
    string firstpart, secondpart, thirdpart, fourthpart;

    static string GeneratePart() {};

    CardNumber() {};

    friend ostream& operator<<(ostream& os, const CardNumber& c) {};
};

// CVV
struct CVV {
    string CVVtype;
    int CVVvalue;

    int GenerateCVV() {};

    void SetCVV() {};
};

// Record
struct Record {
    string firstpart;
    string secondpart;

    void SetRecord() {};

    friend ostream& operator<<(ostream& os, const Record& r) {};
};

// Email
struct Email {
    string emailadress, part1, part2;
    char commercial = '@';
    char dot = '.';

    void SetEmail() {};

    friend ostream& operator<<(ostream& os, const Email& e) {};
};

// Passport
class Passport {
protected:
    string name, surname, patronymic, nationality, sex, idnumber;
    Date birthdate, duedate;
    Record record;
public:
    Passport()
        : name("Mariana"), surname("Tkachenko"),
        patronymic("Ivanivna"), nationality("Ukrainian"),
        sex("Female"),
        birthdate{ 24, 8, 1991 }, duedate{ 21, 12, 2025 },
        record{ "19910824", "00026" }, idnumber("000000000") {
    };

    void SetInfo() {};

    void GetDBInfo() const {};
};

// Wallet forward declaration
class Wallet;

// User
class User {
private:
    vector<Wallet> wallets;
    sqlite3* db;
    char* errMsg = nullptr;

    string HashPassword(const string& password) {};
public:
    User() {};

    Wallet& SelectWallet() {};
    void AddWallet(const Wallet& w) {};

    void Register(const string& username, const string& password) {};
    void CheckLogin(const string& username, const string& password) {};
    void ListWallets() {};

    ~User() {};
};

// UserChild
class UserChild : public User {
private:
    Passport passport;
    string password;
    Wallet& wallet;
public:
    UserChild(Wallet& w) : wallet(w) {}
};

// Transaction
class Transaction {
    string transactioncategory;
    Date transactiondate;
    double amount;
public:
    Transaction(const string& desc, double amt)
        : transactioncategory(desc), amount(amt) {
    }
    void SaveToFile(const string& filename) const {
        ofstream file(filename, ios::app);
        file << transactioncategory << " " << amount << "\n";
    }
};

// DebitCard
class DebitCard {
private:
    string bankname;
    User user;
    CardNumber cardnumber;
    Date duedate;
    CVV cvv;
    double balance{ 0.0 };
    vector<Transaction> transactions;
public:
    DebitCard()
        : bankname("Monobank"),
        user(User()),
        cardnumber(CardNumber()),
        duedate(Date()),
        cvv(CVV()),
        balance(0.0)
    {
    };

    bool CheckBalance() {};
    void AddTransaction(const Transaction& t) {};

    void AddCard() {};
    void AddBalance(double amount) {};

    struct Info {
        string bankname;
        CardNumber cardnumber;
        Date duedate;
        CVV cvv;
        double balance;
    };

    Info GetCardInfo() const {};

    void ShowCardInfo() const {};
};

// CreditCard
class CreditCard : public DebitCard {
private:
    double creditLimit{ 0.0 };
    double debt{ 0.0 };
public:
    CreditCard() : DebitCard(), creditLimit(0.0), debt(0.0) {}
    void SetCreditCardInfo(const string& bank,
        const CardNumber& number,
        const Date& expiry,
        const CVV& security,
        double limit)
    {
    };

    struct CreditInfo {
        DebitCard::Info baseInfo;
        double creditLimit;
        double debt;
    };

    void PayDebt(double amount) {};

    CreditInfo GetCreditCardInfo() const {};

    void ShowCreditCardInfo() const {};
};

// Wallet
class Wallet {
private:
    vector<DebitCard> debitcards;
    vector<CreditCard> creditcards;

public:
    Wallet() {}

    void AddDebitCard() {};
    void AddCreditCard() {};

    DebitCard& SelectDebitCard(int index) {};
    CreditCard& SelectCreditCard(int index) {};

    void ShowWallet() const {};

    void SetWallet() {};
};

// Report
class Report {
private:

public:
    double GetTotalExpensesByDay(const Wallet& wallet, const Date& day) {};
    vector<Transaction> GetTopExpensesWeek(const Wallet& wallet, int week, int year) {};
    double GetTotalExpensesByMonth(const Wallet& wallet, int month, int year) {};

    vector<Transaction> GetTopExpensesWeek(int week, int year) {};
    vector<string> GetTopCategoriesMonth(int month, int year) {};

    void SaveReportToFile(const string& filename, const vector<Transaction>& report) {};
};

// Utility functions
string GeneratePassword(int length = 12);
string SetPassword(int maxlength = 20);

// Main
int main();


