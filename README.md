# JumSparse
A GPU accelerated sparse linear algebra library.

## V1 scope
* Matrix size does not change
* Can change entries as we go CI/CD

## Functionality Roadmap
* Sparse Array class 
    * Feb 25: Added base ability to init CSR array, lazy set/get is
    not great for overall object, will do other in future probably.
    * March 2, 2026: (from array.hpp)
        * Added CSR struct so SparseArray can be templated on format. Currently only CSR implemented, will add CSC and COO later if needed.
        * WARNING: operator() currently inserts structural zeros, so use set() to actually remove entries.
        * Can change this possibly in future, may cause issues. For now, won't use many operator() for writing entries, just for reading.
        * Next is to work on loading from std::vector<std::tuple<int,int,T>>, files, and then implement CSC format. * Will also add storage() accessor to SparseArray to allow direct access
          to get/set/is_nonzero for testing and flexibility.
        * Also want to work on CUDA allocations and simple
          math operations in CSR formats (both CPU and GPU), maybe before implementing CSC?
* spMV and spMM
    * blank
* Reordering
    * blank
* Jacobi
    * blank
* Gauss-Seidel
    * blank
* Over-Relaxation
    * blank
* Reordering
    * blank
* Multi-grid
    * blank
* Krylov solvers
    * blank
* CSC
    * blank
* COO
    * blank
* CM-CSR
    * blank
