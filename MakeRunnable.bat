if not exist "Runnable" mkdir Runnable


if not exist "Runnable\2DEngine\Fonts" mkdir Runnable\2DEngine\Fonts
xcopy /s /y 2DEngine\Fonts Runnable\2DEngine\Fonts
del /s /q /f Runnable\2DEngine\Fonts\*.h
del /s /q /f Runnable\2DEngine\Fonts\*.cpp

if not exist "Runnable\Configuration" mkdir Runnable\Configuration
xcopy /s /y Configuration Runnable\Configuration

if not exist "Runnable\Assets" mkdir Runnable\Assets
xcopy /s /y Assets Runnable\Assets

if not exist "Runnable\Engine\EngineSource\Shader" mkdir Runnable\Engine\EngineSource\Shader
xcopy /s /y Engine\EngineSource\Shader Runnable\Engine\EngineSource\Shader
del /s /q /f Runnable\Engine\EngineSource\Shader\*.h
del /s /q /f Runnable\Engine\EngineSource\Shader\*.cpp

if not exist "Runnable\RipTag" mkdir Runnable\RipTag
xcopy /s /y RipTag\defultEngineSettings.ini Runnable\RipTag
xcopy /s /y x64\Release\RipTag.exe Runnable\RipTag

if not exist "Runnable\x64\Release" mkdir Runnable\x64\Release
xcopy /s /y x64\Release Runnable\x64\Release