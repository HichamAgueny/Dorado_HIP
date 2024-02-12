OPTION(BUILD_KOI_FROM_SOURCE OFF)


function(get_best_compatible_koi_version KOI_CUDA)
    if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "aarch64")
        # Koi only provides binaries for 11.4 and 10.2 when targeting aarch64
        set(SUPPORTED_VERSIONS 11.4 10.2)
    else()
        set(SUPPORTED_VERSIONS 12.0 11.8 11.7 11.3)
    endif()

    list(SORT SUPPORTED_VERSIONS COMPARE NATURAL ORDER DESCENDING)
    foreach(SUPPORTED_VERSION IN LISTS SUPPORTED_VERSIONS)
        if (${CUDAToolkit_VERSION} VERSION_GREATER_EQUAL ${SUPPORTED_VERSION})
            set(${KOI_CUDA} ${SUPPORTED_VERSION} PARENT_SCOPE)
            return()
        endif()
    endforeach()
    message(FATAL_ERROR "Unsupported CUDA toolkit version: ${CUDAToolkit_VERSION}")
endfunction()

if(CMAKE_SYSTEM_NAME STREQUAL "Linux" OR WIN32)
    set(KOI_DIR "${DORADO_3RD_PARTY_DOWNLOAD}/koi")
    set(KOI_INCLUDE ${KOI_DIR}/include)
    set(KOI_LIBRARIES ${KOI_DIR}/lib)

    message(STATUS "DORADO_3RD_PARTY_DOWNLOAD: ${DORADO_3RD_PARTY_DOWNLOAD}")
    message(STATUS "KOI_DIR: ${KOI_DIR}")
    message(STATUS "Koi is already available")

endif()
