cmake_minimum_required(VERSION 3.3)
project(Carrefour)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu11")

add_executable(generateur_trafic generateur_trafic.c generateur_trafic.h voiture.c voiture.h lib.c lib.h)
add_executable(coordinateur coordinateur.c coordinateur.h voiture.c voiture.h lib.c lib.h)