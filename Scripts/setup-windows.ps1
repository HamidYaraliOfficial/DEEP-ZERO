param([string]$UnrealEngineRoot="C:\Program Files\Epic Games\UE_5.8",[string]$ProjectPath="$PSScriptRoot\..\DEEP_ZERO.uproject")
$ErrorActionPreference="Stop"
if(!(Test-Path $ProjectPath)){throw "Project not found: $ProjectPath"}
$vswhere="${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
if(!(Test-Path $vswhere)){throw "vswhere.exe not found. Install Visual Studio."}
$vs=& $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
if(!$vs){throw "MSVC C++ toolchain not found."}
$build=Join-Path $UnrealEngineRoot "Engine\Build\BatchFiles\Build.bat"
if(!(Test-Path $build)){throw "Unreal Build.bat not found: $build"}
& $build DeepZeroEditor Win64 Development (Resolve-Path $ProjectPath) -Progress
if($LASTEXITCODE -ne 0){throw "Editor build failed."}
