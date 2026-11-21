// Task A2 – Understand Runtime Binding (Predict the Output program)
// The EXACT program from the assignment sheet, verbatim.
// Compile: g++ -std=c++17 -Wall -Wextra -o predict_a2 predict_a2.cpp
// Run:     ./predict_a2

#include <iostream>

struct Bill {
    virtual ~Bill() { std::cout << "~Bill "; }
    virtual double total() const = 0;
    void show() const { std::cout << total() << ' '; }
};

struct OPBill : Bill {
    ~OPBill() override { std::cout << "~OPBill "; }
    double total() const override { return 500; }
};

int main()
{
    Bill* b = new OPBill;
    b->show();
    delete b;
    std::cout << '\n';
}
