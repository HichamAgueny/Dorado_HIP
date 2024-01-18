# Install script for directory: /project/project_465000096/hich/Dorado_HIP/dorado_hip/dorado/3rdparty/elzip/extlibs/minizip

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "0")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/pfs/lustrep2/projappl/project_462000125/samantao-public/rocm/rocm-5.7.1/llvm/bin/llvm-objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib64" TYPE STATIC_LIBRARY FILES "/project/project_465000096/hich/Dorado_HIP/dorado_hip/build/dorado/3rdparty/elzip/extlibs/minizip/libminizip.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib64/cmake/minizip/minizip.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib64/cmake/minizip/minizip.cmake"
         "/project/project_465000096/hich/Dorado_HIP/dorado_hip/build/dorado/3rdparty/elzip/extlibs/minizip/CMakeFiles/Export/091a737e45d14b2d4610160694b6ecfe/minizip.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib64/cmake/minizip/minizip-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib64/cmake/minizip/minizip.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib64/cmake/minizip" TYPE FILE FILES "/project/project_465000096/hich/Dorado_HIP/dorado_hip/build/dorado/3rdparty/elzip/extlibs/minizip/CMakeFiles/Export/091a737e45d14b2d4610160694b6ecfe/minizip.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib64/cmake/minizip" TYPE FILE FILES "/project/project_465000096/hich/Dorado_HIP/dorado_hip/build/dorado/3rdparty/elzip/extlibs/minizip/CMakeFiles/Export/091a737e45d14b2d4610160694b6ecfe/minizip-release.cmake")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib64/cmake/minizip" TYPE FILE FILES
    "/project/project_465000096/hich/Dorado_HIP/dorado_hip/build/dorado/3rdparty/elzip/extlibs/minizip/minizip-config-version.cmake"
    "/project/project_465000096/hich/Dorado_HIP/dorado_hip/build/dorado/3rdparty/elzip/extlibs/minizip/minizip-config.cmake"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES
    "/project/project_465000096/hich/Dorado_HIP/dorado_hip/dorado/3rdparty/elzip/extlibs/minizip/mz.h"
    "/project/project_465000096/hich/Dorado_HIP/dorado_hip/dorado/3rdparty/elzip/extlibs/minizip/mz_os.h"
    "/project/project_465000096/hich/Dorado_HIP/dorado_hip/dorado/3rdparty/elzip/extlibs/minizip/mz_crypt.h"
    "/project/project_465000096/hich/Dorado_HIP/dorado_hip/dorado/3rdparty/elzip/extlibs/minizip/mz_strm.h"
    "/project/project_465000096/hich/Dorado_HIP/dorado_hip/dorado/3rdparty/elzip/extlibs/minizip/mz_strm_buf.h"
    "/project/project_465000096/hich/Dorado_HIP/dorado_hip/dorado/3rdparty/elzip/extlibs/minizip/mz_strm_mem.h"
    "/project/project_465000096/hich/Dorado_HIP/dorado_hip/dorado/3rdparty/elzip/extlibs/minizip/mz_strm_split.h"
    "/project/project_465000096/hich/Dorado_HIP/dorado_hip/dorado/3rdparty/elzip/extlibs/minizip/mz_strm_os.h"
    "/project/project_465000096/hich/Dorado_HIP/dorado_hip/dorado/3rdparty/elzip/extlibs/minizip/mz_zip.h"
    "/project/project_465000096/hich/Dorado_HIP/dorado_hip/dorado/3rdparty/elzip/extlibs/minizip/mz_zip_rw.h"
    "/project/project_465000096/hich/Dorado_HIP/dorado_hip/dorado/3rdparty/elzip/extlibs/minizip/mz_strm_zlib.h"
    "/project/project_465000096/hich/Dorado_HIP/dorado_hip/dorado/3rdparty/elzip/extlibs/minizip/mz_compat.h"
    "/project/project_465000096/hich/Dorado_HIP/dorado_hip/build/dorado/3rdparty/elzip/extlibs/minizip/zip.h"
    "/project/project_465000096/hich/Dorado_HIP/dorado_hip/build/dorado/3rdparty/elzip/extlibs/minizip/unzip.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/local/share/pkgconfig/minizip.pc")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/usr/local/share/pkgconfig" TYPE FILE FILES "/project/project_465000096/hich/Dorado_HIP/dorado_hip/build/dorado/3rdparty/elzip/extlibs/minizip/minizip.pc")
endif()

