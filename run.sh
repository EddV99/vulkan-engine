#!/bin/bash

# Assumes this script is in root directory

build_shaders () {
    # go to shaders directory
    cd src/shaders

    glslc blinn.vert -o blinn-vertex.spv
    glslc blinn.frag -o blinn-fragment.spv

    # go back to root directory 
    cd ../..
}

build (){
    echo "building!"

    # go to build directory
    cd build
    cmake ..
    make -j4
    cd ..

    build_shaders
}

clean (){
    cd build
    echo "cleaning!"
    find . ! -name 'CMakeLists.txt' -type d -exec rm -rf {} + 2> /dev/null
    find . ! -name 'CMakeLists.txt' -type f -exec rm -f {} + 2> /dev/null
    cd ..

    rm src/shaders/*.spv
}

run (){
    echo "running!"
    ./build/vulkan-engine
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
