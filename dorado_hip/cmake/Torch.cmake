# Guard against double-inclusion errors. See https://github.com/pytorch/pytorch/issues/25004
include_guard(GLOBAL)

set(TORCH_VERSION 2.0.0)
unset(TORCH_PATCH_SUFFIX)

# If we're building with sanitizers then we want Torch to be dynamic since we don't build the
# static lib with instrumentation and hence get some false-positives.
if(ECM_ENABLE_SANITIZERS)
    set(TRY_USING_STATIC_TORCH_LIB FALSE)
else()
    set(TRY_USING_STATIC_TORCH_LIB TRUE)
endif()
set(USING_STATIC_TORCH_LIB FALSE)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    #find_package(CUDAToolkit REQUIRED)
    # the torch cuda.cmake will set(CUDAToolkit_ROOT "${CUDA_TOOLKIT_ROOT_DIR}") [2]
    # so we need to make CUDA_TOOLKIT_ROOT_DIR is set correctly as per [1]
    # 1. https://cmake.org/cmake/help/latest/module/FindCUDAToolkit.html
    # 2. https://github.com/pytorch/pytorch/blob/5fa71207222620b4efb78989849525d4ee6032e8/cmake/public/cuda.cmake#L40
    set(CMAKE_CUDA_COMPILER "/opt/cray/pe/craype/2.7.23/bin/CC")

    # use python3 to compute shorthash for libnvrtc.so
    # https://github.com/pytorch/pytorch/blob/7289d22d6749465d3bae2cb5a6ce04729318f55b/cmake/public/cuda.cmake#L173
    find_package(Python3 COMPONENTS "Interpreter" REQUIRED)
    set(PYTHON_EXECUTABLE ${Python3_EXECUTABLE})
endif()
set(DORADO_LIBTORCH_DIR "/pfs/lustrep1/projappl/project_465000096/Dorado/dorado_env/lib/python3.9/site-packages/torch/lib")
set(TORCH_LIB ${DORADO_LIBTORCH_DIR})


# Our libtorch should be chosen over any others on the system
list(PREPEND CMAKE_PREFIX_PATH "${TORCH_LIB}")


#find_package(Torch REQUIRED)


# Static builds require a few libs to be added
if (USING_STATIC_TORCH_LIB)
    if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        list(APPEND TORCH_LIBRARIES
            # Missing libs that Torch forgets to link to
            ${TORCH_LIB}/lib/libbreakpad.a
            ${TORCH_LIB}/lib/libbreakpad_common.a

            # Some of the CUDA libs have inter-dependencies, so group them together
            -Wl,--start-group
                CUDA::hiprt_static
                CUDA::hipblas_static
                CUDA::hipblasLt_static
                # AFAICT Torch doesn't provide the symbol required for the callback, so use the nocallback variant
                CUDA::hipfft_static_nocallback
                CUDA::hipsolver_static
                # cusolver is missing this and I don't know why
                ${CUDAToolkit_TARGET_DIR}/lib64/liblapack_static.a
                CUDA::hipsparse_static
                CUDA::cupti
                CUDA::hiprand_static
                CUDA::nvrtc
                CUDA::culibos
            -Wl,--end-group
            # OMP implementation (i=Intel, g=GNU)
            ${TORCH_LIB}/lib/libgomp.so.1.0.0
            # BLAS rather than MKL
            ${TORCH_LIB}/lib/libopenblas.a
            ${TORCH_LIB}/lib/libgfortran.so.4.0.0
        )

        # Setup differences between platforms
        if (CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
            list(APPEND TORCH_LIBRARIES
                # These 2 libs depend on each other, but only libdnnl.a is added to Torch's install cmake, so we
                # need to add it again after bringing in libdnnl_graph.a to fill in the missing symbols.
                ${TORCH_LIB}/lib/libdnnl_graph.a
                ${TORCH_LIB}/lib/libdnnl.a
            )
            set(ont_torch_extra_cuda_libs
                # I don't know why the MKL libs need to be part of the CUDA group, but having them in a
                # separate group causes missing symbol errors
                ${TORCH_LIB}/lib/libmkl_core.a
                ${TORCH_LIB}/lib/libmkl_intel_lp64.a
                ${TORCH_LIB}/lib/libmkl_intel_thread.a
            )
            set(ont_torch_extra_platform_libs
                ${TORCH_LIB}/lib/libnccl_static.a
                ${TORCH_LIB}/lib/libiomp5.so
            )
        else()
            set(ont_torch_extra_cuda_libs
                # cusolver is missing this and I don't know why
                ${CUDAToolkit_TARGET_DIR}/lib64/liblapack_static.a
                CUDA::hiprand_static
                CUDA::nvrtc
            )
            set(ont_torch_extra_platform_libs
                ${TORCH_LIB}/lib/libopenblas.a
                ${TORCH_LIB}/lib/libgfortran.so.5
                ${TORCH_LIB}/lib/libgomp.so.1.0.0
                numa
            )
        endif()

        # Currently we need to make use of a separate lib to avoid getting relocation errors at link time
        # because the final binary would end up too big.
        # See https://github.com/pytorch/pytorch/issues/39968
        set(USE_TORCH_HELPER_LIB TRUE)
        if (USE_TORCH_HELPER_LIB)
            add_library(dorado_torch_lib SHARED
                # We need to use listdir here so projects including us use the correct path
                ${CMAKE_CURRENT_LIST_DIR}/../dorado/torch_half.cpp
            )
            target_link_libraries(dorado_torch_lib PRIVATE
                ${TORCH_LIBRARIES}
                # Some of the CUDA libs have inter-dependencies, so group them together
                -Wl,--start-group
                    ${ont_cuda_internal_linkage_libs}
                -Wl,--end-group
            )
            target_include_directories(dorado_torch_lib PUBLIC
                ${TORCH_INCLUDE_DIRS}
            )
            # Replace the torch libs with the helper lib
            set(TORCH_LIBRARIES dorado_torch_lib)
            # Don't forget to install it
            install(TARGETS dorado_torch_lib LIBRARY)
        endif()

        list(APPEND TORCH_LIBRARIES
            # Some of the CUDA libs have inter-dependencies, so group them together
            -Wl,--start-group
                CUDA::hiprt_static
                CUDA::hipblas_static
                CUDA::hipblasLt_static
                ${ont_cufft_static_libs}
                CUDA::hipsolver_static
                CUDA::hipsparse_static
                ${ont_cuda_internal_linkage_libs}
                ${ont_torch_extra_cuda_libs}
            -Wl,--end-group
            ${ont_torch_extra_platform_libs}
        )

        if (${CMAKE_VERSION} VERSION_LESS 3.23.4 AND EXISTS ${CUDAToolkit_TARGET_DIR}/lib64/libcusolver_lapack_static.a)
            # CUDA::cusolver_static is missing the cusolver_lapack_static target+dependency in older versions of cmake
            list(APPEND TORCH_LIBRARIES
                ${CUDAToolkit_TARGET_DIR}/lib64/libcusolver_lapack_static.a
            )
        endif()
    endif()
endif()