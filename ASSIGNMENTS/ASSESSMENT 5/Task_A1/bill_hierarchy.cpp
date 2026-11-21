// Task A1 – Design the Polymorphic Bill Hierarchy
// Demonstrates: abstract base class, pure virtual functions, runtime polymorphism
// Compile: g++ -std=c++17 -Wall -Wextra -o bill_hierarchy bill_hierarchy.cpp
// Run:     ./bill_hierarchy

#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <iomanip>

// ─── Abstract Base Class ────────────────────────────────────────────────────
class Bill {
protected:
    std::string patientName;
    std::string billId;

public:
    Bill(const std::string& name, const std::string& id)
        : patientName(name), billId(id) {}

    // Pure virtual functions – every concrete bill MUST implement these
    virtual double total()       const = 0;   // <<pure virtual>>
    virtual std::string type()   const = 0;   // <<pure virtual>>
    virtual void print()         const = 0;   // <<pure virtual>>

    // Non-pure virtual destructor – essential for correct cleanup via base ptr
    virtual ~Bill() {
        std::cout << "[~Bill: " << billId << "]\n";
    }

    // Concrete helper available to all subclasses
    const std::string& getPatient() const { return patientName; }
    const std::string& getId()      const { return billId; }
};

// ─── OutPatientBill ──────────────────────────────────────────────────────────
class OutPatientBill : public Bill {
    double consultationFee;
    double testCharges;

public:
    OutPatientBill(const std::string& name, const std::string& id,
                   double consult, double tests)
        : Bill(name, id), consultationFee(consult), testCharges(tests) {}

    double total() const override {                          // <<overridden>>
        return consultationFee + testCharges;
    }
    std::string type() const override { return "OutPatient"; }  // <<overridden>>
    void print() const override {                               // <<overridden>>
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "[" << type() << "] Patient: " << patientName
                  << " | ID: " << billId
                  << " | Consult: " << consultationFee
                  << " | Tests: "   << testCharges
                  << " | TOTAL: "   << total() << "\n";
    }
    ~OutPatientBill() override { std::cout << "[~OutPatientBill]\n"; }
};

// ─── InPatientBill ───────────────────────────────────────────────────────────
class InPatientBill : public Bill {
    int    days;
    double roomRatePerDay;
    double procedureCharges;
    double medicineCharges;

public:
    InPatientBill(const std::string& name, const std::string& id,
                  int d, double rate, double proc, double meds)
        : Bill(name, id), days(d), roomRatePerDay(rate),
          procedureCharges(proc), medicineCharges(meds) {}

    double total() const override {
        return days * roomRatePerDay + procedureCharges + medicineCharges;
    }
    std::string type() const override { return "InPatient"; }
    void print() const override {
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "[" << type() << "] Patient: " << patientName
                  << " | ID: " << billId
                  << " | Days: " << days
                  << " | Room/day: " << roomRatePerDay
                  << " | Procedures: " << procedureCharges
                  << " | Meds: " << medicineCharges
                  << " | TOTAL: " << total() << "\n";
    }
    ~InPatientBill() override { std::cout << "[~InPatientBill]\n"; }
};

// ─── DayCareSurgeryBill ──────────────────────────────────────────────────────
class DayCareSurgeryBill : public Bill {
    double packagePrice;

public:
    DayCareSurgeryBill(const std::string& name, const std::string& id,
                       double pkg)
        : Bill(name, id), packagePrice(pkg) {}

    double total() const override { return packagePrice; }
    std::string type() const override { return "DayCareSurgery"; }
    void print() const override {
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "[" << type() << "] Patient: " << patientName
                  << " | ID: " << billId
                  << " | Package: " << packagePrice
                  << " | TOTAL: " << total() << "\n";
    }
    ~DayCareSurgeryBill() override { std::cout << "[~DayCareSurgeryBill]\n"; }
};

// ─── EmergencyBill ───────────────────────────────────────────────────────────
class EmergencyBill : public Bill {
    double baseCharge;
    double hoursInER;
    double ratePerHour;

public:
    EmergencyBill(const std::string& name, const std::string& id,
                  double base, double hours, double rate)
        : Bill(name, id), baseCharge(base), hoursInER(hours), ratePerHour(rate) {}

    double total() const override {
        return baseCharge + hoursInER * ratePerHour;
    }
    std::string type() const override { return "Emergency"; }
    void print() const override {
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "[" << type() << "] Patient: " << patientName
                  << " | ID: " << billId
                  << " | Base: " << baseCharge
                  << " | Hours: " << hoursInER
                  << " | Rate/hr: " << ratePerHour
                  << " | TOTAL: " << total() << "\n";
    }
    ~EmergencyBill() override { std::cout << "[~EmergencyBill]\n"; }
};

// ─── A1.3 – Process all bills through base-class pointers (polymorphic) ──────
double computeDayTotal(const std::vector<Bill*>& bills) {
    double dayTotal = 0.0;
    for (const Bill* b : bills) {
        b->print();                 // virtual dispatch → correct print()
        dayTotal += b->total();     // virtual dispatch → correct total()
    }
    return dayTotal;
}

int main() {
    std::vector<Bill*> bills;
    bills.push_back(new OutPatientBill ("Alice",   "OP001", 300.0,  450.0));
    bills.push_back(new InPatientBill  ("Bob",     "IP002", 3, 2000.0, 5000.0, 1200.0));
    bills.push_back(new DayCareSurgeryBill("Carol","DC003", 15000.0));
    bills.push_back(new EmergencyBill  ("Dave",    "EM004", 1000.0, 4.5, 500.0));

    std::cout << "===== Today's Hospital Bills =====\n";
    double grand = computeDayTotal(bills);
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "──────────────────────────────────\n";
    std::cout << "Grand Total: Rs. " << grand << "\n\n";

    // Cleanup – virtual destructor ensures correct derived destructor runs
    std::cout << "===== Freeing Bills =====\n";
    for (Bill* b : bills) delete b;

    return 0;
}
