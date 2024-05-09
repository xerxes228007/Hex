# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/HexagonalMap_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/HexagonalMap_autogen.dir/ParseCache.txt"
  "HexagonalMap_autogen"
  )
endif()
