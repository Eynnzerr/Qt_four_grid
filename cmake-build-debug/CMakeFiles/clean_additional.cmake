# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/four_grid_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/four_grid_autogen.dir/ParseCache.txt"
  "four_grid_autogen"
  )
endif()
