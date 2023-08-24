#!/bin/bash

LibDir=$PWD"/lib"
Target=vesc_tool_6.02

lib_array=($(ldd $Target | grep -o "/.*" | grep -o "/.*/[^[:space:]]*"))

$(mkdir $LibDir)

for Variable in ${lib_array[@]}
do
        cp "$Variable" $LibDir
done

