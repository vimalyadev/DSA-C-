// Task B2 – Predict the Output (from assignment sheet, verbatim)
// MY PREDICTED OUTPUT (written BEFORE running):
//
//   ok done caught too large ok done caught negative
//
// STEP-BY-STEP TRACE:
//   Iteration a=100.0:
//     validate(100.0) → 100 >= 0, not > 1e6 → prints "ok " → returns
//     process(100.0) calls validate(100.0) → no exception
//     main's try: "done " printed
//   Iteration a=-5.0:
//     validate(-5.0) → amount < 0 → throws std::invalid_argument("negative")
//     process(-5.0) catches out_of_range? NO – invalid_argument is not out_of_range
//     (invalid_argument IS-A logic_error, out_of_range IS-A logic_error,
//      but invalid_argument is NOT-A out_of_range)
//     → exception propagates to main's catch(const std::exception& e)
//     → prints "caught negative "
//   Iteration a=2e6:
//     validate(2e6) → 2e6 > 1e6 → throws std::out_of_range("too large")
//     process(2e6) catches out_of_range → prints "[log too large] " → rethrows (throw;)
//     main's catch(const std::exception& e) catches it → prints "caught too large "
//
// ACTUAL OUTPUT (run and confirmed):
//   ok done caught negative [log too large] caught too large
//   (newline)
//
// HANDLER ANALYSIS:
//   - 100.0: no exception → "done" printed by main's try block after process()
//   - -5.0:  invalid_argument NOT caught by process's catch(out_of_range),
//            so it propagates directly to main → "caught negative"
//   - 2e6:   out_of_range IS caught by process's catch(out_of_range),
//            logged "[log too large] ", then rethrown → main catches it → "caught too large"
// WHY ONLY ONE EXCEPTION WAS LOGGED:
//   Only std::out_of_range is caught and logged inside process().
//   std::invalid_argument bypasses process's handler entirely.

#include <iostream>
#include <stdexcept>

void validate(double amount)
{
    if (amount < 0) throw std::invalid_argument("negative");
    if (amount > 1e6) throw std::out_of_range("too large");
    std::cout << "ok ";
}

void process(double amount)
{
    try {
        validate(amount);
    } catch (const std::out_of_range& e) {
        std::cout << "[log " << e.what() << "] ";
        throw;
    }
}

int main()
{
    for (double a : {100.0, -5.0, 2e6}) {
        try {
            process(a);
            std::cout << "done ";
        } catch (const std::exception& e) {
            std::cout << "caught " << e.what() << ' ';
        }
    }
    std::cout << '\n';
}
