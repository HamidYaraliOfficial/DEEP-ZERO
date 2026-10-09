# DEEP ZERO — INSTALLATION

## Windows prerequisites
- Windows 10 22H2 or Windows 11 64-bit.
- Epic Games Launcher + Unreal Engine 5.8.x.
- Visual Studio 2022 17.14+ for UE 5.8, or the current supported Visual Studio toolchain.
- Game development with C++.
- Desktop development with C++.
- Visual Studio Tools for Unreal Engine.
- Unreal Engine debugger tools and Unreal Engine Test Adapter.
- Windows SDK.
- Git for Windows.

## Install commands

```powershell
winget install --id Git.Git -e
winget install --id Microsoft.VisualStudio.2022.Community -e
```

Then open Visual Studio Installer and add the Unreal/C++ workloads/components.

## Project setup

```powershell
Set-ExecutionPolicy -Scope Process Bypass
.\Scripts\setup-windows.ps1 -UnrealEngineRoot "C:\Program Files\Epic Games\UE_5.8"
```

## Generate project files
Open `DEEP_ZERO.uproject` in Unreal Editor 5.8.x and use the editor/Explorer option to generate Visual Studio project files.

## Development build

```powershell
.\Scripts\build-development.ps1 -UnrealEngineRoot "C:\Program Files\Epic Games\UE_5.8"
```

## Run editor

```powershell
& "C:\Program Files\Epic Games\UE_5.8\Engine\Binaries\Win64\UnrealEditor.exe" ".\DEEP_ZERO.uproject"
```

## Tests

```powershell
.\Scripts\run-tests.ps1 -UnrealEditor "C:\Program Files\Epic Games\UE_5.8\Engine\Binaries\Win64\UnrealEditor.exe"
```

## Shipping and package

```powershell
.\Scripts\build-shipping.ps1 -UnrealEngineRoot "C:\Program Files\Epic Games\UE_5.8"
.\Scripts\package.ps1 -UnrealEngineRoot "C:\Program Files\Epic Games\UE_5.8"
```

## Common fixes

Delete `Binaries`, `Intermediate` and `Saved`, regenerate project files and rebuild.

If MSVC or SDK errors occur, use Visual Studio Installer to add the C++ workload and Windows SDK.

On the first Lumen/Nanite run, allow shader compilation and Derived Data Cache creation to finish.

For packaging failures inspect Unreal Automation Tool output and `Saved/Logs` from the first failure.
