project (rest-server)

set(sync_source_files
#    request_processor.cc
    rest-server.cc)

set(async_source_files
    liveness_resource.cc
    rest_processor.cc
    async-rest.cc)
    
#set (tests)

add_executable(rest-server ${sync_source_files})
target_link_libraries(rest-server ${CPPNETLIB_LIBRARIES} ${Boost_SYSTEM_LIBRARY})

add_executable(async-server ${async_source_files})
target_link_libraries(async-server ${CPPNETLIB_LIBRARIES} ${Boost_SYSTEM_LIBRARY} ${Boost_THREAD_LIBRARY})
