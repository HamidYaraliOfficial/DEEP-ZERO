#pragma once
#include "CoreMinimal.h"
#include "DZDiagnostics.generated.h"

UENUM(BlueprintType)
enum class EDZDiagnosticCode : uint8
{
    None,
    PowerLow,
    BatteryLow,
    ReactorHot,
    CoolingWeak,
    HullWeak,
    Flooding,
    OxygenLow,
    CO2High,
    SonarDegraded,
    NavigationUncertain,
    CommunicationWeak,
    CrewFatigued,
    ResearchDelayed,
    ScheduleClosed
};

USTRUCT(BlueprintType)
struct FDZDiagnosticItem
{
    GENERATED_BODY()
    UPROPERTY(BlueprintReadOnly) EDZDiagnosticCode Code=EDZDiagnosticCode::None;
    UPROPERTY(BlueprintReadOnly) int32 Priority=0;
    UPROPERTY(BlueprintReadOnly) FString Title;
    UPROPERTY(BlueprintReadOnly) FString Recommendation;
};

UCLASS()
class DEEPZERO_API UDZDiagnosticsLibrary:public UObject
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable) static FDZDiagnosticItem CheckPower(float Allocation,float ChargeRatio);
    UFUNCTION(BlueprintCallable) static FDZDiagnosticItem CheckBattery(float ChargeRatio);
    UFUNCTION(BlueprintCallable) static FDZDiagnosticItem CheckReactor(float Temperature,float Load);
    UFUNCTION(BlueprintCallable) static FDZDiagnosticItem CheckCooling(float Efficiency);
    UFUNCTION(BlueprintCallable) static FDZDiagnosticItem CheckHull(float Integrity,float DepthRatio);
    UFUNCTION(BlueprintCallable) static FDZDiagnosticItem CheckFlooding(float LitersPerSecond);
    UFUNCTION(BlueprintCallable) static FDZDiagnosticItem CheckOxygen(float Ratio);
    UFUNCTION(BlueprintCallable) static FDZDiagnosticItem CheckCO2(float Percent);
    UFUNCTION(BlueprintCallable) static FDZDiagnosticItem CheckSonar(float Noise,float Health);
    UFUNCTION(BlueprintCallable) static FDZDiagnosticItem CheckNavigation(float Confidence,float Drift);
    UFUNCTION(BlueprintCallable) static FDZDiagnosticItem CheckCommunication(float Quality,float Depth);
    UFUNCTION(BlueprintCallable) static FDZDiagnosticItem CheckCrew(float Fatigue,float Morale);
    UFUNCTION(BlueprintCallable) static FDZDiagnosticItem CheckResearch(float Progress,float Power);
    UFUNCTION(BlueprintCallable) static FDZDiagnosticItem CheckSchedule(bool Open,int32 Minutes);
};
