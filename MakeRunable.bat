if not exist "Runable" mkdir Runable


if not exist "Runable\2DEngine\Fonts" mkdir Runable\2DEngine\Fonts
xcopy /s /y 2DEngine\Fonts Runable\2DEngine\Fonts
del /s /q /f Runable\2DEngine\Fonts\*.h
del /s /q /f Runable\2DEngine\Fonts\*.cpp

if not exist "Runable\Configuration" mkdir Runable\Configuration
xcopy /s /y Configuration Runable\Configuration

if not exist "Runable\Assets" mkdir Runable\Assets
xcopy /s /y Assets Runable\Assets

if not exist "Runable\Engine\EngineSource\Shader" mkdir Runable\Engine\EngineSource\Shader
xcopy /s /y Engine\EngineSource\Shader Runable\Engine\EngineSource\Shader
del /s /q /f Runable\Engine\EngineSource\Shader\*.h
del /s /q /f Runable\Engine\EngineSource\Shader\*.cpp

if not exist "Runable\RipTag" mkdir Runable\RipTag
xcopy /s /y RipTag\defultEngineSettings.ini Runable\RipTag
xcopy /s /y x64\Release\RipTag.exe Runable\RipTag

if not exist "Runable\x64\Release" mkdir Runable\x64\Release
xcopy /s /y x64\Release Runable\x64\Release