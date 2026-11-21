// Task A4 – Design the Exception Hierarchy
// Demonstrates: custom exception class hierarchy, throw/catch, what()
// Compile: g++ -std=c++17 -Wall -Wextra -fsanitize=address -o exception_hierarchy exception_hierarchy.cpp
// Run:     ./exception_hierarchy

#include <iostream>
#include <string>
#include <stdexcept>
#include <iomanip>

// ═══════════════════════════════════════════════════════════════════════════════
// Exception Hierarchy for the Hospital Billing System
// ═══════════════════════════════════════════════════════════════════════════════

// ─── Root of all billing exceptions ─────────────────────────────────────────
class BillingException : public std::runtime_error {
public:
    explicit BillingException(const std::string& msg)
        : std::runtime_error(msg) {}
};

// ─── Thrown when a charge value is invalid (e.g., negative) ─────────────────
class InvalidChargeException : public BillingException {
    double badValue;
public:
    InvalidChargeException(const std::string& field, double val)
        : BillingException("Invalid charge in field '" + field +
                           "': " + std::to_string(val)),
          badValue(val) {}
    double value() const { return badValue; }
};

// ─── Thrown when a claimed amount exceeds the payer's policy cover ───────────
class ClaimExceedsCoverException : public BillingException {
    double billed, cover;
public:
    ClaimExceedsCoverException(double b, double c)
        : BillingException("Claim " + std::to_string(b) +
                           " exceeds cover limit " + std::to_string(c)),
          billed(b), cover(c) {}
    double getBilled() const { return billed; }
    double getCover()  const { return cover; }
};

// ─── Thrown when a bill type code is not recognised by the system ────────────
class UnknownBillTypeException : public BillingException {
    std::string code;
public:
    explicit UnknownBillTypeException(const std::string& typeCode)
        : BillingException("Unknown bill type code: '" + typeCode + "'"),
          code(typeCode) {}
    const std::string& typeCode() const { return code; }
};

// ─── Thrown when a payment gateway (bank/insurance portal) fails ─────────────
class PaymentGatewayException : public BillingException {
    int errorCode;
public:
    PaymentGatewayException(const std::string& gateway, int ec)
        : BillingException("Payment gateway '" + gateway +
                           "' failed with code " + std::to_string(ec)),
          errorCode(ec) {}
    int getErrorCode() const { return errorCode; }
};

// ─── Helpers that throw the right exception ───────────────────────────────────
void validateCharge(const std::string& field, double val) {
    if (val < 0.0)
        throw InvalidChargeException(field, val);
}

double approveClaim(double billed, double cover) {
    if (billed < 0.0 || cover < 0.0)
        throw InvalidChargeException("billed/cover", billed < 0 ? billed : cover);
    if (billed > cover)
        throw ClaimExceedsCoverException(billed, cover);
    return billed;
}

double createBill(const std::string& typeCode, double amount) {
    if (typeCode != "OP" && typeCode != "IP" &&
        typeCode != "DC" && typeCode != "EM")
        throw UnknownBillTypeException(typeCode);
    return amount;
}

void payViaGateway(const std::string& gw, bool willFail) {
    if (willFail)
        throw PaymentGatewayException(gw, 503);
    std::cout << "Payment via " << gw << " succeeded.\n";
}

int main() {
    std::cout << "=== Testing Exception Hierarchy ===\n\n";

    // 1. InvalidChargeException
    try {
        validateCharge("consultation", -200.0);
    } catch (const InvalidChargeException& e) {
        std::cout << "[InvalidCharge] " << e.what()
                  << "  (bad value=" << e.value() << ")\n";
    }

    // 2. ClaimExceedsCoverException
    try {
        approveClaim(75000.0, 50000.0);
    } catch (const ClaimExceedsCoverException& e) {
        std::cout << "[ClaimExceedsCover] " << e.what()
                  << "  billed=" << std::fixed << std::setprecision(2)
                  << e.getBilled() << " cover=" << e.getCover() << "\n";
    }

    // 3. UnknownBillTypeException
    try {
        createBill("XX", 5000.0);
    } catch (const UnknownBillTypeException& e) {
        std::cout << "[UnknownBillType] " << e.what()
                  << "  code='" << e.typeCode() << "'\n";
    }

    // 4. PaymentGatewayException
    try {
        payViaGateway("HDFCPay", true);
    } catch (const PaymentGatewayException& e) {
        std::cout << "[PaymentGateway] " << e.what()
                  << "  code=" << e.getErrorCode() << "\n";
    }

    // 5. Catch all billing exceptions through the base pointer
    std::cout << "\n--- Catch all as BillingException ---\n";
    try {
        approveClaim(-1.0, 1000.0);
    } catch (const BillingException& e) {
        std::cout << "[BillingException base catch] " << e.what() << "\n";
    }

    return 0;
}
