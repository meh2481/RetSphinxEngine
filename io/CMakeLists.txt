set(io_src
Logger.h
Logger.cpp
FileOperations.h
FileOperations.cpp
Hash.cpp
Hash.h
PakLoader.h
PakLoader.cpp
StringUtils.cpp
StringUtils.h
ResourceTypes.h
stb_image.h
stb_image_write.h
tinydir.h
tinyxml2.cpp
tinyxml2.h
wfLZ.c
wfLZ.h
)

include_directories (.)
include_directories (rapidjson)

add_library(io ${io_src})
