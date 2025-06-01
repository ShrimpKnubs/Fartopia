@echo off
setlocal enabledelayedexpansion

echo ==========================================
echo       Lines of Code Counter
echo ==========================================
echo.
echo Analyzing project: %CD%
echo.

REM Initialize counters
set /a total_lines=0
set /a total_files=0
set /a cpp_lines=0
set /a h_lines=0
set /a other_lines=0

echo Counting C++ source files (.cpp)...
for /r %%i in (*.cpp) do (
    if exist "%%i" (
        for /f "delims=" %%j in ('type "%%i" ^| find /c /v ""') do (
            set /a file_lines=%%j
            set /a cpp_lines+=!file_lines!
            set /a total_lines+=!file_lines!
            set /a total_files+=1
            echo   %%~nxi: !file_lines! lines
        )
    )
)

echo.
echo Counting C++ header files (.h)...
for /r %%i in (*.h) do (
    if exist "%%i" (
        for /f "delims=" %%j in ('type "%%i" ^| find /c /v ""') do (
            set /a file_lines=%%j
            set /a h_lines+=!file_lines!
            set /a total_lines+=!file_lines!
            set /a total_files+=1
            echo   %%~nxi: !file_lines! lines
        )
    )
)

echo.
echo Counting other code files (.c, .hpp, .txt, .md, Makefile)...
for /r %%i in (*.c *.hpp *.txt *.md Makefile) do (
    if exist "%%i" (
        for /f "delims=" %%j in ('type "%%i" ^| find /c /v ""') do (
            set /a file_lines=%%j
            set /a other_lines+=!file_lines!
            set /a total_lines+=!file_lines!
            set /a total_files+=1
            echo   %%~nxi: !file_lines! lines
        )
    )
)

echo.
echo ==========================================
echo                SUMMARY
echo ==========================================
echo C++ Source Files (.cpp): !cpp_lines! lines
echo Header Files (.h):       !h_lines! lines
echo Other Files:             !other_lines! lines
echo ==========================================
echo TOTAL FILES:             !total_files!
echo TOTAL LINES OF CODE:     !total_lines!
echo ==========================================
echo.

REM Calculate some fun stats
if !total_files! gtr 0 (
    set /a avg_lines=!total_lines!/!total_files!
    echo Average lines per file: !avg_lines!
)

REM Estimate based on typical programming metrics
if !total_lines! gtr 0 (
    set /a estimated_hours=!total_lines!/50
    echo Estimated development time: ~!estimated_hours! hours
    echo (Based on 50 lines per hour average)
) else (
    echo No code lines found - check file permissions or paths
)

echo.
pause