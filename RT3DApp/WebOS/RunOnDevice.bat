::@echo off
call app_info_setup.bat

::Kill old one if running

%PALMPDK%\bin\plink -P 10022 root@localhost -pw "" "killall -9 gdbserver %APPNAME%"
%PALMPDK%\bin\pscp -scp -P 10022 -pw "" %APPNAME% root@localhost:/media/cryptofs/apps/usr/palm/applications/%BUNDLEIDPREFIX%%APPNAME%
start ..\..\shared\win\utils\playwav.exe ..\..\shared\media\success.wav

::Run it
%PALMPDK%\bin\plink -P 10022 root@localhost -pw "" "/media/cryptofs/apps/usr/palm/applications/%BUNDLEIDPREFIX%%APPNAME%/%APPNAME%"

