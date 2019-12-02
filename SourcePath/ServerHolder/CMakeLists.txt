##-------------------------------------------------------------------------
##
##	Name		Server Holder
##	Functional	Server holder sulotion control
##	Creater		Shawn Lee
##	Create time	2019-12-1
##
##
##-------------------------------------------------------------------------
include(${CMAKE_SOURCE_DIR}/CoreInterface.cmake)

if(WIN32)
	add_definitions(-D_WIN_)
endif(WIN32)

set(SERVER_HOLDER_files_sources_files
	ServerHolder.cpp
)

source_group("ServerHolder" FILES ${SERVER_HOLDER_files_sources_files})

set(ServerHolder_source_files  ${SERVER_HOLDER_files_sources_files})


set(SOURCES_
	${ServerHolder_source_files}
	${SOURCES_Interface}
)

	
SET(ProjectName ServerHolder)
ADD_EXECUTABLE(${ProjectName} ${SOURCES_})
ADD_DEPENDENCIES(${ProjectName} PublicLib InterNetCore)
TARGET_LINK_LIBRARIES(${ProjectName} PublicLib InterNetCore)
SET_TARGET_PROPERTIES(${ProjectName} PROPERTIES FOLDER "ExecuteAble")
