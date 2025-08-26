@echo off
set SERVER_IP=127.0.0.1
set MAP_NAME=FirstPersonMap
echo Connecting to %SERVER_IP%...
start "" "CaptureTheFlag.exe" %MAP_NAME% -ExecCmds="open %SERVER_IP%" -scalability=Low -ResX=1280 -ResY=720 -Windowed -log 