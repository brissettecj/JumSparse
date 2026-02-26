#include <gtest/gtest.h>
#include <jumsparse/array.hpp>

namespace jumsparse::test {

TEST(SparseArrayTest, BasicFunctionality) {
    SparseArray<double> arr(3, 3);

    // Initially all zeros
    EXPECT_DOUBLE_EQ(arr.get(0, 0), 0.0);
    EXPECT_DOUBLE_EQ(arr.get(1, 1), 0.0);
    EXPECT_DOUBLE_EQ(arr.get(2, 2), 0.0);

    // Set some values
    arr.set(0, 1, 1.5);
    arr.set(1, 2, -2.5);
    arr.set(2, 0, 3.0);

    // Check values
    EXPECT_DOUBLE_EQ(arr.get(0, 1), 1.5);
    EXPECT_DOUBLE_EQ(arr.get(1, 2), -2.5);
    EXPECT_DOUBLE_EQ(arr.get(2, 0), 3.0);

    // Check that other entries are still zero
    EXPECT_DOUBLE_EQ(arr.get(0, 0), 0.0);
    EXPECT_DOUBLE_EQ(arr.get(1, 1), 0.0);
    EXPECT_DOUBLE_EQ(arr.get(2, 2), 0.0);

    // Update a value
    arr.set(1, 2, -3.5);
    EXPECT_DOUBLE_EQ(arr.get(1, 2), -3.5);

    // Remove a value by setting it to zero
    arr.set(0, 1, 0.0);
    EXPECT_DOUBLE_EQ(arr.get(0, 1), 0.0);
}

} // namespace jumsparse::test