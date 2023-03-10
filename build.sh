#!/bin/bash

usage="usage: $0 Debug|Release [threads]"

if [[ $# != 1 && $# != 2 ]]
then
    echo $usage
    exit
fi

build_type="Release"
if [[ $1 == 'Debug' || $1 == 'Release' ]]
then
    build_type=$1
else
    echo $usage
    exit
fi

mkdir -p $1
cd $1

cmake -DCMAKE_BUILD_TYPE=$build_type ..

num_threads='-j1'
if [[ $# == 2 ]]
then
    num_threads=$2
fi

make $num_threads

