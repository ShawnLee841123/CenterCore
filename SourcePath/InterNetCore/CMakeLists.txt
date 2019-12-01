##-----------------------------------------------------------------
##
##
##
##
##
##
##
##
##-----------------------------------------------------------------
include(${CMAKE_SOURCE_DIR}/common.cmake)
include(${CMAKE_SOURCE_DIR}/CoreInterface.cmake)
##-----------------------------------------------------------------
##	Port Completion	(windows)
##-----------------------------------------------------------------

set(PORT_COMPLETION_files_SourceFiles
	PortCompletion/PortCompleteBaseDefine.cpp
	PortCompletion/PortCompleteBaseDefine.h
	PortCompletion/PortCompleteCore.cpp
	PortCompletion/PortCompleteCore.h
	PortCompletion/PortCompleteQueueElement.cpp
	PortCompletion/PortCompleteQueueElement.h
	PortCompletion/PortCompleteWorker.cpp
	PortCompletion/PortCompleteWorker.h
	PortCompletion/PortCompleteQueueElementDataDefine.h
)

source_group("PortCompletion" FILES ${PORT_COMPLETION_files_SourceFiles})

set(PortCompletion_files ${PORT_COMPLETION_files_SourceFiles})

##-----------------------------------------------------------------
##	Center Core Win (windows)
##-----------------------------------------------------------------
set(CENTER_CORE_WIN_files_SourceFiles
	CenterCoreWinBasic/CenterCoreBasic.cpp
	CenterCoreWinBasic/CenterCoreBasic.h
	CenterCoreWinBasic/CenterLog.cpp
	CenterCoreWinBasic/CenterLog.h
)

source_group("CenterCoreWin" FILES ${CENTER_CORE_WIN_files_SourceFiles})

set(CenterCoreWin_files ${CENTER_CORE_WIN_files_SourceFiles})

##-----------------------------------------------------------------






SET(ProjectName InterNetCore)
if(WIN32)
	add_definitions(-D_WIN_)
	set(SOURCES_
		${PortCompletion_files}
		${CenterCoreWin_files}
		${SOURCES_Interface}
	)
##else(WIN32)
endif(WIN32)


ADD_LIBRARY(${ProjectName} STATIC ${SOURCES_})
ADD_DEPENDENCIES(${ProjectName} PublicLib)

if(WIN32)
	TARGET_LINK_LIBRARIES(${ProjectName} PublicLib "ws2_32.lib")
else(WIN32)
	TARGET_LINK_LIBRARIES(${ProjectName} PublicLib)
endif(WIN32)

SET_TARGET_PROPERTIES(${ProjectName} PROPERTIES FOLDER "Lib")
