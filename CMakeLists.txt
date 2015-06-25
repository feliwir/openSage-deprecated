cmake_minimum_required (VERSION 3.1)

project (openSage)
set(PROJ_NAME openSage)

# Detect the OS
if(${CMAKE_HOST_WIN32})
    set(WINDOWS 1)
elseif(${CMAKE_HOST_APPLE})
    set(MACOSX 1)
elseif(${CMAKE_HOST_UNIX})
    set(LINUX 1)
else()
    message(FATAL_ERROR "Unsupported operating system")
    return()
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin)

set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules")

set (VERSION_MAJOR 0)
set (VERSION_MINOR 1)

set (SOURCES_DIR "src")

#################################################################################################################
# ================================================ SFML SETUP ================================================= #
#################################################################################################################

# Static openSage lib has no interest on Linux or OS X
if (WINDOWS)
    set (OPENSAGE_BUILD_STATIC FALSE CACHE BOOL "TRUE to build static version of openSage")
endif()

if (OPENSAGE_BUILD_STATIC)
    set (SFML_STATIC_LIBRARIES TRUE)
endif()

find_package (SFML 2.3 COMPONENTS graphics window system audio REQUIRED)
message(STATUS "SFML libraries: ${SFML_LIBRARIES}")
message(STATUS "Additional dependencies: ${SFML_DEPENDENCIES}")


#################################################################################################################
# ============================================= LIBMPG123 SETUP =============================================== #
#################################################################################################################

find_package (MPG123 REQUIRED)
message(STATUS "MPG123 libraries: ${MPG123_LIBRARIES}")

#################################################################################################################
# ============================================== FFMPEG SETUP ================================================= #
#################################################################################################################

find_package (FFMPEG REQUIRED)
message(STATUS "FFMPEG libraries: ${FFMPEG_LIBRARIES}")

#################################################################################################################
# ============================================== openSage SETUP =============================================== #
#################################################################################################################

file(GLOB_RECURSE SOURCE_FILES ${SOURCES_DIR}/[a-z,A-Z]*.[c,h]* ${SOURCES_DIR}/DebugTools/[a-z,A-Z]*.[c,h]*)

set(OPENSAGE_INCLUDE_DIRS "${MPG123_INCLUDE_DIR};${SFML_INCLUDE_DIR};${FFMPEG_INCLUDE_DIRS}")
include_directories (${OPENSAGE_INCLUDE_DIRS})

add_executable(${PROJ_NAME} ${SOURCE_FILES})

target_compile_features(${PROJ_NAME} PRIVATE cxx_range_for)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(ADDITIONAL_LIBS "-lboost_system -lboost_filesystem -lz -llzma -lva -lbz2 -lm -lpthread")
endif()

if (OPENSAGE_BUILD_STATIC)
    set_target_properties(${PROJ_NAME} PROPERTIES
        COMPILE_DEFINITIONS SFML_STATIC)
		
	message(STATUS "Link openSage statically against: ${MPG123_LIBRARIES} ${SFML_LIBRARIES} ${SFML_DEPENDENCIES} ${FFMPEG_LIBRARIES} ${ADDITIONAL_LIBS}")
    target_link_libraries(${PROJ_NAME} PRIVATE ${MPG123_LIBRARIES} ${SFML_LIBRARIES} ${SFML_DEPENDENCIES} ${FFMPEG_LIBRARIES} ${ADDITIONAL_LIBS})
else()
    message(STATUS "Link openSage against: ${MPG123_LIBRARIES} ${SFML_LIBRARIES} ${SFML_DEPENDENCIES} ${FFMPEG_LIBRARIES} ${ADDITIONAL_LIBS}")
    target_link_libraries(${PROJ_NAME} PRIVATE ${MPG123_LIBRARIES} ${SFML_LIBRARIES} ${SFML_DEPENDENCIES}  ${FFMPEG_LIBRARIES}  ${ADDITIONAL_LIBS})
endif()

include (${CMAKE_SOURCE_DIR}/cmake/Install.cmake)