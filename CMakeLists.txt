cmake_minimum_required( VERSION 2.8 )
project( task)

# link headers
FIND_PACKAGE(OpenCV REQUIRED)
# show the message of OpenCV
message(STATUS "OpenCV library status:")
message(STATUS "    version:     ${OpenCV_VERSION}")
message(STATUS "    headers:     ${OpenCV_INCLUDE_DIRS}")
message(STATUS "    libraries:     ${OpenCV_LIBS}")
 
INCLUDE_DIRECTORIES(./include)

add_executable(color_block_opencv   src/main.cpp 
                                    src/color.cpp
                                    src/color_opencv.cpp
                                    )
target_link_libraries(color_block_opencv  ${OpenCV_LIBS} )
