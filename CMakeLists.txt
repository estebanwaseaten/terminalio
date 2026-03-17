cmake_minimum_required(VERSION 3.0.0)

project(terminalio)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SOURCE_LIST
    "src/terminal.cpp"
    "src/input.cpp"
    "src/console.cpp"
    "src/buffer.cpp"
)

set(LIBRARIES_LIST
	${PROJECT_SOURCE_DIR}/../threader/       #THIS projects source dir... means we can only build this when threader is installed BESIDE/NEXT TO this library...
)

foreach(LIBRARY ${LIBRARIES_LIST} )            # all libs THIS project depends on must be built first
	add_subdirectory(${LIBRARY} ${LIBRARY}/build)      # first ${LIBRARY} for source dir, second for binary dir (i.e. the dir where all the output files are put, but not the library binary)
endforeach()

add_library(${PROJECT_NAME} SHARED ${SOURCE_LIST} )       #creates THIS target (library terminalio)

target_include_directories(${PROJECT_NAME}                # tell the target where the include dirs are
    PUBLIC
        include
        ${PROJECT_SOURCE_DIR}/../threader/include         # threader is installed BESIDE/NEXT TO this library...
    PRIVATE
        src

)

target_link_libraries(${PROJECT_NAME} PUBLIC threader)    #terminalio depends on threader
set_target_properties(${PROJECT_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})
