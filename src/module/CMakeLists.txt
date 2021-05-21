add_library(chase_and_make STATIC chase_and_make/chase_and_make.cpp)
target_include_directories(chase_and_make PUBLIC)
target_link_libraries(chase_and_make ws rest util ${Boost_LIBRARIES})