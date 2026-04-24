@echo off
chcp 65001 >nul
echo 【STM32专用】批量修复乱码 → 统一 UTF-8 无BOM
echo.

for /r %%f in (*.c *.h *.cpp *.hpp *.ino) do (
    echo 修复: %%~nxf
    powershell -Command "$s = Get-Content '%%f' -Encoding OEM -Raw; Set-Content '%%f' -Value $s -Encoding UTF8"
)

echo.
echo ==============================
echo  修复完成！
echo  现在 Keil、VS Code、GitHub 全都正常
echo ==============================
pause