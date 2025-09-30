#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <random>
#include <stdexcept> 
#include <fstream>
#include "finances.h"
#include "sqlite3.h"

using namespace std;


// Date
void Date::SetMonthYear(int m, int y) {
    if (m < 1 || m > 12) {
        throw invalid_argument("Month must be between 1 and 12");
    }

    if (y >= 0 && y < 100) y += 2000;
    int currentYear = 2025;
    int maxYear = 2035;
    if (y < currentYear) {
        throw invalid_argument("Year cannot be in the past");
    }
    if (y > maxYear) {
        throw invalid_argument("Year cannot exceed 2035");
    }
    day = 0;
    month = m;
    year = y;
}

void Date::SetMonthYear() {
    while (true) {
        try {
            int m, y;
            cout << "Enter month (1-12): ";
            cin >> m;
            cout << "Enter year (YY or YYYY): ";
            cin >> y;
            SetMonthYear(m, y);
            break;
        }
        catch (const exception& e) {
            cout << "Error: " << e.what() << " Try again.\n";
        }
    }
}

void Date::SetFullDate(int d, int m, int y) {
    if (d < 1 || d > 31) {
        throw invalid_argument("Day must be 1-31");
    }
    if (m < 1 || m > 12) {
        throw invalid_argument("Month must be 1-12");
    }
    day = d;
    month = m;
    year = y;
}

void Date::SetFullDate() {
    while (true) {
        try {
            int d, m, y;
            cout << "Enter day: "; cin >> d;
            cout << "Enter month: "; cin >> m;
            cout << "Enter year: "; cin >> y;
            SetFullDate(d, m, y);
            break;
        }
        catch (const exception& e) {
            cout << "Error: " << e.what() << " Try again.\n";
        }
    }
}

ostream& operator<<(ostream& os, const Date& d) {
    if (d.day == 0) {
        os << setw(2) << setfill('0') << d.month
            << "/" << setw(2) << setfill('0') << (d.year % 100);
    }
    else {
        os << setw(2) << setfill('0') << d.day << "."
            << setw(2) << setfill('0') << d.month << "."
            << d.year;
    }
    return os;
}

// CardNumber
string CardNumber::GeneratePart() {
    const string digits = "0123456789";
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dist(0, digits.size() - 1);

    string part;
    part.reserve(4);

    uniform_int_distribution<> firstDist(1, 9);
    part += digits[firstDist(gen)];

    for (int i = 1; i < 4; i++) {
        part += digits[dist(gen)];
    }
    return part;
}

CardNumber::CardNumber() {
    firstpart = GeneratePart();
    secondpart = GeneratePart();
    thirdpart = GeneratePart();
    fourthpart = GeneratePart();
}

ostream& operator<<(ostream& os, const CardNumber& c) {
    os << c.firstpart << ' ' << c.secondpart << ' ' << c.thirdpart << ' ' << c.fourthpart;
    return os;
}

// CVV
int CVV::GenerateCVV() {
    if (CVVtype == "Dynamic") {
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dist(100, 999);
        CVVvalue = dist(gen);
        return CVVvalue;
    }
    else if (CVVtype == "Static") {
        SetCVV();
    }
    else {
        throw invalid_argument("Unknown CVV type.");
    }
}

void CVV::SetCVV() {
    cout << "Enter CVV: ";
    cin >> CVVvalue;
    cout << "The CVV has been succesfully changed.";
}

// Record
void Record::SetRecord() {
    string input;
    cout << "Enter record (format: firstpart-secondpart): ";
    cin >> input;

    size_t dashPos = input.find('-');
    if (dashPos == string::npos) {
        throw invalid_argument("Invalid format. Must contain '-' between two parts.");
    }

    firstpart = input.substr(0, dashPos);
    secondpart = input.substr(dashPos + 1);

    if (firstpart.empty() || secondpart.empty()) {
        throw invalid_argument("Both parts must be non-empty.");
    }
}

ostream& operator<<(ostream& os, const Record& r) {
    os << r.firstpart << "-" << r.secondpart;
    return os;
}

// Email
void Email::SetEmail() {
    string input;
    cout << "Enter email: ";
    cin >> input;

    size_t atPos = input.find(commercial);
    size_t dotPos = input.find(dot);

    if (atPos == string::npos || dotPos == string::npos || atPos > dotPos) {
        throw invalid_argument("Invalid format. Must contain '@' before '.'");
    }

    emailadress = input.substr(0, atPos);
    part1 = input.substr(atPos + 1, dotPos - atPos - 1);
    part2 = input.substr(dotPos + 1);

    if (emailadress.empty() || part1.empty() || part2.empty()) {
        throw invalid_argument("Parts mustn't be empty.");
    }
}

ostream& operator<<(ostream& os, const Email& e) {
    os << e.emailadress << e.commercial << e.part1 << e.part2;
    return os;
}

// Passport
Passport::Passport()
    : name("Mariana"), surname("Tkachenko"),
    patronymic("Ivanivna"), nationality("Ukrainian"),
    sex("Female"),
    birthdate{ 24, 8, 1991 }, duedate{ 21, 12, 2025 },
    record{ "19910824", "00026" }, idnumber("000000000") {
}

void Passport::SetInfo() {
    cout << "Name: ";
    cin >> name;
    cout << "Surname: ";
    cin >> surname;
    cout << "Patronymic: ";
    cin >> patronymic;
    cout << "Nationality: ";
    cin >> nationality;
    cout << "Sex: ";
    cin >> sex;
    birthdate.SetFullDate();
    duedate.SetFullDate();
    record.SetRecord();
    cout << "Id Number: ";
    cin >> idnumber;
}

void Passport::GetDBInfo() const {

}

// Wallet forward declaration
class Wallet;

// User
string User::HashPassword(const string& password) {
    string hashed = password;
    for (char& c : hashed) c = c + 3;
    return hashed;
}

User::User() {
    if (sqlite3_open("users.db", &db) != SQLITE_OK) {
        cerr << "Error opening DB: " << sqlite3_errmsg(db) << endl;
        db = nullptr;
    }
    else {
        const char* sqlCreateTable =
            "CREATE TABLE IF NOT EXISTS users ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT, "
            "username TEXT UNIQUE, "
            "password TEXT);";
        if (sqlite3_exec(db, sqlCreateTable, nullptr, nullptr, &errMsg) != SQLITE_OK) {
            cerr << "Table error: " << errMsg << endl;
            sqlite3_free(errMsg);
        }
    }
}

void User::Register(const string& username, const string& password) {
    if (!db) return;

    string hashed = HashPassword(password);
    sqlite3_stmt* stmt;
    const char* sql = "INSERT INTO users (username, password) VALUES (?, ?);";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, hashed.c_str(), -1, SQLITE_TRANSIENT);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            cout << "User registered successfully!\n";
        }
        else {
            cerr << "Registration failed: " << sqlite3_errmsg(db) << endl;
        }
    }
    sqlite3_finalize(stmt);
}

void User::CheckLogin(const string& username, const string& password) {
    if (!db) return;

    string hashed = HashPassword(password);
    sqlite3_stmt* stmt;
    const char* sql = "SELECT id FROM users WHERE username=? AND password=?;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, hashed.c_str(), -1, SQLITE_TRANSIENT);

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            cout << "Login successful!\n";
        }
        else {
            cout << "Invalid username or password.\n";
        }
    }
    sqlite3_finalize(stmt);
}

void User::ListWallets() {
    if (!db) return;
    const char* sql = "SELECT id, username FROM users;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        cout << "\n=== Registered Users ===\n";
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            const unsigned char* uname = sqlite3_column_text(stmt, 1);
            cout << id << ". " << uname << endl;
        }
    }
    sqlite3_finalize(stmt);
}

User::~User() {
    if (db) sqlite3_close(db);
}

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
Transaction::Transaction(const string& desc, double amt)
    : transactioncategory(desc), amount(amt) {
}
void Transaction::SaveToFile(const string& filename) const {
    ofstream file(filename, ios::app);
    file << transactioncategory << " " << amount << "\n";
}

// DebitCard
DebitCard::DebitCard()
    : bankname("Monobank"),
    user(User()),
    cardnumber(CardNumber()),
    duedate(Date()),
    cvv(CVV()),
    balance(0.0)
{
}

void DebitCard::AddTransaction(const Transaction& t) {
    transactions.push_back(t);
    t.SaveToFile("DebitCard_" + cardnumber.firstpart + ".txt");
}

void DebitCard::AddCard() {
    cout << "Enter bank name: ";
    cin >> bankname;

    cardnumber = CardNumber();

    duedate.SetMonthYear();

    cout << "Enter CVV type (Static/Dynamic): ";
    cin >> cvv.CVVtype;
    cvv.GenerateCVV();

    cout << "Card successfully added!\n";
}

DebitCard::Info DebitCard::GetCardInfo() const {
    return { bankname, cardnumber, duedate, cvv, balance };
}

void DebitCard::ShowCardInfo() const {
    Info info = GetCardInfo();
    cout << "=== Debit Card Info ===" << endl;
    cout << "Bank: " << info.bankname << endl;
    cout << "Card Number: " << info.cardnumber << endl;
    cout << "Expiry: " << info.duedate << endl;
    cout << "CVV: " << info.cvv.CVVvalue << endl;
    cout << "Balance: " << info.balance << " $" << endl;
    cout << endl;
}

// CreditCard
class CreditCard : public DebitCard{
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
        AddCard();
        creditLimit = limit;
        debt = 0.0;
    }

    struct CreditInfo {
        DebitCard::Info baseInfo;
        double creditLimit;
        double debt;
    };

    CreditInfo GetCreditCardInfo() const {
        return { GetCardInfo(), creditLimit, debt };
    }

    void ShowCreditCardInfo() const {
        CreditInfo info = GetCreditCardInfo();
        cout << "=== Credit Card Info ===" << endl;
        cout << "Bank: " << info.baseInfo.bankname << endl;
        cout << "Card Number: " << info.baseInfo.cardnumber << endl;
        cout << "Expiry: " << info.baseInfo.duedate << endl;
        cout << "CVV: " << info.baseInfo.cvv.CVVvalue << endl;
        cout << "Balance: " << info.baseInfo.balance << " $" << endl;
        cout << "Credit Limit: " << info.creditLimit << " $" << endl;
        cout << "Debt: " << info.debt << " $" << endl;
        cout << endl;
    }
};

// Wallet
class Wallet {
private:
    vector<DebitCard> debitcards;
    vector<CreditCard> creditcards;

public:
    Wallet() {}

    void AddDebitCard() {
        DebitCard d;
        d.AddCard();
        debitcards.push_back(d);
    }

    void AddCreditCard() {
        CreditCard c;
        c.AddCard();
        double limit;
        cout << "Enter credit limit: ";
        cin >> limit;
        c.SetCreditCardInfo("Bank", CardNumber(), Date(), CVV(), limit);
        creditcards.push_back(c);
    }

    void ShowWallet() const {
        if (debitcards.empty() && creditcards.empty()) {
            cout << "You have no cards.\n";
            return;
        }

        if (!debitcards.empty()) {
            cout << "\n=== Debit Cards ===\n";
            for (size_t i = 0; i < debitcards.size(); i++) {
                cout << "Debit Card #" << i + 1 << ":\n";
                debitcards[i].ShowCardInfo();
                cout << endl;
            }
            cout << endl;
        }

        if (!creditcards.empty()) {
            cout << "\n=== Credit Cards ===\n";
            for (size_t i = 0; i < creditcards.size(); i++) {
                cout << "Credit Card №" << i + 1 << ":\n";
                creditcards[i].ShowCreditCardInfo();
                cout << endl;
            }
            cout << endl;
        }
    }
};

// Utility functions
string GeneratePassword(int length = 12) {
    const string chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789"
        "!@#$%^&*()_+-={}[]|:;<>,.?/";

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dist(0, chars.size() - 1);

    string password;
    password.reserve(length);
    for (int i = 0; i < length; ++i) {
        password += chars[dist(gen)];
    }
    return password;
}

string SetPassword(int maxlength = 20) {
    string password;
    cout << "Enter password";
    cin >> password;
    return password;
}

// Main
int main()
{
    User user;

    while (true) {
        cout << "\n=== MENU ===\n";
        cout << "Register\n";
        cout << "Login\n";
        cout << "Exit\n";
        cout << "Choose option: ";

        string option;
        cin >> option;

        if (option == "Register") {
            string username, password;
            cout << "Enter username: ";
            cin >> username;
            cout << "Enter password: ";
            cin >> password;
            user.Register(username, password);
        }
        else if (option == "Login") {
            string username, password;
            cout << "Enter username: ";
            cin >> username;
            cout << "Enter password: ";
            cin >> password;
            user.CheckLogin(username, password);
        }
        else if (option == "Exit") {
            cout << "Exiting the program.!\n";
            break;
        }
        else {
            cout << "Invalid option.\n";
        }
    }

    return 0;
}
