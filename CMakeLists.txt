cmake_minimum_required(VERSION 3.10)

if(DEFINED ENV{GITHUB_RUN_NUMBER})
  set(PATCH_NUM $ENV{GITHUB_RUN_NUMBER})
else()
  set(PATCH_NUM "1")
endif()

project(allocator
  LANGUAGES CXX
  VERSION 0.0.${PATCH_NUM}
)

set(CMAKE_BUILD_TYPE Debug)

configure_file(version.h.in version.h)
include_directories(${CMAKE_BINARY_DIR})

option(BUILD_TESTS "Build unittests" OFF)

add_subdirectory(src)

install(TARGETS allocator RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT example@example.com)

include(CPack)

