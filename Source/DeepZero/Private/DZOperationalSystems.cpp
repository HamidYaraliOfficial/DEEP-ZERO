#include "DZOperationalSystems.h"
#include "Kismet/KismetMathLibrary.h"

UDZPowerPolicyComponent::UDZPowerPolicyComponent(){PrimaryComponentTick.bCanEverTick=false;}
void UDZPowerPolicyComponent::ApplyEmergencyPolicy(){if(PolicyBuses.IsEmpty()){for(FName N:{FName(TEXT("LifeSupport")),FName(TEXT("Emergency")),FName(TEXT("Pumps")),FName(TEXT("Navigation")),FName(TEXT("Communications")),FName(TEXT("Sonar")),FName(TEXT("Sensors")),FName(TEXT("Engines")),FName(TEXT("Lights")),FName(TEXT("Laboratory"))}){FDZPowerBus&B=PolicyBuses.AddDefaulted_GetRef();B.Name=N;B.Allocation=.1f;B.Online=true;}}for(FDZPowerBus&B:PolicyBuses){B.Online=B.Name==TEXT("LifeSupport")||B.Name==TEXT("Emergency")||B.Name==TEXT("Pumps")||B.Name==TEXT("Navigation")||B.Name==TEXT("Communications");B.Allocation=B.Online?.2f:0.f;}Normalize();}
void UDZPowerPolicyComponent::ApplyExplorationPolicy(){if(PolicyBuses.IsEmpty())ApplyEmergencyPolicy();for(FDZPowerBus&B:PolicyBuses){B.Online=true;if(B.Name==TEXT("Engines"))B.Allocation=.28f;else if(B.Name==TEXT("LifeSupport"))B.Allocation=.15f;else if(B.Name==TEXT("Sonar"))B.Allocation=.14f;else if(B.Name==TEXT("Navigation"))B.Allocation=.08f;else if(B.Name==TEXT("Sensors"))B.Allocation=.08f;else if(B.Name==TEXT("Lights"))B.Allocation=.08f;else if(B.Name==TEXT("Pumps"))B.Allocation=.06f;else if(B.Name==TEXT("Communications"))B.Allocation=.05f;else if(B.Name==TEXT("Emergency"))B.Allocation=.05f;else B.Allocation=.03f;}Normalize();}
void UDZPowerPolicyComponent::ApplyResearchPolicy(){if(PolicyBuses.IsEmpty())ApplyEmergencyPolicy();for(FDZPowerBus&B:PolicyBuses){B.Online=true;if(B.Name==TEXT("Laboratory"))B.Allocation=.25f;else if(B.Name==TEXT("LifeSupport"))B.Allocation=.16f;else if(B.Name==TEXT("Navigation"))B.Allocation=.08f;else if(B.Name==TEXT("Sensors"))B.Allocation=.1f;else if(B.Name==TEXT("Sonar"))B.Allocation=.12f;else if(B.Name==TEXT("Communications"))B.Allocation=.08f;else if(B.Name==TEXT("Pumps"))B.Allocation=.06f;else if(B.Name==TEXT("Engines"))B.Allocation=.06f;else if(B.Name==TEXT("Emergency"))B.Allocation=.06f;else B.Allocation=.03f;}Normalize();}
void UDZPowerPolicyComponent::SetBus(FName Name,float InAllocation,bool Online){if(FDZPowerBus*B=PolicyBuses.FindByPredicate([&](FDZPowerBus&X){return X.Name==Name;})){B->Allocation=FMath::Clamp(InAllocation,0.f,1.f);B->Online=Online;}else{FDZPowerBus&B=PolicyBuses.AddDefaulted_GetRef();B.Name=Name;B.Allocation=FMath::Clamp(InAllocation,0.f,1.f);B.Online=Online;}Normalize();}
void UDZPowerPolicyComponent::Normalize(){TotalAllocation=0;for(FDZPowerBus&B:PolicyBuses){B.Allocation=FMath::Clamp(B.Allocation,0.f,1.f);if(!B.Online)B.Allocation=0;TotalAllocation+=B.Allocation;}if(TotalAllocation>1){const float Scale=1.f/TotalAllocation;for(FDZPowerBus&B:PolicyBuses)B.Allocation*=Scale;TotalAllocation=1;}}
float UDZPowerPolicyComponent::Allocation(FName Name)const{if(const FDZPowerBus*B=PolicyBuses.FindByPredicate([&](const FDZPowerBus&X){return X.Name==Name;}))return B->Allocation;return 0;}

void UDZAnomalyRegistryComponent::RegisterObservation(const FDZAnomalyRecord&Record){if(Record.Id.IsNone())return;if(FDZAnomalyRecord*Existing=Records.FindByPredicate([&](FDZAnomalyRecord&R){return R.Id==Record.Id;})){Existing->Location=FMath::Lerp(Existing->Location,Record.Location,.25f);Existing->Confidence=FMath::Max(Existing->Confidence,Record.Confidence);Existing->Severity=FMath::Max(Existing->Severity,Record.Severity);Existing->Confirmed=Existing->Confirmed||Record.Confirmed;for(const FString&E:Record.Evidence)Existing->Evidence.AddUnique(E);}else Records.Add(Record);}
void UDZAnomalyRegistryComponent::AddEvidence(FName Id,const FString&Evidence){if(FDZAnomalyRecord*R=Records.FindByPredicate([&](FDZAnomalyRecord&X){return X.Id==Id;}))R->Evidence.AddUnique(Evidence);}
void UDZAnomalyRegistryComponent::Confirm(FName Id,float AdditionalConfidence){if(FDZAnomalyRecord*R=Records.FindByPredicate([&](FDZAnomalyRecord&X){return X.Id==Id;})){R->Confirmed=true;R->Confidence=FMath::Clamp(R->Confidence+FMath::Abs(AdditionalConfidence),0.f,1.f);}}
int32 UDZAnomalyRegistryComponent::ConfirmedCount()const{return Records.FilterByPredicate([](const FDZAnomalyRecord&R){return R.Confirmed;}).Num();}
float UDZAnomalyRegistryComponent::MeanSeverity()const{if(Records.IsEmpty())return 0;float S=0;for(const FDZAnomalyRecord&R:Records)S+=R.Severity;return S/Records.Num();}

FDZRiskPrediction UDZRiskPredictorComponent::Predict(float DistanceKm,float AverageSpeedKnots,float DepthMeters,float CurrentStrength,float HullIntegrity,float BatteryRatio,float OxygenRatio,float CrewEfficiency)const
{
    FDZRiskPrediction R;
    const float SpeedKmH=FMath::Max(.1f,AverageSpeedKnots*1.852f); R.EstimatedDurationHours=FMath::Max(.25f,DistanceKm/SpeedKmH);
    const float DepthFactor=FMath::Clamp(DepthMeters/9000.f,0.f,1.2f); const float CurrentFactor=FMath::Clamp(CurrentStrength,0.f,1.f);
    R.EnvironmentalRisk=FMath::Clamp(DepthFactor*.55f+CurrentFactor*.35f+(1-BatteryRatio)*.1f,0.f,1.f);
    R.MechanicalRisk=FMath::Clamp((1-HullIntegrity)*.65f+(1-CrewEfficiency)*.2f+(1-BatteryRatio)*.15f,0.f,1.f);
    R.EstimatedBatteryUse=FMath::Clamp(R.EstimatedDurationHours*.12f*(1+DepthFactor*.8f+CurrentFactor*.5f),0.f,1.f);
    R.EstimatedOxygenUse=FMath::Clamp(R.EstimatedDurationHours*.018f/(FMath::Max(.2f,CrewEfficiency)),0.f,1.f);
    R.OverallRisk=FMath::Clamp(R.EnvironmentalRisk*.55f+R.MechanicalRisk*.45f,0.f,1.f);
    if(OxygenRatio<.2f)R.OverallRisk=FMath::Clamp(R.OverallRisk+.25f,0.f,1.f);
    R.Summary=FString::Printf(TEXT("%.1fh | Battery %.0f%% | O2 %.0f%% | ENV %.0f%% | MECH %.0f%% | RISK %.0f%%"),R.EstimatedDurationHours,R.EstimatedBatteryUse*100,R.EstimatedOxygenUse*100,R.EnvironmentalRisk*100,R.MechanicalRisk*100,R.OverallRisk*100);
    return R;
}
FDZRiskPrediction UDZRiskPredictorComponent::PredictFromMission(const FDZMission&Mission,float DistanceKm,float DepthMeters,float HullIntegrity,float BatteryRatio,float OxygenRatio)const{const float CrewEfficiency=FMath::Clamp(1-Mission.Risk*.35f,.35f,1.f);FDZRiskPrediction R=Predict(DistanceKm,12,DepthMeters,.5f,HullIntegrity,BatteryRatio,OxygenRatio,CrewEfficiency);R.OverallRisk=FMath::Clamp(R.OverallRisk+Mission.Risk*.35f,0.f,1.f);R.Summary+=FString::Printf(TEXT(" | MISSION %.0f%%"),Mission.Risk*100);return R;}

UDZWorldStateComponent::UDZWorldStateComponent(){PrimaryComponentTick.bCanEverTick=false;}
void UDZWorldStateComponent::Discover(FName Location){if(!Location.IsNone())DiscoveredLocations.Add(Location);}
void UDZWorldStateComponent::SetFlag(FName Flag,bool Enabled){if(Flag.IsNone())return;if(Enabled)PersistentFlags.Add(Flag);else PersistentFlags.Remove(Flag);}
void UDZWorldStateComponent::SetValue(FName Key,float Value){if(!Key.IsNone())StateValues.FindOrAdd(Key)=Value;}
bool UDZWorldStateComponent::IsDiscovered(FName Location)const{return DiscoveredLocations.Contains(Location);}
bool UDZWorldStateComponent::HasFlag(FName Flag)const{return PersistentFlags.Contains(Flag);}
float UDZWorldStateComponent::GetValue(FName Key,float DefaultValue)const{if(const float*V=StateValues.Find(Key))return *V;return DefaultValue;}
void UDZWorldStateComponent::ModifyValue(FName Key,float Delta){SetValue(Key,GetValue(Key)+Delta);}
void UDZWorldStateComponent::ClearRuntimeValues(){StateValues.Empty();}
