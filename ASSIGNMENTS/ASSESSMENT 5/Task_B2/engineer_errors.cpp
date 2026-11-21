// Task B2 – Engineer Error Conditions
// Demonstrates: negative charges, claim above cover, bad_alloc, catch(...) safety
// Compile: g++ -std=c++17 -Wall -Wextra -fsanitize=address -o engineer_errors engineer_errors.cpp
// Run:     ./engineer_errors

#include <iostream>
#include <string>
#include <stdexcept>
#include <new>
#include <vector>
#include <iomanip>
#include <limits>

// ─── Custom Exception ────────────────────────────────────────────────────────
class BillingError : public std::runtime_error {
public:
    explicit BillingError(const std::string& msg) : std::runtime_error(msg) {}
};

// ─── A simple RAII guard that prints its own destruction ────────────────────
struct Resource {
    std::string label;
    explicit Resource(const std::string& s) : label(s) {
        std::cout << "  [+Resource: " << label << "]\n";
    }
    ~Resource() {
        std::cout << "  [-Resource: " << label << "]\n";   // always runs
    }
};

// ─── 1. Negative charge ──────────────────────────────────────────────────────
void testNegativeCharge() {
    std::cout << "\n--- Test 1: Negative charge ---\n";
    try {
        Resource r("NegativeTest");
        double amount = -500.0;
        if (amount < 0.0)
            throw std::invalid_argument("Charge must not be negative: " +
                                        std::to_string(amount));
        std::cout << "  Charge accepted: " << amount << "\n";
    } catch (const std::invalid_argument& e) {
        std::cout << "  CAUGHT invalid_argument: " << e.what() << "\n";
        // Resource r is already destroyed (stack-unwound) before we reach here
    }
}

// ─── 2. Claim above insurance cover ─────────────────────────────────────────
void testClaimAboveCover() {
    std::cout << "\n--- Test 2: Claim above cover ---\n";
    try {
        Resource r("ClaimTest");
        double billed = 80000.0, cover = 50000.0;
        if (billed > cover)
            throw BillingError("Claim " + std::to_string(billed) +
                               " exceeds cover " + std::to_string(cover));
        std::cout << "  Claim approved: " << billed << "\n";
    } catch (const BillingError& e) {
        std::cout << "  CAUGHT BillingError: " << e.what() << "\n";
    }
}

// ─── 3. Unknown package code (std::stod throws std::invalid_argument) ────────
void testUnknownPackageCode() {
    std::cout << "\n--- Test 3: Text where number expected (std::stod) ---\n";
    try {
        Resource r("PackageCode");
        std::string input = "BASIC_PACKAGE";   // not a number
        double amount = std::stod(input);       // throws std::invalid_argument
        std::cout << "  Package price: " << amount << "\n";
    } catch (const std::invalid_argument& e) {
        std::cout << "  CAUGHT invalid_argument (stod): " << e.what() << "\n";
    }
}

// ─── 4. Memory allocation failure (std::bad_alloc) ──────────────────────────
void testBadAlloc() {
    std::cout << "\n--- Test 4: Memory allocation failure (bad_alloc) ---\n";
    try {
        Resource r("MemTest");
        // Use operator new directly with an impossibly large size to trigger bad_alloc
        // (std::vector uses max_size() guard before allocating, throwing length_error
        //  on some platforms; raw new always throws bad_alloc on allocation failure)
        void* p = ::operator new(std::numeric_limits<std::size_t>::max());
        ::operator delete(p);
        std::cout << "  Allocated (this line should NOT print)\n";
    } catch (const std::bad_alloc& e) {
        std::cout << "  CAUGHT bad_alloc: " << e.what() << "\n";
        std::cout << "  Program continues safely.\n";
    } catch (const std::exception& e) {
        // Some platforms raise std::length_error from vector; catch and report it
        std::cout << "  CAUGHT exception (allocation-related): " << e.what() << "\n";
        std::cout << "  Program continues safely.\n";
    }
}

// ─── 5. catch(...) keeps the program alive for unexpected exceptions ──────────
void testCatchAll() {
    std::cout << "\n--- Test 5: catch(...) catches unknown exception ---\n";
    try {
        Resource r("CatchAll");
        throw 42;    // throwing a plain int
    } catch (...) {
        std::cout << "  CAUGHT unknown exception via catch(...)\n";
        std::cout << "  All Resources destroyed (see destructor messages above)\n";
    }
}

int main() {
    std::cout << "===== B2 – Engineering Error Conditions =====\n";
    testNegativeCharge();
    testClaimAboveCover();
    testUnknownPackageCode();
    testBadAlloc();
    testCatchAll();
    std::cout << "\n===== All tests completed; program intact =====\n";
    return 0;
}
