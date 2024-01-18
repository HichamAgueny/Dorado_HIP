# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/project/project_465000096/hich/Dorado_HIP/dorado_hip/dorado/3rdparty/elzip/extlibs/minizip/third-party/zlib"
  "/project/project_465000096/hich/Dorado_HIP/dorado_hip/build/_deps/zlib-build"
  "/pfs/lustrep1/projappl/project_465000096/hich/Dorado_HIP/dorado_hip/build/_deps/zlib-subbuild/zlib-populate-prefix"
  "/pfs/lustrep1/projappl/project_465000096/hich/Dorado_HIP/dorado_hip/build/_deps/zlib-subbuild/zlib-populate-prefix/tmp"
  "/pfs/lustrep1/projappl/project_465000096/hich/Dorado_HIP/dorado_hip/build/_deps/zlib-subbuild/zlib-populate-prefix/src/zlib-populate-stamp"
  "/pfs/lustrep1/projappl/project_465000096/hich/Dorado_HIP/dorado_hip/build/_deps/zlib-subbuild/zlib-populate-prefix/src"
  "/pfs/lustrep1/projappl/project_465000096/hich/Dorado_HIP/dorado_hip/build/_deps/zlib-subbuild/zlib-populate-prefix/src/zlib-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/pfs/lustrep1/projappl/project_465000096/hich/Dorado_HIP/dorado_hip/build/_deps/zlib-subbuild/zlib-populate-prefix/src/zlib-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/pfs/lustrep1/projappl/project_465000096/hich/Dorado_HIP/dorado_hip/build/_deps/zlib-subbuild/zlib-populate-prefix/src/zlib-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
