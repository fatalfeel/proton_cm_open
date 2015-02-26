call app_info_setup.bat
set IPK_NAME=%BUNDLEIDPREFIX%%APPNAME%_1.0.0_all.ipk
del %APPNAME%
del %IPK_NAME%
call build.bat
if not exist %APPNAME% %RT_UTILS%beeper.exe /p
call InstallAndRun.bat
