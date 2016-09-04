set(net_src
minihttp.cpp
minihttp.h
MutexLock.cpp
MutexLock.h
NetworkThread.cpp
NetworkThread.h
SteelSeriesClient.h
SteelSeriesClient.cpp
SteelSeriesEvents.h
ThreadsafeQueue.h
)

include_directories (.)
include_directories (rapidjson)

add_library(net ${net_src})
#target_link_libraries(engine imgui lua53 io ${BOX2D_LIBRARY} ${SDL2_LIBRARY})
