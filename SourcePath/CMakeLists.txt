cmake_minimum_required(VERSION 2.6)


if(WIN32)
	set_property(GLOBAL PROPERTY USE_FOLDERS On)
	set(CMAKE_VS_PLATFORM_TOOLSET "v140" CACHE STRING "Platform Toolset" FORCE)
elseif(WIN32)
	##-------------------------------
	##	网上找的那些什么 set(CMAKE_CXX_FLAGS "-std=c++ xxx")，一律不好使，千万不能加引号
	##-------------------------------
	#add_definitions(-std=c++11)	
	add_definitions(-fvisibility=hidden)
	add_definitions(-D_PORT_VERSION -Wno-deprecated -fPIC)
endif(WIN32)

#	下面这一行一定不能放到if里面，会出事的……
SET(CMAKE_CXX_FLAGS -std=c++11)

if(CMAKE_CONFIGURATION_TYPES STREQUAL "RelWithDebInfo")
	add_definitions(/OD)
endif(CMAKE_CONFIGURATION_TYPES STREQUAL "RelWithDebInfo")



PROJECT(CenterCore)
ADD_SUBDIRECTORY(PublicLib)
ADD_SUBDIRECTORY(InterNetCore)
ADD_SUBDIRECTORY(ServerHolder)

#	Add Client Windows Test Demo
if(WIN32)
	ADD_SUBDIRECTORY(ClientTest)
endif(WIN32)
