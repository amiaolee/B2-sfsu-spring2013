cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
include(CMakeDependentOption)
#
# use ExternalProject
include(ExternalProject)

if(WIN32 OR NOT EXISTS /dev/urandom)
  cmake_minimum_required(VERSION 2.8.5)
endif()
project(ImageRegistration)
include_regular_expression("^.*$")

if( WIN32 )

  string( LENGTH "${CMAKE_CURRENT_SOURCE_DIR}" n )
  if( n GREATER 50 )
    message(
      FATAL_ERROR
      "ITK source code directory path length is too long (${n} > 50)."
      "Please move the ITK source code directory to a directory with a shorter path."
      )
  endif()

  string( LENGTH "${CMAKE_CURRENT_BINARY_DIR}" n )
  if( n GREATER 50 )
    message(
      FATAL_ERROR
      "ITK build directory path length is too long (${n} > 50)."
      "Please set the ITK build directory to a directory with a shorter path."
      )
  endif()

endif()

# Find ITK.
FIND_PACKAGE(ITK)
  IF(ITK_FOUND)
    INCLUDE(${ITK_USE_FILE})
    ELSE()
      MESSAGE(
      		  FATAL_ERROR
              "Cannot build without ITK.  Please set ITK_DIR."
              )
 ENDIF()

add_executable(MultiResImageRegistrationMattes MultiResImageRegistrationMattes.cxx )
target_link_libraries(MultiResImageRegistrationMattes ${ITK_LIBRARIES})

add_executable(DicomRead DicomRead.cxx )
target_link_libraries(DicomRead ${ITK_LIBRARIES})
