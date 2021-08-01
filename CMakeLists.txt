# specify minimun version of cmake
cmake_minimum_required(VERSION 3.15)

if(${CMAKE_VERSION} VERSION_LESS 3.14)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

# Variable setting
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_STANDARD 11)

set(SQRT_PROJECT_NAME SQRT)
set(SQRT_BUILD_NAME sqrt) # name of execute file
set(SQRT_SOURCE_DIR src)
set(SQRT_SOURCE ${SQRT_SOURCE_DIR}/main.cpp) # if not defined , CMake cannot find main.cpp

set(SQRT_LIB_DIR lib)
set(SQRT_LIB mymath) # file /mymath set as library
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)



# project tittle and its version , setting
project(${SQRT_PROJECT_NAME}    
        VERSION 1.0.0
        DESCRIPTION "A sqrt math test for devOps"
        LANGUAGES CXX)


# generate config file
configure_file(
    include/projectConfig.h.in
    ${PROJECT_SOURCE_DIR}/include/projectConfig.h
)


# GoogleTest included within CMake
# =============================================================================================
include(FetchContent)
FetchContent_Declare(
  googletest
  # Specify the commit you depend on and update it regularly.
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Now simply link against gtest or gtest_main as needed. Eg
add_executable(${SQRT_BUILD_NAME} ${SQRT_SOURCE})
target_link_libraries(${SQRT_BUILD_NAME} gtest_main)
add_test(NAME sqrt_test COMMAND ${SQRT_BUILD_NAME})
# =============================================================================================

# When we have lots of libraries , add every one of them.
foreach(LIBRARY ${SQRT_LIB}) 
    add_subdirectory("${SQRT_LIB_DIR}/${LIBRARY}")
endforeach(LIBRARY)


# include and link libraries
target_include_directories(${SQRT_BUILD_NAME} PUBLIC include)
target_link_libraries(${SQRT_BUILD_NAME} ${SQRT_LIB} ${EXTRA_LIBS})


