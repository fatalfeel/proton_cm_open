call app_info_setup.bat
set IPK_NAME=%BUNDLEIDPREFIX%%APPNAME%_1.0.0_all.ipk
REM del %APPNAME%
del %IPK_NAME%
if not exist %APPNAME% %RT_UTILS%beeper.exe /p
call packageit.bat

if not exist %IPK_NAME% beeper.exe /p
call palm-install.bat -r %BUNDLEIDPREFIX%%APPNAME%
call palm-install.bat %IPK_NAME%
call RunOnDevice.bat
