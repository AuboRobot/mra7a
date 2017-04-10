# SET(CMAKE_INSTALL_PREFIX ${PROJECT_BINARY_DIR})

# find libpcan
# FIND_PACKAGE(pcan REQUIRED)

# find MRA_API
FIND_PACKAGE(MRA_API REQUIRED)
 

INCLUDE_DIRECTORIES(${libMRA_API_INCLUDE_DIRS})
ADD_EXECUTABLE(joint_control main.cpp)
TARGET_LINK_LIBRARIES(joint_control ${catkin_LIBRARIES} ${libMRA_API_LIBS})
add_dependencies(joint_control ${catkin_EXPORTED_TARGETS}) # don't build until necessary msgs are finished


