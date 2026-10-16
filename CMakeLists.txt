cmake_minimum_required(VERSION 3.28)
project(ocl-path-tracing LANGUAGES C CXX VERSION 0.1.0)

include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(EXTERNAL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external)
set(SOURCE_DIR   ${CMAKE_CURRENT_SOURCE_DIR}/source)

list(APPEND CMAKE_PREFIX_PATH
    ${EXTERNAL_DIR}/cpp-core-utils
    ${EXTERNAL_DIR}/etu-opengl
    ${EXTERNAL_DIR}/opencl
)

find_package(PkgConfig REQUIRED)
pkg_check_modules(STB REQUIRED IMPORTED_TARGET stb)

find_package(cpp-core-utils CONFIG REQUIRED)
find_package(etu-opengl CONFIG REQUIRED)
find_package(OpenCL REQUIRED)

find_program(CLILOADER_EXECUTABLE cliloader
    HINTS ${EXTERNAL_DIR}/opencl-intercept-layer/bin
    DOC "Intel OpenCL Intercept Layer loader"
)

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS
    ${SOURCE_DIR}/*.cpp
    ${SOURCE_DIR}/*.c
    ${SOURCE_DIR}/*.cc
    ${SOURCE_DIR}/*.cxx
)

file(GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS
    ${SOURCE_DIR}/*.hpp
    ${SOURCE_DIR}/*.h
    ${SOURCE_DIR}/*.hxx
)

if(NOT SOURCE_FILES)
    message(FATAL_ERROR "No source files found in ${SOURCE_DIR}")
endif()

add_executable(${PROJECT_NAME}
    ${SOURCE_FILES}
    ${HEADER_FILES}
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${SOURCE_DIR}
    ${OpenCL_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    cpp-core-utils::cpp-core-utils
    etu-opengl::etu-opengl
    OpenCL::OpenCL
)

if(CLILOADER_EXECUTABLE)
    message(STATUS "Found cliloader: ${CLILOADER_EXECUTABLE}")
    add_custom_target(run-intercept
        COMMAND ${CLILOADER_EXECUTABLE} $<TARGET_FILE:${PROJECT_NAME}>
        DEPENDS ${PROJECT_NAME}
        COMMENT "Running ${PROJECT_NAME} with OpenCL Intercept Layer (cliloader)"
    )
endif()

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
