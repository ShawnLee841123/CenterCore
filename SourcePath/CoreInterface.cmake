## --------------------------------------------------------------------
## 文件名:    CoreInterface.cmake
## 内  容:    CoreInterface 文件
## 说  明:    CoreInterface 管理文件
## 创建日期:  2019年12月1日	
## 创建人:    Shawn Lee
## --------------------------------------------------------------------

set(SOURCES_files_Interface
	${CMAKE_SOURCE_DIR}/CoreInterface/ICompletionPort.h
	${CMAKE_SOURCE_DIR}/CoreInterface/ILogCore.h
	${CMAKE_SOURCE_DIR}/CoreInterface/ICenterCore.h
)

source_group("Interface Files" FILES ${SOURCES_files_Interface})

set(SOURCES_Interface
	${SOURCES_files_Interface}
)

if(WIN32)
	set_property(GLOBAL PROPERTY USE_FOLDERS On)
	set(CMAKE_VS_PLATFORM_TOOLSET "v140" CACHE STRING "Platform Toolset" FORCE)
elseif(WIN32)
	add_definitions(-fvisibility=hidden)
	add_definitions(-D_PORT_VERSION -Wno-deprecated -fPIC)
endif(WIN32)

SET(CMAKE_CXX_FLAGS -std=c++11)

if(CMAKE_CONFIGURATION_TYPES STREQUAL "RelWithDebInfo")
	add_definitions(/OD)
endif(CMAKE_CONFIGURATION_TYPES STREQUAL "RelWithDebInfo")
	
