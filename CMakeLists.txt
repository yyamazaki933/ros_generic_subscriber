cmake_minimum_required(VERSION 2.8.3)
project(generic_subscriber)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  std_msgs
  topic_tools
)

catkin_package(
  INCLUDE_DIRS include
  LIBRARIES ${PROJECT_NAME}
  CATKIN_DEPENDS roscpp std_msgs topic_tools
  DEPENDS
)

include_directories(
  include
  ${catkin_INCLUDE_DIRS}
)

add_library(${PROJECT_NAME} src/generic_subscriber.cpp)
target_link_libraries(${PROJECT_NAME} ${catkin_LIBRARIES})
