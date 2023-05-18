cmake_minimum_required(VERSION 3.16)

SET(PROJ algorithm)

project(${PROJ} VERSION 1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fno-delete-null-pointer-checks")
add_compile_options(-fPIC)
add_compile_options(-Wall)
add_compile_options(-Werror=unused)
add_compile_options(-Werror=address)
add_compile_options(-Werror=return-type)
add_compile_options(-Werror=uninitialized)
add_compile_options(-Werror=unused-parameter)
add_compile_options(-Werror=dangling-else)
add_compile_options(-Wsign-compare)

########################################################################################
# algorithm.lib

file(GLOB_RECURSE SRC "src/*.c" "src/*.cpp" "include/*.hpp" "include/*.h")
add_library(${PROJ} STATIC ${SRC})

set_target_properties(${PROJ} PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib")
set_target_properties(${PROJ} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib")
set_target_properties(${PROJ} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib")
target_include_directories(${PROJ} PRIVATE "src" "include")
add_definitions(-D_CRTDBG_MAP_ALLOC)

# target_precompile_headers(${PROJ} PRIVATE "src/stdafx_runtime.h")

########################################################################################
# algorithm-test

set(TEST_PROJ "algorithm-test")
file(GLOB_RECURSE TEST_SRC "tests/*.cpp" "tests/*.h" "3rd-party/src/googletest-1.10.0/*.h" "3rd-party/src/googletest-1.10.0/src/gtest-all.cc")
add_executable(${TEST_PROJ} ${TEST_SRC})
target_link_libraries(${TEST_PROJ} PRIVATE ${PROJ})

if (NOT WIN32)
	target_link_libraries(${TEST_PROJ} PRIVATE pthread)	# google test needs pthread on Linux
endif()

target_include_directories(${TEST_PROJ} PRIVATE "src" "include" "3rd-party/include" "3rd-party/src/googletest-1.10.0")
