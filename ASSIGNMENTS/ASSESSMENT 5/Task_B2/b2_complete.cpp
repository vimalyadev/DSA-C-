// Task B2 – Complete the Code (TODO 1 and TODO 2 filled in)
// Expected output:
//   approved 40000
//   audit: exceeds cover
//   claim rejected: exceeds cover
//   error: negative amount
// Compile: g++ -std=c++17 -Wall -Wextra -fsanitize=address -o b2_complete b2_complete.cpp
// Run:     ./b2_complete

#include <iostream>
#include <stdexcept>

class ClaimError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

double approve(double billed, double cover)
{
    // ── TODO 1 COMPLETED ────────────────────────────────────────────────────
    if (billed < 0 || cover < 0)
        throw std::invalid_argument("negative amount");
    if (billed > cover)
        throw ClaimError("exceeds cover");
    return billed;
    // ────────────────────────────────────────────────────────────────────────
}

double settle(double billed, double cover)
{
    try {
        return approve(billed, cover);
    } catch (const ClaimError& e) {
        // ── TODO 2 COMPLETED ─────────────────────────────────────────────────
        // Print "audit: " followed by the message, then rethrow the SAME exception.
        // Using bare  throw;  preserves the dynamic type (ClaimError), so the
        // caller's catch(const ClaimError&) can still catch it.
        // Using  throw e;  would slice to ClaimError copy (same type here, but
        // bad practice in general – loses type for deeper hierarchies).
        std::cout << "audit: " << e.what() << '\n';
        throw;      // rethrow the original exception object (no slicing)
        // ─────────────────────────────────────────────────────────────────────
    }
}

int main()
{
    double cases[][2] = {{40000, 50000}, {60000, 50000}, {-1, 50000}};
    for (auto& c : cases) {
        try {
            double paid = settle(c[0], c[1]);
            std::cout << "approved " << paid << '\n';
        } catch (const ClaimError& e) {
            std::cout << "claim rejected: " << e.what() << '\n';
        } catch (const std::exception& e) {
            std::cout << "error: " << e.what() << '\n';
        }
    }
}

// ── DIFFERENCE: throw; vs throw e; ──────────────────────────────────────────
// throw;    → rethrows the ORIGINAL exception object in-place.
//             Its dynamic type is preserved (e.g., a derived ClaimError subclass
//             is still that subclass when caught by an outer handler).
// throw e;  → creates a NEW copy of e, using the STATIC type of e.
//             If e is declared as (const ClaimError& e) and the actual object
//             was a subclass of ClaimError, the subclass info is LOST (sliced).
// Rule of thumb: always use bare throw; inside a catch block to rethrow.
