cmake_minimum_required(VERSION 3.5)

include(ExternalProject)
set_property(DIRECTORY PROPERTY EP_PREFIX ${CMAKE_BINARY_DIR})

# GLI - https://github.com/g-truc/gli
#ExternalProject_Add(gli
#  URL https://github.com/g-truc/gli/archive/0.8.1.0.tar.gz
#  URL_HASH SHA1=1a01654b2ea5fa281442ce26adea50d04e1babf0
#  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}
#)

# GLM - https://github.com/g-truc/glm
ExternalProject_Add(glm
  URL https://github.com/g-truc/glm/archive/0.9.7.4.tar.gz
  URL_HASH SHA1=cd3d91c1361de38cb2f7603ac06d185d8e10dc01
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}
)

# spdlog - https://github.com/gabime/spdlog
ExternalProject_Add(spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog
  UPDATE_DISCONNECTED 1
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}
)

# sqlpp11 - https://github.com/rbock/sqlpp11
# sqlpp11 requires date.h from https://github.com/HowardHinnant/date
file(DOWNLOAD
  "https://raw.githubusercontent.com/HowardHinnant/date/master/date.h"
  "${CMAKE_BINARY_DIR}/include/date.h")
ExternalProject_Add(sqlpp11
  URL https://github.com/rbock/sqlpp11/archive/0.38.tar.gz
  URL_HASH SHA1=05c3603334946a8981618848f197966f9038ef81
  CMAKE_ARGS
    -DHinnantDate_ROOT_DIR=${CMAKE_BINARY_DIR}/include
    -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}
)
ExternalProject_Get_Property(sqlpp11 SOURCE_DIR)
# sqlpp11-connector-sqlite3
ExternalProject_Add(sqlpp11-connector-sqlite3
  URL https://github.com/rbock/sqlpp11-connector-sqlite3/archive/0.17.tar.gz
  URL_HASH SHA1=196d94e6560f2ab219c8625db50346e015acd775
  CMAKE_ARGS
    -DDATE_INCLUDE_DIR=${CMAKE_BINARY_DIR}/include
    -DSQLPP11_INCLUDE_DIR=${SOURCE_DIR}/include
    -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}
)
add_dependencies(sqlpp11-connector-sqlite3 sqlpp11)

