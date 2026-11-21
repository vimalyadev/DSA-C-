// Task B3 – Virtual Functions vs switch vs Exceptions: Performance Benchmark
// B3.1: Correctness on 10 bills (virtual vs switch)
// B3.2: Timing for 1K, 1M, 10M bills
// B3.3: Exception cost (0%, 1%, 50% error rates)
// B3.4: Engineering trade-off analysis (see comments below)
//
// Compile (optimised): g++ -std=c++17 -O2 -Wall -Wextra -o b3_bench b3_bench.cpp
// Run:                 ./b3_bench

#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <stdexcept>
#include <iomanip>
#include <cstdlib>   // rand
#include <ctime>

// ════════════════════════════════════════════════════════════════════════════
// VERSION A – POLYMORPHIC (virtual functions)
// ════════════════════════════════════════════════════════════════════════════
struct BillBase {
    virtual double total() const = 0;
    virtual ~BillBase() = default;
};

struct OP_Virtual : BillBase {
    double c, t;
    OP_Virtual(double c, double t) : c(c), t(t) {}
    double total() const override { return c + t; }
};

struct IP_Virtual : BillBase {
    int d; double r, s;
    IP_Virtual(int d, double r, double s) : d(d), r(r), s(s) {}
    double total() const override { return d * r + s; }
};

struct DC_Virtual : BillBase {
    double pkg;
    DC_Virtual(double p) : pkg(p) {}
    double total() const override { return pkg; }
};

struct EM_Virtual : BillBase {
    double base, h, rate;
    EM_Virtual(double b, double h, double r) : base(b), h(h), rate(r) {}
    double total() const override { return base + h * rate; }
};

// ════════════════════════════════════════════════════════════════════════════
// VERSION B – SWITCH on type field
// ════════════════════════════════════════════════════════════════════════════
enum BillKind { KIND_OP, KIND_IP, KIND_DC, KIND_EM };

struct BillSwitch {
    BillKind kind;
    double a, b, c; int d;

    double total() const {
        switch (kind) {
            case KIND_OP: return a + b;
            case KIND_IP: return d * a + b;
            case KIND_DC: return a;
            case KIND_EM: return a + b * c;
        }
        return 0.0;
    }
};

// ════════════════════════════════════════════════════════════════════════════
// B3.1 – Correctness check on 10 bills
// ════════════════════════════════════════════════════════════════════════════
void correctnessCheck() {
    std::cout << "=== B3.1 Correctness: 10 bills (virtual vs switch) ===\n";
    std::cout << std::fixed << std::setprecision(2);

    // Virtual
    std::vector<BillBase*> vbills = {
        new OP_Virtual(300, 450),  new IP_Virtual(3, 2000, 1200),
        new DC_Virtual(15000),     new EM_Virtual(1000, 4.5, 500),
        new OP_Virtual(200, 100),  new IP_Virtual(5, 1800, 3000),
        new DC_Virtual(12000),     new EM_Virtual(500, 2, 300),
        new OP_Virtual(400, 600),  new IP_Virtual(2, 2500, 800)
    };

    // Switch
    std::vector<BillSwitch> sbills = {
        {KIND_OP, 300, 450,  0,  0}, {KIND_IP, 2000, 1200, 0, 3},
        {KIND_DC, 15000, 0,  0,  0}, {KIND_EM, 1000, 4.5, 500, 0},
        {KIND_OP, 200, 100,  0,  0}, {KIND_IP, 1800, 3000, 0, 5},
        {KIND_DC, 12000, 0,  0,  0}, {KIND_EM, 500, 2, 300, 0},
        {KIND_OP, 400, 600,  0,  0}, {KIND_IP, 2500, 800, 0, 2}
    };

    double vtotal = 0, stotal = 0;
    for (std::size_t i = 0; i < vbills.size(); ++i) {
        double v = vbills[i]->total();
        double s = sbills[i].total();
        std::cout << "  Bill[" << i << "] virtual=" << v
                  << "  switch=" << s
                  << (v == s ? "  ✓" : "  ✗ MISMATCH") << "\n";
        vtotal += v; stotal += s;
    }
    std::cout << "  Grand total virtual=" << vtotal
              << "  switch=" << stotal
              << (vtotal == stotal ? "  ✓ MATCH\n\n" : "  ✗ MISMATCH\n\n");
    for (auto* p : vbills) delete p;
}

// ════════════════════════════════════════════════════════════════════════════
// B3.2 – Performance: 1K, 1M, 10M bills
// ════════════════════════════════════════════════════════════════════════════
void timingBenchmark() {
    std::cout << "=== B3.2 Timing Benchmark (compiled -O2) ===\n";
    std::cout << std::left << std::setw(14) << "N"
              << std::setw(16) << "Virtual (us)"
              << std::setw(16) << "Switch (us)" << "\n";

    for (long long N : {1000LL, 1000000LL, 10000000LL}) {
        // ── Virtual version ──
        std::vector<BillBase*> vb;
        vb.reserve(N);
        for (long long i = 0; i < N; ++i) {
            switch (i % 4) {
                case 0: vb.push_back(new OP_Virtual(300, 450)); break;
                case 1: vb.push_back(new IP_Virtual(3, 2000, 1200)); break;
                case 2: vb.push_back(new DC_Virtual(15000)); break;
                case 3: vb.push_back(new EM_Virtual(1000, 4.5, 500)); break;
            }
        }
        double sink1 = 0;
        auto t1 = std::chrono::high_resolution_clock::now();
        for (auto* p : vb) sink1 += p->total();
        auto t2 = std::chrono::high_resolution_clock::now();
        long long vus = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
        for (auto* p : vb) delete p;

        // ── Switch version ──
        std::vector<BillSwitch> sb;
        sb.reserve(N);
        for (long long i = 0; i < N; ++i) {
            switch (i % 4) {
                case 0: sb.push_back({KIND_OP, 300, 450, 0, 0}); break;
                case 1: sb.push_back({KIND_IP, 2000, 1200, 0, 3}); break;
                case 2: sb.push_back({KIND_DC, 15000, 0, 0, 0}); break;
                case 3: sb.push_back({KIND_EM, 1000, 4.5, 500, 0}); break;
            }
        }
        double sink2 = 0;
        auto t3 = std::chrono::high_resolution_clock::now();
        for (const auto& s : sb) sink2 += s.total();
        auto t4 = std::chrono::high_resolution_clock::now();
        long long sus = std::chrono::duration_cast<std::chrono::microseconds>(t4 - t3).count();

        std::cout << std::left << std::setw(14) << N
                  << std::setw(16) << vus
                  << std::setw(16) << sus
                  << " (sinks: " << (long long)sink1 << "/" << (long long)sink2 << ")\n";
    }
    std::cout << "\n";
}

// ════════════════════════════════════════════════════════════════════════════
// B3.3 – Cost of Exceptions: 0%, 1%, 50% error rates
// ════════════════════════════════════════════════════════════════════════════
static const long long BENCH_N = 1000000LL;

void validateWithException(double amount) {
    if (amount < 0.0) throw std::invalid_argument("negative");
}

bool validateReturnCode(double amount, std::string& errMsg) {
    if (amount < 0.0) { errMsg = "negative"; return false; }
    return true;
}

void exceptionCostBenchmark() {
    std::cout << "=== B3.3 Exception Cost (N=" << BENCH_N << ") ===\n";
    std::cout << std::left << std::setw(10) << "ErrorRate"
              << std::setw(18) << "Exception (us)"
              << std::setw(18) << "ReturnCode (us)" << "\n";

    srand(42);
    for (double errRate : {0.0, 0.01, 0.50}) {
        // ── Exception version ──
        long long caught1 = 0;
        auto t1 = std::chrono::high_resolution_clock::now();
        for (long long i = 0; i < BENCH_N; ++i) {
            double val = (static_cast<double>(rand()) / RAND_MAX < errRate) ? -1.0 : 1.0;
            try {
                validateWithException(val);
            } catch (...) { ++caught1; }
        }
        auto t2 = std::chrono::high_resolution_clock::now();
        long long eus = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();

        // ── Return code version ──
        srand(42);
        long long caught2 = 0;
        auto t3 = std::chrono::high_resolution_clock::now();
        for (long long i = 0; i < BENCH_N; ++i) {
            double val = (static_cast<double>(rand()) / RAND_MAX < errRate) ? -1.0 : 1.0;
            std::string err;
            if (!validateReturnCode(val, err)) ++caught2;
        }
        auto t4 = std::chrono::high_resolution_clock::now();
        long long rus = std::chrono::duration_cast<std::chrono::microseconds>(t4 - t3).count();

        std::cout << std::left << std::setw(10) << (int)(errRate * 100) << "%"
                  << std::setw(18) << eus
                  << std::setw(18) << rus
                  << " (errors: " << caught1 << "/" << caught2 << ")\n";
    }
    std::cout << "\n";
}

// ════════════════════════════════════════════════════════════════════════════
// B3.4 – Engineering Trade-Off Summary (printed at runtime)
// ════════════════════════════════════════════════════════════════════════════
void tradeOffSummary() {
    std::cout << "=== B3.4 Engineering Trade-Off ===\n";
    std::cout <<
R"(
Virtual functions:
  + Natural OOP – each class owns its own behaviour.
  + O(1) dispatch via vtable pointer lookup (one indirection).
  + Adding a new bill type = add one class, zero changes to callers.
  - Each object carries a vptr (~8 bytes); heap allocation overhead.
  - Indirect call may miss branch predictor → slower than a switch on
    perfectly branch-predicted, type-uniform workloads.

Switch on type field:
  + No heap allocation, no vptr, objects are plain structs.
  + CPU branch predictor warms up fast when types repeat in a pattern.
  - Every new bill type → edit EVERY switch statement in every file.
    O(k) touch-points for k types → violates the Open/Closed Principle.
  - Grows to a maintenance nightmare across 15 hospitals.

Exceptions vs return codes:
  + Zero-overhead when no exception is thrown (normal path, 0% error).
  + Code is cleaner: error path separated from normal path.
  - Heavy cost when an exception IS thrown (~10–100x vs return code).
  - Return codes: near-zero cost on both paths but pollute every caller.
  - For 0% errors: use exceptions (no overhead, clean code).
  - For 50% errors (not truly exceptional): return codes / std::optional.
  - Hospital billing: validation errors < 1% → exceptions preferred.

RECOMMENDATION for a hospital group running 15 hospitals:
  Use virtual functions + exceptions.
  The polymorphic design lets any team add new bill types or payers
  by writing a new class only. The switch design would require touching
  every hospital's codebase on every extension, raising defect risk.
  At < 1% error rate the exception overhead is negligible.
)";
}

int main() {
    correctnessCheck();
    timingBenchmark();
    exceptionCostBenchmark();
    tradeOffSummary();
    return 0;
}
