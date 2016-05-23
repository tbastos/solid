#########################################################################
# Find dependencies that must be installed in the system

# SDL2 -- use script 'sdl2-config' to find SDL2
find_program(SDL2_CONFIG sdl2-config)
if(NOT SDL2_CONFIG)
  message(FATAL_ERROR "SDL2 is not installed (sdl2-config is not in the PATH)")
endif()
execute_process(COMMAND sdl2-config --version --cflags --libs
  OUTPUT_VARIABLE SDL2_CONFIG OUTPUT_STRIP_TRAILING_WHITESPACE)
string(REPLACE "\n" ";" SDL2_CONFIG ${SDL2_CONFIG})
list(GET SDL2_CONFIG 0 SDL2_VERSION)
list(GET SDL2_CONFIG 1 SDL2_CFLAGS)
list(GET SDL2_CONFIG 2 SDL2_LIBS)
message(STATUS "Found SDL v${SDL2_VERSION}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SDL2_CFLAGS}")

# ZeroC ICE
#set(Ice_DEBUG ON)
find_package(Ice REQUIRED Ice)
include_directories(SYSTEM ${Ice_INCLUDE_DIRS})

# Bullet Physics
find_package(Bullet REQUIRED)
include_directories(SYSTEM ${BULLET_INCLUDE_DIRS})


#########################################################################
# Download and build other external dependencies

set(EP_PREFIX "${CMAKE_BINARY_DIR}/external")

if(NOT EXISTS "${EP_PREFIX}/_DONE")
  file(MAKE_DIRECTORY ${EP_PREFIX})
  execute_process(COMMAND ${CMAKE_COMMAND} "${CMAKE_SOURCE_DIR}/external"
    -G "${CMAKE_GENERATOR}"
    WORKING_DIRECTORY ${EP_PREFIX} )
  execute_process(COMMAND ${CMAKE_COMMAND} --build .
    WORKING_DIRECTORY ${EP_PREFIX} )
  file(WRITE "${EP_PREFIX}/_DONE" "")
endif()

include_directories(SYSTEM ${EP_PREFIX}/include)
link_directories(${EP_PREFIX}/lib)
