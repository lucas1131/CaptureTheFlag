@echo off
echo Starting listen server...
set MAP_NAME=FirstPersonMap
start "" "CaptureTheFlag.exe" %MAP_NAME%?listen -scalability=Low -ResX=1280 -ResY=720 -Windowed -FullCrashDump -log