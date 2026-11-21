// Task B1 – Complete the Code (TODO 1 to TODO 3 filled in)
// Expected output:
//   OP Ravi: 1700
//   IP Meena: 47500
//   Total 49200
// Compile: g++ -std=c++17 -Wall -Wextra -fsanitize=address -o b1_complete b1_complete.cpp
// Run:     ./b1_complete

#include <iostream>
#include <string>

class Bill {
protected:
    std::string patient;

public:
    explicit Bill(const std::string& p) : patient(p) {}
    virtual ~Bill() = default;

    // ── TODO 1 COMPLETED ────────────────────────────────────────────────────
    // Pure virtual total() – every concrete bill must supply its own formula.
    virtual double total() const = 0;

    // Virtual type() with a default body (not pure); derived classes override it.
    virtual std::string type() const { return "Bill"; }
    // ────────────────────────────────────────────────────────────────────────

    void print() const
    {
        std::cout << type() << ' ' << patient << ": " << total() << '\n';
    }
};

class OutPatient : public Bill {
    double consultation, tests;

public:
    OutPatient(const std::string& p, double c, double t)
        : Bill(p), consultation(c), tests(t) {}

    // ── TODO 2 COMPLETED ────────────────────────────────────────────────────
    double total() const override { return consultation + tests; }
    std::string type() const override { return "OP"; }
    // ────────────────────────────────────────────────────────────────────────
};

class InPatient : public Bill {
    int    days;
    double perDay, surgery;

public:
    InPatient(const std::string& p, int d, double rate, double s)
        : Bill(p), days(d), perDay(rate), surgery(s) {}

    // ── TODO 3 COMPLETED ────────────────────────────────────────────────────
    double total() const override { return days * perDay + surgery; }
    std::string type() const override { return "IP"; }
    // ────────────────────────────────────────────────────────────────────────
};

int main()
{
    Bill* bills[] = {new OutPatient("Ravi",  500, 1200),
                     new InPatient ("Meena", 3, 2500, 40000)};
    double sum = 0;
    for (Bill* b : bills) {
        b->print();          // runtime polymorphism
        sum += b->total();
    }
    std::cout << "Total " << sum << '\n';
    for (Bill* b : bills) delete b;  // virtual destructor runs the right destructor
}
