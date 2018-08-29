cmake_minimum_required(VERSION 2.8)
project (monoVO_featurematching)
find_package(OpenCV 3.3.1 REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

file(GLOB viso
      "src/*.h"
      "src/*.cpp"
)

add_executable( vom ${viso})
target_link_libraries(vom ${OpenCV_LIBS})
