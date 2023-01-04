# specify minimun version of cmake
cmake_minimum_required(VERSION 3.7)

if(${CMAKE_VERSION} VERSION_LESS 3.14)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

# Variable setting
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_STANDARD 11)

set(SCHEDULER_PROJECT_NAME SCHEDULER)
set(SCHEDULER_BUILD_NAME scheduler) # name of execute file
set(SCHEDULER_SOURCE_DIR src)
set(SCHEDULER_SOURCE 
    ${SCHEDULER_SOURCE_DIR}/main.cpp 
    ${SCHEDULER_SOURCE_DIR}/Scheduler.cpp
    ${SCHEDULER_SOURCE_DIR}/SA.cpp
    ${SCHEDULER_SOURCE_DIR}/GA.cpp
    ${SCHEDULER_SOURCE_DIR}/GP.cpp
) # if not defined , CMake cannot find main.cpp

set(SCHEDULER_LIB_DIR lib)
set(SCHEDULER_LIB json) # file /mymath set as library
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# project tittle and its version , setting
project(${SCHEDULER_PROJECT_NAME}    
        VERSION 1.0.0
        DESCRIPTION "A scheduler math test for devOps"
        LANGUAGES CXX)


# generate config file
configure_file(
    include/projectConfig.h.in
    ${PROJECT_SOURCE_DIR}/include/projectConfig.h
)

add_executable(${SCHEDULER_BUILD_NAME} ${SCHEDULER_SOURCE})

add_library(${SCHEDULER_PROJECT_NAME} SHARED
    ${PROJECT_SOURCE_DIR}/src/Scheduler.cpp
    ${PROJECT_SOURCE_DIR}/src/SA.cpp
    ${PROJECT_SOURCE_DIR}/src/GA.cpp
    ${PROJECT_SOURCE_DIR}/include/Scheduler.h
    ${PROJECT_SOURCE_DIR}/include/SA.h
    ${PROJECT_SOURCE_DIR}/include/GA.h
    ${PROJECT_SOURCE_DIR}/include/GP.h
)


# When we have lots of libraries , add every one of them.
foreach(LIBRARY ${SCHEDULER_LIB}) 
    add_subdirectory("${SCHEDULER_LIB_DIR}/${LIBRARY}")
endforeach(LIBRARY)

# include and link libraries
target_include_directories(${SCHEDULER_BUILD_NAME} PUBLIC include src data)
target_link_libraries(${SCHEDULER_BUILD_NAME} ${SCHEDULER_LIB} ${EXTRA_LIBS})


