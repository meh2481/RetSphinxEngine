set(compressor_src
images.cpp
main.cpp
main.h
)

add_executable(compressor ${compressor_src})
target_link_libraries(compressor io)
