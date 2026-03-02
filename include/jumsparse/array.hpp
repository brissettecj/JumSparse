#pragma once

#include <vector>
#include <stdexcept>

// Feb 25, 2026: 
//     Working on preliminary design for only CSR format. Building as immutable
//     size, mutable values. Will add CSC and mutability later if needed.
// March 2, 2026:
//     Working on structs for different formats. Will implement CSR first, then CSC if needed.
//     WARNING: operator() currently inserts structural zeros, so use set() to actually remove entries.
//     Can change this possibly in future, may cause issues. For now, won't use many operator() for writing
//     entries, just for reading. Next is to work on loading from std::vector<std::tuple<int,int,T>>, files,
//     and then implement CSC format. Will also add storage() accessor to SparseArray to allow direct access
//     to get/set/is_nonzero for testing and flexibility. Also want to work on CUDA allocations and simple
//     math operations in CSR formats (both CPU and GPU), maybe before implementing CSC?
// ---------------------------------------------------------------------------
template <typename T>
struct CSR {
    int rows, cols;
    std::vector<int> row_ptr;   // size rows + 1
    std::vector<int> col_ind;   // size nnz
    std::vector<T>   values;    // size nnz

    // Constructor
    CSR(int rows, int cols)
        : rows(rows), cols(cols), row_ptr(rows + 1, 0) {}

    // Bounds checking
    void bounds_check(int row, int col) const {
        if (row < 0 || row >= rows || col < 0 || col >= cols)
            throw std::out_of_range("CSR index out of bounds");
    }

    // Read-only access. Returns zero for missing entries.
    T get(int row, int col) const {
        bounds_check(row, col);
        for (int idx = row_ptr[row]; idx < row_ptr[row + 1]; ++idx) {
            if (col_ind[idx] == col)
                return values[idx];
        }
        return T{};
    }

    // Write access. Inserts, updates, or removes entries as needed.
    void set(int row, int col, T value) {
        bounds_check(row, col);
        for (int idx = row_ptr[row]; idx < row_ptr[row + 1]; ++idx) {
            if (col_ind[idx] == col) {
                if (value == T{}) {
                    // Remove existing nonzero
                    values.erase(values.begin() + idx);
                    col_ind.erase(col_ind.begin() + idx);
                    for (int r = row + 1; r <= rows; ++r)
                        row_ptr[r]--;
                } else {
                    values[idx] = value;
                }
                return;
            }
        }

        // Entry doesn't exist
        if (value == T{})
            return; // No-op: setting a missing entry to zero

        int insert_pos = row_ptr[row + 1];
        col_ind.insert(col_ind.begin() + insert_pos, col);
        values.insert(values.begin() + insert_pos, value);
        for (int r = row + 1; r <= rows; ++r)
            row_ptr[r]++;
    }

    // Check whether a stored nonzero exists at (row, col).
    bool is_nonzero(int row, int col) const {
        bounds_check(row, col);
        for (int idx = row_ptr[row]; idx < row_ptr[row + 1]; ++idx) {
            if (col_ind[idx] == col)
                return true;
        }
        return false;
    }

    // Reference access. Inserts a zero entry if none exists (like std::map::operator[]).
    // For read-only access without insertion, use get().
    T& operator()(int row, int col) {
        bounds_check(row, col);
        for (int idx = row_ptr[row]; idx < row_ptr[row + 1]; ++idx) {
            if (col_ind[idx] == col)
                return values[idx];
        }
        // Insert a zero entry and return a reference to it
        int insert_pos = row_ptr[row + 1];
        col_ind.insert(col_ind.begin() + insert_pos, col);
        values.insert(values.begin() + insert_pos, T{});
        for (int r = row + 1; r <= rows; ++r)
            row_ptr[r]++;
        return values[insert_pos];
    }

    // Remove all structural zeros (entries stored with value == T{}).
    // Returns the number of entries removed.
    int prune() {
        int removed = 0;
        int write = 0;
        for (int r = 0; r < rows; ++r) {
            int new_start = write;
            for (int idx = row_ptr[r]; idx < row_ptr[r + 1]; ++idx) {
                if (values[idx] != T{}) {
                    values[write]  = values[idx];
                    col_ind[write] = col_ind[idx];
                    ++write;
                } else {
                    ++removed;
                }
            }
            row_ptr[r] = new_start;
        }
        row_ptr[rows] = write;
        values.resize(write);
        col_ind.resize(write);
        return removed;
    }

    // Comparison
    bool operator==(const CSR& other) const {
        return rows == other.rows
            && cols == other.cols
            && row_ptr == other.row_ptr
            && col_ind == other.col_ind
            && values  == other.values;
    }
    bool operator!=(const CSR& other) const {
        return !(*this == other);
    }
};


// ---------------------------------------------------------------------------
// SparseArray<T, Format> — high-level sparse matrix with operator() access.
// Format must expose: rows, cols, get(row,col), set(row,col,val), is_nonzero(row,col).
//
// Usage:
//     SparseArray<>              — double + CSR (both defaults)
//     SparseArray<float>         — float  + CSR
//     SparseArray<double, CSR>   — explicit
//     SparseArray<double, CSC>   — alternate format (TODO: implement CSC)
// ---------------------------------------------------------------------------
template <typename T = double, template<typename> class Format = CSR>
class SparseArray {
private:
    Format<T> _storage;

    void _dims_check(const SparseArray& other) const {
        if (_storage.rows != other._storage.rows || _storage.cols != other._storage.cols)
            throw std::invalid_argument("SparseArray dimension mismatch");
    }

public:
    using value_type  = T;
    using format_type = Format<T>;

    // Constructor
    explicit SparseArray(int rows, int cols)
        : _storage(rows, cols)
    {
        if (rows < 0 || cols < 0)
            throw std::invalid_argument("SparseArray dimensions must be non-negative");
    }

    // -- Dimensions ---------------------------------------------------------
    int rows() const { return _storage.rows; }
    int cols() const { return _storage.cols; }

    // -- Element access (array-like) ----------------------------------------

    // Read/write: arr(row, col) — forwards to Format::operator().
    // Inserts a zero entry if none exists (like std::map::operator[]).
    T& operator()(int row, int col) {
        return _storage(row, col);
    }

    // storage() — exposes the underlying Format for direct access to get/set/is_nonzero.
    const Format<T>& storage() const { return _storage; }
    Format<T>&       storage()       { return _storage; }

    // -- Comparison operators -----------------------------------------------
    // TODO: Get rid of these until different formats are implemented and we can compare across formats.
};

// -- Convenience aliases ----------------------------------------------------
template <typename T = double> using CSRArray = SparseArray<T, CSR>;
// template <typename T = double> using CSCArray = SparseArray<T, CSC>;
