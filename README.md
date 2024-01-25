
# HyperLogLog Variants and Benchmarking

## Introduction
This project focuses on implementing and benchmarking a practical family of HyperLogLog algorithms: HL2, HL3, and an enhanced version, HL4. The aim is to develop efficient versions in terms of time and memory usage, accepting rare "accidents" whose impact is negligible in practice.

## HL2 and HL3: The Basics
- **HL2 (HyperLogLog 2)**: An early version of the HyperLogLog algorithm, focusing on basic probabilistic counting with a simple data structure.
- **HL3 (HyperLogLog 3)**: An improved version of HL2, offering better accuracy and efficiency in counting distinct elements.

## HL4: An Advanced HyperLogLog Variant
- **HL4**: Builds upon HL3 by introducing a layered structure comprising a mega counter, a list of super counters, and a list (of lists) of counters.
- **Insertion Logic**: 
  - If the hash value is lower than the mega counter, the process ends.
  - If not, the algorithm checks the corresponding super counter.
  - If the super counter's value is also higher, the algorithm updates the regular counter.

### Parameters of HL4
- **Base**: Experimenting with bases 2 and 4, with a potential exploration of base 8.
- **Counter Size**: Restricted to 2 and 4 for implementation reasons. Sizes 1 and 3 are impractical, and 5 approximates a standard sketch.
- **Super Counter Size**: Set to 4, as 2 might result in too few super counters (thus, fewer sub-sketches), and 6 could lead to an excessively large number of sub-sketches.
- **Number of Super Counters / Sub-Sketches**: This parameter is crucial and needs testing to determine the optimal size that balances memory costs and accident probabilities.

## Benchmarking
The benchmarking process involves comparing HL2, HL3, and HL4 in terms of memory usage and execution time. The goal is to evaluate the performance trade-offs, especially how the enhancements in HL4 impact the overall efficiency and accuracy.

## Conclusion
HL4 represents an innovative approach in the HyperLogLog family, aiming to strike a balance between computational efficiency and accuracy. The benchmarking will shed light on its practical applicability and performance metrics.

---


