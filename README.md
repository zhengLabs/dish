## WHAT IS DISH

Erasure code mode causes significant I/O and network bandwidth overheads when updating data. For example, to update a single block of RS(k, r) code, it is necessary to ensure that the data block is consistent with the parity block, i.e. all parity blocks need to be re-generated and written to the original parity block location. DISH uses a hypergraph approach to map data blocks to hypergraphs, which are then partitioned to obtain relevant data blocks for placement.

## Requirements
- operating system. Linux, Mac OS X and Windows are currently supported.
- A modern, C++14-ready compiler such as g++ version 9 or higher or clang version 11.0.3 or higher.
- The cmake build system.

## Building DISH
1. Clone the repository
2. build Hypergraph Partitioning Tool KaHyPar https://github.com/kahypar/kahypar
3. Create a build directory: mkdir build && cd build
4. Run cmake: cmake .. 
5. Run make: make