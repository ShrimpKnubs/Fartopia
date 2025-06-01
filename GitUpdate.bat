@echo off
echo ==========================================
echo GitHub Repository Clean Update Script
echo ==========================================
echo.
echo This will replace ALL files in your GitHub repo
echo with the current contents of this folder.
echo.
set /p confirm="Are you sure you want to continue? (y/n): "
if /i not "%confirm%"=="y" (
    echo Operation cancelled.
    pause
    exit /b
)

echo.
echo Starting update process...
echo.

REM Navigate to the project directory (adjust path if needed)
cd /d "C:\Users\dusti\Desktop\Emergent Kingdoms"

REM Check if we're in a git repository
if not exist ".git" (
    echo Error: Not in a git repository!
    echo Make sure you're running this from your project folder.
    pause
    exit /b 1
)

echo Step 1: Removing all tracked files from git...
git rm -r --cached .

echo Step 2: Adding all current files...
git add .

echo Step 3: Creating commit...
set /p commit_msg="Enter commit message (or press Enter for default): "
if "%commit_msg%"=="" set commit_msg=Complete repository update - %date% %time%

git commit -m "%commit_msg%"

echo Step 4: Pushing to GitHub...
git push origin master --force

echo.
echo ==========================================
echo Update completed successfully!
echo ==========================================
echo.
echo Your GitHub repository now matches your local folder exactly.
pause