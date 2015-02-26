REM WshShell.SendKeys "tail -f /var/log/messages | grep rtsimpleapp{ENTER}"
Set WshShell = CreateObject("WScript.Shell")
WshShell.Run chr(34) & "%PalmPDK%\bin\DeviceShell.bat" & Chr(34), 0
WScript.Sleep 2500
WshShell.SendKeys "tail -f /var/log/messages{ENTER}"
Set WshShell = Nothing