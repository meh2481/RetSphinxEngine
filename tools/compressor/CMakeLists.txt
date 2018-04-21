set(compressor_src
images.cpp
lua.cpp
mesh.cpp
main.cpp
main.h
shader.cpp
sound.cpp
)

add_executable(compressor ${compressor_src})
target_link_libraries(compressor lua53 io engine squish)
