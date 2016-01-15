cmake_minimum_required(VERSION 3.3)
project(Carrefour)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu11")

add_executable(generateur_trafic generateur_trafic.c headers/generateur_trafic.h voiture.c headers/voiture.h lib.c headers/lib.h)
add_executable(generateur_trafic_prioritaire generateur_trafic_prioritaire.c headers/generateur_trafic.h voiture.c headers/voiture.h lib.c headers/lib.h)
add_executable(coordinateur coordinateur.c headers/coordinateur.h voiture.c headers/voiture.h lib.c headers/lib.h)
add_executable(feux feux.c headers/feux.h voiture.c headers/voiture.h lib.c headers/lib.h)