cmake_minimum_required(VERSION 3.20)
set(CMAKE_POLICY_DEFAULT_CMP0063 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

project(dorado)

# Find Python interpreter
find_package(Python REQUIRED)

set(PYTHON_EXECUTABLE ${Python_EXECUTABLE})

# Get the site-packages directory
execute_process(
  COMMAND ${PYTHON_EXECUTABLE} -c "from distutils.sysconfig import get_python_lib; print(get_python_lib())"
  OUTPUT_VARIABLE PYTHON_SITE_PACKAGES
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

set(CMAKE_PREFIX_PATH "${PYTHON_SITE_PACKAGES}" ${CMAKE_PREFIX_PATH})

# Print the site-packages directory
message(STATUS "Python site-packages directory: ${PYTHON_SITE_PACKAGES}")

# Add Torch & ATen includes
set(ATen_INCLUDE_DIR ${PYTHON_SITE_PACKAGES}/torch/include)

set(TORCH_INCLUDE_DIRS ${PYTHON_SITE_PACKAGES}/torch/include/torch/csrc/api/include)

message(STATUS "--PROJECT_SOURCE_DIR: ${PROJECT_SOURCE_DIR}")

message(STATUS "--ATen_INCLUDE_DIR: ${ATen_INCLUDE_DIR}")
message(STATUS "--TORCH_INCLUDE_DIRS: ${TORCH_INCLUDE_DIRS}")

# Check if the header file is found
if(NOT ATen_INCLUDE_DIR)
    message(FATAL_ERROR "ATen/ATen.h not found. PyTorch installation may be incomplete.")
endif()

get_cmake_property(MULTI_CONFIG GENERATOR_IS_MULTI_CONFIG)
if (NOT MULTI_CONFIG)
  if (NOT EXISTS ${CMAKE_BINARY_DIR}/CMakeCache.txt AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
  endif()
message(STATUS "Build type: " ${CMAKE_BUILD_TYPE})
endif()


if(WIN32)
  # all third parties are release builds, so we must match the windows runtime
  set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreadedDLL)
  message(STATUS "toolset: ${CMAKE_GENERATOR_TOOLSET}")
  message(STATUS "platform: ${CMAKE_GENERATOR_PLATFORM}")
  message(STATUS "vs platform: ${CMAKE_VS_PLATFORM_NAME}")
endif()

if (APPLE AND NOT ECM_ENABLE_SANITIZERS)
    # Prefer static libs if they exist so that we don't run into issues
    # linking to dynamic libraries that live in brew.
    list(PREPEND CMAKE_FIND_LIBRARY_SUFFIXES ".a")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)

function(enable_warnings_as_errors TARGET_NAME)
    if(WIN32)
        # W4 - warning level 4.
        # WX - warnings as errors.
        # external:anglebrackets - treat <header> includes as external.
        # external:W0 - disable warnings for external code.
        target_compile_options(${TARGET_NAME} PRIVATE
          $<$<NOT:$<COMPILE_LANGUAGE:CUDA>>:/W4 /WX /external:anglebrackets /external:W0>
        )
        target_compile_definitions(${TARGET_NAME} PRIVATE
            _CRT_SECURE_NO_WARNINGS
        )
    else()
        target_compile_options(${TARGET_NAME} PRIVATE -Wall -Werror -Wextra)
    endif()
endfunction()

function(disable_warnings TARGET_NAME)
    if(WIN32)
        target_compile_options(${TARGET_NAME} PRIVATE
            $<$<NOT:$<COMPILE_LANGUAGE:CUDA>>:/W0>
        )
    endif()
endfunction()

set(DORADO_3RD_PARTY_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/dorado/3rdparty)
set(DORADO_3RD_PARTY_DOWNLOAD ${CMAKE_CURRENT_SOURCE_DIR}/dorado/3rdparty
    CACHE PATH "Location to download 3rdparty libraries into")

# We don't support GPU builds on macOS/x64
if (APPLE AND CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
    set(DORADO_GPU_BUILD FALSE)
else()
    set(DORADO_GPU_BUILD TRUE)
endif()

#find_package(CUDAToolkit QUIET)

#if(${CUDAToolkit_FOUND})
#  file(REAL_PATH ${CUDAToolkit_TARGET_DIR} CUDAToolkit_REAL_DIR)
#  message(STATUS "Found CUDA ${CUDAToolkit_VERSION} (${CUDAToolkit_TARGET_DIR} -> ${CUDAToolkit_REAL_DIR})")
#endif()

include(cmake/DoradoVersion.cmake)
include(cmake/UpdateSubmodules.cmake)
include(cmake/DownloadAndExtract.cmake)
include(cmake/SharedLibHelpers.cmake)

include(cmake/HDF5.cmake)
include(cmake/VbzPlugin.cmake)
include(cmake/Koi.cmake)
include(cmake/Pod5.cmake)
include(cmake/Torch.cmake)
include(cmake/OpenSSL.cmake)
include(cmake/Htslib.cmake)
if (DORADO_GPU_BUILD AND APPLE)
    include(cmake/Metal.cmake)
endif()

# Add sanitizer options to compilation flags
include(cmake/ECMEnableSanitizers.cmake)
if (ECM_ENABLE_SANITIZERS)
    # Always emit debug info to provide better stack traces
    add_compile_options(-g)
    # Add a way to detect if we're compiling with sanitizers enabled, since UBSan
    # detection isn't available in GCC
    foreach (SANITIZER ${ECM_ENABLE_SANITIZERS})
        string(TOUPPER ${SANITIZER} SANITIZER)
        add_compile_definitions(DORADO_SANITIZE_${SANITIZER})
    endforeach()
endif()

if(DORADO_USING_OLD_CPP_ABI)
  # We need to force the use of the old ABI here, if we are building in an old ABI context, as otherwise elzip builds
  # with the libc++11 ABI and we can't link against it.
  add_compile_definitions(_GLIBCXX_USE_CXX11_ABI=0)
endif()

# Use ccache for C and C++ if it's available
find_program(CCACHE_EXE ccache)
if (CCACHE_EXE)
    set(CMAKE_C_COMPILER_LAUNCHER ${CCACHE_EXE})
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_EXE})
    set(DORADO_ENABLE_PCH FALSE)
else()
    # Fallback to using a PCH if we don't have ccache support, since making them work together isn't simple.
    set(DORADO_ENABLE_PCH TRUE)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	#    if((CMAKE_SYSTEM_PROCESSOR MATCHES "^aarch64*|^arm*") AND (${CUDAToolkit_VERSION} VERSION_LESS 11.0))
        add_compile_definitions(DORADO_TX2)
	#endif()
endif()

# Bring in spdlog
add_subdirectory(${DORADO_3RD_PARTY_SOURCE}/spdlog)
# Avoid namespace clashes with static torch.
target_compile_definitions(spdlog PUBLIC
    "FMT_BEGIN_NAMESPACE=namespace fmt { inline namespace ont {"
    "FMT_END_NAMESPACE=}}"
)

# ELZIP_DECOMPRESS_ONLY stops minizip from adding OpenSSL as a target, preventing use of three dylibs on osx.
set(ELZIP_DECOMPRESS_ONLY ON)
add_subdirectory(${DORADO_3RD_PARTY_SOURCE}/elzip)

option(ONT_MM2_EXE "Build minimap2 exe" OFF)
add_subdirectory(${DORADO_3RD_PARTY_SOURCE}/ont-minimap2 EXCLUDE_FROM_ALL)
message(STATUS "build minimap2 exe: ${ONT_MM2_EXE}")
message(STATUS "build minimap2 lib: ${ONT_MM2_LIB}")
if (ONT_MM2_EXE)
    install(TARGETS minimap2_exe)
    add_custom_target(testing_deps ALL DEPENDS minimap2_exe)
endif()
# Disable warnings from minimap source
disable_warnings(minimap2)

set(BUILD_TESTING OFF)
add_subdirectory(${DORADO_3RD_PARTY_SOURCE}/edlib EXCLUDE_FROM_ALL)
# Disable warnings from edlib source
disable_warnings(edlib)
set(BUILD_TESTING ON)

add_subdirectory(${DORADO_3RD_PARTY_SOURCE}/date EXCLUDE_FROM_ALL)

enable_testing()

add_subdirectory(dorado/utils)
add_subdirectory(dorado/models)
add_subdirectory(dorado/basecall)
add_subdirectory(dorado/modbase)

#if("${CUDA_VERSION}" STREQUAL "")
#  set(CUDA_VERSION ${CUDAToolkit_VERSION})
#else()
#  message(STATUS "CUDA VERSION ALREADY SET TO ${CUDA_VERSION}")
#endif()

configure_file(dorado/Version.h_hip.in dorado/Version.h)

set(LIB_SOURCE_FILES
    dorado/alignment/IndexFileAccess.cpp
    dorado/alignment/IndexFileAccess.h
    dorado/alignment/Minimap2Aligner.cpp
    dorado/alignment/Minimap2Aligner.h
    dorado/alignment/Minimap2Index.cpp
    dorado/alignment/Minimap2Index.h
    dorado/alignment/Minimap2IndexSupportTypes.h
    dorado/alignment/Minimap2Options.h
    dorado/api/runner_creation.cpp
    dorado/api/runner_creation.h
    dorado/api/pipeline_creation.cpp
    dorado/api/pipeline_creation.h
    dorado/read_pipeline/FakeDataLoader.cpp
    dorado/read_pipeline/FakeDataLoader.h
    dorado/read_pipeline/ReadPipeline.cpp
    dorado/read_pipeline/ReadPipeline.h
    dorado/read_pipeline/ClientInfo.h
    dorado/read_pipeline/DefaultClientInfo.h
    dorado/read_pipeline/ScalerNode.cpp
    dorado/read_pipeline/ScalerNode.h
    dorado/read_pipeline/StereoDuplexEncoderNode.cpp
    dorado/read_pipeline/StereoDuplexEncoderNode.h
    dorado/read_pipeline/BasecallerNode.cpp
    dorado/read_pipeline/BasecallerNode.h
    dorado/read_pipeline/ModBaseCallerNode.cpp
    dorado/read_pipeline/ModBaseCallerNode.h
    dorado/read_pipeline/ReadFilterNode.cpp
    dorado/read_pipeline/ReadFilterNode.h
    dorado/read_pipeline/ReadToBamTypeNode.cpp
    dorado/read_pipeline/ReadToBamTypeNode.h
    dorado/read_pipeline/SubreadTaggerNode.cpp
    dorado/read_pipeline/SubreadTaggerNode.h
    dorado/read_pipeline/BaseSpaceDuplexCallerNode.cpp
    dorado/read_pipeline/BaseSpaceDuplexCallerNode.h
    dorado/read_pipeline/AlignerNode.cpp
    dorado/read_pipeline/AlignerNode.h
    dorado/read_pipeline/HtsReader.cpp
    dorado/read_pipeline/HtsReader.h
    dorado/read_pipeline/HtsWriter.cpp
    dorado/read_pipeline/HtsWriter.h
    dorado/read_pipeline/ProgressTracker.h
    dorado/read_pipeline/ResumeLoaderNode.cpp
    dorado/read_pipeline/ResumeLoaderNode.h
    dorado/read_pipeline/DuplexReadTaggingNode.cpp
    dorado/read_pipeline/DuplexReadTaggingNode.h
    dorado/read_pipeline/BarcodeClassifierNode.cpp
    dorado/read_pipeline/BarcodeClassifierNode.h
    dorado/read_pipeline/BarcodeDemuxerNode.cpp
    dorado/read_pipeline/BarcodeDemuxerNode.h
    dorado/read_pipeline/AdapterDetectorNode.cpp
    dorado/read_pipeline/AdapterDetectorNode.h
    dorado/read_pipeline/NullNode.h
    dorado/read_pipeline/NullNode.cpp
    dorado/read_pipeline/PairingNode.cpp
    dorado/read_pipeline/PairingNode.h
    dorado/read_pipeline/PolyACalculator.cpp
    dorado/read_pipeline/PolyACalculator.h
    dorado/read_pipeline/read_utils.cpp
    dorado/read_pipeline/read_utils.h
    dorado/read_pipeline/stereo_features.cpp
    dorado/read_pipeline/stereo_features.h
    dorado/read_pipeline/stitch.cpp
    dorado/read_pipeline/stitch.h
    dorado/read_pipeline/ReadSplitNode.cpp
    dorado/read_pipeline/ReadSplitNode.h
    dorado/splitter/DuplexReadSplitter.cpp
    dorado/splitter/DuplexReadSplitter.h
    dorado/splitter/RNAReadSplitter.cpp
    dorado/splitter/RNAReadSplitter.h
    dorado/splitter/splitter_utils.cpp
    dorado/splitter/splitter_utils.h
    dorado/demux/AdapterDetector.cpp
    dorado/demux/AdapterDetector.h
    dorado/demux/BarcodeClassifier.cpp
    dorado/demux/BarcodeClassifier.h
    dorado/demux/BarcodeClassifierSelector.cpp
    dorado/demux/BarcodeClassifierSelector.h
    dorado/demux/Trimmer.cpp
    dorado/demux/Trimmer.h
    dorado/demux/parse_custom_kit.cpp
    dorado/demux/parse_custom_kit.h
 )

add_library(dorado_lib ${LIB_SOURCE_FILES})

enable_warnings_as_errors(dorado_lib)

set_target_properties(dorado_lib
    PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

target_compile_definitions(dorado_lib
    PUBLIC
        DORADO_GPU_BUILD=$<BOOL:${DORADO_GPU_BUILD}>
)

target_include_directories(dorado_lib
    PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}/dorado
        ${CMAKE_CURRENT_SOURCE_DIR}/dorado
        ${POD5_INCLUDE}
)
# 3rdparty libs should be considered SYSTEM headers
target_include_directories(dorado_lib
    SYSTEM
    PUBLIC
        ${TORCH_INCLUDE_DIRS}
        ${HDF5_INCLUDE_DIRS}
        ${DORADO_3RD_PARTY_SOURCE}/HighFive/include
        ${DORADO_3RD_PARTY_SOURCE}/argparse
        ${DORADO_3RD_PARTY_SOURCE}/toml11
        ${DORADO_3RD_PARTY_SOURCE}/hdf_plugins/vbz_plugin
        ${DORADO_3RD_PARTY_SOURCE}/cxxpool/src
        ${DORADO_3RD_PARTY_SOURCE}/NVTX/c/include
        ${DORADO_3RD_PARTY_SOURCE}/indicators/include
	${ATen_INCLUDE_DIR}  # Include ATen's include directory
)

enable_testing()

if(NOT SKIP_HDF_PLUGINS)
  # Can skip this if we're consuming the lib from a parent project that already has hdf_plugins
  add_hdf_vbz_plugin()
endif()

if (DORADO_ENABLE_PCH)
    target_precompile_headers(dorado_lib REUSE_FROM dorado_utils)
endif()

target_link_libraries(dorado_lib
    PUBLIC
        ${TORCH_LIBRARIES}
        htslib
        vbz_hdf_plugin
        edlib
        dorado_utils
        dorado_basecall
        dorado_modbase
    PRIVATE
        minimap2
)

if(NOT WIN32)
    add_dependencies(dorado_lib htslib_project)
endif()

if(NOT DORADO_DISABLE_DORADO)
    if(NOT WIN32)
        # Set up RPATHs so we can find dependencies
        set(CMAKE_SKIP_RPATH FALSE)
        # Note: we don't need the relative lib dir if everything is in
        if(APPLE)
            set(CMAKE_INSTALL_RPATH "@executable_path/;@executable_path/../lib")
        else()
            set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib/;$ORIGIN")
        endif()
        set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
    endif()

    add_executable(dorado
        dorado/main.cpp
        dorado/cli/aligner.cpp
        dorado/cli/demux.cpp
        dorado/cli/duplex.cpp
        dorado/cli/trim.cpp
        dorado/cli/basecaller.cpp
        dorado/cli/benchmark.cpp
        dorado/cli/download.cpp
        dorado/cli/summary.cpp
        dorado/cli/cli.h
        dorado/cli/cli_utils.h
    )

    set_target_properties(dorado
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )

    if(MSVC)
        set_target_properties(dorado PROPERTIES LINK_OPTIONS "/ignore:4099")
    endif()

    enable_warnings_as_errors(dorado)

    if (DORADO_ENABLE_PCH)
        target_precompile_headers(dorado REUSE_FROM dorado_lib)
    endif()

    target_link_libraries(dorado
        dorado_lib
        dorado_io_lib
        dorado_models_lib
        minimap2
    )

    if (DEFINED DORADO_INSTALL_PATH)
        set(CMAKE_INSTALL_PREFIX ${DORADO_INSTALL_PATH})
    else()
        set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/../dist)
    endif()

    install(TARGETS dorado RUNTIME)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    # CUDA toolkit DLLs we depend on:
    set(VERSIONED_CUDA_LIBS
        libhipblas*.so.*
        libhiprt*.so.*
        libnvrtc*.so.*
        libnvToolsExt*.so.*
    )

    if(CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64")
        list(APPEND VERSIONED_CUDA_LIBS
            "*hipsparse.so*"
            "*hiprand.so*"
            "*hipsolver.so*"
            "*hipfft.so*"
        )
    endif()

    foreach(LIB IN LISTS VERSIONED_CUDA_LIBS)
        # torch may bundle it's own specific copy of the cuda libs. if it does, we want everything to point at them
        file(GLOB TORCH_CUDA_LIBS "${TORCH_LIB}/lib/${LIB}")
        if(TORCH_CUDA_LIBS)
            foreach(TORCH_CUDA_LIB IN LISTS TORCH_CUDA_LIBS)
                # create links to the torch bundled libs with hashes in the name
                # e.g. libcublas.so.11 => libcublas-3b81d170.so.11
                set(target ${TORCH_CUDA_LIB})
                string(REGEX REPLACE "-[0-9a-f]+[.]" "." link ${target})
                get_filename_component(target_name ${target} NAME)
                get_filename_component(link_name ${link} NAME)
                execute_process(COMMAND ln -rfs ${target_name} ${link_name} WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
                install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${link_name} DESTINATION lib COMPONENT redist_libs)

                # create links to the versioned links above
                # e.g. libcublas.so => libcublas.so.11
                string(REGEX REPLACE "[.]so[.0-9]*$" ".so" base_link ${link_name})
                execute_process(COMMAND ln -rfs ${link_name} ${base_link} WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
                install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${base_link} DESTINATION lib COMPONENT redist_libs)
            endforeach()
        else()
            # bundle the libraries from the cuda toolkit
            file(GLOB NATIVE_CUDA_LIBS "${CUDAToolkit_ROOT}/targets/${CMAKE_SYSTEM_PROCESSOR}-linux/lib/${LIB}")
            install(FILES ${NATIVE_CUDA_LIBS} DESTINATION lib COMPONENT redist_libs)
        endif()
    endforeach()

    file(GLOB TORCH_DLLS "${TORCH_LIB}/lib/*.so*")
    install(FILES ${TORCH_DLLS} DESTINATION lib COMPONENT redist_libs)

    if(DYNAMIC_HDF)
        string(REPLACE "."  "" SHARED_LIB_EXT "${CMAKE_SHARED_LIBRARY_SUFFIX}")
        FILTER_LIST("${HDF5_C_LIBRARIES}" DEBUG_LIBRARIES debug optimized ${SHARED_LIB_EXT})
        RESOLVE_SYMLINKS("${DEBUG_LIBRARIES}" NEW_HDF_DEBUG_LIBRARIES)
        foreach(HDF_LIB IN LISTS NEW_HDF_DEBUG_LIBRARIES)
	    if(${HDF_LIB} MATCHES "hdf5")
                install(FILES ${HDF_LIB} DESTINATION lib COMPONENT redist_libs CONFIGURATIONS Debug)
            endif()
        endforeach()
        FILTER_LIST("${HDF5_C_LIBRARIES}" RELEASE_LIBRARIES optimized debug ${SHARED_LIB_EXT})
        RESOLVE_SYMLINKS("${RELEASE_LIBRARIES}" NEW_HDF_RELEASE_LIBRARIES)
        foreach(HDF_LIB IN LISTS NEW_HDF_RELEASE_LIBRARIES)
	    if(${HDF_LIB} MATCHES "hdf5")
                install(FILES ${HDF_LIB} DESTINATION lib COMPONENT redist_libs CONFIGURATIONS Release ReleaseWithDebInfo)
            endif()
        endforeach()
    endif()

    find_library(SZ_DLL sz REQUIRED)
    get_filename_component(SZ_DLL_PATH ${SZ_DLL} DIRECTORY)
    file(GLOB SZ_DLLS "${SZ_DLL_PATH}/libsz.so*")
    install(FILES ${SZ_DLLS} DESTINATION lib COMPONENT redist_libs)

    find_library(AEC_DLL aec REQUIRED)
    get_filename_component(AEC_DLL_PATH ${AEC_DLL} DIRECTORY)
    file(GLOB AEC_DLLS "${AEC_DLL_PATH}/libaec.so*")
    install(FILES ${AEC_DLLS} DESTINATION lib COMPONENT redist_libs)

    # If zstd has been dynamically linked, add the .so to the package
    get_filename_component(ZSTD_LIBRARY_PATH ${ZSTD_LIBRARY_RELEASE} DIRECTORY)
    file(GLOB ZSTD_DLLS "${ZSTD_LIBRARY_PATH}/*zstd.so*")
    install(FILES ${ZSTD_DLLS} DESTINATION lib COMPONENT redist_libs)

elseif(WIN32)
    file(GLOB TORCH_DLLS "${TORCH_LIB}/lib/*.dll")
    install(FILES ${TORCH_DLLS} DESTINATION bin COMPONENT redist_libs)
    file(GLOB HTSLIB_DLLS "${HTSLIB_DIR}/*.dll")
    install(FILES ${HTSLIB_DLLS} DESTINATION bin COMPONENT redist_libs)
elseif(APPLE AND NOT CMAKE_SYSTEM_NAME STREQUAL "iOS")
    file(GLOB TORCH_DLLS "${TORCH_LIB}/lib/*.dylib")
    install(FILES ${TORCH_DLLS} DESTINATION lib COMPONENT redist_libs)
endif()

if(NOT CMAKE_SYSTEM_NAME STREQUAL "iOS")
    add_library(dorado_io_lib
        dorado/data_loader/DataLoader.cpp
        dorado/data_loader/DataLoader.h
        dorado/data_loader/ModelFinder.cpp
        dorado/data_loader/ModelFinder.h
    )

    target_link_libraries(dorado_io_lib
        PUBLIC
            dorado_lib
            dorado_models_lib
        PRIVATE
            ${POD5_LIBRARIES}
            ${HDF5_C_LIBRARIES}
            ${CMAKE_DL_LIBS}
            ${ZLIB_LIBRARIES}
    )

    set_target_properties(dorado_io_lib
        PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    )
    
    enable_warnings_as_errors(dorado_io_lib)

    if (NOT DORADO_DISABLE_TESTS)
        add_subdirectory(tests)
    endif()
endif()

if(NOT DORADO_DISABLE_PACKAGING)
    include(cmake/DoradoPackaging.cmake)
endif()

# For capturing test coverage.
if (GENERATE_TEST_COVERAGE)
    if (CMAKE_SYSTEM_NAME STREQUAL "Linux" AND CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" AND NOT DORADO_DISABLE_TESTS)
        include(cmake/CodeCoverage.cmake)
        foreach(COVERAGE_TARGET dorado_tests dorado_lib dorado_io_lib dorado_models_lib dorado_utils)
            append_coverage_compiler_flags_to_target(${COVERAGE_TARGET})
        endforeach()
        setup_target_for_coverage_gcovr_html(
            NAME dorado_test_coverage
            EXECUTABLE ctest --test-dir ${CMAKE_BINARY_DIR} --verbose -R dorado_tests
            DEPENDENCIES dorado_tests
            BASE_DIRECTORY "${PROJECT_SOURCE_DIR}"
            EXCLUDE "${PROJECT_SOURCE_DIR}/dorado/3rdparty/*" "${PROJECT_SOURCE_DIR}/tests/*"
            )
    endif()
endif()

# GCC 8 ICEs trying to compile this file with ASAN+optimisations enabled, so knock down the optimisation to try and help it out.
if (ECM_ENABLE_SANITIZERS AND (CMAKE_CXX_COMPILER_ID MATCHES "GNU") AND (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0))
    set_source_files_properties(dorado/read_pipeline/stereo_features.cpp PROPERTIES COMPILE_OPTIONS "-O0")
endif()
