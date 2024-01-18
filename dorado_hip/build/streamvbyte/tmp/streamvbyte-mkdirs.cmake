# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/project/project_465000096/hich/Dorado_HIP/dorado_hip/dorado/3rdparty/hdf_plugins/third_party/streamvbyte"
  "/project/project_465000096/hich/Dorado_HIP/dorado_hip/build/streamvbyte/src/streamvbyte-build"
  "/project/project_465000096/hich/Dorado_HIP/dorado_hip/build/streamvbyte"
  "/project/project_465000096/hich/Dorado_HIP/dorado_hip/build/streamvbyte/tmp"
  "/project/project_465000096/hich/Dorado_HIP/dorado_hip/build/streamvbyte/src/streamvbyte-stamp"
  "/project/project_465000096/hich/Dorado_HIP/dorado_hip/build/streamvbyte/src"
  "/project/project_465000096/hich/Dorado_HIP/dorado_hip/build/streamvbyte/src/streamvbyte-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/project/project_465000096/hich/Dorado_HIP/dorado_hip/build/streamvbyte/src/streamvbyte-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/project/project_465000096/hich/Dorado_HIP/dorado_hip/build/streamvbyte/src/streamvbyte-stamp${cfgdir}") # cfgdir has leading slash
endif()
