cmake_minimum_required (VERSION 2.6)
project(fusion-kbd-ctl)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_LIST_DIR})

find_package(USB1 REQUIRED)

add_executable(${CMAKE_PROJECT_NAME} 
	${CMAKE_CURRENT_LIST_DIR}/main.c)

target_link_libraries(
	${CMAKE_PROJECT_NAME}
	${LIBUSB_LIBRARIES})

install(FILES ${CMAKE_PROJECT_NAME}
	DESTINATION bin)
