// ============================================================
//  SMART FARM MANAGEMENT SYSTEM
// ============================================================

// ------------------------------------------------------------
// BLOCK 1: HEADER FILES
// ------------------------------------------------------------
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>

using namespace std;

// ------------------------------------------------------------
// BLOCK 2: CUSTOM EXCEPTION CLASS
// Thrown when area / yield / other input is invalid
// ------------------------------------------------------------
class FarmException : public runtime_error {
public:
    explicit FarmException(const string& msg) : runtime_error(msg) {}
};

// ------------------------------------------------------------
// BLOCK 3: ABSTRACT CLASS  Advisory
// Contains a pure virtual function giveAdvice()
// ------------------------------------------------------------
class Advisory {
public:
    virtual void giveAdvice() const = 0;   // pure virtual function
    virtual ~Advisory() {}                 // virtual destructor
};

// ------------------------------------------------------------
// BLOCK 4: BASE CLASS  Crop
// Attributes: name, area, season, yieldPerAcre
// Includes: constructor overloading, virtual destructor,
//           static member, operator overloading
// ------------------------------------------------------------
class Crop {
protected:                       // accessible to derived classes
    string name;
    double area;                 // in acres
    string season;
    double yieldPerAcre;         // quintals per acre

    static int cropCount;        // static member: number of Crop objects alive

    // Validation helper: throws exception on invalid input
    static void validate(double area, double yieldPerAcre) {
        if (area <= 0)
            throw FarmException("Invalid area: must be greater than 0.");
        if (yieldPerAcre <= 0)
            throw FarmException("Invalid yield per acre: must be greater than 0.");
    }

public:
    // ---- Constructor overloading ----
    // 1) Default constructor
    Crop() : name("Unknown"), area(0), season("Not set"), yieldPerAcre(0) {
        cropCount++;
    }

    // 2) Constructor without season
    Crop(const string& n, double a, double y)
        : name(n), area(a), season("Not set"), yieldPerAcre(y) {
        validate(a, y);
        cropCount++;             // incremented only after validation passes
    }

    // 3) Full constructor
    Crop(const string& n, double a, const string& s, double y)
        : name(n), area(a), season(s), yieldPerAcre(y) {
        validate(a, y);
        cropCount++;
    }

    // Copy constructor (keeps the static count correct)
    Crop(const Crop& other)
        : name(other.name), area(other.area),
          season(other.season), yieldPerAcre(other.yieldPerAcre) {
        cropCount++;
    }

    // ---- Virtual destructor ----
    virtual ~Crop() { cropCount--; }

    // ---- Getters ----
    string getName()   const { return name; }
    double getArea()   const { return area; }
    string getSeason() const { return season; }
    double getYield()  const { return yieldPerAcre; }

    // ---- Static member function ----
    static int getCropCount() { return cropCount; }

    // ---- Virtual functions (runtime polymorphism) ----
    virtual double calculateTotalYield() const {
        return area * yieldPerAcre;
    }

    virtual void display() const {
        cout << left  << setw(12) << name
             << right << setw(8)  << fixed << setprecision(2) << area
             << "  " << left << setw(10) << season
             << right << setw(8)  << yieldPerAcre
             << setw(12) << calculateTotalYield();
    }

    // Converts the object to one line of text for file saving
    virtual string toRecord() const {
        ostringstream os;
        os << "B|" << name << "|" << area << "|" << season << "|" << yieldPerAcre;
        return os.str();
    }

    // ---- Operator overloading: compare crops by total yield ----
    bool operator>(const Crop& other) const {
        return calculateTotalYield() > other.calculateTotalYield();
    }
    bool operator<(const Crop& other) const {
        return calculateTotalYield() < other.calculateTotalYield();
    }
    bool operator==(const Crop& other) const {
        return calculateTotalYield() == other.calculateTotalYield();
    }
};

// Definition + initialization of the static data member
int Crop::cropCount = 0;

// ------------------------------------------------------------
// BLOCK 5: DERIVED CLASS  CerealCrop
// Extra members: fertilizerType, irrigationFrequency
// Also implements Advisory (multiple inheritance)
// ------------------------------------------------------------
class CerealCrop : public Crop, public Advisory {
private:
    string fertilizerType;
    int irrigationFrequency;     // times per month

public:
    // Constructor overloading
    CerealCrop() : Crop(), fertilizerType("None"), irrigationFrequency(0) {}

    CerealCrop(const string& n, double a, double y)
        : Crop(n, a, y), fertilizerType("Urea"), irrigationFrequency(4) {}

    CerealCrop(const string& n, double a, const string& s, double y,
               const string& fert, int irrigation)
        : Crop(n, a, s, y), fertilizerType(fert), irrigationFrequency(irrigation) {
        if (irrigation < 0)
            throw FarmException("Invalid irrigation frequency: cannot be negative.");
    }

    ~CerealCrop() override {}

    // Runtime polymorphism: cereal-specific yield (5% harvest loss assumed)
    double calculateTotalYield() const override {
        return area * yieldPerAcre * 0.95;
    }

    void display() const override {
        Crop::display();
        cout << "  [Cereal] Fertilizer: " << fertilizerType
             << ", Irrigation: " << irrigationFrequency << "x/month\n";
    }

    string toRecord() const override {
        ostringstream os;
        os << "C|" << name << "|" << area << "|" << season << "|" << yieldPerAcre
           << "|" << fertilizerType << "|" << irrigationFrequency;
        return os.str();
    }

    // Implementation of the pure virtual function from Advisory
    void giveAdvice() const override {
        cout << "Advice for " << name << " (Cereal): Apply " << fertilizerType
             << " in split doses, irrigate " << irrigationFrequency
             << " times a month, and harvest when grains turn golden.\n";
    }
};

// ------------------------------------------------------------
// BLOCK 6: DERIVED CLASS  VegetableCrop
// Extra members: soilType, harvestDays
// Also implements Advisory (multiple inheritance)
// ------------------------------------------------------------
class VegetableCrop : public Crop, public Advisory {
private:
    string soilType;
    int harvestDays;             // days from sowing to harvest

public:
    // Constructor overloading
    VegetableCrop() : Crop(), soilType("Loam"), harvestDays(0) {}

    VegetableCrop(const string& n, double a, double y)
        : Crop(n, a, y), soilType("Loam"), harvestDays(60) {}

    VegetableCrop(const string& n, double a, const string& s, double y,
                  const string& soil, int days)
        : Crop(n, a, s, y), soilType(soil), harvestDays(days) {
        if (days <= 0)
            throw FarmException("Invalid harvest days: must be greater than 0.");
    }

    ~VegetableCrop() override {}

    // Runtime polymorphism: vegetables assumed 10% wastage
    double calculateTotalYield() const override {
        return area * yieldPerAcre * 0.90;
    }

    void display() const override {
        Crop::display();
        cout << "  [Vegetable] Soil: " << soilType
             << ", Harvest in: " << harvestDays << " days\n";
    }

    string toRecord() const override {
        ostringstream os;
        os << "V|" << name << "|" << area << "|" << season << "|" << yieldPerAcre
           << "|" << soilType << "|" << harvestDays;
        return os.str();
    }

    // Implementation of the pure virtual function from Advisory
    void giveAdvice() const override {
        cout << "Advice for " << name << " (Vegetable): Use " << soilType
             << " soil, water lightly every day, use organic compost, and harvest after about "
             << harvestDays << " days.\n";
    }
};

// ------------------------------------------------------------
// BLOCK 7: FILE HANDLING FUNCTIONS
// Save crop data to a text file and retrieve it back
// ------------------------------------------------------------
void saveToFile(const vector<unique_ptr<Crop>>& farm, const string& filename) {
    ofstream out(filename);
    if (!out)
        throw FarmException("Cannot open file '" + filename + "' for writing.");

    for (const auto& c : farm)
        out << c->toRecord() << "\n";

    cout << farm.size() << " crop record(s) saved to " << filename << "\n";
}

void loadFromFile(vector<unique_ptr<Crop>>& farm, const string& filename) {
    ifstream in(filename);
    if (!in)
        throw FarmException("Cannot open file '" + filename + "' for reading.");

    vector<unique_ptr<Crop>> loaded;   // load into a temp list first
    string line;
    int lineNo = 0;

    while (getline(in, line)) {
        lineNo++;
        if (line.empty()) continue;

        // split the line using '|'
        vector<string> f;
        string token;
        istringstream ss(line);
        while (getline(ss, token, '|')) f.push_back(token);

        try {
            if (f[0] == "C" && f.size() == 7) {
                loaded.push_back(make_unique<CerealCrop>(
                    f[1], stod(f[2]), f[3], stod(f[4]), f[5], stoi(f[6])));
            } else if (f[0] == "V" && f.size() == 7) {
                loaded.push_back(make_unique<VegetableCrop>(
                    f[1], stod(f[2]), f[3], stod(f[4]), f[5], stoi(f[6])));
            } else {
                throw FarmException("Unrecognised record format.");
            }
        } catch (const invalid_argument&) {
            throw FarmException("Corrupt number on line " + to_string(lineNo) + ".");
        }
    }

    farm = move(loaded);   // replace old data (old objects are destroyed)
    cout << farm.size() << " crop record(s) loaded from " << filename << "\n";
}

// ------------------------------------------------------------
// BLOCK 8: INPUT HELPER FUNCTIONS
// Read validated numbers; throw exceptions on bad input
// ------------------------------------------------------------
string readString(const string& prompt) {
    string s;
    cout << prompt;
    getline(cin, s);
    if (s.empty() || s.find('|') != string::npos)
        throw FarmException("Text cannot be empty or contain '|'.");
    return s;
}

double readDouble(const string& prompt) {
    string s;
    cout << prompt;
    getline(cin, s);
    try {
        size_t pos;
        double v = stod(s, &pos);
        if (pos != s.size()) throw invalid_argument("extra chars");
        return v;
    } catch (const exception&) {
        throw FarmException("'" + s + "' is not a valid number.");
    }
}

int readInt(const string& prompt) {
    string s;
    cout << prompt;
    getline(cin, s);
    try {
        size_t pos;
        int v = stoi(s, &pos);
        if (pos != s.size()) throw invalid_argument("extra chars");
        return v;
    } catch (const exception&) {
        throw FarmException("'" + s + "' is not a valid whole number.");
    }
}

// ------------------------------------------------------------
// BLOCK 9: DISPLAY HELPERS
// Table header, listing and total-yield report
// ------------------------------------------------------------
void printHeader() {
    cout << "\n" << left << setw(12) << "Name"
         << right << setw(8) << "Area"
         << "  " << left << setw(10) << "Season"
         << right << setw(8) << "Yld/Acre"
         << setw(12) << "TotalYield" << "  Details\n";
    cout << string(80, '-') << "\n";
}

void displayAll(const vector<unique_ptr<Crop>>& farm) {
    if (farm.empty()) {
        cout << "No crops recorded yet.\n";
        return;
    }
    printHeader();
    double grand = 0;
    for (const auto& c : farm) {
        c->display();                      // runtime polymorphism
        grand += c->calculateTotalYield(); // runtime polymorphism
    }
    cout << string(80, '-') << "\n";
    cout << "Grand total yield: " << fixed << setprecision(2) << grand << " quintals\n";
    cout << "Total crops (static count): " << Crop::getCropCount() << "\n";
}

// ------------------------------------------------------------
// BLOCK 10: MAIN FUNCTION (MENU DRIVEN)
// ------------------------------------------------------------
int main() {
    vector<unique_ptr<Crop>> farm;
    const string FILE_NAME = "crops.txt";
    int choice = -1;

    cout << "===== SMART FARM MANAGEMENT SYSTEM =====\n";

    do {
        cout << "\n1. Add Cereal Crop"
             << "\n2. Add Vegetable Crop"
             << "\n3. Display All Crops & Total Yield"
             << "\n4. Compare Two Crops (operator overloading)"
             << "\n5. Show Farming Advice"
             << "\n6. Save to File"
             << "\n7. Load from File"
             << "\n0. Exit"
             << "\n";

        // ---- try / catch around every menu action ----
        try {
            choice = readInt("Enter choice: ");

            switch (choice) {

            // -------- Add Cereal Crop --------
            case 1: {
                string n = readString("Crop name: ");
                double a = readDouble("Area (acres): ");
                string s = readString("Season: ");
                double y = readDouble("Yield per acre (quintals): ");
                string f = readString("Fertilizer type: ");
                int irr  = readInt("Irrigation frequency (times/month): ");
                farm.push_back(make_unique<CerealCrop>(n, a, s, y, f, irr));
                cout << "Cereal crop added.\n";
                break;
            }

            // -------- Add Vegetable Crop --------
            case 2: {
                string n = readString("Crop name: ");
                double a = readDouble("Area (acres): ");
                string s = readString("Season: ");
                double y = readDouble("Yield per acre (quintals): ");
                string soil = readString("Soil type: ");
                int days = readInt("Days to harvest: ");
                farm.push_back(make_unique<VegetableCrop>(n, a, s, y, soil, days));
                cout << "Vegetable crop added.\n";
                break;
            }

            // -------- Display --------
            case 3:
                displayAll(farm);
                break;

            // -------- Compare two crops --------
            case 4: {
                if (farm.size() < 2)
                    throw FarmException("Add at least two crops to compare.");
                int i = readInt("First crop number (1-" + to_string(farm.size()) + "): ");
                int j = readInt("Second crop number (1-" + to_string(farm.size()) + "): ");
                if (i < 1 || j < 1 || i > (int)farm.size() || j > (int)farm.size())
                    throw FarmException("Crop number out of range.");

                const Crop& c1 = *farm[i - 1];
                const Crop& c2 = *farm[j - 1];

                if (c1 > c2)
                    cout << c1.getName() << " has a HIGHER total yield than " << c2.getName() << ".\n";
                else if (c1 < c2)
                    cout << c1.getName() << " has a LOWER total yield than " << c2.getName() << ".\n";
                else
                    cout << c1.getName() << " and " << c2.getName() << " have EQUAL total yield.\n";
                break;
            }

            // -------- Advice (via Advisory pointer) --------
            case 5: {
                if (farm.empty())
                    throw FarmException("No crops available.");
                for (const auto& c : farm) {
                    const Advisory* adv = dynamic_cast<const Advisory*>(c.get());
                    if (adv) adv->giveAdvice();
                }
                break;
            }

            // -------- Save --------
            case 6:
                saveToFile(farm, FILE_NAME);
                break;

            // -------- Load --------
            case 7:
                loadFromFile(farm, FILE_NAME);
                break;

            case 0:
                cout << "Goodbye!\n";
                break;

            default:
                throw FarmException("Invalid menu choice.");
            }
        }
        catch (const FarmException& e) {          // our own errors
            cout << "Error: " << e.what() << "\n";
        }
        catch (const exception& e) {              // any other standard error
            cout << "Unexpected error: " << e.what() << "\n";
        }

    } while (choice != 0 && cin);   // stop on exit choice or closed input

    return 0;
}
