# Install script for directory: /project/project_465000096/hich/Dorado_HIP/dorado_hip

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/project/project_465000096/hich/Dorado_HIP/dorado_hip/build/../dist")
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

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/project/project_465000096/hich/Dorado_HIP/dorado_hip/build/dorado/3rdparty/spdlog/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/project/project_465000096/hich/Dorado_HIP/dorado_hip/build/dorado/3rdparty/elzip/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/project/project_465000096/hich/Dorado_HIP/dorado_hip/build/dorado/utils/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/project/project_465000096/hich/Dorado_HIP/dorado_hip/build/dorado/models/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/project/project_465000096/hich/Dorado_HIP/dorado_hip/build/dorado/basecall/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/project/project_465000096/hich/Dorado_HIP/dorado_hip/build/dorado/modbase/cmake_install.cmake")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/dorado" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/dorado")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/dorado"
         RPATH "\$ORIGIN/../lib/:\$ORIGIN")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "/project/project_465000096/hich/Dorado_HIP/dorado_hip/build/bin/dorado")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/dorado" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/dorado")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/dorado"
         OLD_RPATH "/appl/lumi/SW/LUMI-23.09/G/EB/zstd/1.5.5-cpeCray-23.09/lib:"
         NEW_RPATH "\$ORIGIN/../lib/:\$ORIGIN")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/pfs/lustrep2/projappl/project_462000125/samantao-public/rocm/rocm-5.7.1/llvm/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/dorado")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "redist_libs" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE FILE FILES
    "/lib/ld-2.31.so"
    "/lib/ld-linux.so.2"
    "/lib/ld-lsb.so.3"
    "/lib/libBrokenLocale-2.31.so"
    "/lib/libBrokenLocale.so.1"
    "/lib/libSegFault.so"
    "/lib/libanl-2.31.so"
    "/lib/libanl.so.1"
    "/lib/libc-2.31.so"
    "/lib/libc.so.6"
    "/lib/libdl-2.31.so"
    "/lib/libdl.so.2"
    "/lib/libgcc_s.so.1"
    "/lib/libm-2.31.so"
    "/lib/libm.so.6"
    "/lib/libnsl-2.31.so"
    "/lib/libnsl.so.1"
    "/lib/libnss_compat-2.31.so"
    "/lib/libnss_compat.so.2"
    "/lib/libnss_db-2.31.so"
    "/lib/libnss_db.so.2"
    "/lib/libnss_dns-2.31.so"
    "/lib/libnss_dns.so.2"
    "/lib/libnss_files-2.31.so"
    "/lib/libnss_files.so.2"
    "/lib/libnss_hesiod-2.31.so"
    "/lib/libnss_hesiod.so.2"
    "/lib/libpthread-2.31.so"
    "/lib/libpthread.so.0"
    "/lib/libresolv-2.31.so"
    "/lib/libresolv.so.2"
    "/lib/librt-2.31.so"
    "/lib/librt.so.1"
    "/lib/libthread_db-1.0.so"
    "/lib/libthread_db.so.1"
    "/lib/libutil-2.31.so"
    "/lib/libutil.so.1"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "redist_libs" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE FILE FILES
    "/appl/lumi/SW/LUMI-23.09/G/EB/Szip/2.1.1-cpeCray-23.09/lib/libsz.so"
    "/appl/lumi/SW/LUMI-23.09/G/EB/Szip/2.1.1-cpeCray-23.09/lib/libsz.so.2"
    "/appl/lumi/SW/LUMI-23.09/G/EB/Szip/2.1.1-cpeCray-23.09/lib/libsz.so.2.0.0"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "redist_libs" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE FILE FILES
    "/appl/lumi/SW/LUMI-23.09/G/EB/libaec/1.0.6-cpeCray-23.09/lib/libaec.so"
    "/appl/lumi/SW/LUMI-23.09/G/EB/libaec/1.0.6-cpeCray-23.09/lib/libaec.so.0"
    "/appl/lumi/SW/LUMI-23.09/G/EB/libaec/1.0.6-cpeCray-23.09/lib/libaec.so.0.0.12"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "redist_libs" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE FILE FILES
    "/appl/lumi/SW/LUMI-23.09/G/EB/zstd/1.5.5-cpeCray-23.09/lib/libzstd.so"
    "/appl/lumi/SW/LUMI-23.09/G/EB/zstd/1.5.5-cpeCray-23.09/lib/libzstd.so.1"
    "/appl/lumi/SW/LUMI-23.09/G/EB/zstd/1.5.5-cpeCray-23.09/lib/libzstd.so.1.5.5"
    )
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/project/project_465000096/hich/Dorado_HIP/dorado_hip/build/tests/cmake_install.cmake")
endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/project/project_465000096/hich/Dorado_HIP/dorado_hip/build/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
