include_directories(${GTEST_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR}/../server ${THIRDPARTY_ROOT}/include ${CMAKE_SOURCE_DIR}/common ${SOCI_INCLUDE_DIRS})

add_executable(test_client test_client.cpp utils.cpp ../common/utils.cpp)
add_dependencies(test_client googletest)
target_link_libraries(test_client ${GTEST_LIBS_DIR}/libgtest.a ${GTEST_LIBS_DIR}/libgtest_main.a ${CMAKE_THREAD_LIBS_INIT} dl pthread broker_client)
add_test(NAME test_client COMMAND test_client)

#if(BUILD_DB)
#    add_executable(test_db test_db.cpp utils.cpp ../common/utils.cpp ../common/dbaccess.cpp)
#    add_dependencies(test_db googletest _database)
#    target_link_libraries(test_db ${GTEST_LIBS_DIR}/libgtest.a ${GTEST_LIBS_DIR}/libgtest_main.a ${CMAKE_THREAD_LIBS_INIT} dl pthread ${SOCI_LIBRARIES})
#    add_test(NAME test_db COMMAND test_db)
#endif()
