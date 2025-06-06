add_library(dorado_utils
    alignment_utils.cpp
    alignment_utils.h
    AsyncQueue.h
    bam_utils.cpp
    bam_utils.h
    barcode_kits.cpp
    barcode_kits.h
    basecaller_utils.cpp
    basecaller_utils.h
    compat_utils.cpp
    compat_utils.h
    dev_utils.cpp
    dev_utils.h
    #driver_query.cpp
    #driver_query.h
    duplex_utils.cpp
    duplex_utils.h
    fs_utils.cpp
    fs_utils.h
    log_utils.cpp
    log_utils.h
    math_utils.h
    memory_utils.cpp
    memory_utils.h
    module_utils.h
    parameters.cpp
    parameters.h
    PostCondition.h
    SampleSheet.cpp
    SampleSheet.h
    sequence_utils.cpp
    sequence_utils.h
    stats.cpp
    stats.h
    sys_stats.cpp
    sys_stats.h
    tensor_utils.cpp
    tensor_utils.h
    time_utils.cpp
    time_utils.h
    torch_utils.h
    trim.cpp
    trim.h
    tty_utils.h
    types.cpp
    types.h
    uuid_utils.cpp
    uuid_utils.h
)

if (DORADO_GPU_BUILD)
    if(APPLE)
        target_sources(dorado_utils
            PRIVATE
            metal_utils.cpp
            metal_utils.h
        )
    else()
        target_sources(dorado_utils
            PRIVATE
            cuda_utils.cpp
            cuda_utils.h
        )
    endif()
endif()

target_compile_definitions(dorado_utils
    PUBLIC
        DORADO_GPU_BUILD=$<BOOL:${DORADO_GPU_BUILD}>
)

target_include_directories(dorado_utils
    INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# 3rdparty libs should be considered SYSTEM headers
target_include_directories(dorado_utils
    SYSTEM
    PUBLIC
    ${TORCH_INCLUDE_DIRS}
    ${ATen_INCLUDE_DIR}
    ${DORADO_3RD_PARTY_SOURCE}/NVTX/c/include
    ${DORADO_3RD_PARTY_DOWNLOAD}/metal-cpp/metal-cpp
)

target_link_libraries(dorado_utils
    PUBLIC
        ${TORCH_LIBRARIES}
        date::date
        edlib
        spdlog::spdlog
    PRIVATE
        minimap2
        OpenSSL::SSL
        htslib
)

if (DORADO_ENABLE_PCH)
    add_subdirectory(pch)
    target_link_libraries(dorado_utils PUBLIC dorado_pch)
    target_precompile_headers(dorado_utils REUSE_FROM dorado_pch)
endif()

if(APPLE AND DORADO_GPU_BUILD)
    add_dependencies(dorado_utils metal-lib)
    target_link_libraries(
        dorado_utils
        PUBLIC
        ${APPLE_FWK_FOUNDATION}
        ${APPLE_FWK_QUARTZ_CORE}
        ${APPLE_FWK_METAL}
    )   
endif()

if(APPLE AND NOT CMAKE_SYSTEM_NAME STREQUAL "iOS")
    target_link_libraries(dorado_utils PUBLIC ${IOKIT})
endif()

if(NOT WIN32)
    add_dependencies(dorado_utils htslib_project)
endif()

# GCC 8 ICEs trying to compile this file with ASAN+optimisations enabled, so knock down the optimisation to try and help it out.
if (ECM_ENABLE_SANITIZERS AND (CMAKE_CXX_COMPILER_ID MATCHES "GNU") AND (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0))
    set_source_files_properties(duplex_utils.cpp PROPERTIES COMPILE_OPTIONS "-O0")
endif()

enable_warnings_as_errors(dorado_utils)
