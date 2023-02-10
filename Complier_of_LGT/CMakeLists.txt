cmake_minimum_required(VERSION 3.20)
project(Complier_of_LGT)

set(CMAKE_CXX_STANDARD 14)

add_executable(Complier_of_LGT main.cpp Syn_Function.cpp Syn_Head.h Lexer_Analyze.cpp Phraser_Analyse.cpp Syn_Analyse.cpp Lexer_Function.cpp Lexer_Head.h Phraser_Function.cpp Phraser_Head.h)