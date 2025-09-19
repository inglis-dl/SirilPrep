cmake_minimum_required(VERSION 3.16...3.21 FATAL_ERROR)

project(SirilPrep
    VERSION 0.1.0
    DESCRIPTION "Preprocessing tool for Siril and FITS images"
    LANGUAGES CXX
)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enforce out-of-source builds
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
    message(FATAL_ERROR "SirilPrep requires an out-of-source build. Please use a separate build directory.")
endif()

# Custom CMake modules (if any)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Option for static/shared build (default: static)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

# MSVC runtime settings: match static/shared
if(MSVC)
    if(BUILD_SHARED_LIBS)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
    else()
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    endif()
endif()

# Build type defaults
set(DEFAULT_BUILD_TYPE "Debug")
if(CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Supported build types" FORCE)
    set_property(CACHE CMAKE_CONFIGURATION_TYPES PROPERTY STRINGS "Debug" "Release")
elseif(NOT CMAKE_BUILD_TYPE)
    message(STATUS "Setting build type to '${DEFAULT_BUILD_TYPE}'")
    set(CMAKE_BUILD_TYPE "${DEFAULT_BUILD_TYPE}" CACHE STRING "Choose the type of build." FORCE)
endif()

# Output directories
include(GNUInstallDirs)
foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/bin)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/bin)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/lib)
endforeach()

# Qt5 dependencies
find_package(Qt5 COMPONENTS Widgets Sql REQUIRED)

# Find CFITSIO (required for FITS file support)
# Allow user to specify CFITSIO_ROOT, CFITSIO_INCLUDE_DIR, and CFITSIO_LIBRARY

set(CFITSIO_ROOT "" CACHE PATH "Root directory of CFITSIO installation")

# Try CMake config mode first
find_package(CFITSIO CONFIG)

if(NOT TARGET CFITSIO::cfitsio)
    # Fallback: try to find manually, using user hints if provided
    if(CFITSIO_ROOT)
        set(_cfitsio_inc_paths "${CFITSIO_ROOT}/include")
        set(_cfitsio_lib_paths "${CFITSIO_ROOT}/lib")
    else()
        set(_cfitsio_inc_paths "")
        set(_cfitsio_lib_paths "")
    endif()

    find_path(CFITSIO_INCLUDE_DIR fitsio.h
        HINTS ${_cfitsio_inc_paths}
        DOC "Path to cfitsio headers"
    )
    find_library(CFITSIO_LIBRARY NAMES cfitsio
        HINTS ${_cfitsio_lib_paths}
        DOC "Path to cfitsio library"
    )

    if(CFITSIO_INCLUDE_DIR AND CFITSIO_LIBRARY)
        message(STATUS "Found CFITSIO manually: ${CFITSIO_INCLUDE_DIR}, ${CFITSIO_LIBRARY}")
        add_library(CFITSIO::cfitsio UNKNOWN IMPORTED)
        set_target_properties(CFITSIO::cfitsio PROPERTIES
            IMPORTED_LOCATION "${CFITSIO_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${CFITSIO_INCLUDE_DIR}"
        )
    else()
        message(FATAL_ERROR "CFITSIO library not found. Please install cfitsio development files or specify -DCFITSIO_ROOT, -DCFITSIO_INCLUDE_DIR, and -DCFITSIO_LIBRARY.")
    endif()
else()
    message(STATUS "Found CFITSIO via CMake config: using CFITSIO::cfitsio target")
endif()

# Qt auto features
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Source files
set(SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/MainWindow.h
    src/DatabaseManager.cpp
    src/DatabaseManager.h
    src/FileRenamer.cpp
    src/FileRenamer.h
    src/SirilProcessor.cpp
    src/SirilProcessor.h
    src/ImageOrganizer.cpp
    src/ImageOrganizer.h
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        Qt5::Widgets
        Qt5::Sql
        CFITSIO::cfitsio
)

# Optional: Doxygen documentation
option(BUILD_DOC "Build documentation" OFF)
if(BUILD_DOC)
    find_package(Doxygen)
    if(DOXYGEN_FOUND)
        set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/doc/doxyfile.in)
        set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/doxyfile)
        configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
        add_custom_target(doc_doxygen ALL
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM
        )
    else()
        message("Doxygen not found. Documentation will not be generated.")
    endif()
endif()

# Packaging
set(CPACK_PACKAGE_NAME "SirilPrep")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Preprocessing tool for Siril and FITS images")
set(CPACK_PACKAGE_VENDOR "Your Organization")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_INSTALL_DIRECTORY "SirilPrep")
set(CPACK_PACKAGE_EXECUTABLES "${PROJECT_NAME}" "SirilPrep")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_GENERATOR "NSIS")
include(CPack)

# Feature summary
include(FeatureSummary)
feature_summary(WHAT ENABLED_FEATURES DISABLED_FEATURES PACKAGES_FOUND)
feature_summary(FILENAME ${CMAKE_CURRENT_BINARY_DIR}/features.log WHAT ALL)