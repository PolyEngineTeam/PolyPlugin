# Include dir
set(POLYPLUGIN_INCLUDE ${CMAKE_CURRENT_LIST_DIR}/src)

# gather source files list
file(GLOB_RECURSE POLYPLUGIN_SRCS RELATIVE ${CMAKE_CURRENT_LIST_DIR}
    ${POLYPLUGIN_INCLUDE}/*.cpp 
    ${POLYPLUGIN_INCLUDE}/*.hpp 
    ${POLYPLUGIN_INCLUDE}/*.h)
GenerateSourceGoups("${POLYPLUGIN_SRCS}")

# add library and custom target so the header files will be visible in the IDE
add_library(${POLYPLUGIN_TARGET} INTERFACE)
add_custom_target(${POLYPLUGIN_TARGET}_ SOURCES ${POLYPLUGIN_SRCS})

# require C++17 support
target_compile_features(${POLYPLUGIN_TARGET} INTERFACE cxx_std_17)

# set target include directories
target_include_directories(${POLYPLUGIN_TARGET} INTERFACE
    $<BUILD_INTERFACE:${POLYPLUGIN_INCLUDE}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)