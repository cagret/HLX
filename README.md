
# HyperLogLog Variants and Benchmarking

## Introduction
This project focuses on implementing and benchmarking a practical family of HyperLogLog algorithms: HL2, HL3, and an enhanced version, HL4. The aim is to develop efficient versions in terms of time and memory usage, accepting rare "accidents" whose impact is negligible in practice.


## Guide to Clone, Compile, and Run the HLX Project

This guide explains how to clone the HLX project's Git repository, compile the source code, and run the `./benchmark` program with custom or default parameters.

## Cloning the Project

1. Open your terminal.

2. Clone the HLX repository from GitHub using the `git clone` command:
```bash
git clone git@github.com:cagret/HLX.git
cd HLX
```

## Compiling the Project

1. Ensure you have GCC (the C compiler) installed on your system.

2. Use the `make` command to compile the project. This command will utilize the provided `Makefile`:

```bash
make
```


## Running the `./benchmark` Program

You can execute the `./benchmark` program using the following command with custom `p` and `q` parameters, or use the default parameters if you prefer:

```bash
./benchmark -p 15 -q 8 -n 100000
```

In our HyperLogLog (HLL) implementation, we utilize the `XXH64` function from the XXHash library for efficient hashing of data elements. This function is integral to the accuracy and performance of the HLL algorithm. Here's a brief overview of how `XXH64` is used:

- **Function Purpose**: `XXH64` is a high-speed hashing function that generates a 64-bit hash value. It's known for its exceptional speed and is widely used in scenarios where performance is critical.

- **Parameters**:
  - `p`: This is a pointer to the data you wish to hash. The data can be of any type, such as a string, a number, or a custom structure.
  - `q` (length): The size of the data pointed to by `p` in bytes. This length tells `XXH64` how much data to read and hash from `p`.
  - `n`: Number of unique elements to generate and insert into the HyperLogLog algorithm for benchmarking tests.

- **Usage in HLL**: The hash value returned by `XXH64` is used to determine the register index to update and the rank of the hash, which are key components in estimating the cardinality (the number of unique elements) in a dataset.
- The `-p` option allows you to specify the value of `p` (e.g., `-p 15`).

- The `-q` option allows you to specify the value of `q` (e.g., `-q 8`).

- The `-n` option allows you to specify the value of `n` (e.g., `-n 100000`).


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

## Contributions
Contributions are always welcome! Please fork the project and open a pull request with your proposed changes.

## License
Distributed under the GNU GENERAL PUBLIC LICENSE Version 3, dated 29 June 2007. See [LICENSE](LICENSE) for more information.



