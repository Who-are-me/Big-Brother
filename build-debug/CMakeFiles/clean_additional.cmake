# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "Big-Brother_autogen"
  "CMakeFiles/Big-Brother_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/Big-Brother_autogen.dir/ParseCache.txt"
  )
endif()
