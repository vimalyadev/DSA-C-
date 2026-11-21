// Task A3 – Design with Abstract Classes: Payer Hierarchy
// Demonstrates: abstract class, pure virtual, open/closed principle (OCP)
// Compile: g++ -std=c++17 -Wall -Wextra -fsanitize=address -o payer payer.cpp
// Run:     ./payer

#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <iomanip>
#include <algorithm>

// ─── Forward-declare Bill so Payer can reference it ──────────────────────────
class Bill;

// ─── Abstract Base Class: Bill ───────────────────────────────────────────────
class Bill {
protected:
    std::string patientName;
    std::string billId;
public:
    Bill(const std::string& name, const std::string& id)
        : patientName(name), billId(id) {}
    virtual double      total() const = 0;
    virtual std::string type()  const = 0;
    virtual void        print() const = 0;
    virtual ~Bill() = default;
    const std::string& getPatient() const { return patientName; }
    const std::string& getId()      const { return billId; }
};

// ─── Concrete Bill Types ──────────────────────────────────────────────────────
class OutPatientBill : public Bill {
    double consultation, tests;
public:
    OutPatientBill(const std::string& n, const std::string& id, double c, double t)
        : Bill(n, id), consultation(c), tests(t) {}
    double total() const override { return consultation + tests; }
    std::string type() const override { return "OutPatient"; }
    void print() const override {
        std::cout << std::fixed << std::setprecision(2)
                  << "[OP] " << patientName << " consult=" << consultation
                  << " tests=" << tests << " total=" << total() << "\n";
    }
};

class InPatientBill : public Bill {
    int days; double perDay, procedures, medicines;
public:
    InPatientBill(const std::string& n, const std::string& id,
                  int d, double r, double p, double m)
        : Bill(n, id), days(d), perDay(r), procedures(p), medicines(m) {}
    double total() const override { return days*perDay + procedures + medicines; }
    std::string type() const override { return "InPatient"; }
    void print() const override {
        std::cout << std::fixed << std::setprecision(2)
                  << "[IP] " << patientName << " days=" << days
                  << " total=" << total() << "\n";
    }
};

class DayCareSurgeryBill : public Bill {
    double packagePrice;
public:
    DayCareSurgeryBill(const std::string& n, const std::string& id, double pkg)
        : Bill(n, id), packagePrice(pkg) {}
    double total() const override { return packagePrice; }
    std::string type() const override { return "DayCareSurgery"; }
    void print() const override {
        std::cout << std::fixed << std::setprecision(2)
                  << "[DC] " << patientName << " package=" << packagePrice << "\n";
    }
};

class EmergencyBill : public Bill {
    double base, hours, ratePerHour;
public:
    EmergencyBill(const std::string& n, const std::string& id,
                  double b, double h, double r)
        : Bill(n, id), base(b), hours(h), ratePerHour(r) {}
    double total() const override { return base + hours * ratePerHour; }
    std::string type() const override { return "Emergency"; }
    void print() const override {
        std::cout << std::fixed << std::setprecision(2)
                  << "[EM] " << patientName << " base=" << base
                  << " hours=" << hours << " total=" << total() << "\n";
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
// TASK A3.1 – Abstract Payer Interface
// ═══════════════════════════════════════════════════════════════════════════════
class Payer {
public:
    // Pure virtual – every concrete payer MUST implement this
    virtual double amountCovered(const Bill& b) const = 0;  // <<pure virtual>>
    virtual std::string name()                  const = 0;  // <<pure virtual>>
    virtual ~Payer() = default;
};

// ─── SelfPay: patient covers 100 % of the bill ───────────────────────────────
class SelfPay : public Payer {
public:
    double amountCovered(const Bill& b) const override {    // <<overridden>>
        return b.total();                                   // full amount
    }
    std::string name() const override { return "SelfPay"; }
};

// ─── Insurance: covers up to a limit, then patient pays co-payment % ─────────
class Insurance : public Payer {
    double coverLimit;       // maximum the insurer will pay
    double coPayPercent;     // % of remaining amount borne by patient (0-100)
public:
    Insurance(double limit, double coPayPct)
        : coverLimit(limit), coPayPercent(coPayPct) {}

    double amountCovered(const Bill& b) const override {    // <<overridden>>
        // Insurer pays min(total, limit), adjusted for co-payment:
        //   covered = total * (1 - coPayPercent/100), capped at coverLimit
        double gross = b.total() * (1.0 - coPayPercent / 100.0);
        return std::min(gross, coverLimit);
    }
    std::string name() const override { return "Insurance"; }
};

// ─── GovernmentScheme: fixed package rates independent of actual bill ─────────
class GovernmentScheme : public Payer {
    double packageRate;      // fixed government-approved rate for any procedure
public:
    explicit GovernmentScheme(double rate) : packageRate(rate) {}

    double amountCovered(const Bill& b) const override {    // <<overridden>>
        // Government pays the fixed package rate, not the hospital's billed amount
        (void)b;             // bill identity not needed; scheme is fixed
        return packageRate;
    }
    std::string name() const override { return "GovernmentScheme"; }
};

// ─── A3.2 – Extension: CorporateTieUp – new payer WITHOUT changing any above ──
// Demonstrates Open/Closed Principle: open for extension, closed for modification
class CorporateTieUp : public Payer {
    double discountPercent; // corporate negotiated discount
    double maxReimbursement;
public:
    CorporateTieUp(double disc, double maxR)
        : discountPercent(disc), maxReimbursement(maxR) {}

    double amountCovered(const Bill& b) const override {    // <<overridden>>
        double covered = b.total() * (discountPercent / 100.0);
        return std::min(covered, maxReimbursement);
    }
    std::string name() const override { return "CorporateTieUp"; }
};

// ─── Utility: Settle a bill with a given payer ───────────────────────────────
void settle(const Bill& bill, const Payer& payer) {
    double covered   = payer.amountCovered(bill);
    double patientDue = bill.total() - covered;
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "  Payer        : " << payer.name()    << "\n";
    std::cout << "  Bill total   : " << bill.total()    << "\n";
    std::cout << "  Covered      : " << covered         << "\n";
    std::cout << "  Patient pays : " << patientDue      << "\n\n";
}

int main() {
    // One bill of each kind
    OutPatientBill    op ("Alice",   "OP001", 300.0,  450.0);
    InPatientBill     ip ("Bob",     "IP002", 3, 2000.0, 5000.0, 1200.0);
    DayCareSurgeryBill dc("Carol",   "DC003", 15000.0);
    EmergencyBill      em("Dave",    "EM004", 1000.0, 4.5, 500.0);

    // One instance of each payer
    SelfPay           selfPay;
    Insurance         insurance(10000.0, 20.0);  // limit=10000, copay=20%
    GovernmentScheme  govScheme(8000.0);          // fixed package=8000
    CorporateTieUp    corporate(75.0, 12000.0);   // 75% covered, max=12000

    std::vector<const Bill*>  bills   = {&op, &ip, &dc, &em};
    std::vector<const Payer*> payers  = {&selfPay, &insurance, &govScheme, &corporate};

    for (const Bill* b : bills) {
        b->print();
        std::cout << "─────────────────────────────────────\n";
        for (const Payer* p : payers) {
            settle(*b, *p);
        }
        std::cout << "=====================================\n\n";
    }

    return 0;
}
