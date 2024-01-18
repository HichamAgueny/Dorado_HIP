# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/project/project_465000096/hich/Dorado_HIP/dorado_hip/build/htslib_build/htslib"
  "/project/project_465000096/hich/Dorado_HIP/dorado_hip/build/htslib_build/src/htslib_project-build"
  "/project/project_465000096/hich/Dorado_HIP/dorado_hip/build/htslib_build"
  "/project/project_465000096/hich/Dorado_HIP/dorado_hip/build/htslib_build/tmp"
  "/project/project_465000096/hich/Dorado_HIP/dorado_hip/build/htslib_build/src/htslib_project-stamp"
  "/project/project_465000096/hich/Dorado_HIP/dorado_hip/build/htslib_build/src"
  "/project/project_465000096/hich/Dorado_HIP/dorado_hip/build/htslib_build/src/htslib_project-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/project/project_465000096/hich/Dorado_HIP/dorado_hip/build/htslib_build/src/htslib_project-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/project/project_465000096/hich/Dorado_HIP/dorado_hip/build/htslib_build/src/htslib_project-stamp${cfgdir}") # cfgdir has leading slash
endif()
