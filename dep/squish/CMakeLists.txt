set(squish_src
alpha.cpp
clusterfit.h
colourblock.h
colourset.cpp
maths.cpp
rangefit.h
simd_float.h
singlecolourfit.cpp
squish.cpp
alpha.h
colourfit.cpp
colourset.h
maths.h
simd_sse.h
singlecolourfit.h
squish.h
clusterfit.cpp
colourblock.cpp
colourfit.h
config.h
rangefit.cpp
simd.h
simd_ve.h
)

add_library(squish ${squish_src})
