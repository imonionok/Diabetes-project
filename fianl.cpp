#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <windows.h>

using namespace std;

void setColor(int color) 
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

class PatientManagement 
{
private:
    struct Patient 
    {
        string name;
        int age;
        double glucoseLevel;
        double weight;
        string status; // Healthy, Pre-diabetic, Diabetic
    };

    struct TreeNode 
    {
        Patient patient;
        TreeNode* left;
        TreeNode* right;

        TreeNode(Patient p) : patient(p), left(nullptr), right(nullptr) {}
    };

    TreeNode* root;
    string filename;

    TreeNode* insertPatientToBST(TreeNode* root, Patient p) 
    {
        if (root == nullptr) 
        {
            return new TreeNode(p);
        }

        if (p.glucoseLevel < root->patient.glucoseLevel) 
        {
            root->left = insertPatientToBST(root->left, p);
        } 
        else 
        {
            root->right = insertPatientToBST(root->right, p);
        }

        return root;
    }

    void displayPatients(TreeNode* root) 
    {
        if (root == nullptr) return;
        setColor(7);
        displayPatients(root->left);
        cout << "Name: " << root->patient.name;
        cout << " | Age: " << root->patient.age;
        cout << " | Weight: " << root->patient.weight;
        cout << " | Glucose Level: " << root->patient.glucoseLevel;
        cout << " | Status: " << root->patient.status;
        cout << endl;
        displayPatients(root->right);
    }

    void loadPatientsFromFile() 
    {
        ifstream file(filename);
        if (!file.is_open()) 
        {
            cerr << "Could not open file: " << filename << endl;
            return;
        }

        string line;
        while (getline(file, line)) 
        {
            stringstream ss(line);
            Patient p;
            string glucoseStr, weightStr;

            getline(ss, p.name, ',');
            ss >> p.age;
            ss.ignore(1, ',');
            getline(ss, weightStr, ',');
            getline(ss, glucoseStr, ',');
            getline(ss, p.status, ',');

            try 
            {
                p.weight = stod(weightStr);
                p.glucoseLevel = stod(glucoseStr);
            } 
            catch (const invalid_argument& e) 
            {
                cerr << "Invalid data format in line: " << line << endl;
                continue;
            }

            root = insertPatientToBST(root, p);
        }
        file.close();
    }

    TreeNode* searchPatientByName(TreeNode* root, const string& name) 
    {
        if (root == nullptr) 
        {
            return nullptr;
        }

        if (root->patient.name == name) 
        {
            return root;
        }

        TreeNode* foundInLeft = searchPatientByName(root->left, name);
        if (foundInLeft) 
        {
            return foundInLeft;
        }

        return searchPatientByName(root->right, name);
    }

public:
    PatientManagement(string file) : root(nullptr), filename(file) 
    {
        loadPatientsFromFile();
    }

    Patient addPatient() 
    {
        setColor(9);
        Patient p;
        cin.ignore();
        cout << "Name: ";
        getline(cin, p.name);
        cout << "Age: ";
        while (!(cin >> p.age)) 
        {
            setColor(4);
            cout << "PLZ ENTER A VALID NUMBER: ";
            cin.clear();
            cin.ignore(100, '\n');
        }
        setColor(9);
        cout << "Glucose Level: ";
        while (!(cin >> p.glucoseLevel)) 
        {
            setColor(4);
            cout << "PLZ ENTER A VALID NUMBER: ";
            cin.clear();
            cin.ignore(100, '\n');
        }
        setColor(9);
        cout << "Weight: ";
        while (!(cin >> p.weight)) 
        {
            setColor(4);
            cout << "PLZ ENTER A VALID NUMBER: ";
            cin.clear();
            cin.ignore(100, '\n');
        }
        int type;
        setColor(7);
        cout << "\n==== Type ====\n\n";
        setColor(8);
        cout << "1. Fasting\n";
        cout << "2. After Eating\n";
        cout << "3. (2-3) Hours After Eating\n";
        cout << "Enter type: ";
        cin >> type;

        switch (type) 
        {
            case 1:
            {
                p.status = (p.glucoseLevel < 100) ? "Healthy" : (p.glucoseLevel <= 125) ? "Pre-diabetic" : "Diabetic";
                break;
            }
            case 2:
            {
                p.status = (p.glucoseLevel < 200) ? "Healthy" : (p.glucoseLevel <= 230) ? "Pre-diabetic" : "Diabetic";
                break;
            }
            case 3:
            {
                p.status = (p.glucoseLevel < 140) ? "Healthy" : (p.glucoseLevel <= 160) ? "Pre-diabetic" : "Diabetic";
                break;
            }
            default:
                p.status = "Unknown";
        }

        return p;
    }

    void addPatientToSystem() 
    {
        Patient newPatient = addPatient();
        root = insertPatientToBST(root, newPatient);

        ofstream file(filename, ios::app);
        if (file.is_open()) 
        {
            file<< newPatient.name << "," << newPatient.age << "," << newPatient.glucoseLevel 
            << "," << newPatient.weight << ","<< newPatient.status << endl;
            file.close();
        } 
        else 
        {
            cerr << "Error saving patient data to file.\n";
        }
        cout << "Patient " << newPatient.name << " added.\n";
    }

    void displayAllPatients() 
    {
        setColor(2);
        cout << "\nPatients:\n";
        displayPatients(root);
    }

    void searchPatient() 
    {
        cin.ignore();
        string searchName;
        cout << "Enter the name of the patient to search: ";
        getline(cin, searchName);

        TreeNode* foundPatient = searchPatientByName(root, searchName);
        if (foundPatient) 
        {
            setColor(2);
            cout << "Patient found:\n";
            setColor(7);
            cout << "Name: " << foundPatient->patient.name
                << " | Age: " << foundPatient->patient.age
                << " | Glucose Level: " << foundPatient->patient.glucoseLevel
                << " | Weight: " << foundPatient->patient.weight
                << " | Status: " << foundPatient->patient.status;
            cout<< endl;
        } else {
            setColor(4);
            cout << "Patient not found.\n";
        }
    }

    void menu() 
    {
        int userChoice;

        while (true) 
        {
            setColor(6);
            cout << "\n========  Main Menu  ========" << "\n\n";
            setColor(11);
            cout << "1. Add Patient\n";
            cout << "2. Display Patients\n";
            cout << "3. Search Patient\n";
            cout << "4. Exit\n";
            cout << "Enter your choice: ";
            cin >> userChoice;

            switch (userChoice) {
                case 1:
                    addPatientToSystem();
                    break;
                case 2:
                    displayAllPatients();
                    break;
                case 3:
                    searchPatient();
                    break;
                case 4:
                    setColor(2);
                    cout << "Exiting program...\n";
                    return;
                default:
                    setColor(4);
                    cout << "Invalid choice. Please try again.\n";
            }
        }
    }
};

int main() 
{
    PatientManagement pm("random_data.csv");
    pm.menu();
    return 0;
}
