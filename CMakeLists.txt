set(CMAKE_BUILD_TYPE DEBUG)

project(correctness)
add_executable(correctness correctness.cc test.h kvstore.h skiplist.h skiplist.cpp kvstore_api.h kvstore.cc)

project(persistence)
add_executable(persistence persistence.cc test.h kvstore.h skiplist.h skiplist.cpp kvstore_api.h kvstore.cc)

project(read_sst)
add_executable(read_sst read_sst.cpp)