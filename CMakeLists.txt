# CMakeList.txt : CMake project for TriangulateOBJ, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("TriangulateOBJ")

# Add source to this project's executable.
add_executable (TriangulateOBJ "main.cpp" "cmd.h" "out.h" "TriangulateOBJ.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET TriangulateOBJ PROPERTY CXX_STANDARD 17)
  target_compile_definitions(TriangulateOBJ PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

if (CMAKE_VERSION VERSION_GREATER 3.6)
  set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT TriangulateOBJ)
endif()

# TODO: Add tests and install targets if needed.
