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

if(CMAKE_SYSTEM_NAME STREQUAL "Linux" OR WIN32)
    set(CMAKE_HIP_COMPILER /opt/cray/pe/craype/2.7.23/bin/CC)
    ##find_package(CUDAToolkit REQUIRED)
    # the torch cuda.cmake will set(CUDAToolkit_ROOT "${CUDA_TOOLKIT_ROOT_DIR}") [2]
    # so we need to make CUDA_TOOLKIT_ROOT_DIR is set correctly as per [1]
    # 1. https://cmake.org/cmake/help/latest/module/FindCUDAToolkit.html
    # 2. https://github.com/pytorch/pytorch/blob/5fa71207222620b4efb78989849525d4ee6032e8/cmake/public/cuda.cmake#L40
    ##if(DEFINED CUDAToolkit_ROOT)
    ##  set(CUDA_TOOLKIT_ROOT_DIR ${CUDAToolkit_ROOT})
    ##else()
        # Bodge for Torch, since static linking assumes find_package(CUDA) has already been called
    ##    find_package(CUDA REQUIRED)
    ##endif()
    ##if(NOT DEFINED CMAKE_CUDA_COMPILER)
    ##  if(DEFINED CUDAToolkit_ROOT)
    ##    set(CMAKE_CUDA_COMPILER ${CUDAToolkit_ROOT}/bin/nvcc)
    ##  else()
    ##    set(CMAKE_CUDA_COMPILER ${CUDAToolkit_NVCC_EXECUTABLE})
    ##  endif()
    ##endif()

    # use python3 to compute shorthash for libnvrtc.so
    # https://github.com/pytorch/pytorch/blob/7289d22d6749465d3bae2cb5a6ce04729318f55b/cmake/public/cuda.cmake#L173
    find_package(Python COMPONENTS "Interpreter" REQUIRED)
    set(PYTHON_EXECUTABLE ${Python_EXECUTABLE})

    set(CUDNN_LIBRARY_PATH ${DORADO_3RD_PARTY_SOURCE}/fake_cudnn/libcudnn.a)
    set(CUDNN_INCLUDE_PATH ${DORADO_3RD_PARTY_SOURCE}/fake_cudnn)

    ##set(CMAKE_CUDA_ARCHITECTURES 62 70 72 75)
    ##if(${CUDAToolkit_VERSION} VERSION_GREATER_EQUAL 11.3)
    ##    list(APPEND CMAKE_CUDA_ARCHITECTURES 80 86)
    ##endif()
    ##if(${CUDAToolkit_VERSION} VERSION_GREATER_EQUAL 11.4)
    ##  list(APPEND CMAKE_CUDA_ARCHITECTURES 87)
    ##endif()
    ##if(${CUDAToolkit_VERSION} VERSION_GREATER_EQUAL 11.8)
    ##  list(APPEND CMAKE_CUDA_ARCHITECTURES 90)
    ##endif()
endif()

if(DEFINED DORADO_LIBTORCH_DIR)
    # Use the existing libtorch we have been pointed at
    message(STATUS "Using existing libtorch at ${DORADO_LIBTORCH_DIR}")

    set(TORCH_LIB ${DORADO_LIBTORCH_DIR})

endif()

# Our libtorch should be chosen over any others on the system
list(PREPEND CMAKE_PREFIX_PATH "${TORCH_LIB}")

if (APPLE AND CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
    # For some reason the RPATHs of the dylibs are pointing to the libiomp5.dylib in functools rather
    # than the one that's next to them, so correct that here before we import the package.
    file(GLOB TORCH_DYLIBS "${TORCH_LIB}/lib/*.dylib")
    foreach(TORCH_DYLIB IN LISTS TORCH_DYLIBS)
        execute_process(
            COMMAND
                ${CMAKE_INSTALL_NAME_TOOL}
                -change "@loader_path/../../functorch/.dylibs/libiomp5.dylib" "@loader_path/libiomp5.dylib"
                ${TORCH_DYLIB}
            RESULT_VARIABLE RETVAL
            OUTPUT_VARIABLE OUTPUT
            ERROR_VARIABLE ERRORS
        )
        if (NOT RETVAL EQUAL 0)
            message(FATAL_ERROR "Error running ${CMAKE_INSTALL_NAME_TOOL}: ${RETVAL}\nOUTPUT=${OUTPUT}\nERRORS=${ERRORS}")
        endif()
    endforeach()
endif()

find_package(Torch REQUIRED)

if(APPLE)
    set(TORCH_BUILD_VERSION ${TORCH_VERSION})
else()
    if(EXISTS "${TORCH_LIB}/build-version")
        file(STRINGS "${TORCH_LIB}/build-version" TORCH_BUILD_VERSION)
    else()
        set(PYTORCH_BUILD_VERSION "import torch; print('%s+cu%s' % (torch.__version__, torch.version.cuda.replace('.', '')), end='')")
        execute_process(COMMAND python3 -c "${PYTORCH_BUILD_VERSION}" OUTPUT_VARIABLE TORCH_BUILD_VERSION WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
    endif()
    message(STATUS "TORCH_BUILD_VERSION: ${TORCH_BUILD_VERSION}")
endif()

if (NOT TORCH_BUILD_VERSION VERSION_EQUAL TORCH_VERSION)
  message(WARNING "expected ${TORCH_VERSION} but found ${TORCH_BUILD_VERSION}")
endif()

if(WIN32 AND DEFINED MKL_ROOT)
    link_directories(${MKL_ROOT}/lib/intel64)
endif()

# Static builds require a few libs to be added
if (USING_STATIC_TORCH_LIB)
    if(WIN32)
        list(APPEND TORCH_LIBRARIES
            CUDA::hiprt_static
            CUDA::hipblas
            CUDA::hipfft
            CUDA::hipsolver
            CUDA::hipsparse
        )

    elseif(APPLE)
        find_library(ACCELERATE_FRAMEWORK Accelerate REQUIRED)
        find_library(FOUNDATION_FRAMEWORK Foundation REQUIRED)
        list(APPEND TORCH_LIBRARIES
            ${ACCELERATE_FRAMEWORK}
            ${FOUNDATION_FRAMEWORK}
        )
        if (NOT CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
            find_library(METAL_FRAMEWORK Metal REQUIRED)
            find_library(MPS_FRAMEWORK MetalPerformanceShaders REQUIRED)
            find_library(MPSG_FRAMEWORK MetalPerformanceShadersGraph REQUIRED)
            list(APPEND TORCH_LIBRARIES
                ${METAL_FRAMEWORK}
                ${MPS_FRAMEWORK}
                ${MPSG_FRAMEWORK}
            )
        endif()

    elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND ${CUDAToolkit_VERSION} VERSION_LESS 11.0)
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

    elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        # Older versions of cmake don't have the static_nocallback target for cuFFT that Torch needs, so we
        # make it ourselves.
        if (TARGET CUDA::hipfft_static_nocallback)
            set(ont_cufft_static_libs CUDA::hipfft_static_nocallback)
        else()
            set(ont_cufft_static_libs ${CUDAToolkit_TARGET_DIR}/lib64/libhipfft_static_nocallback.a)
        endif()

        # Some CUDA lib symbols have internal linkage, so they must be part of the helper lib too
        set(ont_cuda_internal_linkage_libs CUDA::culibos CUDA::cudart_static)
        if (TARGET CUDA::cupti_static)
            list(APPEND ont_cuda_internal_linkage_libs CUDA::cupti_static)
        elseif(TARGET CUDA::cupti)
            # CUDA::cupti appears to be static if CUDA::cupti_static doesn't exist
            list(APPEND ont_cuda_internal_linkage_libs CUDA::cupti)
        elseif(EXISTS ${CUDAToolkit_TARGET_DIR}/extras/CUPTI/lib64/libcupti_static.a)
            # CMake sometimes can't find cupti for reasons which are not fully clear
            list(APPEND ont_cuda_internal_linkage_libs ${CUDAToolkit_TARGET_DIR}/extras/CUPTI/lib64/libcupti_static.a)
        else()
            message(FATAL_ERROR "Can't find CUPTI")
        endif()

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
                CUDA::curand_static
                CUDA::nvrtc
            )
            set(ont_torch_extra_platform_libs
                ${TORCH_LIB}/lib/libopenblas.a
                ${TORCH_LIB}/lib/libgfortran.so.5
                ${TORCH_LIB}/lib/libgomp.so.1.0.0
                numa
            )
        endif()

        # Link to the cuDNN libs
        list(APPEND TORCH_LIBRARIES
            # Note: the order of the cuDNN libs matter
            # We aren't going to do any training, so these don't need to be whole-archived
            ${TORCH_LIB}/lib/libcudnn_adv_train_static.a
            ${TORCH_LIB}/lib/libcudnn_cnn_train_static.a
            ${TORCH_LIB}/lib/libcudnn_ops_train_static.a
            # I'm assuming we need this for https://github.com/pytorch/pytorch/issues/50153
            -Wl,--whole-archive
                # Note: libtorch is still setup to link to these dynamically (https://github.com/pytorch/pytorch/issues/81692)
                # though that shouldn't be a problem on Linux
                ${TORCH_LIB}/lib/libcudnn_adv_infer_static.a
                ${TORCH_LIB}/lib/libcudnn_cnn_infer_static.a
                ${TORCH_LIB}/lib/libcudnn_ops_infer_static.a
            -Wl,--no-whole-archive
        )

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
                ${ATen_INCLUDE_DIR}
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
                ${ont_hipfft_static_libs}
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
