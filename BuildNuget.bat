@echo off
REM Jenkins entry point - mirrors Core\BuildNuget.bat. All logic lives in BuildNuget.ps1.
PowerShell -NonInteractive -NoProfile -ExecutionPolicy Bypass -File "%~dp0BuildNuget.ps1" %*
exit /b %ERRORLEVEL%
