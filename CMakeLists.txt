project(ImageRegistration)
include_regular_expression("^.*$")

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

add_executable(MultiResImageRegistrationViola MultiResImageRegistrationViola.cxx )
target_link_libraries(MultiResImageRegistrationViola ${ITK_LIBRARIES})

add_executable(MultiResImageRegistrationMattes MultiResImageRegistrationMattes.cxx )
target_link_libraries(MultiResImageRegistrationMattes ${ITK_LIBRARIES})

