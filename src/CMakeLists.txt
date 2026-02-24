set(JUMSPARSE_SOURCES
    formats/csr.cu
    formats/csc.cu
    formats/coo.cu
    formats/cm_csr.cu
    ops/spmv.cu
    ops/spmm.cu
    solvers/jacobi.cu
    solvers/gauss_seidel.cu
    solvers/over_relaxation.cu
    solvers/multigrid.cu
    solvers/krylov.cu
    utils/reordering.cu
)

add_library(jumsparse STATIC ${JUMSPARSE_SOURCES})

target_include_directories(jumsparse
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)

set_target_properties(jumsparse PROPERTIES
    CUDA_SEPARABLE_COMPILATION ON
    POSITION_INDEPENDENT_CODE  ON
)
