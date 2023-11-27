cmake_minimum_required(VERSION 2.8)

# 项目名称
project(MultiDownload)

# 添加编译参数
add_compile_options(-std=c++11 -g -O0)

# 设置产物输出位置
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)

# 导入工程文件
file (GLOB_RECURSE SRC_FILE ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
                            ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h)

# 添加头文件查找地址
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src 
                    ${CMAKE_CURRENT_SOURCE_DIR}/src/file 
                    ${CMAKE_CURRENT_SOURCE_DIR}/src/ftp 
                    ${CMAKE_CURRENT_SOURCE_DIR}/src/http)

# 生成可执行程序
add_executable(${PROJECT_NAME} ${SRC_FILE} main.cpp)

# 连接三方库
target_link_libraries(${PROJECT_NAME} PRIVATE curl pthread)