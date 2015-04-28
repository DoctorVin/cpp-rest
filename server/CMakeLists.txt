project (rest-server)

set(sync_source_files
#    request_processor.cc
    rest-server.cc)

set(async_source_files
    event_persister.cc
    request_context.cc
    memory_events.cc
    event_resource.cc
    liveness_resource.cc
    rest_processor.cc
    async-rest.cc)

MESSAGE(STATUS "CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")
set(private_include_directories ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${private_include_directories})
MESSAGE(STATUS "Include directories: ${private_include_directories}")

foreach(filename ${async_source_files})
    MESSAGE(STATUS "Evaluating tests for ${filename}")
    get_filename_component(test_name ${filename} NAME_WE)
    set(filepath ${CMAKE_CURRENT_SOURCE_DIR}/${filename})
    set(testpath ${CMAKE_CURRENT_SOURCE_DIR}/test/${test_name}_test.cc)
    if (EXISTS ${testpath})
        set(test_sources ${filepath} ${testpath})
        MESSAGE(STATUS "Adding test for ${test_name} using ${test_sources}")
        add_boost_test(${test_name} "${test_sources}")
    endif()
endforeach()

#file(GLOB TESTS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} test/*.cc)
#MESSAGE(STATUS "Test files: ${TESTS}")
#add_boost_tests(${TESTS})    

#add_executable(rest-server ${sync_source_files})
#target_link_libraries(rest-server ${CPPNETLIB_LIBRARIES} ${Boost_SYSTEM_LIBRARY})

add_executable(async-server ${async_source_files})
target_link_libraries(async-server ${CPPNETLIB_LIBRARIES} ${Boost_SYSTEM_LIBRARY} ${Boost_THREAD_LIBRARY})
