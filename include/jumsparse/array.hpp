#pragma once

#include <vector>
#include <stdexcept>

// ---------------------------------------------------------------------------
// SparseArray<T> — sparse array using compressed sparse row/column format.
// ---------------------------------------------------------------------------
// Feb 25, 2026: 
//     Working on preliminary design for only CSR format. Building as immutable
//     size, mutable values. Will add CSC and mutability later if needed.
// ---------------------------------------------------------------------------
template <typename T>
class SparseArray {
private:
    int _rows, _cols;
    std::vector<int> _row_ptr;  // size rows + 1
    std::vector<int> _col_ind;  // size number of nonzeros (nnz)
    std::vector<T>   _values;   // size nnz

    void _bounds_check(int row, int col) const {
        if (row < 0 || row >= _rows || col < 0 || col >= _cols)
            throw std::out_of_range("SparseArray index out of bounds");
    }

    void _dims_check(const SparseArray& other) const {
        if (_rows != other._rows || _cols != other._cols)
            throw std::invalid_argument("SparseArray dimension mismatch");
    }

public:
    // Constructor
    explicit SparseArray(int rows, int cols)
        : _rows(rows), _cols(cols), _row_ptr(rows + 1, 0)
    {
        if (rows < 0 || cols < 0)
            throw std::invalid_argument("SparseArray dimensions must be non-negative");
    }


    // -- Element access -----------------------------------------------------

    // Read-only access. Returns zero for missing entries.
    T get(int row, int col) const {
        _bounds_check(row, col);
        for (int idx = _row_ptr[row]; idx < _row_ptr[row + 1]; ++idx) {
            if (_col_ind[idx] == col)
                return _values[idx];
        }
        return T{};
    }

    // Write access. Inserts, updates, or removes entries as needed.
    void set(int row, int col, T value) {
        _bounds_check(row, col);

        for (int idx = _row_ptr[row]; idx < _row_ptr[row + 1]; ++idx) {
            if (_col_ind[idx] == col) {
                if (value == T{}) {
                    // Remove existing nonzero
                    _values.erase(_values.begin() + idx);
                    _col_ind.erase(_col_ind.begin() + idx);
                    for (int r = row + 1; r <= _rows; ++r)
                        _row_ptr[r]--;
                } else {
                    _values[idx] = value;
                }
                return;
            }
        }

        // Entry doesn't exist
        if (value == T{})
            return; // No-op: setting a missing entry to zero

        int insert_pos = _row_ptr[row + 1];
        _col_ind.insert(_col_ind.begin() + insert_pos, col);
        _values.insert(_values.begin() + insert_pos, value);
        for (int r = row + 1; r <= _rows; ++r)
            _row_ptr[r]++;
    }

    // Comparison operators
    bool operator==(const SparseArray& other) const {
        return _rows == other._rows
            && _cols == other._cols
            && _row_ptr == other._row_ptr
            && _col_ind == other._col_ind
            && _values  == other._values;
    }
    bool operator!=(const SparseArray& other) const {
        return !(*this == other);
    }
};
