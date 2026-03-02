#include <gtest/gtest.h>
#include <jumsparse/array.hpp>

namespace jumsparse::test {

TEST(SparseArrayTest, BasicFunctionality) {
    SparseArray<double> arr(3, 3);

    // Initially all zeros (read via operator())
    EXPECT_DOUBLE_EQ(arr(0, 0), 0.0);
    EXPECT_DOUBLE_EQ(arr(1, 1), 0.0);
    EXPECT_DOUBLE_EQ(arr(2, 2), 0.0);

    // Set some values via operator()
    arr(0, 1) = 1.5;
    arr(1, 2) = -2.5;
    arr(2, 0) = 3.0;

    // Check values via operator()
    EXPECT_DOUBLE_EQ(arr(0, 1), 1.5);
    EXPECT_DOUBLE_EQ(arr(1, 2), -2.5);
    EXPECT_DOUBLE_EQ(arr(2, 0), 3.0);

    // Check that other entries are still zero
    EXPECT_DOUBLE_EQ(arr(0, 0), 0.0);
    EXPECT_DOUBLE_EQ(arr(1, 1), 0.0);
    EXPECT_DOUBLE_EQ(arr(2, 2), 0.0);

    // Update a value
    arr(1, 2) = -3.5;
    EXPECT_DOUBLE_EQ(arr(1, 2), -3.5);

    // Remove a value by setting it to zero
    arr(0, 1) = 0.0;
    EXPECT_DOUBLE_EQ(arr(0, 1), 0.0);
}

TEST(SparseArrayTest, OperatorParens) {
    SparseArray<double> arr(3, 3);

    arr(0, 0) = 5.0;
    arr(1, 2) = 7.0;

    // operator() read access
    EXPECT_DOUBLE_EQ(arr(0, 0), 5.0);
    EXPECT_DOUBLE_EQ(arr(1, 2), 7.0);
    EXPECT_DOUBLE_EQ(arr(2, 2), 0.0);  // missing entry returns zero
}

TEST(SparseArrayTest, IsNonzero) {
    SparseArray<double> arr(3, 3);

    arr(0, 1) = 1.0;
    arr(2, 0) = -4.5;

    EXPECT_TRUE(arr.storage().is_nonzero(0, 1));
    EXPECT_TRUE(arr.storage().is_nonzero(2, 0));
    EXPECT_FALSE(arr.storage().is_nonzero(0, 0));
    EXPECT_FALSE(arr.storage().is_nonzero(1, 1));

    // Remove entry via set() — operator() leaves structural zeros,
    // so set() is needed to actually erase the entry.
    arr.storage().set(0, 1, 0.0);
    EXPECT_FALSE(arr.storage().is_nonzero(0, 1));
}

TEST(CSRTest, DirectGetSet) {
    CSR<int> csr(2, 3);

    csr.set(0, 1, 10);
    csr.set(1, 0, 20);

    EXPECT_EQ(csr.get(0, 1), 10);
    EXPECT_EQ(csr.get(1, 0), 20);
    EXPECT_EQ(csr.get(0, 0), 0);

    EXPECT_TRUE(csr.is_nonzero(0, 1));
    EXPECT_FALSE(csr.is_nonzero(0, 2));
}

TEST(SparseArrayTest, Dimensions) {
    SparseArray<float> arr(4, 5);

    EXPECT_EQ(arr.rows(), 4);
    EXPECT_EQ(arr.cols(), 5);
}

TEST(SparseArrayTest, StorageAccessor) {
    SparseArray<double> arr(2, 2);
    arr(0, 0) = 1.0;

    const auto& storage = arr.storage();
    EXPECT_DOUBLE_EQ(storage.get(0, 0), 1.0);
    EXPECT_TRUE(storage.is_nonzero(0, 0));
    EXPECT_FALSE(storage.is_nonzero(1, 1));
}

TEST(CSRTest, Prune) {
    CSR<double> csr(3, 3);

    // Insert some values
    csr(0, 1) = 5.0;
    csr(1, 0) = 3.0;
    csr(2, 2) = 7.0;

    // Create structural zeros via operator()
    csr(0, 0) = 0.0;  // structural zero
    csr(1, 2) = 0.0;  // structural zero

    // 5 entries stored, 2 of which are structural zeros
    EXPECT_TRUE(csr.is_nonzero(0, 0));  // exists as structural zero
    EXPECT_EQ(csr.values.size(), 5u);

    // Prune removes structural zeros
    int removed = csr.prune();
    EXPECT_EQ(removed, 2);
    EXPECT_EQ(csr.values.size(), 3u);

    // Real values are preserved
    EXPECT_DOUBLE_EQ(csr.get(0, 1), 5.0);
    EXPECT_DOUBLE_EQ(csr.get(1, 0), 3.0);
    EXPECT_DOUBLE_EQ(csr.get(2, 2), 7.0);

    // Structural zeros are gone
    EXPECT_FALSE(csr.is_nonzero(0, 0));
    EXPECT_FALSE(csr.is_nonzero(1, 2));
}

TEST(SparseArrayTest, PruneViaStorage) {
    SparseArray<double> arr(2, 2);

    arr(0, 0) = 1.0;
    arr(0, 1) = 0.0;  // structural zero
    arr(1, 1) = 2.0;

    EXPECT_EQ(arr.storage().values.size(), 3u);

    arr.storage().prune();

    EXPECT_EQ(arr.storage().values.size(), 2u);
    EXPECT_DOUBLE_EQ(arr(0, 0), 1.0);
    EXPECT_DOUBLE_EQ(arr(1, 1), 2.0);
    EXPECT_FALSE(arr.storage().is_nonzero(0, 1));
}

} // namespace jumsparse::test