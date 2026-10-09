#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DZTypes.h"
#include "DZExtendedSystems.generated.h"

UENUM(BlueprintType)
enum class EDZItemCategory : uint8 { Food, Water, Oxygen, Medical, SparePart, Battery, Tool, Sample, Equipment };

USTRUCT(BlueprintType)
struct FDZInventoryItem
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere,BlueprintReadWrite) FName Id;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) FString DisplayName;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) EDZItemCategory Category=EDZItemCategory::Tool;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) int32 Quantity=0;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) float UnitMassKg=1;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) float VolumeM3=.01f;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) float Durability=1;
};

UCLASS(ClassGroup=(DeepZero),meta=(BlueprintSpawnableComponent))
class DEEPZERO_API UDZInventoryComponent:public UActorComponent
{
    GENERATED_BODY()
public:
    UDZInventoryComponent();
    UPROPERTY(EditAnywhere,BlueprintReadWrite) float CapacityMassKg=2000;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) float CapacityVolumeM3=120;
    UPROPERTY(BlueprintReadOnly) TArray<FDZInventoryItem> Items;
    UPROPERTY(BlueprintReadOnly) float CurrentMassKg=0;
    UPROPERTY(BlueprintReadOnly) float CurrentVolumeM3=0;
    UFUNCTION(BlueprintCallable) bool AddItem(const FDZInventoryItem& Item,int32 Quantity=1);
    UFUNCTION(BlueprintCallable) bool RemoveItem(FName Id,int32 Quantity=1);
    UFUNCTION(BlueprintCallable) int32 GetQuantity(FName Id)const;
    UFUNCTION(BlueprintCallable) bool Consume(FName Id,int32 Quantity=1);
    UFUNCTION(BlueprintPure) float MassRatio()const;
    UFUNCTION(BlueprintPure) float VolumeRatio()const;
private:
    void RecalculateLoad();
};

USTRUCT(BlueprintType)
struct FDZNavPoint
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere,BlueprintReadWrite) FName Id;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) FVector Location=FVector::ZeroVector;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) float EstimatedDepth=0;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) bool Known=true;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) bool Anomalous=false;
};

UCLASS(ClassGroup=(DeepZero),meta=(BlueprintSpawnableComponent))
class DEEPZERO_API UDZNavigationComponent:public UActorComponent
{
    GENERATED_BODY()
public:
    UDZNavigationComponent();
    UPROPERTY(BlueprintReadOnly) TArray<FDZNavPoint> Points;
    UPROPERTY(BlueprintReadOnly) float GyroHeadingDegrees=0;
    UPROPERTY(BlueprintReadOnly) float EstimatedDriftMeters=0;
    UPROPERTY(BlueprintReadOnly) float MapConfidence=0;
    UFUNCTION(BlueprintCallable) void AddPoint(const FDZNavPoint& Point);
    UFUNCTION(BlueprintCallable) void UpdateDrift(float DeltaSeconds,float CurrentStrength,float Noise);
    UFUNCTION(BlueprintCallable) void CorrectWithReference(FVector ReferenceLocation,float Confidence);
    UFUNCTION(BlueprintCallable) void RecordSonarPoint(FVector Location,float Confidence,bool Anomaly);
    UFUNCTION(BlueprintPure) FVector BestKnownPosition()const;
protected:
    virtual void TickComponent(float Delta,ELevelTick Type,FActorComponentTickFunction*Fn)override;
};

UCLASS(ClassGroup=(DeepZero),meta=(BlueprintSpawnableComponent))
class DEEPZERO_API UDZAtmosphereComponent:public UActorComponent
{
    GENERATED_BODY()
public:
    UDZAtmosphereComponent();
    UPROPERTY(EditAnywhere,BlueprintReadWrite) float OxygenPercent=20.95f;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) float CO2Percent=.04f;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) float TemperatureC=20;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) float PressureKPa=101.325f;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) float Humidity=.55f;
    UPROPERTY(BlueprintReadOnly) bool Breathable=true;
    UPROPERTY(BlueprintReadOnly) bool FireDanger=false;
    UFUNCTION(BlueprintCallable) void AddCrewLoad(int32 CrewCount);
    UFUNCTION(BlueprintCallable) void AddSmoke(float Amount);
    UFUNCTION(BlueprintCallable) void SetVentilation(float Efficiency);
    UFUNCTION(BlueprintCallable) void SetScrubber(float Efficiency);
    UFUNCTION(BlueprintPure) bool IsCritical()const;
protected:
    virtual void TickComponent(float Delta,ELevelTick Type,FActorComponentTickFunction*Fn)override;
private:
    int32 Crew=0;
    float Smoke=0;
    float Ventilation=.9f;
    float Scrubber=.9f;
};

USTRUCT(BlueprintType)
struct FDZResearchCapture
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere,BlueprintReadWrite) FName Id;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) FString Subject;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) FVector Location=FVector::ZeroVector;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) float Quality=.5f;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) double WorldTime=0;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) FString Notes;
};

UCLASS(ClassGroup=(DeepZero),meta=(BlueprintSpawnableComponent))
class DEEPZERO_API UDZResearchCaptureComponent:public UActorComponent
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadOnly) TArray<FDZResearchCapture> Captures;
    UFUNCTION(BlueprintCallable) void Capture(const FDZResearchCapture& Data);
    UFUNCTION(BlueprintCallable) bool AddNote(FName Id,const FString& Note);
    UFUNCTION(BlueprintPure) float AverageQuality()const;
};

UCLASS()
class DEEPZERO_API UDZTelemetryFormatter:public UObject
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable) static FString SystemStateText(EDZSystemState State);
    UFUNCTION(BlueprintCallable) static FString SeverityText(EDZAlarmSeverity Severity);
    UFUNCTION(BlueprintCallable) static FString BuildDashboard(const TArray<FDZTelemetry>& Telemetry,const TArray<FDZAlarm>& Alarms);
    UFUNCTION(BlueprintCallable) static FString FormatPercent(float Ratio);
    UFUNCTION(BlueprintCallable) static FString FormatDepth(float Meters);
    UFUNCTION(BlueprintCallable) static FString FormatPressure(float KPa);
};
