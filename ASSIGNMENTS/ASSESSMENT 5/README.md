# Assignment 5 – CO5: Reliable Software with Runtime Polymorphism and Exceptions

**Name:** Vimalya Dev  
**Register Number:** 192421350  
**Course Code:** DSA01 - Object-Oriented Programming in C++  
**Compiler:** g++ (MinGW) 13.2.0  
**Standard:** C++17  
**OS:** Windows 11  

---

## Compile Command (used for every program)

```
g++ -std=c++17 -Wall -Wextra -fsanitize=address -o <output> <source>.cpp
```

Performance benchmark (B3) compiled with optimisation:

```
g++ -std=c++17 -O2 -Wall -Wextra -o Task_B3/b3_bench Task_B3/b3_bench.cpp
```

---

## Repository Structure

```
.
├── Task_A1/   bill_hierarchy.cpp          – A1: Polymorphic bill class hierarchy
├── Task_A2/   predict_a2.cpp             – A2: Predict the output (vtable / virtual dtor)
├── Task_A3/   payer.cpp                  – A3: Abstract Payer interface + 4 concrete payers
├── Task_A4/   exception_hierarchy.cpp    – A4: Custom exception class hierarchy
├── Task_A5/   predict_a5.cpp             – A5: Predict the output (RAII / stack unwinding)
├── Task_B1/   b1_complete.cpp            – B1: Complete-the-code (TODO 1–3)
├── Task_B2/   engineer_errors.cpp        – B2: Engineered error conditions
│              predict_b2.cpp             – B2: Predict the output (validate/process)
│              b2_complete.cpp            – B2: Complete-the-code (approve/settle)
├── Task_B3/   b3_bench.cpp               – B3: Performance benchmark (virtual vs switch vs exceptions)
├── Task_B4/   b4_extension.cpp           – B4: Extension + production decision
└── README.md
```

---

## How to Run Each Program

| Task | Command | Expected output summary |
|------|---------|------------------------|
| A1 | `Task_A1/bill_hierarchy.exe` | Bills printed, Grand Total Rs 31200.00 |
| A2 | `Task_A2/predict_a2.exe` | `500 ~OPBill ~Bill` |
| A3 | `Task_A3/payer.exe` | All 4 bills × 4 payers settled |
| A4 | `Task_A4/exception_hierarchy.exe` | 5 exception types caught and reported |
| A5 | `Task_A5/predict_a5.exe` | `~f ~main caught 42` |
| B1 | `Task_B1/b1_complete.exe` | `OP Ravi: 1700 / IP Meena: 47500 / Total 49200` |
| B2-predict | `Task_B2/predict_b2.exe` | `ok done caught negative [log too large] caught too large` |
| B2-complete | `Task_B2/b2_complete.exe` | `approved 40000 / audit: exceeds cover / claim rejected / error: negative amount` |
| B2-errors | `Task_B2/engineer_errors.exe` | All 5 error conditions caught safely |
| B3 | `Task_B3/b3_bench.exe` | Correctness + timing + exception-cost tables |
| B4 | `Task_B4/b4_extension.exe` | 5 bills × 4 payers, new PhysiotherapyBill + CorporateTieUp |

---

## Short Result Notes

- **A1**: All four bill types compute correct totals via polymorphic dispatch. Virtual destructor correctly chains ~Derived → ~Bill.
- **A2**: Output `500 ~OPBill ~Bill` confirmed. `show()` calls `total()` through the vtable even though `show()` is non-virtual; `virtual ~Bill` causes `~OPBill` then `~Bill` in correct order.
- **A3**: Four payer types cover the same bills differently with zero changes to the Bill hierarchy — OCP demonstrated.
- **A4**: Custom exception hierarchy rooted at `BillingException : std::runtime_error`. Four derived types caught correctly by both specific and base handlers.
- **A5**: `~f ~main caught 42` — destructors run LIFO during stack unwinding, "not printed" skipped.
- **B1**: TODOs completed; output matches expected exactly.
- **B2**: Predict output verified; complete-the-code output matches expected exactly; all 5 engineered error conditions caught with RAII cleanup.
- **B3**: Virtual ~33 μs/M bills; Switch ~24 μs/M bills; Exception overhead enormous at 50% errors but negligible at 0–1%.
- **B4**: `PhysiotherapyBill` + `CorporateTieUp` added with zero changes to existing classes; sanitizer report clean; production recommendation: use virtual + exceptions.
