cmake_minimum_required(VERSION 3.14)

project(Server LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Network)
add_executable(Server
  main.cpp
  server.h server.cpp
  game.h game.cpp
  field.h field.cpp
  structure.h structure.cpp
  biome.h biome.cpp
  unit.h unit.cpp
  user.h user.cpp
)
target_link_libraries(Server Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(Server Qt${QT_VERSION_MAJOR}::Network)

include(GNUInstallDirs)
install(TARGETS Server
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
