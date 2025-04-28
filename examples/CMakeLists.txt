project(examples)
include_directories(../src/include)

add_executable(clear_screen clear_screen/main.c clear_screen/scene.c)
target_link_libraries(clear_screen gix-engine)
