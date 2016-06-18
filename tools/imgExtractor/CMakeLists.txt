set(imgCompressor_src
main.cpp
)

add_executable(imgCompressor ${imgCompressor_src})
target_link_libraries(imgCompressor io)
