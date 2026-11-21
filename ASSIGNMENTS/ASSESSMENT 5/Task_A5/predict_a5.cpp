// Task A5 – Design Recovery and Rethrowing
// Predict-the-Output program – EXACT code from assignment sheet.
// Compile: g++ -std=c++17 -Wall -Wextra -fsanitize=address -o predict_a5 predict_a5.cpp
// Run:     ./predict_a5
//
// MY PREDICTED OUTPUT (written BEFORE running):
//   ~f ~main caught 42
// (newline at end)
//
// EXPLANATION:
//   1. Guard m("main") is constructed in the try block.
//   2. f() is called; Guard g("f") is constructed.
//   3. throw 42 is executed. Stack unwinding begins:
//      - g goes out of scope → ~Guard prints "~f "
//      - m goes out of scope → ~Guard prints "~main "
//      - "not printed " is SKIPPED because the exception bypassed that statement.
//   4. catch(int x) catches the int 42 → prints "caught 42"
//   5. std::cout << '\n' at the end of main prints the newline.
//
// ORDER OF DESTRUCTOR CALLS:
//   g (Guard "f")   is destroyed first because it was constructed AFTER m.
//   m (Guard "main") is destroyed second (LIFO order of construction).
//
// WHY "not printed" NEVER APPEARS:
//   When an exception is thrown, control jumps immediately to the nearest
//   matching handler. All code between the throw and the handler is skipped.
//   "not printed " is after f(); so it never executes.

#include <iostream>
#include <string>

struct Guard {
    std::string n;
    explicit Guard(const std::string& s) : n(s) {}
    ~Guard() { std::cout << '~' << n << ' '; }
};

void f()
{
    Guard g("f");
    throw 42;
}

int main()
{
    try {
        Guard m("main");
        f();
        std::cout << "not printed ";
    } catch (int x) {
        std::cout << "caught " << x;
    } catch (...) {
        std::cout << "other";
    }
    std::cout << '\n';
}
