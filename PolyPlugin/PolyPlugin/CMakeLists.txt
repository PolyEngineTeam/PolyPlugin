# Include dir
set(POLYPLUGIN_INCLUDE ${CMAKE_CURRENT_LIST_DIR}/src)

# SWIG interface files
file(GLOB_RECURSE POLYPLUGIN_SWIG_INTERFACE_FILES RELATIVE ${CMAKE_CURRENT_LIST_DIR}
     ${POLYPLUGIN_INCLUDE}/*.i)
GenerateSourceGoups("${POLYPLUGIN_SWIG_INTERFACE_FILES}")

# Source files list
file(GLOB_RECURSE POLYPLUGIN_SRCS RELATIVE ${CMAKE_CURRENT_LIST_DIR}
    ${POLYPLUGIN_INCLUDE}/*.cpp 
    ${POLYPLUGIN_INCLUDE}/*.hpp 
    ${POLYPLUGIN_INCLUDE}/*.h)
GenerateSourceGoups("${POLYPLUGIN_SRCS}")



add_library(${POLYPLUGIN_TARGET} INTERFACE)
add_custom_target(${POLYPLUGIN_TARGET}_ SOURCES ${POLYPLUGIN_SRCS})
target_include_directories(${POLYPLUGIN_TARGET} INTERFACE ${POLYPLUGIN_INCLUDE})



foreach(Source ${POLYPLUGIN_SWIG_INTERFACE_FILES})
	get_filename_component(SourceName ${Source} NAME_WE)
    SET_SOURCE_FILES_PROPERTIES(${Source} PROPERTIES CPLUSPLUS ON)

    swig_add_library(${SourceName} LANGUAGE python SOURCES ${Source})

    swig_link_libraries(${SourceName} ${PYTHON_LIBRARIES})
    target_link_libraries(${SourceName} ${POLYPLUGIN_TARGET})
endforeach()