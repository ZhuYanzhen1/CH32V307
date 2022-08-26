@echo off
taskkill /f /im putty.exe 2>nul
start /b %~dp0/putty.exe -serial %1 -sercfg %2,8,n,1,N