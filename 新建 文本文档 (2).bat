@echo off
chcp 65001 >nul
echo 批量转为 GBK，给 Keil 专用
echo.

for /r %%f in (*.c *.h) do (
    echo 处理：%%~nxf
    powershell -Command "$c = Get-Content '%%f' -Encoding UTF8 -Raw; Set-Content -Path '%%f' -Value $c -Encoding Default"
)

echo.
echo 转 GBK 完成！现在打开 Keil 看
pause