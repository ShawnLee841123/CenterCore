##-------------------------------------------------------------------------
##
##-------------------------------------------------------------------------
include(${CMAKE_SOURCE_DIR}/CoreInterface.cmake)

if(WIN32)
	add_definitions(-D_WIN_)
endif(WIN32)

set(CLIENT_DEMO_files_sources_files
	ClientTest.cpp
	ClientDemoCore.cpp
	ClientDemoCore.h
)

source_group("ClientDemor" FILES ${CLIENT_DEMO_files_sources_files})

set(ClientDemo_source_files  ${CLIENT_DEMO_files_sources_files})


set(SOURCES_
	${ClientDemo_source_files}
	${SOURCES_Interface}
)

	
SET(ProjectName ClientTest)

##-----------------------------------------------------------------
##	set stack memory size 999999KB	not right
##-----------------------------------------------------------------
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl, --stack, 999999")
#MATH(EXPR stack_size "4*1024*1024")
#set(CMAKE_EXE_LINKER_FLAGS "-Wl,--stack,${stack_size}")

ADD_EXECUTABLE(${ProjectName} ${SOURCES_})
ADD_DEPENDENCIES(${ProjectName} PublicLib InterNetCore)
TARGET_LINK_LIBRARIES(${ProjectName} PublicLib InterNetCore)
SET_TARGET_PROPERTIES(${ProjectName} PROPERTIES FOLDER "ExecuteAble")
