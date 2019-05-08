cmake_minimum_required(VERSION 3.0)
project(Json CXX)
#set_target_properties(Json PROPERTIES LINKER_LANGUAGE CXX)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# =====================================================
# setup sources

set(SOURCES
    "src/json/json.hpp"
	"src/json/json.cpp"
)

add_library(json STATIC ${SOURCES})
target_include_directories(json PUBLIC src)

