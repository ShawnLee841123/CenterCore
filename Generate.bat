@echo on
::@set ClientLogic=LogicModule
@set CenterCore=SourcePath

::@set VSCLDir=VS_Client
@set VSCCDir=Build

@set ProRoot=G:/Works/CPlus/CenterCore

::md %VSCLDir%
::cd %VSCLDir%
::cmake -G"Visual Studio 15 2017 Win64" %ProRoot%/%ClientLogic% -DBINARY_DIR=%ProRoot%/%VSCLDir% -DCMAKE_CONFIGURATION_TYPES=RelWithDebInfo
::cmake -G"Visual Studio 15 2017 Win64" %ClientLogic% -DBINARY_DIR=%VSCLDir% -DCMAKE_CONFIGURATION_TYPES=RelWithDebInfo

@echo ---------------------generate client project completed---------------------

md %VSCCDir%
cd %VSCCDir%
cmake -G"Visual Studio 15 2017 Win64" %ProRoot%/%CenterCore% -DBINARY_DIR=%ProRoot%/%VSCCDir% -DCMAKE_CONFIGURATION_TYPES=Debug

@echo ---------------------generate server project completed---------------------
cd ..
pause