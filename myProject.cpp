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

TreeNode* root = nullptr;
string filename = "random_data.csv";

Patient addPatient() 
{ 
    setColor(3);
    Patient p;
    cin.ignore(); 
    cout << "Name: ";
    getline(cin, p.name);

    cout << "Age: ";
    while (!(cin>>p.age))
    {
        setColor(4);
        cout<<"\nPLZ ENTER A VALID NUMBER: ";
        cin.clear();
        cin.ignore(100, '\n');
    }
    setColor(3);
    cout << "Glucose Level: ";
    while (!(cin>>p.glucoseLevel))
    {
        setColor(4);
        cout<<"\nPLZ ENTER A VALID NUMBER: ";
        cin.clear();
        cin.ignore(100, '\n');
    }
    setColor(3);
    cout << "Weight: ";
    while (!(cin>>p.weight))
    {
        setColor(4);
        cout<<"\nPLZ ENTER A VALID NUMBER: ";
        cin.clear();
        cin.ignore(100, '\n');
    }
    setColor(8);
    int type;
    cout << "==== Type ====\n";
    cout << "1. Fasting\n";
    cout << "2. After Eating\n";
    cout << "3. (2-3) Hours After Eating\n";
    cout << "Enter type: ";
    cin >> type;

    switch (type) {
        case 1: // Fasting
            if (p.glucoseLevel < 100)
                p.status = "Healthy";
            else if (p.glucoseLevel <= 125)
                p.status = "Pre-diabetic";
            else
                p.status = "Diabetic";
            break;
        case 2: // After Eating
            if (p.glucoseLevel < 200)
                p.status = "Healthy";
            else if (p.glucoseLevel <= 230)
                p.status = "Pre-diabetic";
            else
                p.status = "Diabetic";
            break;
        case 3: // 2 - 3 Hours After Eating
            if (p.glucoseLevel < 140)
                p.status = "Healthy";
            else if (p.glucoseLevel <= 160)
                p.status = "Pre-diabetic";
            else
                p.status = "Diabetic";
            break;
        default:
            cout << "Invalid choice. Setting status as Unknown.\n";
            p.status = "Unknown";
    }

    return p;
}

// وظيفة لإضافة مريض إلى شجرة
TreeNode* insertPatientToBST(TreeNode* root, Patient p) 
{
    if (root == nullptr) 
    {
        return new TreeNode(p);
    }

    if (p.glucoseLevel < root->patient.glucoseLevel) {
        root->left = insertPatientToBST(root->left, p);
    } 
    else 
    {
        root->right = insertPatientToBST(root->right, p);
    }

    return root;
}

// وظيفة لعرض جميع المرضى
void displayPatients(TreeNode* root) 
{
    if (root == nullptr) return;

    displayPatients(root->left);
    cout<<"Name: " << root->patient.name;
    cout<<" | Age: " << root->patient.age;
    cout<<" | Weight: " << root->patient.weight;
    cout<<" | Glucose Level: " << root->patient.glucoseLevel;
    cout<<" | Status: " << root->patient.status;
    cout<< endl;
    displayPatients(root->right);
}

// تحميل المرضى من ملف
void loadPatientsFromFile(TreeNode*& root, const string& filename) 
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

// حفظ المرضى إلى ملف
void savePatientsToFile(TreeNode* root, ofstream& file) 
{
    if (root == nullptr) return;

    savePatientsToFile(root->left, file);
    file << root->patient.name << ","
        << root->patient.age << ","
        << root->patient.glucoseLevel << ","
        << root->patient.weight << ","
        << root->patient.status 
        << endl;
    savePatientsToFile(root->right, file);
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

    // ابحث في الفروع اليسرى واليمنى
    TreeNode* foundInLeft = searchPatientByName(root->left, name);
    if (foundInLeft) 
    {
        return foundInLeft;
    }

    return searchPatientByName(root->right, name);
}

void choice(TreeNode*& root) 
{
    
    int userChoice;

    while (true) 
    {
        setColor(6);
        cout << "\n========  Main Menu  ========" << "\n";
        setColor(11);
        cout << "\n1. Add Patient\n";
        cout << "2. Display Patients\n";
        cout << "3. Search Patient\n";
        cout << "4. Exit\n";
        cout << "Enter your choice: ";
        cin >> userChoice;
        setColor(7);
        switch (userChoice) 
        {
            case 1:
            {
                Patient newPatient = addPatient();
                root = insertPatientToBST(root, newPatient);

                ofstream file(filename, ios::app);
                if (file.is_open()) 
                {
                    file<< newPatient.name << ","
                        << newPatient.age << ","
                        << newPatient.glucoseLevel << ","
                        << newPatient.weight << ","
                        << newPatient.status 
                        << endl;
                    file.close();
                } 
                else 
                {
                    cerr << "Error saving patient data to file.\n";
                }
                cout << "Patient " << newPatient.name << " added.\n";
                break;
            }
            case 2:
            {
                cout << "\nPatients:\n";
                displayPatients(root);
                break;
            }
            case 3: 
            {
                cin.ignore();
                string searchName;
                cout << "Enter the name of the patient to search: ";
                getline(cin, searchName);

                TreeNode* foundPatient = searchPatientByName(root, searchName);
                if (foundPatient) 
                {
                    setColor(2);
                    cout<< "Patient found:\n";
                    setColor(7);
                    cout<< "Name: " << foundPatient->patient.name
                        << " | Age: " << foundPatient->patient.age
                        << " | Glucose Level: " << foundPatient->patient.glucoseLevel
                        << " | Weight: " << foundPatient->patient.weight
                        << " | Status: " << foundPatient->patient.status << endl;
                } else 
                {
                    setColor(4);
                    cout << "Patient not found.\n";
                }
                break;
            }
            case 4:
            {
                setColor(2);
                cout << "\nExiting the program...\n";
                return;
            }
            default:
                setColor(4);
                cout << "\n!Invalid choice. Please try again.\n";
        }
    }
}

int main() 
{
    loadPatientsFromFile(root, filename);
    choice(root);

    return 0;
}
