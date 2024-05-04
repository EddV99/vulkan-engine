#!/bin/bash

# do everything in the build directory
cd build

build (){
    echo "building!"
    cmake .
    make 
}

clean (){
    echo "cleaning!"
    rm -rf CMakeFiles CMakeCache.txt Makefile cmake_install.cmake vulkan-engine
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
