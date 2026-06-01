#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
using namespace std;

class Person {
protected:
    string name;
    int age;

public:
    Person() : name(""), age(0) {}
    Person(string n, int a) : name(n), age(a) {}
    virtual ~Person() {}
    string getName() const { return name; }
    int getAge() const { return age; }

    virtual void display() const = 0;
};

class Employee : public Person {
protected:
    int id;
    double salary;

public:
    Employee() : id(0), salary(0) {}
    Employee(int i, string n, int a, double s)
        : Person(n, a), id(i), salary(s) {}
    virtual ~Employee() {}

    int getID() const { return id; }
    double getSalary() const { return salary; }

    virtual double calculateBonus() const = 0;
    virtual string getType() const = 0;

    virtual void display() const override {
        cout << "ID: " << id
             << " | Name: " << name
             << " | Age: " << age
             << " | Salary: " << salary
             << " | Bonus: " << calculateBonus();
    }
};

class Manager : public Employee {
    int teamSize;

public:
    Manager() : teamSize(0) {}
    Manager(int i, string n, int a, double s, int t)
        : Employee(i, n, a, s), teamSize(t) {}

    double calculateBonus() const override { return 1000 * teamSize; }
    string getType() const override { return "Manager"; }

    void display() const override {
        cout << "[Manager] ";
        Employee::display();
        cout << " | Team Size: " << teamSize << endl;
    }

    int getTeamSize() const { return teamSize; }
};

class Engineer : public Employee {
    int projectCount;

public:
    Engineer() : projectCount(0) {}
    Engineer(int i, string n, int a, double s, int p)
        : Employee(i, n, a, s), projectCount(p) {}

    double calculateBonus() const override { return 1000 * projectCount; }
    string getType() const override { return "Engineer"; }

    void display() const override {
        cout << "[Engineer] ";
        Employee::display();
        cout << " | Projects: " << projectCount << endl;
    }

    int getProjectCount() const { return projectCount; }
};

class SimpleEmployee : public Employee {
public:
    SimpleEmployee(int i, string n, int a, double s)
        : Employee(i, n, a, s) {}

    double calculateBonus() const override { return 1000; }
    string getType() const override { return "Employee"; }

    void display() const override {
        cout << "[Employee] ";
        Employee::display();
        cout << endl;
    }
};

class AdminSystem {
    vector<Employee*> employees;
    string username = "admin";
    string password = "1234";
    const string filename = "employees.txt";

public:
    AdminSystem() { loadFromFile(); }
    ~AdminSystem() {
        saveToFile();
        for (auto e : employees) delete e;
    }

    bool login() {
        string u, p;
        cout << "=========================\n";
        cout << "        Admin Login      \n";
        cout << "=========================\n";
        cout << "Username: "; cin >> u;
        cout << "Password: "; cin >> p;
        return u == username && p == password;
    }

    void addEmployee() {
        int id, age, type;
        double salary;
        string name;
        cout << "Enter ID: "; cin >> id; cin.ignore();
        cout << "Enter Name: "; getline(cin, name);
        cout << "Enter Age: "; cin >> age;
        cout << "Enter Salary: "; cin >> salary;
        cout << "Select Role:\n1. Manager\n2. Engineer\n3. Normal Employee\nChoice: "; cin >> type;

        if (type == 1) {
            int team; cout << "Enter Team Size: "; cin >> team;
            employees.push_back(new Manager(id, name, age, salary, team));
        } else if (type == 2) {
            int proj; cout << "Enter Project Count: "; cin >> proj;
            employees.push_back(new Engineer(id, name, age, salary, proj));
        }
        else if (type == 3) {
            employees.push_back(new SimpleEmployee(id, name, age, salary));
        }
        else cout << "Invalid type.\n"; 
    }

    void removeEmployee() {
        int id; cout << "Enter ID to remove: "; cin >> id;
        for (size_t i = 0; i < employees.size(); i++) {
            if (employees[i]->getID() == id) {
                delete employees[i];
                employees.erase(employees.begin() + i);
                cout << "Employee removed.\n"; return;
            }
        }
        cout << "Employee not found.\n";
    }

    void searchEmployee() {
        int id; cout << "Enter ID to search: "; cin >> id;
        for (auto e : employees) {
            if (e->getID() == id) { e->display(); return; }
        }
        cout << "Employee not found.\n";
    }

    void listEmployees() {
        if (employees.empty()) {
            cout << "No employees found.\n";
            return;
        }
        for (auto e : employees) {
            cout << "--------------------------\n";
            e->display();
        }
    }

   // Save function
void saveToFile() {
    ofstream out(filename);
    if (!out) {
        cout << "Error opening file for writing: " << filename << endl;
        return;
    }

    for (auto e : employees) {
        string type = e->getType();
        out << type << ";"
            << e->getID() << ";"
            << e->getName() << ";"
            << e->getAge() << ";"
            << e->getSalary();

        if (type == "Manager") {
            Manager* m = dynamic_cast<Manager*>(e);
            if (m) out << ";" << m->getTeamSize();
        } 
        else if (type == "Engineer") {
            Engineer* eng = dynamic_cast<Engineer*>(e);
            if (eng) out << ";" << eng->getProjectCount();
        }

        out << "\n"; // new line per employee
    }
}

// Load function
void loadFromFile() {
    ifstream in(filename);
    if (!in) {
        cout << "File not found, starting fresh." << endl;
        return;
    }

    string line;
    while (getline(in, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        string type, idStr, name, ageStr, salaryStr, extraStr;

        // Mandatory fields
        if (!getline(ss, type, ';')) continue;
        if (!getline(ss, idStr, ';')) continue;
        if (!getline(ss, name, ';')) continue;
        if (!getline(ss, ageStr, ';')) continue;
        if (!getline(ss, salaryStr, ';')) continue;

        try {
            int id = stoi(idStr);
            int age = stoi(ageStr);
            double salary = stod(salaryStr);

            if (type == "Manager") {
                if (!getline(ss, extraStr, ';')) continue;
                int teamSize = stoi(extraStr);
                employees.push_back(new Manager(id, name, age, salary, teamSize));
            } 
            else if (type == "Engineer") {
                if (!getline(ss, extraStr, ';')) continue;
                int projectCount = stoi(extraStr);
                employees.push_back(new Engineer(id, name, age, salary, projectCount));
            } 
            else if (type == "Employee") {
                employees.push_back(new SimpleEmployee(id, name, age, salary));
            }
        } 
        catch (const std::exception& e) {
            cout << "Skipping malformed line: " << line << endl;
            continue;
        }
    }

    cout << "Loaded " << employees.size() << " employees from file." << endl;
}

    void menu() {
    char repeatMenu;
    do {
        system("cls");
        int choice;
        cout << "\n--- Admin Menu ---\n";
        cout << "1. Add Employee\n";
        cout << "2. Remove Employee\n";
        cout << "3. Search Employee\n";
        cout << "4. List All Employees\n";
        cout << "5. Exit\n";
        cout << "Choice: ";
        cin >> choice;

        switch (choice) {
            case 1: addEmployee(); break;
            case 2: removeEmployee(); break;
            case 3: searchEmployee(); break;
            case 4: listEmployees(); break;
            case 5: return;  // Exit the program
            default: cout << "Invalid choice.\n";
        }

        cout << "\nDo you want to return to the main menu? (y/n): ";
        cin >> repeatMenu;
    } while (repeatMenu == 'y' || repeatMenu == 'Y');
}
};
int main() {
    cout << "                             ================================\n";
    cout << "                                Employee Management System   \n";
    cout << "                             ================================\n\n\n";
    AdminSystem system;
    if (!system.login()) { cout << "Login failed.\n"; return 0; }
    cout << "Login successful!\n";
    system.menu();
    return 0;
}