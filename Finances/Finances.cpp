#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <random>
#include <stdexcept>
#include <fstream>
#include <algorithm>
#include <ctime>
#include <functional>
#include "sqlite3.h"

using namespace std;

// Date
struct Date {
    int day{ 0 }, month{ 0 }, year{ 0 };

    void SetMonthYear(int m, int y) {
        if (m < 1 || m > 12) throw invalid_argument("Month must be between 1 and 12");
        if (y >= 0 && y < 100) y += 2000;
        int currentYear = 2025;
        int maxYear = 2035;
        if (y < currentYear) throw invalid_argument("Year cannot be in the past");
        if (y > maxYear) throw invalid_argument("Year cannot exceed 2035");
        day = 0;
        month = m;
        year = y;
    }

    void SetMonthYear() {
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
                cin.clear();
                string dummy; getline(cin, dummy);
            }
        }
    }

    void SetFullDate(int d, int m, int y) {
        if (d < 1 || d > 31) throw invalid_argument("Day must be 1-31");
        if (m < 1 || m > 12) throw invalid_argument("Month must be 1-12");
        day = d; month = m; year = y;
    }

    void SetFullDate() {
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

    string ToString() const {
        char buf[32];
        if (day == 0) {
            snprintf(buf, sizeof(buf), "%02d/%04d", month, year);
        }
        else {
            snprintf(buf, sizeof(buf), "%02d.%02d.%04d", day, month, year);
        }
        return string(buf);
    }

    friend ostream& operator<<(ostream& os, const Date& d) {
        os << d.ToString();
        return os;
    }

    static Date FromString(const string& s) {
        Date r;
        if (s.find('.') != string::npos) {
            size_t p1 = s.find('.');
            size_t p2 = s.find('.', p1 + 1);
            if (p1 == string::npos || p2 == string::npos) throw invalid_argument("Bad date");
            string sd = s.substr(0, p1);
            string sm = s.substr(p1 + 1, p2 - p1 - 1);
            string sy = s.substr(p2 + 1);
            r.day = stoi(sd);
            r.month = stoi(sm);
            r.year = stoi(sy);
        }
        else if (s.find('/') != string::npos) {
            size_t p = s.find('/');
            string sm = s.substr(0, p);
            string sy = s.substr(p + 1);
            r.day = 0;
            r.month = stoi(sm);
            r.year = stoi(sy);
        }
        else {
            throw invalid_argument("Unknown date format");
        }
        return r;
    }

    long ToDays() const {
        tm t = {};
        t.tm_mday = (day == 0 ? 1 : day);
        t.tm_mon = month - 1;
        t.tm_year = year - 1900;
        t.tm_hour = 12;
        time_t tt = mktime(&t);
        return tt / (24 * 3600);
    }

    int AgeOn(int curDay, int curMonth, int curYear) const {
        int age = curYear - year;
        if (curMonth < month || (curMonth == month && curDay < day)) age--;
        return age;
    }
};

// CardNumber
struct CardNumber {
    string firstpart, secondpart, thirdpart, fourthpart;

    static string GeneratePart() {
        static const string digits = "0123456789";
        static random_device rd;
        static mt19937 gen(rd());
        uniform_int_distribution<> dist(0, 9);
        string part;
        part.reserve(4);
        for (int i = 0; i < 4; ++i) part.push_back(digits[dist(gen)]);
        return part;
    }

    CardNumber() {
        firstpart = GeneratePart();
        secondpart = GeneratePart();
        thirdpart = GeneratePart();
        fourthpart = GeneratePart();
    }

    friend ostream& operator<<(ostream& os, const CardNumber& c) {
        os << c.firstpart << ' ' << c.secondpart << ' ' << c.thirdpart << ' ' << c.fourthpart;
        return os;
    }
};

// CVV
struct CVV {
    string cvvtype{ "Static" };
    int cvvcode{ 0 };

    int GenerateCVV() {
        if (cvvtype == "Dynamic") {
            static random_device rd;
            static mt19937 gen(rd());
            uniform_int_distribution<> dist(100, 999);
            cvvcode = dist(gen);
            return cvvcode;
        }
        else if (cvvtype == "Static") {
            SetCVV();
            return cvvcode;
        }
        else {
            throw invalid_argument("Unknown CVV type.");
        }
    }

    void SetCVV() {
        cout << "Enter CVV: ";
        cin >> cvvcode;
        cout << "The CVV has been succesfully changed.\n";
    }
};

// Record

struct Record {
    string firstpart, secondpart;
    string input;

    void SetRecord() {
        cout << "Enter record: ";
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

    string GetRecord() const {
        return input;
    }

    friend ostream& operator<<(ostream& os, const Record& r) {
        os << r.firstpart << "-" << r.secondpart;
        return os;
    }
};

// Email
struct Email {
    string emailadress;
    char commercial = '@';
    char dot = '.';
    string part1, part2;

    void SetEmail() {
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

    friend ostream& operator<<(ostream& os, const Email& e) {
        os << e.emailadress << e.commercial << e.part1 << e.part2;
        return os;
    }
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
        sex("Female"), birthdate{ 24,8,1991 }, duedate{ 21,12,2025 },
        record{ "19910824","00026" }, idnumber("000000000")
    {
    }

    void SetInfo() {
        cout << "Name: "; cin >> name;
        cout << "Surname: "; cin >> surname;
        cout << "Patronymic: "; cin >> patronymic;
        cout << "Nationality: "; cin >> nationality;
        cout << "Sex: "; cin >> sex;
        birthdate.SetFullDate();
        duedate.SetFullDate();
        record.SetRecord();
        cout << "Id Number: "; cin >> idnumber;
    }

    string GetName() const { return name; }
    string GetSurname() const { return surname; }
    Date GetBirthdate() const { return birthdate; }
    string GetIdNumber() const { return idnumber; }
    string GetRecordCombined() const { return record.firstpart + "-" + record.secondpart; }

    void GetDBInfo() const {
        cout << "Name: " << name << "\nSurname: " << surname << "\nPatronymic: " << patronymic
            << "\nNationality: " << nationality << "\nSex: " << sex << "\nBirth Date: " << birthdate
            << "\nDue Date: " << duedate << "\nRecord: " << record.GetRecord() << "\n"
            << "\nDocument number: " << idnumber << "\n";
    }
};

// Wallt forward declaration
class Wallet;

// Transaction
class Transaction {
private:
    string category;
    double amount;
    Date date;
    string currency{ "$" };
public:
    Transaction() : category(""), amount(0.0), date(), currency("$") {}

    Transaction(const string& cat, double amt)
        : category(cat), amount(amt) {
        date.SetFullDate(4, 10, 2025);
    }

    Transaction(const string& cat, double amt, const Date& dt)
        : category(cat), amount(amt), date(dt) {
    }

    Transaction(const string& cat, double amt, int d, int m, int y)
        : category(cat), amount(amt), date() {
        date.SetFullDate(d, m, y);
    }

    string getCategory() const { return category; }
    double getAmount() const { return amount; }
    Date getDate() const { return date; }

    string Serialize() const {
        string out = category + ";" + to_string(amount) + ";" + date.ToString();
        return out;
    }

    void saveToFile(const string& filename = "transactions.txt") const {
        ofstream file(filename, ios::app);
        if (!file.is_open()) {
            cerr << "Error: unable to open file " << filename << "\n";
            return;
        }
        file << category << ";" << amount << ";" << date << "\n";
        file.close();
    }

    static vector<Transaction> loadFromFile(const string& filename = "transactions.txt") {
        vector<Transaction> transactions;
        ifstream file(filename);
        if (!file.is_open()) {
            return transactions;
        }
        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;
            size_t p1 = line.find(';');
            if (p1 == string::npos) continue;
            size_t p2 = line.find(';', p1 + 1);
            if (p2 == string::npos) continue;
            string cat = line.substr(0, p1);
            string amtStr = line.substr(p1 + 1, p2 - p1 - 1);
            string dateStr = line.substr(p2 + 1);
            double amt = 0.0;
            try { amt = stod(amtStr); }
            catch (...) { continue; }
            try {
                Date d = Date::FromString(dateStr);
                transactions.emplace_back(cat, amt, d);
            }
            catch (...) {

            }
        }
        file.close();
        return transactions;
    }

    void print() const {
        cout << "[" << date << "] " << category << " : " << fixed << setprecision(2) << amount << currency << "\n";
    }

    static void printAll(const vector<Transaction>& transactions) {
        for (const auto& t : transactions) t.print();
    }
};

// DebitCard
class DebitCard {
private:
    string bankname{ "Monobank" };
    CardNumber cardnumber;
    Date duedate;
    CVV cvv;
    double balance{ 0.0 };
    vector<Transaction> transactions;
public:
    DebitCard() = default;
    DebitCard(const string& bank) : bankname(bank) {}

    bool CheckBalance(double amount = 0.0) const { return balance >= amount; }

    void AddTransaction(const Transaction& t) {
        double amt = t.getAmount();
        if (!CheckBalance(amt)) {
            cerr << "Insufficient funds for " << amt << " on card " << cardnumber << "\n";
            throw runtime_error("Insufficient funds");
        }
        balance -= amt;
        transactions.push_back(t);
    }

    void AddBalance(double amount) {
        if (amount <= 0) throw invalid_argument("Amount must be positive");
        balance += amount;
    }

    const vector<Transaction>& GetTransactions() const { return transactions; }
    double GetBalance() const { return balance; }
    string GetBankName() const { return bankname; }
    string GetCardNumberStr() const {
        return cardnumber.firstpart + " " + cardnumber.secondpart + " " + cardnumber.thirdpart + " " + cardnumber.fourthpart;
    }

    void AddCardInteractive() {
        cout << "Enter bank name: "; cin >> bankname;
        cardnumber = CardNumber();
        duedate.SetMonthYear();
        cout << "Enter CVV type (Static/Dynamic): "; cin >> cvv.cvvtype;
        cvv.GenerateCVV();
        cout << "Card successfully added!\n";
    }

    struct Info {
        string bankname;
        CardNumber cardnumber;
        Date duedate;
        CVV cvv;
        double balance;
    };

    Info GetCardInfo() const {
        return { bankname, cardnumber, duedate, cvv, balance };
    }

    void ShowCardInfo() const {
        Info info = GetCardInfo();
        cout << "=== Debit Card Info ===\n";
        cout << "Bank: " << info.bankname << "\n";
        cout << "Card Number: " << info.cardnumber << "\n";
        cout << "Expiry: " << info.duedate << "\n";
        cout << "CVV: " << info.cvv.cvvcode << "\n";
        cout << "Balance: " << info.balance << " $\n\n";
    }
};

// CreditCard
class CreditCard : public DebitCard {
private:
    double creditLimit{ 0.0 };
    double debt{ 0.0 };
public:
    CreditCard() : DebitCard(), creditLimit(0.0), debt(0.0) {}

    void SetCreditLimit(double limit) {
        if (limit < 0) throw invalid_argument("Limit must be non-negative");
        creditLimit = limit;
    }

    void AddTransactionCredit(const Transaction& t) {
        double amt = t.getAmount();
        double available = GetBalance() + (creditLimit - debt);
        if (amt > available) {
            cerr << "Exceeds credit available (" << available << ")\n";
            throw runtime_error("Exceeds credit");
        }
        double bal = GetBalance();
        if (bal >= amt) {
            DebitCard::AddTransaction(t);
        }
        else {
            double remain = amt - bal;
            debt += amt;
            DebitCard::AddBalance(remain);
            DebitCard::AddTransaction(t);
            debt += remain;
        }
    }

    double GetDebt() const { return debt; }
    double GetCreditLimit() const { return creditLimit; }

    void PayDebt(double amount) {
        if (amount <= 0) throw invalid_argument("Amount must be positive");
        if (amount > debt) {
            double remainder = amount - debt;
            debt = 0;
            DebitCard::AddBalance(remainder);
        }
        else {
            debt -= amount;
        }
    }

    struct CreditInfo {
        DebitCard::Info baseInfo;
        double creditLimit;
        double debt;
    };

    CreditInfo GetCreditCardInfo() const {
        return { DebitCard::GetCardInfo(), creditLimit, debt };
    }

    void ShowCreditCardInfo() const {
        CreditInfo info = GetCreditCardInfo();
        cout << "=== Credit Card Info ===\n";
        cout << "Bank: " << info.baseInfo.bankname << "\n";
        cout << "Card Number: " << info.baseInfo.cardnumber << "\n";
        cout << "Expiry: " << info.baseInfo.duedate << "\n";
        cout << "CVV: " << info.baseInfo.cvv.cvvcode << "\n";
        cout << "Balance: " << info.baseInfo.balance << " $\n";
        cout << "Credit Limit: " << info.creditLimit << " $\n";
        cout << "Debt: " << info.debt << " $\n\n";
    }
};

// Wallet
class Wallet {
private:
    vector<DebitCard> debitcards;
    vector<CreditCard> creditcards;
public:
    Wallet() {}

    void AddDebitCard(const DebitCard& c) { debitcards.push_back(c); }
    void AddCreditCard(const CreditCard& c) { creditcards.push_back(c); }

    DebitCard& SelectDebitCard(int index) {
        if (index < 0 || index >= (int)debitcards.size()) throw out_of_range("Debit card index out of range");
        return debitcards[index];
    }

    CreditCard& SelectCreditCard(int index) {
        if (index < 0 || index >= (int)creditcards.size()) throw out_of_range("Credit card index out of range");
        return creditcards[index];
    }

    vector<DebitCard>& GetDebitCards() { return debitcards; }
    vector<CreditCard>& GetCreditCards() { return creditcards; }

    const vector<DebitCard>& GetDebitCards() const { return debitcards; }
    const vector<CreditCard>& GetCreditCards() const { return creditcards; }

    void ShowWallet() const {
        cout << "=== Wallet Summary ===\n";
        cout << "Debit cards: " << debitcards.size() << "\n";
        for (size_t i = 0; i < debitcards.size(); ++i) {
            cout << "  [" << i << "] Bank: " << debitcards[i].GetBankName()
                << " Balance: " << fixed << setprecision(2) << debitcards[i].GetBalance() << "\n";
        }
        cout << "Credit cards: " << creditcards.size() << "\n";
        for (size_t i = 0; i < creditcards.size(); ++i) {
            cout << "  [" << i << "] Balance: " << fixed << setprecision(2) << creditcards[i].GetBalance()
                << " Debt: " << creditcards[i].GetDebt() << "\n";
        }
        cout << "======================\n";
    }

    void SetWalletInteractive() {
        int choice;
        cout << "1) Add Debit card\n2) Add Credit card\nChoose: ";
        cin >> choice;
        if (choice == 1) {
            DebitCard c;
            c.AddCardInteractive();
            double bal;
            cout << "Initial balance (0 if none): "; cin >> bal;
            if (bal > 0) c.AddBalance(bal);
            AddDebitCard(c);
        }
        else if (choice == 2) {
            CreditCard cc;
            cc.AddCardInteractive();
            double lim; cout << "Credit limit: "; cin >> lim; cc.SetCreditLimit(lim);
            AddCreditCard(cc);
        }
    }
};

// User & database interaction
class User {
protected:
    Passport passport;
    string username;
    string passwordHash;
    sqlite3* db{ nullptr };
    char* errMsg{ nullptr };
    vector<Wallet> wallets;

    Date GetPassportBirthdate() const {
        return passport.GetBirthdate();
    }
public:
    User() = default;

    static string HashPassword(const string& pwd) {
        size_t h = std::hash<string>{}(pwd);
        // hex string
        char buf[32];
        snprintf(buf, sizeof(buf), "%zx", h);
        return string(buf);
    }

    void OpenDB(const string& filename = "users.db") {
        int rc = sqlite3_open(filename.c_str(), &db);
        if (rc) throw runtime_error("Cannot open DB");
        const char* users_sql = "CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY AUTOINCREMENT, username TEXT UNIQUE, password_hash TEXT, is_child INTEGER DEFAULT 0);";
        rc = sqlite3_exec(db, users_sql, 0, 0, &errMsg);
        if (rc != SQLITE_OK) {
            string msg = errMsg ? errMsg : "unknown";
            sqlite3_free(errMsg);
            throw runtime_error("SQL error creating users table: " + msg);
        }
        const char* passports_sql = "CREATE TABLE IF NOT EXISTS passports (id INTEGER PRIMARY KEY AUTOINCREMENT, user_id INTEGER, name TEXT, surname TEXT, birthdate TEXT, record TEXT, idnumber TEXT, FOREIGN KEY(user_id) REFERENCES users(id));";
        rc = sqlite3_exec(db, passports_sql, 0, 0, &errMsg);
        if (rc != SQLITE_OK) {
            string msg = errMsg ? errMsg : "unknown";
            sqlite3_free(errMsg);
            throw runtime_error("SQL error creating passports table: " + msg);
        }
        const char* tx_sql = "CREATE TABLE IF NOT EXISTS transactions (id INTEGER PRIMARY KEY AUTOINCREMENT, user_id INTEGER, category TEXT, amount REAL, date TEXT, FOREIGN KEY(user_id) REFERENCES users(id));";
        rc = sqlite3_exec(db, tx_sql, 0, 0, &errMsg);
        if (rc != SQLITE_OK) {
            string msg = errMsg ? errMsg : "unknown";
            sqlite3_free(errMsg);
            throw runtime_error("SQL error creating transactions table: " + msg);
        }
    }

    Passport& GetPassport() { return passport; }
    const Passport& GetPassport() const { return passport; }

    void CloseDB() {
        if (db) sqlite3_close(db);
        db = nullptr;
    }

    virtual ~User() { CloseDB(); }

    long Register(const string& uname, const string& pwd) {
        username = uname;
        passwordHash = HashPassword(pwd);
        cout << "Fill passport info:\n";
        passport.SetInfo();

        int curd = 4;
        int curm = 10;
        int cury = 2025;

        int age = passport.GetBirthdate().AgeOn(curd, curm, cury);
        int is_child = (age < 14) ? 1 : 0;

        sqlite3_stmt* stmt = nullptr;
        const char* insertUser = "INSERT INTO users (username, password_hash, is_child) VALUES (?, ?, ?);";
        if (sqlite3_prepare_v2(db, insertUser, -1, &stmt, nullptr) != SQLITE_OK) {
            throw runtime_error("Prepare insert user failed");
        }
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, passwordHash.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 3, is_child);
        int rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
            sqlite3_finalize(stmt);
            throw runtime_error("Insert user failed (maybe username taken)");
        }
        sqlite3_finalize(stmt);

        long long userId = sqlite3_last_insert_rowid(db);

        const char* insertPass = "INSERT INTO passports (user_id, name, surname, birthdate, record, idnumber) VALUES (?, ?, ?, ?, ?, ?);";
        if (sqlite3_prepare_v2(db, insertPass, -1, &stmt, nullptr) != SQLITE_OK) throw runtime_error("Prepare insert passport failed");
        sqlite3_bind_int64(stmt, 1, userId);
        sqlite3_bind_text(stmt, 2, passport.GetName().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, passport.GetSurname().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 4, passport.GetBirthdate().ToString().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 5, passport.GetRecordCombined().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 6, passport.GetIdNumber().c_str(), -1, SQLITE_TRANSIENT);
        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
            sqlite3_finalize(stmt);
            throw runtime_error("Insert passport failed");
        }
        sqlite3_finalize(stmt);

        cout << "Registered user '" << username << "' id=" << userId << " is_child=" << is_child << "\n";
        return userId;
    }

    long CheckLogin(const string& uname, const string& pwd) {
        username = uname;
        passwordHash = HashPassword(pwd);
        sqlite3_stmt* stmt = nullptr;
        const char* q = "SELECT id FROM users WHERE username = ? AND password_hash = ?;";
        if (sqlite3_prepare_v2(db, q, -1, &stmt, nullptr) != SQLITE_OK) throw runtime_error("Prepare login failed");
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, passwordHash.c_str(), -1, SQLITE_TRANSIENT);
        int rc = sqlite3_step(stmt);
        if (rc == SQLITE_ROW) {
            long long id = sqlite3_column_int64(stmt, 0);
            sqlite3_finalize(stmt);
            cout << "Login success for " << username << " id=" << id << "\n";
            return id;
        }
        else {
            sqlite3_finalize(stmt);
            cout << "Login failed\n";
            return -1;
        }
    }

    void SaveTransactionToDB(long long userId, const Transaction& t) {
        sqlite3_stmt* stmt = nullptr;
        const char* ins = "INSERT INTO transactions (user_id, category, amount, date) VALUES (?, ?, ?, ?);";
        if (sqlite3_prepare_v2(db, ins, -1, &stmt, nullptr) != SQLITE_OK) throw runtime_error("Prepare insert tx failed");
        sqlite3_bind_int64(stmt, 1, userId);
        sqlite3_bind_text(stmt, 2, t.getCategory().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_double(stmt, 3, t.getAmount());
        sqlite3_bind_text(stmt, 4, t.getDate().ToString().c_str(), -1, SQLITE_TRANSIENT);
        int rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
            sqlite3_finalize(stmt);
            throw runtime_error("Insert transaction failed");
        }
        sqlite3_finalize(stmt);
        t.saveToFile("transactions.txt");
    }

    vector<Transaction> LoadTransactionsFromDB(long long userId) {
        vector<Transaction> out;
        sqlite3_stmt* stmt = nullptr;
        const char* q = "SELECT category, amount, date FROM transactions WHERE user_id = ? ORDER BY id DESC;";
        if (sqlite3_prepare_v2(db, q, -1, &stmt, nullptr) != SQLITE_OK) throw runtime_error("Prepare select tx failed");
        sqlite3_bind_int64(stmt, 1, userId);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            const unsigned char* catc = sqlite3_column_text(stmt, 0);
            double amt = sqlite3_column_double(stmt, 1);
            const unsigned char* datec = sqlite3_column_text(stmt, 2);
            string cat = catc ? reinterpret_cast<const char*>(catc) : "";
            string ds = datec ? reinterpret_cast<const char*>(datec) : "";
            try {
                Date d = Date::FromString(ds);
                out.emplace_back(cat, amt, d);
            }
            catch (...) {
            }
        }
        sqlite3_finalize(stmt);
        return out;
    }

    void AddWallet(const Wallet& w) { wallets.push_back(w); }
    Wallet& SelectWallet(size_t idx) {
        if (idx >= wallets.size()) throw out_of_range("Wallet index out of range");
        return wallets[idx];
    }
    void ListWallets() const {
        cout << "User has " << wallets.size() << " wallets.\n";
    }
};

// UserChild
class UserChild : public User {
private:
    double spendingLimitPerDay{ 50.0 };
public:
    UserChild() : User() {}

    void SetSpendingLimit(double v) { spendingLimitPerDay = v; }
    double GetSpendingLimit() const { return spendingLimitPerDay; }
};

// Report
class Report {
private:
    static vector<Transaction> CollectAll(const Wallet& w) {
        vector<Transaction> out;
        for (const auto& dc : w.GetDebitCards()) {
            const vector<Transaction>& v = dc.GetTransactions();
            out.insert(out.end(), v.begin(), v.end());
        }
        for (const auto& cc : w.GetCreditCards()) {
            const vector<Transaction>& v = cc.GetTransactions();
            out.insert(out.end(), v.begin(), v.end());
        }
        return out;
    }

    static bool SameDay(const Date& a, const Date& b) {
        return (a.day == b.day && a.month == b.month && a.year == b.year);
    }

    static long WeekIndex(const Date& d) {
        return d.ToDays() / 7;
    }

public:
    static double GetTotalExpensesByDay(const Wallet& wallet, const Date& day) {
        vector<Transaction> all = CollectAll(wallet);
        double sum = 0.0;
        for (const auto& t : all) {
            if (SameDay(t.getDate(), day)) sum += t.getAmount();
        }
        return sum;
    }

    static double GetTotalExpensesByMonth(const Wallet& wallet, int month, int year) {
        vector<Transaction> all = CollectAll(wallet);
        double sum = 0.0;
        for (const auto& t : all) {
            Date d = t.getDate();
            if (d.day == 0) {
                if (d.month == month && d.year == year) sum += t.getAmount();
            }
            else {
                if (d.month == month && d.year == year) sum += t.getAmount();
            }
        }
        return sum;
    }

    static vector<Transaction> GetTopExpensesWeek(const Wallet& wallet, int week, int year, int topN = 3) {
        vector<Transaction> all = CollectAll(wallet);
        vector<Transaction> filtered;

        for (const auto& t : all) {
            Date d = t.getDate();
            long wi = WeekIndex(d);

            tm time_in = {};
            time_in.tm_mday = (d.day == 0 ? 1 : d.day);
            time_in.tm_mon = d.month - 1;
            time_in.tm_year = d.year - 1900;
            mktime(&time_in);
            int woy = time_in.tm_yday / 7 + 1;
            if (d.year == year && woy == week) filtered.push_back(t);
        }
        sort(filtered.begin(), filtered.end(), [](const Transaction& a, const Transaction& b) {
            return a.getAmount() > b.getAmount();
            });
        if ((int)filtered.size() > topN) filtered.resize(topN);
        return filtered;
    }

    static vector<Transaction> GetTopExpensesMonth(const Wallet& wallet, int month, int year, int topN = 3) {
        vector<Transaction> all = CollectAll(wallet);
        vector<Transaction> filtered;
        for (const auto& t : all) {
            Date d = t.getDate();
            if (d.month == month && d.year == year) filtered.push_back(t);
        }
        sort(filtered.begin(), filtered.end(), [](const Transaction& a, const Transaction& b) {
            return a.getAmount() > b.getAmount();
            });
        if ((int)filtered.size() > topN) filtered.resize(topN);
        return filtered;
    }

    static vector<pair<string, double>> GetTopCategoriesMonth(const Wallet& wallet, int month, int year, int topN = 3) {
        vector<Transaction> all = CollectAll(wallet);
        vector<pair<string, double>> agg;
        for (const auto& t : all) {
            Date d = t.getDate();
            if (d.month == month && d.year == year) {
                bool found = false;
                for (auto& p : agg) {
                    if (p.first == t.getCategory()) { p.second += t.getAmount(); found = true; break; }
                }
                if (!found) agg.emplace_back(t.getCategory(), t.getAmount());
            }
        }
        sort(agg.begin(), agg.end(), [](const pair<string, double>& a, const pair<string, double>& b) {
            return a.second > b.second;
            });
        if ((int)agg.size() > topN) agg.resize(topN);
        return agg;
    }

    static vector<pair<string, double>> GetTopCategoriesWeek(const Wallet& wallet, int week, int year, int topN = 3) {
        vector<Transaction> all = CollectAll(wallet);
        vector<pair<string, double>> agg;
        for (const auto& t : all) {
            Date d = t.getDate();
            tm time_in = {};
            time_in.tm_mday = (d.day == 0 ? 1 : d.day);
            time_in.tm_mon = d.month - 1;
            time_in.tm_year = d.year - 1900;
            mktime(&time_in);
            int woy = time_in.tm_yday / 7 + 1;
            if (d.year == year && woy == week) {
                bool found = false;
                for (auto& p : agg) {
                    if (p.first == t.getCategory()) { p.second += t.getAmount(); found = true; break; }
                }
                if (!found) agg.emplace_back(t.getCategory(), t.getAmount());
            }
        }
        sort(agg.begin(), agg.end(), [](const pair<string, double>& a, const pair<string, double>& b) {
            return a.second > b.second;
            });
        if ((int)agg.size() > topN) agg.resize(topN);
        return agg;
    }

    static void SaveReportToFile(const string& filename, const vector<string>& lines) {
        ofstream f(filename);
        if (!f.is_open()) {
            cerr << "Unable to write report to " << filename << "\n";
            return;
        }
        for (const auto& l : lines) f << l << "\n";
        f.close();
    }

    static void PrintAndSaveReport_Day(const Wallet& wallet, const Date& day, const string& outFile = "report_day.txt") {
        vector<string> lines;
        lines.push_back("┌────────────────────────────────────────────┐");
        lines.push_back("│             Daily Expense Report           │");
        lines.push_back("├────────────────────────────────────────────┤");
        string header = "│ Day: " + day.ToString();
        header.append(40 - header.size(), ' ');
        header += "│";
        lines.push_back(header);
        lines.push_back("├────────────────────────────────────────────┤");

        double total = GetTotalExpensesByDay(wallet, day);
        lines.push_back("│ Total expenses: " + to_string(total) + "                     │");
        lines.push_back("├────────────────────────────────────────────┤");
        lines.push_back("│ Transactions:                               │");

        vector<Transaction> all = CollectAll(wallet);
        for (const auto& t : all) {
            if (SameDay(t.getDate(), day)) {
                char buf[128];
                snprintf(buf, sizeof(buf), "│ %-20s %10.2f %6s │", t.getCategory().c_str(), t.getAmount(), t.getDate().ToString().c_str());
                lines.push_back(string(buf));
            }
        }
        lines.push_back("└────────────────────────────────────────────┘");

        for (const auto& l : lines) cout << l << "\n";

        time_t now = time(nullptr);
        string fname = outFile;
        SaveReportToFile(fname, lines);
        cout << "Saved daily report to " << fname << "\n";
    }

    static void PrintAndSaveReport_Month(const Wallet& wallet, int month, int year, const string& outFile = "report_month.txt") {
        vector<string> lines;
        lines.push_back("┌────────────────────────────────────────────┐");
        lines.push_back("│            Monthly Expense Report          │");
        lines.push_back("├────────────────────────────────────────────┤");
        char headerBuf[64];
        snprintf(headerBuf, sizeof(headerBuf), "│ Month: %02d/%04d", month, year);
        string header = headerBuf;
        header.append(40 - header.size(), ' ');
        header += "│";
        lines.push_back(header);
        lines.push_back("├────────────────────────────────────────────┤");

        double total = GetTotalExpensesByMonth(wallet, month, year);
        char totBuf[64];
        snprintf(totBuf, sizeof(totBuf), "│ Total expenses: %.2f                      │", total);
        lines.push_back(string(totBuf));
        lines.push_back("├────────────────────────────────────────────┤");
        lines.push_back("│ Top-3 transactions:                         │");

        auto topTx = GetTopExpensesMonth(wallet, month, year, 3);
        for (const auto& t : topTx) {
            char buf[128];
            snprintf(buf, sizeof(buf), "│ %-18s %10.2f %6s │", t.getCategory().c_str(), t.getAmount(), t.getDate().ToString().c_str());
            lines.push_back(string(buf));
        }

        lines.push_back("├────────────────────────────────────────────┤");
        lines.push_back("│ Top-3 categories:                           │");
        auto topCat = GetTopCategoriesMonth(wallet, month, year, 3);
        for (const auto& p : topCat) {
            char buf[128];
            snprintf(buf, sizeof(buf), "│ %-18s %10.2f            │", p.first.c_str(), p.second);
            lines.push_back(string(buf));
        }

        lines.push_back("└────────────────────────────────────────────┘");

        for (const auto& l : lines) cout << l << "\n";
        SaveReportToFile(outFile, lines);
        cout << "Saved monthly report to " << outFile << "\n";
    }

    static void PrintAndSaveReport_Week(const Wallet& wallet, int week, int year, const string& outFile = "report_week.txt") {
        vector<string> lines;
        lines.push_back("┌────────────────────────────────────────────┐");
        lines.push_back("│             Weekly Expense Report          │");
        lines.push_back("├────────────────────────────────────────────┤");
        char headerBuf[64];
        snprintf(headerBuf, sizeof(headerBuf), "│ Week: %02d  Year: %04d", week, year);
        string header = headerBuf;
        header.append(40 - header.size(), ' ');
        header += "│";
        lines.push_back(header);
        lines.push_back("├────────────────────────────────────────────┤");

        auto topTx = GetTopExpensesWeek(wallet, week, year, 3);
        lines.push_back("│ Top-3 transactions:                         │");
        for (const auto& t : topTx) {
            char buf[128];
            snprintf(buf, sizeof(buf), "│ %-18s %10.2f %6s │", t.getCategory().c_str(), t.getAmount(), t.getDate().ToString().c_str());
            lines.push_back(string(buf));
        }

        lines.push_back("├────────────────────────────────────────────┤");
        lines.push_back("│ Top-3 categories:                           │");
        auto topCat = GetTopCategoriesWeek(wallet, week, year, 3);
        for (const auto& p : topCat) {
            char buf[128];
            snprintf(buf, sizeof(buf), "│ %-18s %10.2f            │", p.first.c_str(), p.second);
            lines.push_back(string(buf));
        }

        lines.push_back("└────────────────────────────────────────────┘");

        for (const auto& l : lines) cout << l << "\n";
        SaveReportToFile(outFile, lines);
        cout << "Saved weekly report to " << outFile << "\n";
    }
};

// Main realization
int main() {
    try {
        User app;
        app.OpenDB("users.db");

        cout << "Welcome. Choose:\n1) Register\n2) Login\nChoice: ";
        int opt; cin >> opt;
        long long uid = -1;
        if (opt == 1) {
            string uname, pwd;
            cout << "Username: "; cin >> uname;
            cout << "Password: "; cin >> pwd;
            uid = app.Register(uname, pwd);
            int curd = 4;
            int curm = 10;
            int cury = 2025;
            int age = app.GetPassport().GetBirthdate().AgeOn(curd, curm, cury);
        }
        else {
            string uname, pwd;
            cout << "Username: "; cin >> uname;
            cout << "Password: "; cin >> pwd;
            uid = app.CheckLogin(uname, pwd);
            if (uid == -1) {
                cout << "Login failed. Exiting.\n";
                return 0;
            }
        }

        Wallet w;
        DebitCard dc("Monobank");
        dc.AddBalance(500.0);
        w.AddDebitCard(dc);

        CreditCard cc;
        cc.SetCreditLimit(1000.0);
        w.AddCreditCard(cc);

        app.AddWallet(w);

        Date today;
        today.SetFullDate(4, 10, 2025);

        Transaction t1("Food", 45.50, today);
        Transaction t2("Transport", 12.30, today);
        Transaction t3("Gadgets", 200.00, today);

        try {
            app.SelectWallet(0).SelectDebitCard(0).AddTransaction(t1);
            app.SaveTransactionToDB(uid, t1);
        }
        catch (const exception& e) { cerr << e.what() << "\n"; }

        try {
            app.SelectWallet(0).SelectDebitCard(0).AddTransaction(t2);
            app.SaveTransactionToDB(uid, t2);
        }
        catch (const exception& e) { cerr << e.what() << "\n"; }

        try {
            app.SelectWallet(0).SelectCreditCard(0).AddTransactionCredit(t3);
            app.SaveTransactionToDB(uid, t3);
        }
        catch (const exception& e) { cerr << e.what() << "\n"; }

        cout << "Transactions recorded. Current wallet:\n";
        app.SelectWallet(0).ShowWallet();

        Report::PrintAndSaveReport_Day(app.SelectWallet(0), today, "report_day.txt");
        tm time_in = {};
        time_in.tm_mday = today.day;
        time_in.tm_mon = today.month - 1;
        time_in.tm_year = today.year - 1900;
        mktime(&time_in);
        int weekOfYear = time_in.tm_yday / 7 + 1;
        Report::PrintAndSaveReport_Week(app.SelectWallet(0), weekOfYear, today.year, "report_week.txt");
        Report::PrintAndSaveReport_Month(app.SelectWallet(0), today.month, today.year, "report_month.txt");

        app.CloseDB();
    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << "\n";
    }
    return 0;
}
