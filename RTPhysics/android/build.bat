call app_info_setup.bat
:Get the emulator ready if it isn't, because it takes a freakin' long time to load
:start emulator %EMULATOR_AVD%

:build the C/C++ parts
call ndk-build -j7
if not exist libs/armeabi/lib%SMALL_PACKAGE_NAME%.so beeper.exe /p

:Copy refresh resources, assuming the windows version had them built with update_media recently...
rmdir assets /S /Q

mkdir assets

mkdir assets\interface
xcopy ..\bin\interface assets\interface /E /F /Y

mkdir assets\game
xcopy ..\bin\game assets\game /E /F /Y

mkdir assets\audio
xcopy ..\bin\audio assets\audio /E /F /Y

:Kill old files handing around to avoid confusion
rmdir bin /S /Q
rmdir gen /S /Q
:Next package it with the java part
call ant debug

:and finally, load it into the emulator

REM Waiting for device to get ready...
:adb wait-for-device -s emulator-5554
REM Installing...
:adb -s emulator-5554 install -r bin\%APP_NAME%-debug.apk
call InstallOnDefaultPhone.bat
:adb logcat
pause