cmake_minimum_required(VERSION 3.10)
project(AllTPs C)

set(CMAKE_C_STANDARD 99)

file(GLOB TP_DIRS RELATIVE ${CMAKE_SOURCE_DIR} TP*)

message(STATUS "Found TP dirs: ${TP_DIRS}")

foreach(tp_dir ${TP_DIRS})
    message(STATUS "Processing directory: ${tp_dir}")

    file(GLOB_RECURSE TP_SOURCES ${CMAKE_SOURCE_DIR}/${tp_dir}/*.c)
    message(STATUS "Found source files in ${tp_dir}: ${TP_SOURCES}")

    foreach(src_file ${TP_SOURCES})

        # Skip known manual object sources
        if(${src_file} STREQUAL "${CMAKE_SOURCE_DIR}/TP1/TP1/logical_MAKKAWI.c" OR
           ${src_file} STREQUAL "${CMAKE_SOURCE_DIR}/TP3/TP3/packing_MAKKAWI.c")
            message(STATUS "Skipping manually handled file: ${src_file}")
            continue()
        endif()

        get_filename_component(exec_name ${src_file} NAME_WE)

        if(exec_name STREQUAL "TP1")
            add_executable(${exec_name} ${src_file} ${CMAKE_SOURCE_DIR}/TP1/TP1/logical_Wang.o)
        elseif(exec_name STREQUAL "TP3")
            add_executable(${exec_name} ${src_file} ${CMAKE_SOURCE_DIR}/TP3/TP3/packing_Wang.o)
        else()
            add_executable(${exec_name} ${src_file})
        endif()

        target_link_libraries(${exec_name} m)
        set_target_properties(${exec_name} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
        )

        message(STATUS "Added target: ${exec_name}")
    endforeach()
endforeach()


# Manual target for packer (if needed)
set(MAKKAWI_MAIN_SRC ${CMAKE_SOURCE_DIR}/TP3/TP3/TP3.c)
set(MAKKAWI_OBJ
    ${CMAKE_SOURCE_DIR}/TP3/TP3/packing_Wang.o
    ${CMAKE_SOURCE_DIR}/TP1/TP1/logical_Wang.o
)

add_executable(packer ${MAKKAWI_MAIN_SRC})
target_link_libraries(packer ${MAKKAWI_OBJ} m)
set_target_properties(packer PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

file(COPY ${CMAKE_SOURCE_DIR}/TP3/TP3/function_data.bin
     DESTINATION ${CMAKE_BINARY_DIR}/bin)
