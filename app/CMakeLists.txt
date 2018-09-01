cmake_minimum_required(VERSION 3.10)

file(GLOB_RECURSE SRC_FILES src/*)
add_executable(
	app
    ${SRC_FILES}
)

target_link_libraries(
    app
    PRIVATE
        libgame
        libview
)
