call app_info_setup.bat

set STAGING_DIR=STAGING\%BUNDLEIDPREFIX%%APPNAME%
set STRIP=y

rmdir /S /Q STAGING
del *.ipk
mkdir %STAGING_DIR%
REM call buildit.cmd
copy appinfo.json %STAGING_DIR%
copy icon.png %STAGING_DIR%
REM copy icon-256x256.png %STAGING_DIR%

copy %APPNAME% %STAGING_DIR%
if %STRIP%==y arm-none-linux-gnueabi-strip %STAGING_DIR%\%APPNAME%
REM copy ..\res\*.* %STAGING_DIR%

mkdir %STAGING_DIR%\interface
xcopy ..\bin\interface %STAGING_DIR%\interface /E /F /Y

mkdir %STAGING_DIR%\audio
xcopy ..\bin\audio %STAGING_DIR%\audio /E /F /Y

del %STAGING_DIR%\audio\*.mp3

mkdir %STAGING_DIR%\game
xcopy ..\bin\game %STAGING_DIR%\game /E /F /Y

echo filemode.755=%APPNAME% > %STAGING_DIR%\package.properties
call palm-package --use-v1-format %STAGING_DIR%
