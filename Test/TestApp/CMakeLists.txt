# Include dir
set(TEST_APP_INCLUDE ${CMAKE_CURRENT_LIST_DIR}/src)

# Source files list
file(GLOB_RECURSE TEST_APP_SRCS RELATIVE ${CMAKE_CURRENT_LIST_DIR}
    ${TEST_APP_INCLUDE}/*.cpp 
    ${TEST_APP_INCLUDE}/*.hpp 
    ${TEST_APP_INCLUDE}/*.h)
GenerateSourceGoups("${TEST_APP_SRCS}")

add_executable(${TEST_APP_TARGET} ${TEST_APP_SRCS})
target_include_directories(${TEST_APP_TARGET} PUBLIC ${TEST_APP_INCLUDE})

target_link_libraries(${TEST_APP_TARGET} ${POLYPLUGIN_TARGET})
target_link_libraries(${TEST_APP_TARGET} ${CALCULATOR_PLUGIN_API_TARGET})
#add_dependencies(${TEST_APP_TARGET} ${CALCULATOR_PLUGIN_TARGET})