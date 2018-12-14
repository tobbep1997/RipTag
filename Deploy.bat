if not exist "..\Deploy" mkdir ..\Deploy


if not exist "..\Deploy\2DEngine\Fonts" mkdir ..\Deploy\2DEngine\Fonts
xcopy /s /y ..\2DEngine\Fonts ..\Deploy\2DEngine\Fonts


if not exist "..\Deploy\Configuration" mkdir ..\Deploy\Configuration
xcopy /s /y ..\Configuration ..\Deploy\Configuration

if not exist "..\Deploy\Assets" mkdir ..\Deploy\Assets
xcopy /s /y ..\Assets ..\Deploy\Assets
del /s /q /f ..\Deploy\Assets\*.png
del /s /q /f ..\Deploy\Assets\*.jpg


if not exist "..\Deploy\Engine\EngineSource\Shader" mkdir ..\Deploy\Engine\EngineSource\Shader
xcopy /s /y ..\Engine\EngineSource\Shader ..\Deploy\Engine\EngineSource\Shader

if not exist "..\Deploy\Engine\Source\Shader" mkdir ..\Deploy\Engine\Source\Shader
xcopy /s /y ..\Engine\Source\Shader ..\Deploy\Engine\Source\Shader

if not exist "..\Deploy\RipTag" mkdir ..\Deploy\RipTag
xcopy /s /y ..\RipTag\defultEngineSettings.ini ..\Deploy\RipTag

xcopy /s /y ..\External\FMOD\lib\*.dll ..\Deploy\RipTag\

del /s /q /f ..\Deploy\*.h
del /s /q /f ..\Deploy\*.cpp

xcopy /s /y ..\Assets\WindowIcon ..\Deploy\RipTag

start "" ..\Deploy\RipTag\IconChanger.exe ..\Deploy\RipTag\RipTag.exe ..\Deploy\RipTag\fav.ico