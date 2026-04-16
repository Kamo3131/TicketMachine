cmake_minimum_required(VERSION 4.0.0)

project(tickets LANGUAGES C CXX)
if(WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
    add_compile_definitions(SFML_STATIC)
endif()
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
set(SOURCE_DIR "${PROJECT_SOURCE_DIR}/src")

include(FetchContent)

set(SFML_STATIC_LIBRARIES ON)
set(BUILD_SHARED_LIBS OFF)
set(SFML_BUILD_EXAMPLES OFF)
set(SFML_BUILD_TESTS OFF)
set(SFML_BUILD_DOC OFF)
set(SFML_BUILD_GRAPHICS OFF CACHE INTERNAL "")
set(SFML_BUILD_WINDOW OFF CACHE INTERNAL "")
set(SFML_BUILD_AUDIO OFF CACHE INTERNAL "")
set(SFML_VERSION 3.0.2)

FetchContent_Declare(
    SFML
    URL "https://github.com/SFML/SFML/archive/${SFML_VERSION}.zip"
)

FetchContent_MakeAvailable(SFML)

add_executable(ticket_server ${SOURCE_DIR}/server_main.cpp 
${SOURCE_DIR}/Server.cpp)
target_include_directories(ticket_server PRIVATE ${INCLUDE_DIR} ${sfml_SOURCE_DIR}/include)
target_link_libraries(ticket_server PRIVATE sfml-network)

add_executable(ticket_client ${SOURCE_DIR}/client_main.cpp 
${SOURCE_DIR}/Client.cpp)
target_include_directories(ticket_client PRIVATE ${INCLUDE_DIR} ${sfml_SOURCE_DIR}/include)
target_link_libraries(ticket_client PRIVATE sfml-network)

set(ALL_TARGETS ticket_server ticket_client)
foreach(TGT IN LISTS ALL_TARGETS)
    if(WIN32)
        target_link_libraries(${TGT} PRIVATE winmm ws2_32) 
    endif()

    if(MSVC)
        target_compile_options(${TGT} PRIVATE /W4)
    else()
        target_compile_options(${TGT} PRIVATE -Wall -Wextra -pedantic)
    endif()
endforeach()

enable_testing()
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/CMakeLists.txt")
    add_subdirectory(tests)
endif()