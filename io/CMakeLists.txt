set(io_src
easylogging++.h
FileOperations.h
FileOperations.cpp
PakLoader.h
PakLoader.cpp
Parse.cpp
Parse.h
ResourceTypes.h
stb_image.h
stb_image_write.h
tinydir.h
tinyxml2.cpp
tinyxml2.h
wfLZ.c
wfLZ.h
)

add_library(io ${io_src})
