#!/bin/sh

# fixdylibs_mac.sh
#
# Fix dylib paths inside final executable
#
# Created by Mark H on 7/18/14.

install_name_tool -change @executable_path/../Frameworks/SDL2.framework/Versions/A/SDL2 @executable_path/SDL2 ./Pony48_mac
install_name_tool -change ./libfmodexL.dylib @executable_path/libfmodexL.dylib ./Pony48_mac
