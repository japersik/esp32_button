cmake_minimum_required(VERSION 3.16)

set(srcs src/vrt_button.c)
set(include_dir include)

idf_component_register(
        SRCS ${srcs}
        INCLUDE_DIRS ${include_dir}
)
