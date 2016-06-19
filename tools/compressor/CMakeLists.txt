set(compressor_src
main.cpp
)

add_executable(compressor ${compressor_src})
target_link_libraries(compressor io)
