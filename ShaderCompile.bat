@echo off
setlocal

cd /d "%~dp0"

:: Set the DXC compiler path
set VS_VERSION=[Your_version]
set DXC_PATH="C:\Program Files (x86)\Windows Kits\10\bin\%VS_VERSION%\x64\dxc.exe"
echo %DXC_PATH%

:: Set the shader directory
set SHADER_DIR="./DX12Project/Shaders"

:: Set the output directory
set OUTPUT_DIR="./DX12Project/Shaders/ByteCodes"

:: If there isn't an output directory, create the directory.
if not exist %OUTPUT_DIR% mkdir %OUTPUT_DIR%

:: All .hlsl files in SHADER_DIR are compiled to a shader byte code.
for %%f in (%SHADER_DIR%\*.hlsl) do (
    echo Compiling for vertex shader: %%f 
    %DXC_PATH% -T vs_6_0 -E VS -Fo "%OUTPUT_DIR%\%%~nf_vs.cso" "%%f"

    echo Compiling for fragment shader: %%f
    %DXC_PATH% -T ps_6_0 -E PS -Fo "%OUTPUT_DIR%\%%~nf_fs.cso" "%%f"
)

echo All shaders have been compiled.
endlocal

pause