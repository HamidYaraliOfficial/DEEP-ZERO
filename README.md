# DEEP ZERO

## English

**DEEP ZERO** is an Unreal Engine 5 + C++ first-person submarine survival-horror simulation architecture focused on deep-ocean pressure, darkness, resource scarcity, system failures, sonar uncertainty, crew simulation, investigation, dynamic missions, persistent saves and adaptive horror pacing.

### Implemented core systems

- Ocean depth/pressure/temperature/visibility/current/salinity/activity sampling.
- Submarine power, battery, reactor, propulsion, ballast, hull, thermal, oxygen, CO2 and emergency telemetry.
- Compartment flooding, bulkheads, pumps, fire, smoke and atmosphere.
- Active/passive sonar with noise, detection probability, contacts and anomalies.
- Threat AI with stimulus memory, curiosity, aggression, investigation and hunting states.
- Horror Director with context-based tension and anti-repetition cooldown.
- Crew roles, skill, fatigue, morale, trust, availability and task assignment.
- Context-aware missions with required and optional objectives.
- Versioned SaveGame, migration, integrity token and slot operations.
- Three languages and RTL/LTR direction state.
- Windows 11 / Light / Dark / Windows Default / Red Alert / Deep Blue theme state.
- User-defined time windows with open/closed state, remaining duration and next-window calculation.
- Automation tests for schedule, ocean pressure and save migration.
- Win64 setup/build/test/package PowerShell scripts.

### Controls

| Action | Default |
|---|---|
| Move | W / S |
| Vertical | Space / Left Ctrl |
| Look | Mouse |
| Interact | E |
| Sonar Ping | Q |

### Time-window engine

Users can enter any start minute and duration. The schedule subsystem calculates whether the window is open, how long the current window remains open, and how long until the next enabled window.

### Installation — Windows 10/11

1. Install Epic Games Launcher and Unreal Engine 5.8.x.
2. Install Visual Studio 2022 17.14+ or newer supported Visual Studio.
3. Select **Game development with C++**, **Desktop development with C++**, **Visual Studio Tools for Unreal Engine**, **Unreal Engine debugger tools**, **Unreal Engine Test Adapter**, and a current Windows SDK.
4. Install Git for Windows.
5. Open `DEEP_ZERO.uproject` and generate project files.
6. In PowerShell:

```powershell
Set-ExecutionPolicy -Scope Process Bypass
.\Scripts\setup-windows.ps1 -UnrealEngineRoot "C:\Program Files\Epic Games\UE_5.8"
```

### Build Development

```powershell
.\Scripts\build-development.ps1 -UnrealEngineRoot "C:\Program Files\Epic Games\UE_5.8"
```

### Run Automation Tests

```powershell
.\Scripts\run-tests.ps1 -UnrealEditor "C:\Program Files\Epic Games\UE_5.8\Engine\Binaries\Win64\UnrealEditor.exe"
```

### Build Shipping

```powershell
.\Scripts\build-shipping.ps1 -UnrealEngineRoot "C:\Program Files\Epic Games\UE_5.8"
```

### Package

```powershell
.\Scripts\package.ps1 -UnrealEngineRoot "C:\Program Files\Epic Games\UE_5.8"
```

### Toolchain command notes

Visual Studio Installer can be launched with:

```powershell
& "C:\Program Files (x86)\Microsoft Visual Studio\Installer\vs_installer.exe"
```

Git for Windows can be installed with winget when available:

```powershell
winget install --id Git.Git -e
```

Visual Studio Community can be installed with winget when available:

```powershell
winget install --id Microsoft.VisualStudio.2022.Community -e
```

Use Visual Studio Installer to add the Unreal/C++ workloads and SDK components after installation.

### Repository layout

```text
Config/
Content/{Maps,Data,Submarine,Creatures,AI,UI,Audio,VFX,Missions,Lore}/
Docs/
Plugins/
Scripts/
Source/DeepZero/
Source/{Simulation,Submarine,AI,Crew,Sonar,Ocean,Physics,Systems,Missions,Research,Dialogue,Save,UI,Networking,Tools}/
Tests/
DEEP_ZERO.uproject
README (1).markdown
INSTALLATION.md
```

---

## فارسی

**DEEP ZERO** یک معماری بازی Unreal Engine 5 و C++ برای تجربه First-Person Submarine Survival Horror + Exploration + Simulation است و سیستم‌های فشار اعماق، تاریکی، منابع، خرابی، سونار، هوش مصنوعی تهدید، خدمه، مأموریت، ذخیره‌سازی و زمان‌بندی را در Core پیاده می‌کند.

### امکانات اصلی

- شبیه‌سازی عمق، فشار، دما، دید، جریان آب، شوری و فعالیت اقیانوس.
- Reactor، Battery، Power Distribution، Propulsion، Ballast، Hull، Cooling، Oxygen، CO2 و Emergency.
- Flooding به‌صورت Compartment-Based، Bulkhead، Pump، Fire، Smoke و Atmosphere.
- سونار Active/Passive، نویز، احتمال تشخیص، Contact و Anomaly.
- Threat AI با Stimulus، Memory، Curiosity، Aggression، Investigation و Hunting.
- Horror Director بر اساس Darkness، Isolation، Alarms، Unknown Signal و Threat Proximity.
- Crew Simulation با Role، Skill، Fatigue، Morale، Trust، Task و Availability.
- Missionهای Dynamic با Objectiveهای اجباری و اختیاری.
- Save Versioning، Migration و Integrity Token.
- سه زبان English، Persian RTL و Chinese LTR.
- تم‌های Windows 11، Light، Dark، Windows Default، Red Alert و Deep Blue.
- موتور زمان‌بندی که ساعت شروع، مدت، وضعیت باز بودن و زمان تا بازه بعدی را محاسبه می‌کند.
- تست‌های Automation برای Schedule، Ocean و Save.

### کنترل‌ها

| عملکرد | کلید |
|---|---|
| حرکت | W / S |
| حرکت عمودی | Space / Left Ctrl |
| نگاه | Mouse |
| تعامل | E |
| سونار | Q |

### نصب Windows 10/11

1. Epic Games Launcher و Unreal Engine 5.8.x را نصب کنید.
2. Visual Studio 2022 نسخه 17.14 یا بالاتر و در صورت استفاده نسخه جدیدتر پشتیبانی‌شده را نصب کنید.
3. در Visual Studio Installer گزینه‌های **Game development with C++**، **Desktop development with C++**، **Visual Studio Tools for Unreal Engine**، **Unreal Engine debugger tools**، **Unreal Engine Test Adapter** و Windows SDK را فعال کنید.
4. Git for Windows را نصب کنید.
5. `DEEP_ZERO.uproject` را باز و Project Files را Generate کنید.
6. در PowerShell:

```powershell
Set-ExecutionPolicy -Scope Process Bypass
.\Scripts\setup-windows.ps1 -UnrealEngineRoot "C:\Program Files\Epic Games\UE_5.8"
```

Build توسعه:

```powershell
.\Scripts\build-development.ps1 -UnrealEngineRoot "C:\Program Files\Epic Games\UE_5.8"
```

تست:

```powershell
.\Scripts\run-tests.ps1 -UnrealEditor "C:\Program Files\Epic Games\UE_5.8\Engine\Binaries\Win64\UnrealEditor.exe"
```

Build نهایی:

```powershell
.\Scripts\build-shipping.ps1 -UnrealEngineRoot "C:\Program Files\Epic Games\UE_5.8"
```

Package:

```powershell
.\Scripts\package.ps1 -UnrealEngineRoot "C:\Program Files\Epic Games\UE_5.8"
```

نصب Git با winget در صورت فعال بودن:

```powershell
winget install --id Git.Git -e
```

نصب Visual Studio Community 2022 با winget:

```powershell
winget install --id Microsoft.VisualStudio.2022.Community -e
```

جزئیات کامل در `INSTALLATION.md` قرار دارد.

---

## 中文

**DEEP ZERO** 是 Unreal Engine 5 + C++ 的第一人称深海潜艇生存恐怖与模拟架构，核心围绕深度压力、黑暗、资源限制、系统故障、声呐不确定性、船员、调查、任务、持久化存档以及动态恐怖节奏展开。

### 主要功能

- 深度、压力、温度、能见度、洋流、盐度和环境活动模拟。
- Reactor、Battery、Power、Propulsion、Ballast、Hull、Cooling、Oxygen、CO2 与 Emergency 系统。
- 基于舱室的进水、Bulkhead、Pump、Fire、Smoke 与 Atmosphere。
- Active/Passive Sonar、Noise、Detection、Contact、Anomaly。
- Threat AI：Stimulus、Memory、Curiosity、Aggression、Investigation、Hunting。
- Horror Director 根据黑暗、孤立、警报、未知信号和威胁距离调整节奏，并通过冷却时间减少重复。
- Crew Simulation：Role、Skill、Fatigue、Morale、Trust、Task、Availability。
- Dynamic Mission 与必选/可选 Objectives。
- Save Versioning、Migration、Integrity Token。
- English、Persian RTL、Chinese LTR。
- Windows 11、Light、Dark、Windows Default、Red Alert、Deep Blue。
- 用户自定义开始时间和持续时间的时间窗口引擎，可计算当前是否开放、剩余多久以及下一窗口何时开始。
- Schedule、Ocean Pressure、Save Migration 自动化测试。

### 控制

| 操作 | 默认 |
|---|---|
| 移动 | W / S |
| 垂直控制 | Space / Left Ctrl |
| 视角 | Mouse |
| 交互 | E |
| 声呐 | Q |

### Windows 10/11 安装

1. 使用 Epic Games Launcher 安装 Unreal Engine 5.8.x。
2. 安装 Visual Studio 2022 17.14+ 或更高版本的受支持 Visual Studio。
3. 在 Visual Studio Installer 中安装 **Game development with C++**、**Desktop development with C++**、**Visual Studio Tools for Unreal Engine**、**Unreal Engine debugger tools**、**Unreal Engine Test Adapter** 与 Windows SDK。
4. 安装 Git for Windows。
5. 打开 `DEEP_ZERO.uproject` 并生成项目文件。
6. PowerShell：

```powershell
Set-ExecutionPolicy -Scope Process Bypass
.\Scripts\setup-windows.ps1 -UnrealEngineRoot "C:\Program Files\Epic Games\UE_5.8"
```

Development Build：

```powershell
.\Scripts\build-development.ps1 -UnrealEngineRoot "C:\Program Files\Epic Games\UE_5.8"
```

测试：

```powershell
.\Scripts\run-tests.ps1 -UnrealEditor "C:\Program Files\Epic Games\UE_5.8\Engine\Binaries\Win64\UnrealEditor.exe"
```

Shipping Build：

```powershell
.\Scripts\build-shipping.ps1 -UnrealEngineRoot "C:\Program Files\Epic Games\UE_5.8"
```

Packaging：

```powershell
.\Scripts\package.ps1 -UnrealEngineRoot "C:\Program Files\Epic Games\UE_5.8"
```

Git：

```powershell
winget install --id Git.Git -e
```

Visual Studio Community 2022：

```powershell
winget install --id Microsoft.VisualStudio.2022.Community -e
```

完整安装步骤位于 `INSTALLATION.md`。
