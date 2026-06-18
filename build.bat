@echo off
chcp 65001 > nul
echo ==================================================
echo   C++ 智慧點餐收銀系統 - 單擊編譯工具
echo ==================================================
echo.

:: Check if g++ is installed
where g++ >nul 2>nul
if %errorlevel% neq 0 (
    echo [錯誤] 找不到 g++ 編譯器。請確認是否已安裝 MinGW 且已加入系統 Path 變數。
    pause
    exit /b 1
)

echo [*] 正在編譯 C++ 原始碼...
g++ -std=c++17 -O2 src/main.cpp src/Menu.cpp src/Order.cpp src/UI.cpp -Isrc -o FoodOrderingSystem.exe

if %errorlevel% neq 0 (
    echo.
    echo [失敗] 編譯過程中出錯！
    pause
    exit /b 1
)

echo [成功] 編譯完成！產生 FoodOrderingSystem.exe。
echo.
echo [*] 正在啟動系統...
echo ==================================================
echo.
FoodOrderingSystem.exe
pause
