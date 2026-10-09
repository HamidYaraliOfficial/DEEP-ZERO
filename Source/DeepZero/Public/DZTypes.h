#pragma once
#include "CoreMinimal.h"
#include "DZTypes.generated.h"

UENUM(BlueprintType)
enum class EDZSystemState : uint8 { Offline, Standby, Nominal, Degraded, Failed, Emergency };
UENUM(BlueprintType)
enum class EDZAlarmSeverity : uint8 { Info, Advisory, Warning, Critical, Emergency };
UENUM(BlueprintType)
enum class EDZDamageType : uint8 { Pressure, Electrical, Mechanical, Flooding, Fire, Sensor, Structural };
UENUM(BlueprintType)
enum class EDZDoorState : uint8 { Open, Closed, Locked, Jammed, Damaged, EmergencySeal };
UENUM(BlueprintType)
enum class EDZThreatState : uint8 { Dormant, Curious, Investigating, Hunting, Territorial, Retreating };
UENUM(BlueprintType)
enum class EDZLanguage : uint8 { English, Persian, Chinese };
UENUM(BlueprintType)
enum class EDZTheme : uint8 { Windows11, Light, Dark, WindowsDefault, RedAlert, DeepBlue };
UENUM(BlueprintType)
enum class EDZMissionType : uint8 { Exploration, Research, Recovery, Rescue, Repair, Investigation, Navigation, Survival, Escape };
UENUM(BlueprintType)
enum class EDZCrewRole : uint8 { Captain, Engineer, SonarOperator, Navigator, Medic, Researcher, Technician };
UENUM(BlueprintType)
enum class EDZCrewTask : uint8 { Idle, Repair, Navigate, Sonar, Medical, Research, Maintenance, Rest, Emergency };
UENUM(BlueprintType)
enum class EDZSonarMode : uint8 { Passive, Active, Mapping };

USTRUCT(BlueprintType)
struct FDZOceanSample
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere,BlueprintReadWrite) float DepthMeters=0;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) float PressureKPa=101.325f;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) float TemperatureC=4;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) float VisibilityMeters=30;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) float CurrentMetersPerSecond=.2f;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) float SalinityPSU=35;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) float OxygenFraction=.2095f;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) float GeologicalActivity=.1f;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) float BiologicalActivity=.4f;
};

USTRUCT(BlueprintType)
struct FDZAlarm
{
    GENERATED_BODY()
    UPROPERTY(BlueprintReadOnly) FName Id;
    UPROPERTY(BlueprintReadOnly) EDZAlarmSeverity Severity=EDZAlarmSeverity::Info;
    UPROPERTY(BlueprintReadOnly) FString Message;
    UPROPERTY(BlueprintReadOnly) bool Acknowledged=false;
};

USTRUCT(BlueprintType)
struct FDZTelemetry
{
    GENERATED_BODY()
    UPROPERTY(BlueprintReadOnly) FName Name;
    UPROPERTY(BlueprintReadOnly) EDZSystemState State=EDZSystemState::Nominal;
    UPROPERTY(BlueprintReadOnly) float Health=1;
    UPROPERTY(BlueprintReadOnly) float Efficiency=1;
    UPROPERTY(BlueprintReadOnly) float Load=0;
    UPROPERTY(BlueprintReadOnly) FString Diagnostic;
};

USTRUCT(BlueprintType)
struct FDZCompartment
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere,BlueprintReadWrite) FName Name;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) float VolumeM3=80;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) float WaterLiters=0;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) float OxygenPercent=20.95f;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) float CO2Percent=.04f;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) float TemperatureC=20;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) float Fire=0;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) float Smoke=0;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) float Integrity=1;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) EDZDoorState Bulkhead=EDZDoorState::Open;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) bool Ventilation=true;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) bool Pump=true;
};

USTRUCT(BlueprintType)
struct FDZPowerBus
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere,BlueprintReadWrite) FName Name;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) float Allocation=.1f;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) float Demand=0;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) bool Online=true;
};

USTRUCT(BlueprintType)
struct FDZSonarContact
{
    GENERATED_BODY()
    UPROPERTY(BlueprintReadOnly) FVector RelativeLocation=FVector::ZeroVector;
    UPROPERTY(BlueprintReadOnly) float RangeMeters=0;
    UPROPERTY(BlueprintReadOnly) float BearingDegrees=0;
    UPROPERTY(BlueprintReadOnly) float Confidence=0;
    UPROPERTY(BlueprintReadOnly) float AcousticSignature=0;
    UPROPERTY(BlueprintReadOnly) bool Anomalous=false;
};

USTRUCT(BlueprintType)
struct FDZCrewMember
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere,BlueprintReadWrite) FName Id;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) FString Name;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) EDZCrewRole Role=EDZCrewRole::Technician;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) float Skill=.7f;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) float Fatigue=0;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) float Morale=.75f;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) float Trust=.75f;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) EDZCrewTask Task=EDZCrewTask::Idle;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) FName Target=NAME_None;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) bool Available=true;
};

USTRUCT(BlueprintType)
struct FDZObjective
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere,BlueprintReadWrite) FName Id;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) FString Description;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) float Progress=0;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) float Required=1;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) bool Optional=false;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) bool Completed=false;
};

USTRUCT(BlueprintType)
struct FDZMission
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere,BlueprintReadWrite) FName Id;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) FString Title;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) EDZMissionType Type=EDZMissionType::Exploration;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) float Risk=.2f;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) float ResourceCost=10;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) int32 Reward=100;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) TArray<FDZObjective> Objectives;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) bool Active=false;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) bool Completed=false;
};

USTRUCT(BlueprintType)
struct FDZScheduleWindow
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere,BlueprintReadWrite) FString Name;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) int32 StartMinute=0;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) int32 DurationMinutes=60;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) bool Enabled=true;
};

USTRUCT(BlueprintType)
struct FDZScheduleResult
{
    GENERATED_BODY()
    UPROPERTY(BlueprintReadOnly) bool Open=false;
    UPROPERTY(BlueprintReadOnly) int32 MinutesUntilNext=-1;
    UPROPERTY(BlueprintReadOnly) int32 CurrentWindowRemaining=-1;
    UPROPERTY(BlueprintReadOnly) int32 NextWindowDuration=0;
    UPROPERTY(BlueprintReadOnly) FString Summary;
};
