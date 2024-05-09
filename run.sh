#!/bin/bash

# Assumes this script is in root directory

# do everything in the build directory
cd build

build_shaders () {
    # go to shaders directory
    cd ../src/shaders

    glslc basic.vert -o vert.spv
    glslc basic.frag -o frag.spv

    # go back to build directory
    cd ../../build
}

build (){
    echo "building!"
    cmake .
    make 

    build_shaders
}

clean (){
    echo "cleaning!"
    find . ! -name 'CMakeLists.txt' -type d -exec rm -rf {} + 2> /dev/null
    find . ! -name 'CMakeLists.txt' -type f -exec rm -f {} + 2> /dev/null
}

run (){
    echo "running!"
    ./vulkan-engine
}



if [[ $1 == "run" ]]; then
    run
elif [[ $1 == "build" ]]; then
    build
elif [[ $1 == "clean" ]]; then
    clean
else
    build
    run
fi
