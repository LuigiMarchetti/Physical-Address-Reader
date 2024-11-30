cmake_minimum_required(VERSION 3.10)
project(untitled1)

# Definir o padrão do compilador C
set(CMAKE_C_STANDARD 99)

# Adicionar o arquivo de origem ao seu projeto
add_executable(untitled1 main.c)

# Adicionar a biblioteca comctl32 para vinculação
target_link_libraries(untitled1 PRIVATE comctl32)
