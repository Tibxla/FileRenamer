# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Release")
  file(REMOVE_RECURSE
  "CMakeFiles\\FileRenamer_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\FileRenamer_autogen.dir\\ParseCache.txt"
  "FileRenamer_autogen"
  )
endif()
