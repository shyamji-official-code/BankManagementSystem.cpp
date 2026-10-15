/*
    Bank Management Application
    ----------------------------
    Features:
      - Create account
      - Deposit
      - Withdraw
      - Balance inquiry
      - Persistent storage using file handling (accounts.txt)
      - Object-Oriented Design (Encapsulation via Account & Bank classes)
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>

using namespace std;

// ---------------------------------------------------------
// Account class: represents a single bank account
// ---------------------------------------------------------
class Account {
private:
    int accountNumber;
    string name;
    double balance;

public:
    Account() : accountNumber(0), balance(0.0) {}

    Account(int accNo, string holderName, double bal)
        : accountNumber(accNo), name(holderName), balance(bal) {}

    int getAccountNumber() const { return accountNumber; }
    string getName() const { return name; }
    double getBalance() const { return balance; }

    void deposit(double amount) {
        balance += amount;
    }

    bool withdraw(double amount) {
        if (amount > balance) {
            return false; // insufficient funds
        }
        balance -= amount;
        return true;
    }

    void display() const {
        cout << left << setw(15) << accountNumber
             << setw(20) << name
             << fixed << setprecision(2) << balance << endl;
    }

    // Convert account data to a single line of text (for file storage)
    string toFileString() const {
        ostringstream oss;
        oss << accountNumber << "," << name << "," << fixed << setprecision(2) << balance;
        return oss.str();
    }

    // Build an Account object from a stored line of text
    static Account fromFileString(const string &line) {
        stringstream ss(line);
        string accNoStr, nameStr, balStr;

        getline(ss, accNoStr, ',');
        getline(ss, nameStr, ',');
        getline(ss, balStr, ',');

        int accNo = stoi(accNoStr);
        double bal = stod(balStr);

        return Account(accNo, nameStr, bal);
    }
};

// ---------------------------------------------------------
// Bank class: manages all accounts + file persistence
// ---------------------------------------------------------
class Bank {
private:
    vector<Account> accounts;
    const string fileName = "accounts.txt";

    // Load accounts from file into memory
    void loadFromFile() {
        accounts.clear();
        ifstream inFile(fileName);
        if (!inFile) return; // file may not exist yet on first run

        string line;
        while (getline(inFile, line)) {
            if (!line.empty()) {
                accounts.push_back(Account::fromFileString(line));
            }
        }
        inFile.close();
    }

    // Save all in-memory accounts back to file
    void saveToFile() {
        ofstream outFile(fileName);
        for (const auto &acc : accounts) {
            outFile << acc.toFileString() << "\n";
        }
        outFile.close();
    }

    // Find index of account by account number, -1 if not found
    int findAccountIndex(int accNo) {
        for (size_t i = 0; i < accounts.size(); i++) {
            if (accounts[i].getAccountNumber() == accNo) {
                return static_cast<int>(i);
            }
        }
        return -1;
    }

    // Generate the next available account number
    int generateAccountNumber() {
        int maxAcc = 1000; // starting range
        for (const auto &acc : accounts) {
            if (acc.getAccountNumber() >= maxAcc) {
                maxAcc = acc.getAccountNumber() + 1;
            }
        }
        return maxAcc;
    }

public:
    Bank() {
        loadFromFile();
    }

    void createAccount(const string &name, double initialDeposit) {
        int newAccNo = generateAccountNumber();
        Account newAcc(newAccNo, name, initialDeposit);
        accounts.push_back(newAcc);
        saveToFile();
        cout << "\nAccount created successfully! Your Account Number is: "
             << newAccNo << "\n";
    }

    void deposit(int accNo, double amount) {
        int idx = findAccountIndex(accNo);
        if (idx == -1) {
            cout << "\nAccount not found.\n";
            return;
        }
        accounts[idx].deposit(amount);
        saveToFile();
        cout << "\nDeposit successful. New balance: "
             << fixed << setprecision(2) << accounts[idx].getBalance() << "\n";
    }

    void withdraw(int accNo, double amount) {
        int idx = findAccountIndex(accNo);
        if (idx == -1) {
            cout << "\nAccount not found.\n";
            return;
        }
        if (accounts[idx].withdraw(amount)) {
            saveToFile();
            cout << "\nWithdrawal successful. New balance: "
                 << fixed << setprecision(2) << accounts[idx].getBalance() << "\n";
        } else {
            cout << "\nInsufficient balance. Current balance: "
                 << fixed << setprecision(2) << accounts[idx].getBalance() << "\n";
        }
    }

    void checkBalance(int accNo) {
        int idx = findAccountIndex(accNo);
        if (idx == -1) {
            cout << "\nAccount not found.\n";
            return;
        }
        cout << "\nAccount Holder: " << accounts[idx].getName()
             << "\nBalance: " << fixed << setprecision(2)
             << accounts[idx].getBalance() << "\n";
    }

    void displayAllAccounts() {
        if (accounts.empty()) {
            cout << "\nNo accounts found.\n";
            return;
        }
        cout << "\n" << left << setw(15) << "Acc No."
             << setw(20) << "Name" << "Balance\n";
        cout << "---------------------------------------------\n";
        for (const auto &acc : accounts) {
            acc.display();
        }
    }
};

// ---------------------------------------------------------
// Helper: safely read an integer/double from user input
// ---------------------------------------------------------
double readAmount() {
    double amount;
    while (!(cin >> amount) || amount <= 0) {
        cout << "Invalid amount. Please enter a positive number: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    return amount;
}

// ---------------------------------------------------------
// main(): menu-driven interface
// ---------------------------------------------------------
int main() {
    Bank bank;
    int choice;

    cout << "=========================================\n";
    cout << "     WELCOME TO C++ BANK MANAGEMENT      \n";
    cout << "=========================================\n";

    do {
        cout << "\n----------- MAIN MENU -----------\n";
        cout << "1. Create New Account\n";
        cout << "2. Deposit\n";
        cout << "3. Withdraw\n";
        cout << "4. Check Balance\n";
        cout << "5. Display All Accounts\n";
        cout << "6. Exit\n";
        cout << "Enter your choice: ";

        while (!(cin >> choice)) {
            cout << "Invalid input. Enter a number between 1-6: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        switch (choice) {
            case 1: {
                cin.ignore();
                string name;
                cout << "Enter account holder name: ";
                getline(cin, name);
                cout << "Enter initial deposit amount: ";
                double amount = readAmount();
                bank.createAccount(name, amount);
                break;
            }
            case 2: {
                int accNo;
                cout << "Enter account number: ";
                cin >> accNo;
                cout << "Enter deposit amount: ";
                double amount = readAmount();
                bank.deposit(accNo, amount);
                break;
            }
            case 3: {
                int accNo;
                cout << "Enter account number: ";
                cin >> accNo;
                cout << "Enter withdrawal amount: ";
                double amount = readAmount();
                bank.withdraw(accNo, amount);
                break;
            }
            case 4: {
                int accNo;
                cout << "Enter account number: ";
                cin >> accNo;
                bank.checkBalance(accNo);
                break;
            }
            case 5: {
                bank.displayAllAccounts();
                break;
            }
            case 6: {
                cout << "\nThank you for using the Bank Management App. Goodbye!\n";
                break;
            }
            default: {
                cout << "\nInvalid choice. Please select between 1-6.\n";
            }
        }

    } while (choice != 6);

    return 0;
}
