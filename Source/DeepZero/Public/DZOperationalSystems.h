#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DZTypes.h"
#include "DZOperationalSystems.generated.h"

USTRUCT(BlueprintType)
struct FDZRiskPrediction
{
    GENERATED_BODY()
    UPROPERTY(BlueprintReadOnly) float EstimatedDurationHours=1;
    UPROPERTY(BlueprintReadOnly) float EstimatedBatteryUse=0;
    UPROPERTY(BlueprintReadOnly) float EstimatedOxygenUse=0;
    UPROPERTY(BlueprintReadOnly) float EnvironmentalRisk=0;
    UPROPERTY(BlueprintReadOnly) float MechanicalRisk=0;
    UPROPERTY(BlueprintReadOnly) float OverallRisk=0;
    UPROPERTY(BlueprintReadOnly) FString Summary;
};

UCLASS(ClassGroup=(DeepZero),meta=(BlueprintSpawnableComponent))
class DEEPZERO_API UDZPowerPolicyComponent:public UActorComponent
{
    GENERATED_BODY()
public:
    UDZPowerPolicyComponent();
    UPROPERTY(EditAnywhere,BlueprintReadWrite) TArray<FDZPowerBus> PolicyBuses;
    UPROPERTY(BlueprintReadOnly) float TotalAllocation=0;
    UFUNCTION(BlueprintCallable) void ApplyEmergencyPolicy();
    UFUNCTION(BlueprintCallable) void ApplyExplorationPolicy();
    UFUNCTION(BlueprintCallable) void ApplyResearchPolicy();
    UFUNCTION(BlueprintCallable) void SetBus(FName Name,float Allocation,bool Online);
    UFUNCTION(BlueprintCallable) void Normalize();
    UFUNCTION(BlueprintPure) float Allocation(FName Name)const;
};

USTRUCT(BlueprintType)
struct FDZAnomalyRecord
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere,BlueprintReadWrite) FName Id;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) FString Codename;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) FVector Location=FVector::ZeroVector;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) float Confidence=0;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) float Severity=0;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) bool Confirmed=false;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) TArray<FString> Evidence;
};

UCLASS(ClassGroup=(DeepZero),meta=(BlueprintSpawnableComponent))
class DEEPZERO_API UDZAnomalyRegistryComponent:public UActorComponent
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere,BlueprintReadWrite) TArray<FDZAnomalyRecord> Records;
    UFUNCTION(BlueprintCallable) void RegisterObservation(const FDZAnomalyRecord& Record);
    UFUNCTION(BlueprintCallable) void AddEvidence(FName Id,const FString& Evidence);
    UFUNCTION(BlueprintCallable) void Confirm(FName Id,float AdditionalConfidence);
    UFUNCTION(BlueprintPure) int32 ConfirmedCount()const;
    UFUNCTION(BlueprintPure) float MeanSeverity()const;
};

UCLASS(ClassGroup=(DeepZero),meta=(BlueprintSpawnableComponent))
class DEEPZERO_API UDZRiskPredictorComponent:public UActorComponent
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable) FDZRiskPrediction Predict(float DistanceKm,float AverageSpeedKnots,float DepthMeters,float CurrentStrength,float HullIntegrity,float BatteryRatio,float OxygenRatio,float CrewEfficiency)const;
    UFUNCTION(BlueprintCallable) FDZRiskPrediction PredictFromMission(const FDZMission& Mission,float DistanceKm,float DepthMeters,float HullIntegrity,float BatteryRatio,float OxygenRatio)const;
};

UCLASS(ClassGroup=(DeepZero),meta=(BlueprintSpawnableComponent))
class DEEPZERO_API UDZWorldStateComponent:public UActorComponent
{
    GENERATED_BODY()
public:
    UDZWorldStateComponent();
    UPROPERTY(BlueprintReadOnly) TSet<FName> DiscoveredLocations;
    UPROPERTY(BlueprintReadOnly) TSet<FName> PersistentFlags;
    UPROPERTY(BlueprintReadOnly) TMap<FName,float> StateValues;
    UFUNCTION(BlueprintCallable) void Discover(FName Location);
    UFUNCTION(BlueprintCallable) void SetFlag(FName Flag,bool Enabled);
    UFUNCTION(BlueprintCallable) void SetValue(FName Key,float Value);
    UFUNCTION(BlueprintPure) bool IsDiscovered(FName Location)const;
    UFUNCTION(BlueprintPure) bool HasFlag(FName Flag)const;
    UFUNCTION(BlueprintPure) float GetValue(FName Key,float DefaultValue=0)const;
    UFUNCTION(BlueprintCallable) void ModifyValue(FName Key,float Delta);
    UFUNCTION(BlueprintCallable) void ClearRuntimeValues();
};
