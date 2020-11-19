cmake_minimum_required( VERSION 3.7 )

set( CMAKE_EXPORT_COMPILE_COMMANDS ON )
set( CMAKE_CXX_STANDARD 20 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )

project( Lexer )

option( LexerTest ON )

add_library( ${PROJECT_NAME} STATIC "src/Lexer.cpp" )

target_include_directories( ${PROJECT_NAME} PUBLIC src )

if( LexerTest )
	add_executable( ${PROJECT_NAME}_Test "src/main.cpp" )
	target_include_directories( ${PROJECT_NAME}_Test PRIVATE "src" )
	target_link_libraries( ${PROJECT_NAME}_Test PRIVATE ${PROJECT_NAME} )
endif( LexerTest )
