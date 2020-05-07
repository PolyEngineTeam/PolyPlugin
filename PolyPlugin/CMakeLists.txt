cmake_minimum_required(VERSION 3.12)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

##
# Function that generates source groups
##
function(GenerateSourceGoups Sources)
	foreach(Source ${Sources})
		get_filename_component(SourcePath "${Source}" PATH)
		if (WIN32)
			string(REPLACE "/" "\\" SourcePath "${SourcePath}")
		endif()
		source_group("${SourcePath}" FILES "${Source}")
	endforeach()
endfunction()

##
# Start
##
project(PolyPlugin VERSION "1.0.0")
include(GNUInstallDirs)

configure_file(
	"${PROJECT_SOURCE_DIR}/config/CmakeConfig.hpp.in" 
	"${PROJECT_BINARY_DIR}/config/CmakeConfig.hpp"
)
include_directories("${PROJECT_BINARY_DIR}/config")

set(POLY_PLUGIN_ROOT_DIR ${CMAKE_SOURCE_DIR})
set(DIST_DIR "${POLY_PLUGIN_ROOT_DIR}/../Dist")
set(COMMON_BUILD_DIR "${POLY_PLUGIN_ROOT_DIR}/../CommonBuild")

# Output dirs
if (WIN32)
	set(FULL_DIST_DIR "${DIST_DIR}") # Config name is added autmatically on windows
else()
	set(FULL_DIST_DIR "${DIST_DIR}/${BUILD_CONFIG_NAME}")
endif()
	
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${FULL_DIST_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${FULL_DIST_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${FULL_DIST_DIR})

# C++ standard 
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(POLYPLUGIN_TARGET PolyPlugin)
set(CALCULATOR_PLUGIN_API_TARGET CalculatorPluginAPI)
set(CALCULATOR_PLUGIN_TARGET CalculatorPlugin)
set(TEST_APP_TARGET TestApp)

add_subdirectory(PolyPlugin)
add_subdirectory(CalculatorPluginAPI)
add_subdirectory(CalculatorPlugin)
add_subdirectory(TestApp)



#################################################
# installation instructions
#################################################

install(TARGETS ${POLYPLUGIN_TARGET}
    EXPORT ${POLYPLUGIN_TARGET}_Targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

include(CMakePackageConfigHelpers)
write_basic_package_version_file("${POLYPLUGIN_TARGET}ConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion)

configure_package_config_file(
    "${PROJECT_SOURCE_DIR}/cmake/${POLYPLUGIN_TARGET}Config.cmake.in"
    "${PROJECT_BINARY_DIR}/${POLYPLUGIN_TARGET}Config.cmake"
	INSTALL_DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${POLYPLUGIN_TARGET}/cmake)

install(EXPORT ${POLYPLUGIN_TARGET}_Targets
	FILE ${POLYPLUGIN_TARGET}Targets.cmake
	NAMESPACE ${POLYPLUGIN_TARGET}::
	DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${POLYPLUGIN_TARGET}/cmake)

install(FILES "${PROJECT_BINARY_DIR}/${POLYPLUGIN_TARGET}Config.cmake"
	"${PROJECT_BINARY_DIR}/${POLYPLUGIN_TARGET}ConfigVersion.cmake"
	DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${POLYPLUGIN_TARGET}/cmake)
	
install(DIRECTORY ${POLYPLUGIN_INCLUDE} DESTINATION include)