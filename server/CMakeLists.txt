# dependencies
include( ExternalProject )

# boost
set( boost_URL https://github.com/boostorg/boost.git )
set( boost_INSTALL ${THIRDPARTY_ROOT} )
set( boost_INCLUDE_DIR ${boost_INSTALL}/include )
set( boost_LIB_DIR ${boost_INSTALL}/lib )
ExternalProject_Add( external_boost
    PREFIX ${CMAKE_BINARY_DIR}/boost
    GIT_REPOSITORY ${boost_URL}
    GIT_TAG boost-1.73.0
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND ./bootstrap.sh
        --with-libraries=program_options
        --with-libraries=log
        --with-libraries=date_time
        --with-libraries=system
        --prefix=<INSTALL_DIR>
    BUILD_COMMAND ./b2 cxxflags=${CMAKE_CXX_FLAGS} install link=static variant=release runtime-link=shared threading=multi -d+2
    INSTALL_COMMAND ""
    INSTALL_DIR ${boost_INSTALL}
)

add_library( boost::system STATIC IMPORTED )
set_property( TARGET boost::system PROPERTY IMPORTED_LOCATION ${boost_LIB_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}boost_system${CMAKE_STATIC_LIBRARY_SUFFIX} )
add_dependencies( boost::system external_boost )

add_library( boost::date_time STATIC IMPORTED )
set_property( TARGET boost::date_time PROPERTY IMPORTED_LOCATION ${boost_LIB_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}boost_date_time${CMAKE_STATIC_LIBRARY_SUFFIX} )
add_dependencies( boost::date_time external_boost )

add_library( boost::program_options STATIC IMPORTED )
set_property( TARGET boost::program_options PROPERTY IMPORTED_LOCATION ${boost_LIB_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}boost_program_options${CMAKE_STATIC_LIBRARY_SUFFIX} )
add_dependencies( boost::program_options external_boost )

add_library( boost::log STATIC IMPORTED )
set_property( TARGET boost::log PROPERTY IMPORTED_LOCATION ${boost_LIB_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}boost_log${CMAKE_STATIC_LIBRARY_SUFFIX} )
add_dependencies( boost::log external_boost )

add_library( boost::log_setup STATIC IMPORTED )
set_property( TARGET boost::log_setup PROPERTY IMPORTED_LOCATION ${boost_LIB_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}boost_log_setup${CMAKE_STATIC_LIBRARY_SUFFIX} )
add_dependencies( boost::log_setup external_boost )

add_library( boost::thread STATIC IMPORTED )
set_property( TARGET boost::thread PROPERTY IMPORTED_LOCATION ${boost_LIB_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}boost_thread${CMAKE_STATIC_LIBRARY_SUFFIX} )
add_dependencies( boost::thread external_boost )

add_library( boost::filesystem STATIC IMPORTED )
set_property( TARGET boost::filesystem PROPERTY IMPORTED_LOCATION ${boost_LIB_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}boost_filesystem${CMAKE_STATIC_LIBRARY_SUFFIX} )
add_dependencies( boost::filesystem external_boost )

add_library( boost::regex STATIC IMPORTED )
set_property( TARGET boost::regex PROPERTY IMPORTED_LOCATION ${boost_LIB_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}boost_regex${CMAKE_STATIC_LIBRARY_SUFFIX} )
add_dependencies( boost::regex external_boost )

add_library( boost::atomic STATIC IMPORTED )
set_property( TARGET boost::atomic PROPERTY IMPORTED_LOCATION ${boost_LIB_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}boost_atomic${CMAKE_STATIC_LIBRARY_SUFFIX} )
add_dependencies( boost::atomic external_boost )

add_library( boost::chrono STATIC IMPORTED )
set_property( TARGET boost::chrono PROPERTY IMPORTED_LOCATION ${boost_LIB_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}boost_chrono${CMAKE_STATIC_LIBRARY_SUFFIX} )
add_dependencies( boost::chrono external_boost )

#set(Boost_LIBRARIES boost::program_options boost::log boost::log_setup boost::filesystem boost::regex boost::thread boost::chrono boost::date_time boost::atomic boost::system)
set(Boost_LIBRARIES boost::program_options boost::log_setup boost::log boost::thread)

# soci
ExternalProject_Add(soci
    PREFIX            ${CMAKE_BINARY_DIR}/soci
    GIT_REPOSITORY    https://github.com/SOCI/soci.git
    UPDATE_COMMAND    ""
    CONFIGURE_COMMAND ${CMAKE_COMMAND} "-G${CMAKE_GENERATOR}" <SOURCE_DIR>
        -DWITH_BOOST:BOOL=FALSE
        -DSOCI_TESTS:BOOL=FALSE
        -DCMAKE_CXX_FLAGS:STRING=${CMAKE_CXX_FLAGS}
        -DCMAKE_SOURCE_DIR:PATH=<SOURCE_DIR>
        -DCMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER}
        -DCMAKE_C_COMPILER:FILEPATH=${CMAKE_C_COMPILER}
        -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
        -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
        -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
        -DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=ON
        -DCMAKE_DISABLE_FIND_PACKAGE_PNG=TRUE
        -DSOCI_CXX11:BOOL=ON
    INSTALL_DIR  ${THIRDPARTY_ROOT}
    LOG_DOWNLOAD ON
    LOG_INSTALL  ON 
)
ExternalProject_Get_Property(soci INSTALL_DIR)
add_library(soci::core  SHARED IMPORTED)
add_library(soci::mysql SHARED IMPORTED)
set_target_properties(soci::core  PROPERTIES IMPORTED_LOCATION ${INSTALL_DIR}/lib64/${CMAKE_SHARED_LIBRARY_PREFIX}soci_core${CMAKE_SHARED_LIBRARY_SUFFIX})
set_target_properties(soci::mysql PROPERTIES IMPORTED_LOCATION ${INSTALL_DIR}/lib64/${CMAKE_SHARED_LIBRARY_PREFIX}soci_mysql${CMAKE_SHARED_LIBRARY_SUFFIX})
set(SOCI_INCLUDE_DIRS ${INSTALL_DIR}/include)
set(SOCI_LIBRARIES    soci::mysql soci::core)
add_dependencies(soci::core  soci)
add_dependencies(soci::mysql soci)

# cryptopp
ExternalProject_Add(cryptopp
    PREFIX            ${CMAKE_BINARY_DIR}/cryptopp
    GIT_REPOSITORY    https://github.com/weidai11/cryptopp.git
    GIT_TAG           CRYPTOPP_5_6_5
    UPDATE_COMMAND    ""
    CONFIGURE_COMMAND ${CMAKE_COMMAND} "-G${CMAKE_GENERATOR}" <SOURCE_DIR>
        -DWITH_BOOST:BOOL=FALSE
        -DCMAKE_CXX_FLAGS:STRING=${CMAKE_CXX_FLAGS}
        -DCMAKE_SOURCE_DIR:PATH=<SOURCE_DIR>
        -DCMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER}
        -DCMAKE_C_COMPILER:FILEPATH=${CMAKE_C_COMPILER}
        -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
        -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
        -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
        -DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=ON
        -DCMAKE_DISABLE_FIND_PACKAGE_PNG=TRUE
    INSTALL_DIR  ${THIRDPARTY_ROOT}
    LOG_DOWNLOAD ON
    LOG_INSTALL  ON 
)
ExternalProject_Get_Property(cryptopp INSTALL_DIR)
add_library(cryptopp::cryptopp  STATIC IMPORTED)
set_target_properties(cryptopp::cryptopp PROPERTIES IMPORTED_LOCATION ${INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}cryptopp${CMAKE_STATIC_LIBRARY_SUFFIX})
set(CRYPTOPP_INCLUDE_DIRS ${INSTALL_DIR}/include/cryptopp)
set(CRYPTOPP_LIBRARIES    cryptopp::cryptopp)
add_dependencies(cryptopp::cryptopp cryptopp)

# project
set(PROJECT_NAME server)

# proto file
get_filename_component(broker_proto "myservice.proto" ABSOLUTE)
get_filename_component(broker_proto_path "${broker_proto}" PATH)

# generated service sources (nb. set generated src scope to PARENT_SCOPE so they can be referenced by the server client lib)
set(broker_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/myservice.pb.cc")
set(broker_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/myservice.pb.h")
set(broker_grpc_srcs  "${CMAKE_CURRENT_BINARY_DIR}/myservice.grpc.pb.cc") 
set(broker_grpc_hdrs  "${CMAKE_CURRENT_BINARY_DIR}/myservice.grpc.pb.h")
add_custom_command(
    OUTPUT "${broker_proto_srcs}" "${broker_proto_hdrs}" "${broker_grpc_srcs}" "${broker_grpc_hdrs}"
    COMMAND ${_PROTOBUF_PROTOC}
        ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I"${broker_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${broker_proto}"
    DEPENDS "${broker_proto}"
)
add_custom_target(broker_grpc_source_generation DEPENDS ${broker_proto_srcs} ${broker_proto_hdrs} ${broker_grpc_srcs} ${broker_grpc_hdrs})

# server app
if(BUILD_DEPS_ONLY)
    set(SOURCES dummy.cpp)
else()
    set(SOURCES main.cpp server.cpp ../common/utils.cpp ../common/logger_syslog.cpp ../common/dbaccess.cpp)
endif()
include_directories("${CMAKE_CURRENT_BINARY_DIR}" "${CMAKE_SOURCE_DIR}/common" "${CMAKE_SOURCE_DIR}/server" "${boost_INCLUDE_DIR}" "${SOCI_INCLUDE_DIRS}" "${CRYPTOPP_INCLUDE_DIRS}")
add_executable(${PROJECT_NAME} ${SOURCES} ${broker_proto_srcs} ${broker_grpc_srcs})
target_link_libraries(${PROJECT_NAME} ${_GRPC_GRPCPP} ${_PROTOBUF_LIBPROTOBUF} ${Boost_LIBRARIES} ${SOCI_LIBRARIES} ${CRYPTOPP_LIBRARIES}) 
add_dependencies(${PROJECT_NAME} broker_grpc_source_generation)
set_target_properties(${PROJECT_NAME} PROPERTIES INSTALL_RPATH "$ORIGIN")
if(NOT BUILD_DEPS_ONLY)
    # server client library
    set(LIBRARY_PROJECT_NAME client)
    set(LIBRARY_SOURCES client.cpp ../common/logger_syslog.cpp)
    add_library(${LIBRARY_PROJECT_NAME} ${LIBRARY_SOURCES} ${broker_proto_srcs} ${broker_grpc_srcs})
    target_link_libraries(${LIBRARY_PROJECT_NAME} ${_GRPC_GRPCPP} ${_PROTOBUF_LIBPROTOBUF} ${Boost_LIBRARIES} ${CRYPTOPP_LIBRARY}) 
    add_dependencies(${LIBRARY_PROJECT_NAME} broker_grpc_source_generation)
endif()

# create test ca / certs
configure_file(${CMAKE_SOURCE_DIR}/keys/create_certs.sh.in ${CMAKE_SOURCE_DIR}/keys/create_certs.sh @ONLY)

