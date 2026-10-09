#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Subsystems/WorldSubsystem.h"
#include "GameFramework/SaveGame.h"
#include "DZTypes.h"
#include "DZCoreSimulation.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDZTelemetryDelegate,const FDZTelemetry&,Data);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDZAlarmDelegate,const FDZAlarm&,Data);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDZSonarDelegate,const FDZSonarContact&,Data);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDZMissionDelegate,const FDZMission&,Data);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDZCrewDelegate,const FDZCrewMember&,Data);

UCLASS(ClassGroup=(DeepZero),meta=(BlueprintSpawnableComponent))
class DEEPZERO_API UDZOceanSimulationComponent:public UActorComponent
{
    GENERATED_BODY()
public:
    UDZOceanSimulationComponent();
    UPROPERTY(EditAnywhere,BlueprintReadWrite) float SeaLevelCm=0;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) float PressurePerMeterKPa=10.05f;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) float SurfaceTempC=17;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) float DeepTempC=2;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) float SurfaceVisibility=150;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) float DeepVisibility=4;
    UPROPERTY(BlueprintReadOnly) FDZOceanSample Sample;
    UFUNCTION(BlueprintCallable) FDZOceanSample SampleAtDepth(float DepthMeters)const;
    UFUNCTION(BlueprintCallable) FVector CurrentAtDepth(float DepthMeters,float TimeSeconds)const;
    UFUNCTION(BlueprintCallable) void Resample();
private:
    float Temperature(float Depth)const;
    float Visibility(float Depth)const;
};

UCLASS(ClassGroup=(DeepZero),meta=(BlueprintSpawnableComponent))
class DEEPZERO_API UDZSubmarineSimulationComponent:public UActorComponent
{
    GENERATED_BODY()
public:
    UDZSubmarineSimulationComponent();
    UPROPERTY(EditAnywhere,BlueprintReadWrite) float MaxDepthMeters=9000;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) float ReactorMW=20;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) float BatteryMWh=80;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) float MaxOxygenKg=180;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) float FreshWaterLiters=3200;
    UPROPERTY(BlueprintReadOnly) float ChargeMWh=80;
    UPROPERTY(BlueprintReadOnly) float OxygenKg=180;
    UPROPERTY(BlueprintReadOnly) float CO2Percent=.04f;
    UPROPERTY(BlueprintReadOnly) float HullIntegrity=1;
    UPROPERTY(BlueprintReadOnly) float CoolingEfficiency=1;
    UPROPERTY(BlueprintReadOnly) float InternalTempC=20;
    UPROPERTY(BlueprintReadOnly) float Ballast=.5f;
    UPROPERTY(BlueprintReadOnly) float FloodingRateLps=0;
    UPROPERTY(BlueprintReadOnly) FDZOceanSample Ocean;
    UPROPERTY(BlueprintReadOnly) TArray<FDZPowerBus> Buses;
    UPROPERTY(BlueprintReadOnly) TArray<FDZTelemetry> Telemetry;
    UPROPERTY(BlueprintReadOnly) TArray<FDZAlarm> Alarms;
    UPROPERTY(BlueprintAssignable) FDZTelemetryDelegate OnTelemetry;
    UPROPERTY(BlueprintAssignable) FDZAlarmDelegate OnAlarm;
    UFUNCTION(BlueprintCallable) void SetOcean(const FDZOceanSample& InOcean);
    UFUNCTION(BlueprintCallable) void SetThrottle(float Value);
    UFUNCTION(BlueprintCallable) void SetDesiredDepth(float Depth);
    UFUNCTION(BlueprintCallable) void AdjustBallast(float Delta);
    UFUNCTION(BlueprintCallable) void SetCooling(float Efficiency);
    UFUNCTION(BlueprintCallable) void ApplyDamage(EDZDamageType Type,float Amount,FName System);
    UFUNCTION(BlueprintCallable) void Repair(FName System,float Amount);
    UFUNCTION(BlueprintCallable) void SetPower(FName Bus,float Allocation,bool Online);
    UFUNCTION(BlueprintCallable) void AckAlarm(FName AlarmId);
    UFUNCTION(BlueprintPure) float StructuralRisk()const;
    UFUNCTION(BlueprintPure) bool Emergency()const;
    UFUNCTION(BlueprintPure) float RangeHours()const;
protected:
    virtual void BeginPlay()override;
    virtual void TickComponent(float Delta,ELevelTick TickType,FActorComponentTickFunction* ThisTickFunction)override;
private:
    float Throttle=0;
    float DesiredDepth=1000;
    float ReactorLoad=.3f;
    float WaterMassKg=0;
    void InitBuses();
    void InitTelemetry();
    void SimPower(float Dt);
    void SimAtmosphere(float Dt);
    void SimThermal(float Dt);
    void SimHull(float Dt);
    void SimFlooding(float Dt);
    void SimPropulsion(float Dt);
    void UpdateAlarms();
    void Raise(FName Id,EDZAlarmSeverity Severity,const FString& Message);
    FDZTelemetry* FindTelemetry(FName Name);
    FDZPowerBus* FindBus(FName Name);
};

UCLASS(ClassGroup=(DeepZero),meta=(BlueprintSpawnableComponent))
class DEEPZERO_API UDZFloodingComponent:public UActorComponent
{
    GENERATED_BODY()
public:
    UDZFloodingComponent();
    UPROPERTY(EditAnywhere,BlueprintReadWrite) TArray<FDZCompartment> Compartments;
    UPROPERTY(BlueprintReadOnly) float TotalWaterLiters=0;
    UFUNCTION(BlueprintCallable) void AddLeak(FName Compartment,float LitersPerSecond);
    UFUNCTION(BlueprintCallable) void SetBulkhead(FName Compartment,EDZDoorState State);
    UFUNCTION(BlueprintCallable) void SetPump(FName Compartment,bool Enabled);
    UFUNCTION(BlueprintCallable) void SetFire(FName Compartment,float Intensity);
    UFUNCTION(BlueprintPure) bool Safe(FName Compartment)const;
protected:
    virtual void BeginPlay()override;
    virtual void TickComponent(float Delta,ELevelTick TickType,FActorComponentTickFunction* ThisTickFunction)override;
private:
    TMap<FName,float> Leaks;
    FDZCompartment* Find(FName Name);
    const FDZCompartment* FindConst(FName Name)const;
};

UCLASS(ClassGroup=(DeepZero),meta=(BlueprintSpawnableComponent))
class DEEPZERO_API UDZSonarComponent:public UActorComponent
{
    GENERATED_BODY()
public:
    UDZSonarComponent();
    UPROPERTY(EditAnywhere,BlueprintReadWrite) EDZSonarMode Mode=EDZSonarMode::Passive;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) float MaxRangeMeters=5000;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) float PingInterval=4;
    UPROPERTY(BlueprintReadOnly) float Noise=0;
    UPROPERTY(BlueprintReadOnly) TArray<FDZSonarContact> Contacts;
    UPROPERTY(BlueprintAssignable) FDZSonarDelegate OnContact;
    UFUNCTION(BlueprintCallable) void SetMode(EDZSonarMode NewMode);
    UFUNCTION(BlueprintCallable) void Ping();
    UFUNCTION(BlueprintCallable) void Detect(const FVector& WorldLocation,float Signature,bool Anomaly);
    UFUNCTION(BlueprintPure) float Probability(float Range,float Signature)const;
protected:
    virtual void BeginPlay()override;
    virtual void TickComponent(float Delta,ELevelTick TickType,FActorComponentTickFunction* ThisTickFunction)override;
private:
    float Timer=0;
};

UCLASS(ClassGroup=(DeepZero),meta=(BlueprintSpawnableComponent))
class DEEPZERO_API UDZCrewComponent:public UActorComponent
{
    GENERATED_BODY()
public:
    UDZCrewComponent();
    UPROPERTY(EditAnywhere,BlueprintReadWrite) TArray<FDZCrewMember> Crew;
    UPROPERTY(BlueprintReadOnly) float AverageMorale=.75f;
    UPROPERTY(BlueprintAssignable) FDZCrewDelegate OnCrewChanged;
    UFUNCTION(BlueprintCallable) void Assign(FName CrewId,EDZCrewTask Task,FName Target);
    UFUNCTION(BlueprintCallable) void SetAvailable(FName CrewId,bool Available);
    UFUNCTION(BlueprintCallable) void AddFatigue(FName CrewId,float Amount);
    UFUNCTION(BlueprintCallable) void AddStress(FName CrewId,float Amount);
    UFUNCTION(BlueprintPure) float Efficiency(FName CrewId)const;
    UFUNCTION(BlueprintPure) int32 AvailableCount()const;
protected:
    virtual void BeginPlay()override;
    virtual void TickComponent(float Delta,ELevelTick TickType,FActorComponentTickFunction* ThisTickFunction)override;
private:
    FDZCrewMember* Find(FName Id);
};

UCLASS()
class DEEPZERO_API ADZThreatActor:public AActor
{
    GENERATED_BODY()
public:
    ADZThreatActor();
    UPROPERTY(VisibleAnywhere) USphereComponent* Collision;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) float DetectionRange=5000;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) float AcousticSensitivity=.8f;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) float Curiosity=.55f;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) float Aggression=.3f;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) float MemorySeconds=45;
    UPROPERTY(BlueprintReadOnly) EDZThreatState State=EDZThreatState::Dormant;
    UPROPERTY(BlueprintReadOnly) FVector LastStimulus=FVector::ZeroVector;
    UFUNCTION(BlueprintCallable) void Hear(const FVector& Location,float Strength);
    UFUNCTION(BlueprintCallable) void SeeLight(const FVector& Location,float Strength);
    UFUNCTION(BlueprintPure) float Interest()const;
protected:
    virtual void Tick(float Delta)override;
private:
    float Age=999; float Strength=0; float Idle=0;
};

UCLASS()
class DEEPZERO_API UDZHorrorDirectorSubsystem:public UWorldSubsystem
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadOnly) float Tension=0;
    UPROPERTY(BlueprintReadOnly) int32 EventsSinceQuiet=0;
    UFUNCTION(BlueprintCallable) void SetContext(float Darkness,float Isolation,float Alarms,float Signal,float Threat);
    UFUNCTION(BlueprintCallable) bool TryEvent(FName Preferred=NAME_None);
    UFUNCTION(BlueprintPure) float Cooldown()const{return CooldownTimer;}
private:
    float CooldownTimer=0; FRandomStream Random;
};

UCLASS()
class DEEPZERO_API UDZMissionSubsystem:public UGameInstanceSubsystem
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadOnly) TArray<FDZMission> Available;
    UPROPERTY(BlueprintReadOnly) TArray<FDZMission> Active;
    UPROPERTY(BlueprintAssignable) FDZMissionDelegate OnMission;
    UFUNCTION(BlueprintCallable) void Generate(float Depth,float ResourceRatio,float Anomaly);
    UFUNCTION(BlueprintCallable) bool Start(FName Id);
    UFUNCTION(BlueprintCallable) bool Advance(FName Id,FName Objective,float Amount);
    UFUNCTION(BlueprintCallable) bool Complete(FName Id);
private:
    FDZMission Make(FName Id,const FString& Title,EDZMissionType Type,float Risk,int32 Reward)const;
};

UCLASS()
class DEEPZERO_API UDZScheduleSubsystem:public UGameInstanceSubsystem
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadOnly) TArray<FDZScheduleWindow> Windows;
    UFUNCTION(BlueprintCallable) void AddWindow(const FDZScheduleWindow& Window);
    UFUNCTION(BlueprintCallable) void RemoveWindow(const FString& Name);
    UFUNCTION(BlueprintCallable) void Replace(const TArray<FDZScheduleWindow>& NewWindows);
    UFUNCTION(BlueprintPure) FDZScheduleResult Query(int32 MinuteOfDay)const;
    UFUNCTION(BlueprintPure) FDZScheduleResult QueryTime(int32 Hour,int32 Minute)const{return Query(Hour*60+Minute);}
    UFUNCTION(BlueprintPure) FString FormatMinutes(int32 Minutes)const;
};

UCLASS()
class DEEPZERO_API UDZLocalizationSubsystem:public UGameInstanceSubsystem
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadOnly) EDZLanguage Language=EDZLanguage::English;
    UPROPERTY(BlueprintReadOnly) EDZTheme Theme=EDZTheme::Windows11;
    UFUNCTION(BlueprintCallable) void SetLanguage(EDZLanguage NewLanguage){Language=NewLanguage;}
    UFUNCTION(BlueprintCallable) void SetTheme(EDZTheme NewTheme){Theme=NewTheme;}
    UFUNCTION(BlueprintPure) bool IsRTL()const{return Language==EDZLanguage::Persian;}
    UFUNCTION(BlueprintPure) FString Direction()const{return IsRTL()?TEXT("RTL"):TEXT("LTR");}
    UFUNCTION(BlueprintPure) FText Text(FName Key)const;
    virtual void Initialize(FSubsystemCollectionBase& Collection)override;
private:
    TMap<FName,TArray<FText>> Dict;
};

UCLASS()
class DEEPZERO_API UDZSaveGame:public USaveGame
{
    GENERATED_BODY()
public:
    static constexpr int32 CurrentVersion=4;
    UPROPERTY() int32 Version=CurrentVersion;
    UPROPERTY() FString SlotGuid;
    UPROPERTY() FDateTime SavedAt;
    UPROPERTY() FVector Location=FVector::ZeroVector;
    UPROPERTY() FRotator Rotation=FRotator::ZeroRotator;
    UPROPERTY() float Hull=1;
    UPROPERTY() float Battery=80;
    UPROPERTY() float Oxygen=180;
    UPROPERTY() float DesiredDepth=1000;
    UPROPERTY() TArray<FName> DiscoveredLocations;
    UPROPERTY() TArray<FName> StoryFlags;
    UPROPERTY() TArray<FName> CompletedMissions;
    UFUNCTION(BlueprintCallable) bool Migrate();
    UFUNCTION(BlueprintPure) FString IntegrityToken()const;
};

UCLASS()
class DEEPZERO_API UDZSaveSubsystem:public UGameInstanceSubsystem
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable) bool Save(int32 Slot,UDZSaveGame* Data);
    UFUNCTION(BlueprintCallable) UDZSaveGame* Load(int32 Slot);
    UFUNCTION(BlueprintCallable) bool Remove(int32 Slot);
    UFUNCTION(BlueprintPure) bool Exists(int32 Slot)const;
private:
    FString SlotName(int32 Slot)const{return FString::Printf(TEXT("DeepZeroSlot_%02d"),Slot);}
};
