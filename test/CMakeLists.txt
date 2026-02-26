include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.14.0
)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)

# ── Helper: register a test executable ───────────────────────────────────────
function(jumsparse_add_test TEST_NAME TEST_SOURCE)
    add_executable(${TEST_NAME} ${TEST_SOURCE})
    target_link_libraries(${TEST_NAME}
        PRIVATE jumsparse GTest::gtest_main
    )
    set_target_properties(${TEST_NAME} PROPERTIES
        CUDA_SEPARABLE_COMPILATION ON
    )
    gtest_discover_tests(${TEST_NAME})
endfunction()

# ── Format tests ─────────────────────────────────────────────────────────────
jumsparse_add_test(test_csr     formats/test_csr.cu)
jumsparse_add_test(test_csc     formats/test_csc.cu)
jumsparse_add_test(test_coo     formats/test_coo.cu)
jumsparse_add_test(test_cm_csr  formats/test_cm_csr.cu)

# ── Operation tests ──────────────────────────────────────────────────────────
jumsparse_add_test(test_spmv    ops/test_spmv.cu)
jumsparse_add_test(test_spmm    ops/test_spmm.cu)

# ── Solver tests ─────────────────────────────────────────────────────────────
jumsparse_add_test(test_jacobi       solvers/test_jacobi.cu)
jumsparse_add_test(test_gauss_seidel solvers/test_gauss_seidel.cu)
jumsparse_add_test(test_krylov       solvers/test_krylov.cu)

# ── Utility tests ────────────────────────────────────────────────────────────
jumsparse_add_test(test_reordering   utils/test_reordering.cu)
jumsparse_add_test(test_array        utils/test_array.cpp)
