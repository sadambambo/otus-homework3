
#target_compile_features(ip_filter PUBLIC cxx_std_17)
#target_include_directories(ip_filter INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(allocator memory_allocator.cpp)
