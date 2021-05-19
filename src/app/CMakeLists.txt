
add_executable(chase_and_make_app chase_and_make_app.cpp)
target_link_libraries(chase_and_make_app ssl crypto ws rest util chase_and_make ${Boost_LIBRARIES})
