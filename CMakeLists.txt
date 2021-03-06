cmake_minimum_required(VERSION 3.7.0 FATAL_ERROR)

find_package(IRODS REQUIRED)

set(IRODS_PLUGIN_REVISION "0")

set(CMAKE_C_COMPILER ${IRODS_EXTERNALS_FULLPATH_CLANG}/bin/clang)
set(CMAKE_CXX_COMPILER ${IRODS_EXTERNALS_FULLPATH_CLANG}/bin/clang++)
set(CMAKE_EXE_LINKER_FLAGS_INIT "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")

project(irods_rule_engine_plugin-metadata_guard C CXX)

include(${IRODS_TARGETS_PATH})

if (NOT CPACK_PACKAGING_INSTALL_PREFIX)
    set(CPACK_PACKAGING_INSTALL_PREFIX "/" CACHE STRING "Package root path. \"/\" is correct for normal package builds.." FORCE)
    message(STATUS "Setting unspecified CPACK_PACKAGING_INSTALL_PREFIX to '${CPACK_PACKAGING_INSTALL_PREFIX}'. This is the correct setting for normal builds.")
endif()

if (NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build {Debug, Release}." FORCE)
    message(STATUS "Setting unspecified CMAKE_BUILD_TYPE to '${CMAKE_BUILD_TYPE}'")
endif()

if (NOT IRODS_EXTERNALS_PACKAGE_ROOT)
    set(IRODS_EXTERNALS_PACKAGE_ROOT "/opt/irods-externals" CACHE STRING "Choose the location of iRODS external packages." FORCE)
    message(STATUS "Setting unspecified IRODS_EXTERNALS_PACKAGE_ROOT to '${IRODS_EXTERNALS_PACKAGE_ROOT}'")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_INSTALL_RPATH ${IRODS_EXTERNALS_FULLPATH_CLANG_RUNTIME}/lib)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -stdlib=libc++ -Wl,-z,defs")

set(PLUGIN irods_rule_engine_plugin-metadata_guard)

add_library(${PLUGIN} MODULE ${CMAKE_SOURCE_DIR}/src/main.cpp
                             ${CMAKE_SOURCE_DIR}/src/user_administration.cpp)

set_target_properties(${PLUGIN} PROPERTIES CXX_STANDARD ${IRODS_CXX_STANDARD})

target_compile_options(${PLUGIN} PRIVATE -Wno-write-strings -nostdinc++)

target_compile_definitions(${PLUGIN} PRIVATE ${IRODS_COMPILE_DEFINITIONS}
                                             RODS_SERVER
                                             IRODS_ENABLE_SYSLOG
                                             IRODS_FILESYSTEM_ENABLE_SERVER_SIDE_API
                                             IRODS_USER_ADMINISTRATION_ENABLE_SERVER_SIDE_API)
                                             #IRODS_IO_TRANSPORT_ENABLE_SERVER_SIDE_API
                                             #FMT_HEADER_ONLY
                                             #SPDLOG_FMT_EXTERNAL)

target_include_directories(${PLUGIN} PRIVATE ${IRODS_INCLUDE_DIRS}
                                             ${IRODS_EXTERNALS_FULLPATH_CLANG}/include/c++/v1
                                             ${IRODS_EXTERNALS_FULLPATH_BOOST}/include
                                             ${IRODS_EXTERNALS_FULLPATH_SPDLOG}/include
                                             ${IRODS_EXTERNALS_FULLPATH_JSON}/include)

target_link_libraries(${PLUGIN} PRIVATE irods_server
                                        irods_common
                                        ${IRODS_EXTERNALS_FULLPATH_BOOST}/lib/libboost_filesystem.so
                                        ${IRODS_EXTERNALS_FULLPATH_BOOST}/lib/libboost_system.so
                                        c++abi
                                        dl)

install(TARGETS ${PLUGIN} LIBRARY DESTINATION ${IRODS_PLUGINS_DIRECTORY}/rule_engines)

install(FILES ${CMAKE_SOURCE_DIR}/packaging/test_rule_engine_plugin_metadata_guard.py
        DESTINATION ${IRODS_HOME_DIRECTORY}/scripts/irods/test
        PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)

install(FILES ${CMAKE_SOURCE_DIR}/packaging/run_metadata_guard_test.py
        DESTINATION ${IRODS_HOME_DIRECTORY}/scripts
        PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)

set(PLUGIN_PACKAGE_NAME irods-rule-engine-plugin-metadata_guard)

set(CPACK_PACKAGE_VERSION ${IRODS_VERSION}.${IRODS_PLUGIN_REVISION})
set(CPACK_PACKAGE_FILE_NAME "${PLUGIN_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${IRODS_LINUX_DISTRIBUTION_NAME}${IRODS_LINUX_DISTRIBUTION_VERSION_MAJOR}-${CMAKE_SYSTEM_PROCESSOR}")
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
set(CPACK_COMPONENT_INCLUDE_TOPLEVEL_DIRECTORY OFF)
set(CPACK_COMPONENTS_GROUPING IGNORE)
set(CPACK_PACKAGE_CONTACT "Renaissance Computing Institute <info@irods.org>")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "The integrated Rule-Oriented Data System")
set(CPACK_PACKAGE_VENDOR "Renaissance Computing Institute <info@irods.org>")

set(CPACK_DEB_COMPONENT_INSTALL OFF)
set(CPACK_DEBIAN_PACKAGE_SECTION "contrib/science")
set(CPACK_DEBIAN_COMPRESSION_TYPE "gzip")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "extra")
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://irods.org")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS OFF)
set(CPACK_DEBIAN_PACKAGE_CONTROL_STRICT_PERMISSION ON)
set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${CMAKE_SOURCE_DIR}/packaging/postinst;")
set(CPACK_DEBIAN_PACKAGE_BREAKS ${PLUGIN_PACKAGE_NAME})
set(CPACK_DEBIAN_PACKAGE_REPLACES ${PLUGIN_PACKAGE_NAME})

set(CPACK_RPM_COMPONENT_INSTALL OFF)
set(CPACK_RPM_PACKAGE_RELEASE "1")
set(CPACK_RPM_PACKAGE_LICENSE "BSD-3-Clause")
set(CPACK_RPM_PACKAGE_VENDOR "iRODS Consortium")
set(CPACK_RPM_PACKAGE_URL "https://irods.org")
set(CPACK_RPM_PACKAGE_AUTOREQ 0)
set(CPACK_RPM_PACKAGE_AUTOPROV 0)
set(CPACK_RPM_PACKAGE_OBSOLETES ${PLUGIN_PACKAGE_NAME})
set(CPACK_RPM_POST_INSTALL_SCRIPT_FILE "${CMAKE_SOURCE_DIR}/packaging/postinst")

set(CPACK_ARCHIVE_COMPONENT_INSTALL OFF)

set(CPACK_DEBIAN_PACKAGE_NAME ${PLUGIN_PACKAGE_NAME})
set(CPACK_DEBIAN_PACKAGE_DEPENDS "${IRODS_PACKAGE_DEPENDENCIES_STRING}, irods-server (= ${IRODS_VERSION}), irods-runtime (= ${IRODS_VERSION}), libc6")

set(CPACK_RPM_PACKAGE_NAME ${PLUGIN_PACKAGE_NAME})
if (IRODS_LINUX_DISTRIBUTION_NAME STREQUAL "centos" OR
    IRODS_LINUX_DISTRIBUTION_NAME STREQUAL "centos linux" OR
    IRODS_LINUX_DISTRIBUTION_NAME STREQUAL "opensuse")
    set(CPACK_RPM_PACKAGE_REQUIRES "${IRODS_PACKAGE_DEPENDENCIES_STRING}, irods-server = ${IRODS_VERSION}, irods-runtime = ${IRODS_VERSION}")
endif()

if (NOT CPACK_GENERATOR)
    set(CPACK_GENERATOR ${IRODS_CPACK_GENERATOR} CACHE STRING "CPack generator to use, e.g. {DEB, RPM, TGZ}." FORCE)
    message(STATUS "Setting unspecified CPACK_GENERATOR to ${CPACK_GENERATOR}. This is the correct setting for normal builds.")
endif()

include(CPack)
