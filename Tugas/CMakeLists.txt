cmake_minimum_required(VERSION 3.0)

project( oprec )

find_package( OpenCV REQUIRED )

add_executable( oprec src/main.cpp src/gameball.cpp )

##add_executable( oke1 src/progrobot.cpp )

target_link_libraries( oprec ${OpenCV_LIBS} )
