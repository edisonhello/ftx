file(GLOB SOURCES "*cc" "*cpp")
add_library(chase_and_make STATIC ${SOURCES})
target_include_directories(chase_and_make PUBLIC)
target_link_libraries(chase_and_make)
