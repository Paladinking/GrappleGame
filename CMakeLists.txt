cmake_minimum_required(VERSION 3.10)

project(GrappleGame VERSION 1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(ZLIB REQUIRED)

get_target_property(SDL2_INCLUDE_DIRS SDL2::SDL2 INTERFACE_INCLUDE_DIRECTORIES)
get_target_property(SDL2_image_INCLUDE_DIRS SDL2_image::SDL2_image INTERFACE_INCLUDE_DIRECTORIES)
get_target_property(SDL2_ttf_INCLUDE_DIRS SDL2_ttf::SDL2_ttf INTERFACE_INCLUDE_DIRECTORIES)

include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${SDL2_INCLUDE_DIRS})
include_directories(${SDL2_image_INCLUDE_DIRS})
include_directories(${SDL2_ttf_INCLUDE_DIRS})
include_directories(${ZLIB_INCLUDE_DIRS})

set(ENGINE_DIR ${PROJECT_SOURCE_DIR}/src/engine)
set(FileIO_DIR ${PROJECT_SOURCE_DIR}/src/file)
set(UTIL_DIR ${PROJECT_SOURCE_DIR}/src/util)
set(GAME_DIR ${PROJECT_SOURCE_DIR}/src/game)
set(NFD_DIR ${PROJECT_SOURCE_DIR}/src/nativefiledialog)

add_compile_options(/MD)

add_library(nfd STATIC 
	${NFD_DIR}/nfd_win.cpp 
	${NFD_DIR}/nfd_common.c
	${NFD_DIR}/nfdcpp.cpp
)

target_compile_options(nfd PRIVATE /w)
target_link_libraries(nfd User32.lib)

add_library(
	FileIO OBJECT
	${FileIO_DIR}/json.cpp
	${FileIO_DIR}/compression.cpp
)

add_library(
	Engine OBJECT 
	${ENGINE_DIR}/game.cpp
	${ENGINE_DIR}/engine.cpp
	${ENGINE_DIR}/input.cpp
	${ENGINE_DIR}/texture.cpp
	${ENGINE_DIR}/ui.cpp
)

add_library(
	Util OBJECT
	${UTIL_DIR}/geometry.cpp
)

add_library(
	Game OBJECT
	${GAME_DIR}/climbGame.cpp
	${GAME_DIR}/config.cpp
	${GAME_DIR}/entity.cpp
	${GAME_DIR}/level.cpp
	${GAME_DIR}/levelMaker.cpp
	${GAME_DIR}/menu.cpp	
)

add_executable(main src/main.cpp)

target_link_options(main PUBLIC /SUBSYSTEM:CONSOLE)
target_link_options(main PUBLIC /ENTRY:WinMainCRTStartup)

target_link_libraries(main shell32)
target_link_libraries(main Engine)
target_link_libraries(main FileIO)
target_link_libraries(main Util)
target_link_libraries(main Game)
target_link_libraries(main nfd)
target_link_libraries(main ${SDL2_LIBRARIES})
target_link_libraries(main SDL2_image::SDL2_image)
target_link_libraries(main SDL2_ttf::SDL2_ttf)
target_link_libraries(main ZLIB::ZLIB)


add_custom_command (TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:SDL2::SDL2> $<TARGET_FILE_DIR:main>
)
add_custom_command (TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:SDL2_image::SDL2_image> $<TARGET_FILE_DIR:main>
)
add_custom_command (TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:SDL2_ttf::SDL2_ttf> $<TARGET_FILE_DIR:main>
)

