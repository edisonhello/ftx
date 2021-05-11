file(GLOB SOURCES "*cc" "*.cpp")
add_library(util STATIC ${SOURCES})
target_include_directories(util PUBLIC)
target_link_libraries(util ssl crypto)
