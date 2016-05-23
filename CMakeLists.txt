cmake_minimum_required(VERSION 3.5)

project(solid)
include(external/dependencies.cmake)

# Generate clang's compilation database (for tools)
set(CMAKE_EXPORT_COMPILE_COMMANDS on)

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -std=c++14")
  #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions -fno-rtti")
endif()

add_definitions(-DPROJECT_SOURCE_DIR=\"${PROJECT_SOURCE_DIR}\")

file(GLOB PROJECT_HEADERS src/*.h)
file(GLOB PROJECT_SOURCES src/*.c src/*.cpp)

add_executable(${PROJECT_NAME} ${PROJECT_HEADERS} ${PROJECT_SOURCES})

target_link_libraries(${PROJECT_NAME}
  ${BULLET_LIBRARIES}
  ${Ice_LIBRARIES}
  ${SDL2_LIBS}
  sqlite3
  sqlpp11-connector-sqlite3
)

set_target_properties(${PROJECT_NAME} PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
