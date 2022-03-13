cmake_minimum_required (VERSION 3.8)
set(CMAKE_CXX_STANDARD 20)

project ("mandelbrot")

if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
   message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
   file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/master/conan.cmake"
                  "${CMAKE_BINARY_DIR}/conan.cmake")
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)

conan_cmake_run(REQUIRES 
				ogg/1.3.5
				sfml/2.5.1
                BASIC_SETUP)
				

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O3  -fconstexpr-ops-limit=1000000000 -fconstexpr-loop-limit=100000000")

add_executable (mandelbrot "src/mandelbrot.cpp" )

target_link_libraries(mandelbrot ${CONAN_LIBS})
