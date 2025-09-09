# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/matt/Project/c/Controller/build/_deps/picotool-src"
  "/home/matt/Project/c/Controller/build/_deps/picotool-build"
  "/home/matt/Project/c/Controller/build/_deps"
  "/home/matt/Project/c/Controller/build/picotool/tmp"
  "/home/matt/Project/c/Controller/build/picotool/src/picotoolBuild-stamp"
  "/home/matt/Project/c/Controller/build/picotool/src"
  "/home/matt/Project/c/Controller/build/picotool/src/picotoolBuild-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/matt/Project/c/Controller/build/picotool/src/picotoolBuild-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/matt/Project/c/Controller/build/picotool/src/picotoolBuild-stamp${cfgdir}") # cfgdir has leading slash
endif()
