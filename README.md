# Mastermind
A mastermind game that runs Knuth's algorithm and is optimized with bundled assembly

The profiling shows that more than 90% of the runtime is spent in one function scoreCodes.
This is the function we optimized by re-writing in assembly.

The optimisation resulted in a 1.5x runtime improvement!
