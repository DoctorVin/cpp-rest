#add boost tests for a given code unit. 
macro (add_boost_test UNIT_NAME SOURCE_LIST)
    MESSAGE(STATUS "UNIT_NAME: ${UNIT_NAME}")
    MESSAGE(STATUS "SOURCES: ${SOURCE_LIST}")
    add_executable(test_${UNIT_NAME} ${SOURCE_LIST})
    target_link_libraries(test_${UNIT_NAME} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
    set_target_properties(test_${UNIT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/test)
    add_test(NAME ${UNIT_NAME}
             WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/test
             COMMAND ${CMAKE_BINARY_DIR}/test/test_${UNIT_NAME})
endmacro(add_boost_test)
