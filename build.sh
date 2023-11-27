#!/bin/bash

build_dir=build

# 检查是否存在 clean 参数
if [[ $# -eq 1 && "$1" == "clean" ]]; then
    # 如果存在 clean 参数，删除 build 文件夹
    if [ -d "$build_dir" ]; then
        rm -r "$build_dir"
    fi
fi

if [ -d "$build_dir" ]; then
    echo "当前文件夹存在"
else
    mkdir $build_dir
fi

cd $build_dir
cmake ..
make