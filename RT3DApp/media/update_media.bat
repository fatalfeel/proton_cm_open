cls
set ZIP_EXE=..\..\shared\win\utils\7za.exe

rmdir ..\bin\game /S /Q
rmdir ..\bin\interface /S /Q
rmdir ..\bin\audio /S /Q
rmdir ..\bin\shaders /S /Q

cd game
for /r %%i in (*.x *.b3d *.bsp *.obj) do ..\%ZIP_EXE% a %%~pni.zip %%i
cd ..

cd interface
for /r %%i in (*.x *.b3d *.bsp *.obj) do ..\%ZIP_EXE% a %%~pni.zip %%i
cd ..

mkdir ..\bin\game
xcopy game ..\bin\game /E /F /Y /EXCLUDE:xcopy_exclude.txt

mkdir ..\bin\interface
xcopy interface ..\bin\interface /E /F /Y /EXCLUDE:xcopy_exclude.txt

mkdir ..\bin\audio
xcopy audio ..\bin\audio /E /F /Y

mkdir ..\bin\shaders
xcopy shaders ..\bin\shaders /E /F /Y

mkdir ..\android\assets\game
xcopy game ..\android\assets\game /E /F /Y /EXCLUDE:xcopy_exclude.txt
xcopy game\quake ..\android\assets\game\quake /E /F /Y
xcopy game\house_scene ..\android\assets\game\house_scene /E /F /Y

mkdir ..\android\assets\interface
xcopy interface ..\android\assets\interface /E /F /Y /EXCLUDE:xcopy_exclude.txt

mkdir ..\android\assets\audio
xcopy audio ..\android\assets\audio /E /F /Y

mkdir ..\android\assets\shaders
xcopy shaders ..\android\assets\shaders /E /F /Y

cd game
for /r %%i in (*.zip) do del %%i
cd ..

cd interface
for /r %%i in (*.zip) do del %%i
cd ..

cd ..\bin\game\quake
for /r %%i in (*.zip) do del %%i
cd..

cd house_scene
for /r %%i in (*.zip) do del %%i
cd ..\..\..\media

cd ..\android\assets\game\quake
for /r %%i in (*.zip) do del %%i

cd ..\house_scene
for /r %%i in (*.zip) do del %%i
cd ..\..\..\..\media

pause
