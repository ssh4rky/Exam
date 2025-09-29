#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <random>
#include <stdexcept> 
#include <fstream>

using namespace std;

struct Date {
    int day{ 0 };
    int month{ 0 };
    int year{ 0 };

    void SetMonthYear(int m, int y) {
        if (m < 1 || m > 12)
            throw invalid_argument("Month must be between 1 and 12");
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
            }
        }
    }

    void SetFullDate(int d, int m, int y) {
        if (d < 1 || d > 31) throw invalid_argument("Day must be 1-31");
        if (m < 1 || m > 12) throw invalid_argument("Month must be 1-12");
        day = d;
        month = m;
        year = y;
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

    friend ostream& operator<<(ostream& os, const Date& d) {
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
};

struct CardNumber {
    string firstpart;
    string secondpart;
    string thirdpart;
    string fourthpart;

    static string GeneratePart() {
        const string digits = "0123456789";
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dist(0, digits.size() - 1);

        string part;
        part.reserve(4);
        for (int i = 0; i < 4; i++) {
            part += digits[dist(gen)];
        }
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

struct CVV {
    string CVVtype;
    int CVVvalue;

    int GenerateCVV() {
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

    void SetCVV() {
        cout << "Enter CVV: ";
        cin >> CVVvalue;
        cout << "The CVV has been succesfully changed.";
    }
};

struct Record {
	string firstpart;
	string secondpart;

    void SetRecord() {
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

    friend ostream& operator<<(ostream& os, const Record& r) {
        os << r.firstpart << "-" << r.secondpart;
        return os;
    }
};

struct Email {
    string emailadress;
    char commercial = '@';
    char dot = '.';
    string part1;
    string part2;

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

class Passport {
protected:
    string name;
    string surname;
    string patronymic;
    string nationality;

    string sex;
    Date birthdate;
    Date duedate;
    Record record;
    string idnumber;
public:
    Passport()
        : name("Mariana"), surname("Tkachenko"),
        patronymic("Ivanivna"), nationality("Ukrainian"),
        sex("Female"),
        birthdate{ 24, 8, 1991 }, duedate{ 21, 12, 2025 },
        record{ "19910824", "00026" }, idnumber("000000000") {
    }

    void SetInfo() {
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

    void GetInfo() const{
        cout << "Name: " << name << endl;
        cout << "Surname: " << surname << endl;
        cout << "Patronymic: " << patronymic << endl;
        cout << "Nationality: " << nationality << endl;

        cout << "Sex: " << sex << endl;
        cout << "Birth Date: " << birthdate << endl;
        cout << "Due Date: " << duedate << endl;
        cout << "Record: " << record << endl;
        cout << "Document number: " << idnumber << endl;
    }
};

class Wallet;

class User {
private:
    Passport passport;
    Email email;
    string passwordHash;
    Wallet wallet;

    string HashPassword(const string& password) {
        string hashed = password;
        for (char& c : hashed) c = c + 3;
        return hashed;
    }

    string Encrypt(const string& text, char key = 'K') const {
        string result = text;
        for (char& c : result) c ^= key;
        return result;
    }

    string Decrypt(const string& text, char key = 'K') const {
        return Encrypt(text, key);
    }

public:
    User() {}

    void Register() {
        cout << "\n=== Registration ===\n";
        passport.SetInfo();
        email.SetEmail();

        cout << "Create password: ";
        string pass = SetPassword();
        passwordHash = HashPassword(pass);

        cout << "\nNow setup your wallet:\n";
        wallet.SetWallet();

        cout << "Registration complete!\n";
    }

    bool Login() {
        cout << "\n=== Login ===\n";
        string emailInput, passInput;
        cout << "Enter email: ";
        cin >> emailInput;
        cout << "Enter password: ";
        cin >> passInput;

        if (emailInput == email.emailadress && passwordHash == HashPassword(passInput)) {
            cout << "Login successful!\n";
            return true;
        }
        else {
            cout << "Invalid email or password!\n";
            return false;
        }
    }

    Wallet& GetWallet() {
        return wallet;
    }

    void SaveToFile(const string& filename) {
        ofstream fout(filename);
        if (!fout) {
            cerr << "File error!\n";
            return;
        }

        fout << Encrypt(passport.series + " " + passport.number) << endl;
        fout << Encrypt(email.emailadress) << endl;
        fout << Encrypt(passwordHash) << endl;

        fout << Encrypt("DebitCards: " + to_string(wallet.GetDebitCount())) << endl;
        fout << Encrypt("CreditCards: " + to_string(wallet.GetCreditCount())) << endl;

        fout.close();
        cout << "User saved to file.\n";
    }

    void LoadFromFile(const string& filename) {
        ifstream fin(filename);
        if (!fin) {
            cerr << "No saved file!\n";
            return;
        }

        string line;

        getline(fin, line);
        passport.series = Decrypt(line).substr(0, 2);
        passport.number = Decrypt(line).substr(3);

        getline(fin, line);
        email.emailadress = Decrypt(line);

        getline(fin, line);
        passwordHash = Decrypt(line);

        // wallet counts (not full restore yet, just demo)
        getline(fin, line);
        cout << Decrypt(line) << endl;
        getline(fin, line);
        cout << Decrypt(line) << endl;

        fin.close();
        cout << "User loaded from file.\n";
    }
};

class UserChild : public User {
private:
    Passport passport;
    string password;
    Wallet& wallet;
public:

};

class DebitCard {
private:
    string bankname;
    User user;
    CardNumber cardnumber;
	Date duedate;
    CVV cvv;
    double balance{0.0};
public:
    DebitCard()
        : bankname("Monobank"),
        user(User()),
        cardnumber(CardNumber()),
        duedate(Date()),
        cvv(CVV()),
        balance(0.0)
    {}

    void AddCard() {
        cout << "Enter bank name: ";
        cin >> bankname;

        cardnumber = CardNumber();

        duedate.SetMonthYear();

        cout << "Enter CVV type (Static/Dynamic): ";
        cin >> cvv.CVVtype;
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
        cout << "=== Debit Card Info ===" << endl;
        cout << "Bank: " << info.bankname << endl;
        cout << "Card Number: " << info.cardnumber << endl;
        cout << "Expiry: " << info.duedate << endl;
        cout << "CVV: " << info.cvv.CVVvalue << endl;
        cout << "Balance: " << info.balance << " $" << endl;
    }
};

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
    }
};

class Wallet {
private:
    vector<DebitCard> debitcards;
    DebitCard bufferdebitcard;
    vector<CreditCard> creditcards;
    CreditCard buffercreditcard;
public:
    Wallet() {};

    void SetWallet() {
        string answer;
        cout << "Do you want to add a debit card? (Yes/No): ";
        cin >> answer;
        if (answer == "Yes") {
            DebitCard d;
            d.AddCard();
            debitcards.push_back(d);
        }

        cout << "Do you want to add a credit card? (Yes/No): ";
        cin >> answer;
        if (answer == "Yes") {
            CreditCard c;
            c.AddCard();
            double limit;
            cout << "Enter credit limit: ";
            cin >> limit;
            c.SetCreditCardInfo("Bank", CardNumber(), Date(), CVV(), limit);
            creditcards.push_back(c);
        }
    }

    void GetWallet() const {
        if (debitcards.empty() && creditcards.empty()) {
            cout << "You have no cards.\n";
            return;
        }

        if (!debitcards.empty()) {
            cout << "\n=== Debit Cards ===\n";
            for (size_t i = 0; i < debitcards.size(); i++) {
                cout << "Debit Card №" << i + 1 << ":\n";
                debitcards[i].ShowCardInfo();
                cout << endl;
            }
        }
        else {
            cout << "You have no debit cards.\n";
        }

        if (!creditcards.empty()) {
            cout << "\n=== Credit Cards ===\n";
            for (size_t i = 0; i < creditcards.size(); i++) {
                cout << "Credit Card №" << i + 1 << ":\n";
                creditcards[i].ShowCreditCardInfo();
                cout << endl;
            }
        }
        else {
            cout << "You have no credit cards.\n";
        }
    }
};

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

int main()
{
    ofstream fout("myfile.txt");

}
