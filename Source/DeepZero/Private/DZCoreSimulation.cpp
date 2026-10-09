#include "DZCoreSimulation.h"
#include "DeepZero.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/SecureHash.h"
#include "GameFramework/Pawn.h"

UDZOceanSimulationComponent::UDZOceanSimulationComponent(){PrimaryComponentTick.bCanEverTick=true;PrimaryComponentTick.TickInterval=.1f;}
void UDZOceanSimulationComponent::BeginPlay(){Super::BeginPlay();Resample();}
FDZOceanSample UDZOceanSimulationComponent::SampleAtDepth(float DepthMeters)const
{
    const float D=FMath::Max(0.f,DepthMeters); FDZOceanSample S; S.DepthMeters=D;
    S.PressureKPa=101.325f+D*PressurePerMeterKPa; S.TemperatureC=Temperature(D);
    S.VisibilityMeters=Visibility(D); S.CurrentMetersPerSecond=.12f+.75f*FMath::Clamp(D/7000.f,0.f,1.f);
    S.SalinityPSU=34.4f+.9f*FMath::Clamp(D/11000.f,0.f,1.f); S.OxygenFraction=.2095f-.04f*FMath::Clamp(D/9000.f,0.f,1.f);
    S.GeologicalActivity=.05f+.35f*FMath::Abs(FMath::Sin(D*.00073f)); S.BiologicalActivity=FMath::Clamp(.9f-D/9000.f,.04f,.9f); return S;
}
FVector UDZOceanSimulationComponent::CurrentAtDepth(float DepthMeters,float TimeSeconds)const
{
    const FDZOceanSample S=SampleAtDepth(DepthMeters); const float P=DepthMeters*.002f+TimeSeconds*.12f;
    return FVector(FMath::Cos(P),FMath::Sin(P*.71f),0)*S.CurrentMetersPerSecond*100.f;
}
void UDZOceanSimulationComponent::Resample(){if(!GetOwner())return; const float D=FMath::Max(0.f,(SeaLevelCm-GetOwner()->GetActorLocation().Z)/100.f); Sample=SampleAtDepth(D);}
float UDZOceanSimulationComponent::Temperature(float D)const{const float A=FMath::Clamp(D/3000.f,0.f,1.f);return FMath::Lerp(SurfaceTempC,DeepTempC,A)+3.f*FMath::Max(0.f,FMath::Sin(D*.0011f));}
float UDZOceanSimulationComponent::Visibility(float D)const{return FMath::Lerp(SurfaceVisibility,DeepVisibility,FMath::Clamp(D/7000.f,0.f,1.f));}

UDZSubmarineSimulationComponent::UDZSubmarineSimulationComponent(){PrimaryComponentTick.bCanEverTick=true;PrimaryComponentTick.TickInterval=.1f;}
void UDZSubmarineSimulationComponent::BeginPlay(){Super::BeginPlay();ChargeMWh=BatteryMWh;OxygenKg=MaxOxygenKg;InitBuses();InitTelemetry();}
void UDZSubmarineSimulationComponent::InitBuses(){Buses.Empty();for(FName N:{FName(TEXT("Engines")),FName(TEXT("Sonar")),FName(TEXT("Lights")),FName(TEXT("LifeSupport")),FName(TEXT("Laboratory")),FName(TEXT("Communications")),FName(TEXT("Sensors")),FName(TEXT("Pumps")),FName(TEXT("Navigation")),FName(TEXT("Emergency"))}){FDZPowerBus&B=Buses.AddDefaulted_GetRef();B.Name=N;B.Allocation=.1f;B.Online=true;}}
void UDZSubmarineSimulationComponent::InitTelemetry(){Telemetry.Empty();for(FName N:{FName(TEXT("Reactor")),FName(TEXT("Battery")),FName(TEXT("Propulsion")),FName(TEXT("Ballast")),FName(TEXT("LifeSupport")),FName(TEXT("Cooling")),FName(TEXT("Hull")),FName(TEXT("Flooding")),FName(TEXT("Navigation")),FName(TEXT("Sonar")),FName(TEXT("Communications"))}){FDZTelemetry&T=Telemetry.AddDefaulted_GetRef();T.Name=N;T.State=EDZSystemState::Nominal;}}
void UDZSubmarineSimulationComponent::SetOcean(const FDZOceanSample& InOcean){Ocean=InOcean;}
void UDZSubmarineSimulationComponent::SetThrottle(float Value){Throttle=FMath::Clamp(Value,-1.f,1.f);}
void UDZSubmarineSimulationComponent::SetDesiredDepth(float Depth){DesiredDepth=FMath::Clamp(Depth,0.f,MaxDepthMeters*1.2f);}
void UDZSubmarineSimulationComponent::AdjustBallast(float Delta){Ballast=FMath::Clamp(Ballast+Delta,0.f,1.f);}
void UDZSubmarineSimulationComponent::SetCooling(float Efficiency){CoolingEfficiency=FMath::Clamp(Efficiency,0.f,1.f);}
void UDZSubmarineSimulationComponent::TickComponent(float Dt,ELevelTick Type,FActorComponentTickFunction* Fn){Super::TickComponent(Dt,Type,Fn);SimPower(Dt);SimAtmosphere(Dt);SimThermal(Dt);SimHull(Dt);SimFlooding(Dt);SimPropulsion(Dt);UpdateAlarms();}
void UDZSubmarineSimulationComponent::SimPower(float Dt)
{
    float Demand=0;for(FDZPowerBus&B:Buses){B.Demand=B.Online?B.Allocation*(.25f+FMath::Abs(Throttle)*.75f):0;Demand+=B.Demand;}
    ReactorLoad=FMath::Clamp(Demand/3.f,0.f,1.2f);const float Net=(ReactorMW*FMath::Min(ReactorLoad,1.f)-Demand*ReactorMW*.9f)*Dt/3600.f;ChargeMWh=FMath::Clamp(ChargeMWh+Net,0.f,BatteryMWh);
    if(FDZTelemetry*T=FindTelemetry(TEXT("Battery"))){T->Load=Demand;T->Health=FMath::Clamp(ChargeMWh/BatteryMWh,0.f,1.f);T->Efficiency=T->Health;T->State=T->Health<.08f?EDZSystemState::Emergency:EDZSystemState::Nominal;OnTelemetry.Broadcast(*T);}
}
void UDZSubmarineSimulationComponent::SimAtmosphere(float Dt)
{
    const bool Online=FindBus(TEXT("LifeSupport"))&&FindBus(TEXT("LifeSupport"))->Online;const float Crew=FMath::Max(1.f,4.f);
    OxygenKg=FMath::Max(0.f,OxygenKg-.00011f*Crew*Dt*(1.f+FMath::Abs(Throttle)*.25f));CO2Percent+=.00005f*Crew*Dt;
    if(Online){OxygenKg=FMath::Min(MaxOxygenKg,OxygenKg+.00003f*Dt);CO2Percent=FMath::Max(.04f,CO2Percent-.009f*Dt);}
    if(FDZTelemetry*T=FindTelemetry(TEXT("LifeSupport"))){T->Health=OxygenKg/MaxOxygenKg;T->Efficiency=Online?1.f:.25f;T->State=T->Health<.1f?EDZSystemState::Emergency:EDZSystemState::Nominal;OnTelemetry.Broadcast(*T);}
}
void UDZSubmarineSimulationComponent::SimThermal(float Dt){const float Heat=ReactorLoad*.8f;const float Cooling=(InternalTempC-Ocean.TemperatureC)*CoolingEfficiency*.035f;InternalTempC+=(Heat-Cooling)*Dt;if(FDZTelemetry*T=FindTelemetry(TEXT("Cooling"))){T->Efficiency=CoolingEfficiency;T->Health=CoolingEfficiency;T->State=CoolingEfficiency<.3f?EDZSystemState::Emergency:EDZSystemState::Nominal;}}
void UDZSubmarineSimulationComponent::SimHull(float Dt){const float Excess=FMath::Max(0.f,Ocean.DepthMeters-MaxDepthMeters*.9f);HullIntegrity=FMath::Clamp(HullIntegrity-Excess/MaxDepthMeters*.00015f*Dt,0.f,1.f);if(FDZTelemetry*T=FindTelemetry(TEXT("Hull"))){T->Health=HullIntegrity;T->Efficiency=1-StructuralRisk();T->State=HullIntegrity<.1f?EDZSystemState::Emergency:HullIntegrity<.35f?EDZSystemState::Degraded:EDZSystemState::Nominal;}}
void UDZSubmarineSimulationComponent::SimFlooding(float Dt){if(FloodingRateLps<=0)return;WaterMassKg+=FloodingRateLps*Dt;const bool Pump=FindBus(TEXT("Pumps"))&&FindBus(TEXT("Pumps"))->Online;if(Pump)WaterMassKg=FMath::Max(0.f,WaterMassKg-30.f*Dt);HullIntegrity=FMath::Clamp(HullIntegrity-FloodingRateLps*.000002f*Dt,0.f,1.f);if(FDZTelemetry*T=FindTelemetry(TEXT("Flooding"))){T->Load=FloodingRateLps;T->Health=HullIntegrity;T->State=EDZSystemState::Emergency;}}
void UDZSubmarineSimulationComponent::SimPropulsion(float Dt){const bool Engine=FindBus(TEXT("Engines"))&&FindBus(TEXT("Engines"))->Online;const float Eff=FMath::Clamp(Engine?1.f:.1f,0.f,1.f);if(FDZTelemetry*T=FindTelemetry(TEXT("Propulsion"))){T->Load=FMath::Abs(Throttle);T->Efficiency=Eff;T->Health=Eff;T->State=Eff>.5f?EDZSystemState::Nominal:EDZSystemState::Degraded;}}
void UDZSubmarineSimulationComponent::UpdateAlarms(){auto Has=[this](FName Id){return Alarms.ContainsByPredicate([&](const FDZAlarm&A){return A.Id==Id&&!A.Acknowledged;});};if(Ocean.DepthMeters>MaxDepthMeters&&!Has(TEXT("DEPTH")))Raise(TEXT("DEPTH"),EDZAlarmSeverity::Emergency,TEXT("Depth rating exceeded."));if(OxygenKg<MaxOxygenKg*.18f&&!Has(TEXT("O2")))Raise(TEXT("O2"),EDZAlarmSeverity::Critical,TEXT("Oxygen reserve critically low."));if(InternalTempC>65&&!Has(TEXT("TEMP")))Raise(TEXT("TEMP"),EDZAlarmSeverity::Warning,TEXT("Internal temperature rising."));if(HullIntegrity<.45f&&!Has(TEXT("HULL")))Raise(TEXT("HULL"),EDZAlarmSeverity::Critical,TEXT("Hull integrity degraded."));if(FloodingRateLps>0&&!Has(TEXT("FLOOD")))Raise(TEXT("FLOOD"),EDZAlarmSeverity::Critical,TEXT("Flooding detected."));}
void UDZSubmarineSimulationComponent::Raise(FName Id,EDZAlarmSeverity Severity,const FString& Message){FDZAlarm A;A.Id=Id;A.Severity=Severity;A.Message=Message;Alarms.Add(A);OnAlarm.Broadcast(A);}
void UDZSubmarineSimulationComponent::ApplyDamage(EDZDamageType Type,float Amount,FName System){const float D=FMath::Clamp(Amount,0.f,1.f);if(Type==EDZDamageType::Structural||Type==EDZDamageType::Pressure||Type==EDZDamageType::Flooding)HullIntegrity=FMath::Clamp(HullIntegrity-D,0.f,1.f);if(FDZTelemetry*T=FindTelemetry(System)){T->Health=FMath::Clamp(T->Health-D,0.f,1.f);T->State=T->Health<.05f?EDZSystemState::Failed:T->Health<.35f?EDZSystemState::Emergency:T->Health<.65f?EDZSystemState::Degraded:EDZSystemState::Nominal;T->Diagnostic=FString::Printf(TEXT("Damage %.2f applied"),D);OnTelemetry.Broadcast(*T);}if(Type==EDZDamageType::Mechanical)SetThrottle(Throttle*(1-D*.5f));if(Type==EDZDamageType::Fire)InternalTempC+=D*30;}
void UDZSubmarineSimulationComponent::Repair(FName System,float Amount){if(FDZTelemetry*T=FindTelemetry(System)){T->Health=FMath::Clamp(T->Health+FMath::Abs(Amount),0.f,1.f);T->State=T->Health>.85f?EDZSystemState::Nominal:T->Health>.5f?EDZSystemState::Degraded:EDZSystemState::Emergency;T->Diagnostic=TEXT("Repair completed");}}
void UDZSubmarineSimulationComponent::SetPower(FName Bus,float Allocation,bool Online){if(FDZPowerBus*B=FindBus(Bus)){B->Allocation=FMath::Clamp(Allocation,0.f,1.f);B->Online=Online;}}
void UDZSubmarineSimulationComponent::AckAlarm(FName Id){for(FDZAlarm&A:Alarms)if(A.Id==Id)A.Acknowledged=true;}
float UDZSubmarineSimulationComponent::StructuralRisk()const{const float DepthRatio=Ocean.DepthMeters/FMath::Max(1.f,MaxDepthMeters);return FMath::Clamp(FMath::Max(0.f,(DepthRatio-.8f)/.2f)+1-HullIntegrity,0.f,1.f);}
bool UDZSubmarineSimulationComponent::Emergency()const{return HullIntegrity<.2f||OxygenKg<MaxOxygenKg*.15f||ChargeMWh<BatteryMWh*.1f||InternalTempC>75||Ocean.DepthMeters>MaxDepthMeters;}
float UDZSubmarineSimulationComponent::RangeHours()const{return (ChargeMWh+ReactorMW*2)/FMath::Max(.1f,ReactorMW*(.25f+FMath::Abs(Throttle)*.75f)*.5f);}
FDZTelemetry*UDZSubmarineSimulationComponent::FindTelemetry(FName Name){return Telemetry.FindByPredicate([&](FDZTelemetry&T){return T.Name==Name;});}
FDZPowerBus*UDZSubmarineSimulationComponent::FindBus(FName Name){return Buses.FindByPredicate([&](FDZPowerBus&B){return B.Name==Name;});}

UDZFloodingComponent::UDZFloodingComponent(){PrimaryComponentTick.bCanEverTick=true;PrimaryComponentTick.TickInterval=.2f;}
void UDZFloodingComponent::BeginPlay(){Super::BeginPlay();if(Compartments.IsEmpty()){const TArray<FString>Names={TEXT("Command"),TEXT("Reactor"),TEXT("Engine"),TEXT("Battery"),TEXT("Sonar"),TEXT("Navigation"),TEXT("Communications"),TEXT("Medical"),TEXT("Cargo"),TEXT("Laboratory"),TEXT("Crew"),TEXT("Airlock"),TEXT("Maintenance"),TEXT("Emergency")};for(const FString&N:Names){FDZCompartment&C=Compartments.AddDefaulted_GetRef();C.Name=FName(*N);C.VolumeM3=80;}}}
void UDZFloodingComponent::TickComponent(float Dt,ELevelTick Type,FActorComponentTickFunction*Fn){Super::TickComponent(Dt,Type,Fn);for(FDZCompartment&C:Compartments){if(float*L=Leaks.Find(C.Name))C.WaterLiters+=FMath::Max(0.f,*L)*Dt;if(C.Pump)C.WaterLiters=FMath::Max(0.f,C.WaterLiters-30.f*Dt);if(C.Fire>0){C.Fire=FMath::Max(0.f,C.Fire-(C.Ventilation ? 0.01f : 0.002f)*Dt);C.Smoke=FMath::Min(1.f,C.Smoke+C.Fire*.04f*Dt);C.OxygenPercent=FMath::Max(0.f,C.OxygenPercent-C.Fire*.02f*Dt);}C.CO2Percent=FMath::Max(.04f,C.CO2Percent+(C.Ventilation ? -0.0005f : 0.0001f)*Dt);}TotalWaterLiters=0;for(const FDZCompartment&C:Compartments)TotalWaterLiters+=C.WaterLiters;}
void UDZFloodingComponent::AddLeak(FName Compartment,float LitersPerSecond){if(LitersPerSecond<=0)Leaks.Remove(Compartment);else Leaks.FindOrAdd(Compartment)=LitersPerSecond;}
void UDZFloodingComponent::SetBulkhead(FName Compartment,EDZDoorState State){if(FDZCompartment*C=Find(Compartment))C->Bulkhead=State;}
void UDZFloodingComponent::SetPump(FName Compartment,bool Enabled){if(FDZCompartment*C=Find(Compartment))C->Pump=Enabled&&C->Bulkhead!=EDZDoorState::Damaged;}
void UDZFloodingComponent::SetFire(FName Compartment,float Intensity){if(FDZCompartment*C=Find(Compartment))C->Fire=FMath::Clamp(Intensity,0.f,1.f);}
bool UDZFloodingComponent::Safe(FName Compartment)const{if(const FDZCompartment*C=FindConst(Compartment))return C->WaterLiters<C->VolumeM3*800&&! (C->OxygenPercent<16||C->CO2Percent>2.5f)&&C->Integrity>.5f;return false;}
FDZCompartment*UDZFloodingComponent::Find(FName Name){return Compartments.FindByPredicate([&](FDZCompartment&C){return C.Name==Name;});}
const FDZCompartment*UDZFloodingComponent::FindConst(FName Name)const{return Compartments.FindByPredicate([&](const FDZCompartment&C){return C.Name==Name;});}

UDZSonarComponent::UDZSonarComponent(){PrimaryComponentTick.bCanEverTick=true;PrimaryComponentTick.TickInterval=.1f;}
void UDZSonarComponent::BeginPlay(){Super::BeginPlay();}
void UDZSonarComponent::TickComponent(float Dt,ELevelTick Type,FActorComponentTickFunction*Fn){Super::TickComponent(Dt,Type,Fn);Timer+=Dt;Noise=FMath::Max(0.f,Noise-Dt*.08f);if(Mode==EDZSonarMode::Active&&Timer>=PingInterval){Timer=0;Ping();}for(FDZSonarContact&C:Contacts)C.Confidence=FMath::Max(0.f,C.Confidence-Dt*.003f);}
void UDZSonarComponent::SetMode(EDZSonarMode NewMode){Mode=NewMode;}
void UDZSonarComponent::Ping(){Noise=FMath::Clamp(Noise+.4f,0.f,1.f);if(GetOwner())Detect(GetOwner()->GetActorLocation()+GetOwner()->GetActorForwardVector()*18000.f,.8f,false);}
void UDZSonarComponent::Detect(const FVector&WorldLocation,float Signature,bool Anomaly){if(!GetOwner())return;const FVector R=WorldLocation-GetOwner()->GetActorLocation();const float Range=R.Size()/100.f;if(Range>MaxRangeMeters||FMath::FRand()>Probability(Range,Signature))return;FDZSonarContact C;C.RelativeLocation=R;C.RangeMeters=Range;C.BearingDegrees=FMath::RadiansToDegrees(FMath::Atan2(R.Y,R.X));C.Confidence=Probability(Range,Signature);C.AcousticSignature=Signature;C.Anomalous=Anomaly;Contacts.Add(C);OnContact.Broadcast(C);}
float UDZSonarComponent::Probability(float Range,float Signature)const{const float RangeFactor=1-FMath::Clamp(Range/MaxRangeMeters,0.f,1.f);const float NoisePenalty=FMath::Clamp(1-Noise*.6f,.1f,1.f);const float Bonus=Mode==EDZSonarMode::Active?.2f:0;return FMath::Clamp((.15f+RangeFactor*.8f+Bonus)*Signature*NoisePenalty,0.f,1.f);}

UDZCrewComponent::UDZCrewComponent(){PrimaryComponentTick.bCanEverTick=true;PrimaryComponentTick.TickInterval=.5f;}
void UDZCrewComponent::BeginPlay(){Super::BeginPlay();if(Crew.IsEmpty()){FDZCrewMember&A=Crew.AddDefaulted_GetRef();A.Id=TEXT("captain");A.Name=TEXT("Mara Voss");A.Role=EDZCrewRole::Captain;A.Skill=.92f;FDZCrewMember&B=Crew.AddDefaulted_GetRef();B.Id=TEXT("engineer");B.Name=TEXT("Eli Mercer");B.Role=EDZCrewRole::Engineer;B.Skill=.88f;FDZCrewMember&C=Crew.AddDefaulted_GetRef();C.Id=TEXT("sonar");C.Name=TEXT("Jun Wei");C.Role=EDZCrewRole::SonarOperator;C.Skill=.86f;FDZCrewMember&D=Crew.AddDefaulted_GetRef();D.Id=TEXT("researcher");D.Name=TEXT("Inez Rao");D.Role=EDZCrewRole::Researcher;D.Skill=.83f;}}
void UDZCrewComponent::TickComponent(float Dt,ELevelTick Type,FActorComponentTickFunction*Fn){Super::TickComponent(Dt,Type,Fn);AverageMorale=0;for(FDZCrewMember&M:Crew){if(M.Available&&(M.Task!=EDZCrewTask::Idle&&M.Task!=EDZCrewTask::Rest)){M.Fatigue=FMath::Clamp(M.Fatigue+Dt*.0025f,0.f,1.f);if(M.Fatigue>.9f)M.Task=EDZCrewTask::Rest;}else M.Fatigue=FMath::Max(0.f,M.Fatigue-Dt*.004f);AverageMorale+=M.Morale;}if(!Crew.IsEmpty())AverageMorale/=Crew.Num();}
void UDZCrewComponent::Assign(FName CrewId,EDZCrewTask Task,FName Target){if(FDZCrewMember*M=Find(CrewId)){M->Task=Task;M->Target=Target;OnCrewChanged.Broadcast(*M);}}
void UDZCrewComponent::SetAvailable(FName CrewId,bool Available){if(FDZCrewMember*M=Find(CrewId)){M->Available=Available;if(!Available)M->Task=EDZCrewTask::Idle;}}
void UDZCrewComponent::AddFatigue(FName CrewId,float Amount){if(FDZCrewMember*M=Find(CrewId)){M->Fatigue=FMath::Clamp(M->Fatigue+FMath::Abs(Amount),0.f,1.f);}}
void UDZCrewComponent::AddStress(FName CrewId,float Amount){if(FDZCrewMember*M=Find(CrewId)){M->Morale=FMath::Clamp(M->Morale-FMath::Abs(Amount),0.f,1.f);M->Trust=FMath::Clamp(M->Trust-FMath::Abs(Amount)*.25f,0.f,1.f);}}
float UDZCrewComponent::Efficiency(FName CrewId)const{const FDZCrewMember*M=Crew.FindByPredicate([&](const FDZCrewMember&X){return X.Id==CrewId;});if(!M||!M->Available)return 0;return FMath::Clamp(M->Skill*(1-M->Fatigue*.55f)*(.65f+M->Morale*.35f),0.f,1.f);}
int32 UDZCrewComponent::AvailableCount()const{return Crew.FilterByPredicate([](const FDZCrewMember&M){return M.Available;}).Num();}
FDZCrewMember*UDZCrewComponent::Find(FName Id){return Crew.FindByPredicate([&](FDZCrewMember&M){return M.Id==Id;});}

ADZThreatActor::ADZThreatActor(){PrimaryActorTick.bCanEverTick=true;Collision=CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));RootComponent=Collision;Collision->InitSphereRadius(100);Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);}
void ADZThreatActor::Hear(const FVector&Location,float InStrength){const float R=InStrength*AcousticSensitivity;if(R<.05f)return;LastStimulus=Location;Strength=FMath::Max(Strength,R);Age=0;State=R>.75f?EDZThreatState::Investigating:EDZThreatState::Curious;}
void ADZThreatActor::SeeLight(const FVector&Location,float InStrength){if(InStrength*(.3f+Curiosity*.5f)>.15f){LastStimulus=Location;Age=0;State=EDZThreatState::Curious;}}
float ADZThreatActor::Interest()const{return FMath::Clamp(Strength*(1-FMath::Clamp(Age/MemorySeconds,0.f,1.f))*(.5f+Aggression),0.f,1.f);}
void ADZThreatActor::Tick(float Dt){Super::Tick(Dt);Age+=Dt;Idle+=Dt;if(Age>MemorySeconds)State=EDZThreatState::Dormant;if(Interest()>.85f&&Aggression>.75f)State=EDZThreatState::Hunting;if(State==EDZThreatState::Investigating||State==EDZThreatState::Curious){const FVector Dir=(LastStimulus-GetActorLocation()).GetSafeNormal();AddActorWorldOffset(Dir*80.f*Dt,true);}}

void UDZHorrorDirectorSubsystem::SetContext(float Darkness,float Isolation,float Alarms,float Signal,float Threat){const float Target=FMath::Clamp(Darkness*.22f+Isolation*.18f+Alarms*.18f+Signal*.2f+Threat*.22f,0.f,1.f);Tension=FMath::FInterpTo(Tension,Target,.1f,.35f);CooldownTimer=FMath::Max(0.f,CooldownTimer-.1f);}
bool UDZHorrorDirectorSubsystem::TryEvent(FName Preferred){if(CooldownTimer>0||Tension<.2f||EventsSinceQuiet>4)return false;if(Random.FRand()>Tension*.45f)return false;++EventsSinceQuiet;CooldownTimer=FMath::Lerp(8.f,45.f,1-Tension);return true;}

FDZMission UDZMissionSubsystem::Make(FName Id,const FString&Title,EDZMissionType Type,float Risk,int32 Reward)const{FDZMission M;M.Id=Id;M.Title=Title;M.Type=Type;M.Risk=Risk;M.Reward=Reward;return M;}
void UDZMissionSubsystem::Generate(float Depth,float ResourceRatio,float Anomaly){Available.Empty();if(Depth>2500){FDZMission M=Make(TEXT("deep_research"),TEXT("Deep Layer Survey"),EDZMissionType::Research,.55f,420);M.Objectives.Add({TEXT("scan"),TEXT("Acquire sonar map"),0,1,false,false});M.Objectives.Add({TEXT("sample"),TEXT("Secure one sample"),0,1,true,false});Available.Add(M);}if(ResourceRatio<.35f){FDZMission M=Make(TEXT("resource_recovery"),TEXT("Emergency Resource Recovery"),EDZMissionType::Recovery,.4f,280);M.Objectives.Add({TEXT("recover"),TEXT("Recover marked cache"),0,1,false,false});Available.Add(M);}if(Anomaly>.45f){FDZMission M=Make(TEXT("signal"),TEXT("Signal Investigation"),EDZMissionType::Investigation,.7f,650);M.Objectives.Add({TEXT("record"),TEXT("Record anomalous signal"),0,1,false,false});M.Objectives.Add({TEXT("decode"),TEXT("Decode stable segment"),0,1,true,false});Available.Add(M);}FDZMission S=Make(TEXT("systems"),TEXT("Systems Integrity Sweep"),EDZMissionType::Repair,.2f,160);S.Objectives.Add({TEXT("systems"),TEXT("Complete systems check"),0,3,false,false});Available.Add(S);}
bool UDZMissionSubsystem::Start(FName Id){for(int32 i=Available.Num()-1;i>=0;--i)if(Available[i].Id==Id){Available[i].Active=true;Active.Add(Available[i]);Available.RemoveAt(i);OnMission.Broadcast(Active.Last());return true;}return false;}
bool UDZMissionSubsystem::Advance(FName Id,FName Objective,float Amount){for(FDZMission&M:Active)if(M.Id==Id)for(FDZObjective&O:M.Objectives)if(O.Id==Objective&&!O.Completed){O.Progress=FMath::Min(O.Required,O.Progress+FMath::Max(0.f,Amount));O.Completed=O.Progress>=O.Required;OnMission.Broadcast(M);return true;}return false;}
bool UDZMissionSubsystem::Complete(FName Id){for(int32 i=Active.Num()-1;i>=0;--i)if(Active[i].Id==Id){bool Required=true;for(const FDZObjective&O:Active[i].Objectives)if(!O.Optional&&!O.Completed)Required=false;if(!Required)return false;Active[i].Completed=true;OnMission.Broadcast(Active[i]);Active.RemoveAt(i);return true;}return false;}

void UDZScheduleSubsystem::AddWindow(const FDZScheduleWindow&Window){FDZScheduleWindow W=Window;W.StartMinute=((W.StartMinute%1440)+1440)%1440;W.DurationMinutes=FMath::Clamp(W.DurationMinutes,1,1440);Windows.RemoveAll([&](const FDZScheduleWindow&X){return X.Name==W.Name;});Windows.Add(W);}
void UDZScheduleSubsystem::RemoveWindow(const FString&Name){Windows.RemoveAll([&](const FDZScheduleWindow&X){return X.Name==Name;});}
void UDZScheduleSubsystem::Replace(const TArray<FDZScheduleWindow>&NewWindows){Windows.Empty();for(const FDZScheduleWindow&W:NewWindows)AddWindow(W);}
FDZScheduleResult UDZScheduleSubsystem::Query(int32 MinuteOfDay)const{const int32 Now=((MinuteOfDay%1440)+1440)%1440;FDZScheduleResult R;int32 Best=MAX_int32;for(const FDZScheduleWindow&W:Windows){if(!W.Enabled)continue;const int32 S=W.StartMinute;const int32 E=(S+W.DurationMinutes)%1440;const bool Open=W.DurationMinutes>=1440||(S+W.DurationMinutes<=1440?Now>=S&&Now<S+W.DurationMinutes:Now>=S||Now<E);if(Open){R.Open=true;R.CurrentWindowRemaining=(S+W.DurationMinutes<=1440?S+W.DurationMinutes-Now:(Now>=S?(S+W.DurationMinutes)-Now:W.DurationMinutes-Now));R.NextWindowDuration=W.DurationMinutes;R.Summary=W.Name+TEXT(": OPEN for ")+FormatMinutes(R.CurrentWindowRemaining);return R;}int32 Wait=S-Now;if(Wait<0)Wait+=1440;if(Wait<Best){Best=Wait;R.NextWindowDuration=W.DurationMinutes;}}R.MinutesUntilNext=Best==MAX_int32?-1:Best;R.Summary=R.MinutesUntilNext<0?TEXT("No enabled window."):TEXT("NEXT IN ")+FormatMinutes(R.MinutesUntilNext)+TEXT(" | OPEN FOR ")+FormatMinutes(R.NextWindowDuration);return R;}
FString UDZScheduleSubsystem::FormatMinutes(int32 Minutes)const{const int32 H=FMath::Max(0,Minutes)/60;const int32 M=FMath::Max(0,Minutes)%60;return H?FString::Printf(TEXT("%dh %02dm"),H,M):FString::Printf(TEXT("%dm"),M);}

void UDZLocalizationSubsystem::Initialize(FSubsystemCollectionBase&Collection){Super::Initialize(Collection);auto Add=[this](FName K,const FString&E,const FString&F,const FString&Z){Dict.Add(K,{FText::FromString(E),FText::FromString(F),FText::FromString(Z)});};Add(TEXT("CONTINUE"),TEXT("Continue"),TEXT("ادامه"),TEXT("继续"));Add(TEXT("NEW"),TEXT("New Expedition"),TEXT("اکسپدیشن جدید"),TEXT("新的远征"));Add(TEXT("SETTINGS"),TEXT("Settings"),TEXT("تنظیمات"),TEXT("设置"));Add(TEXT("DEPTH"),TEXT("Depth"),TEXT("عمق"),TEXT("深度"));Add(TEXT("PRESSURE"),TEXT("Pressure"),TEXT("فشار"),TEXT("压力"));Add(TEXT("OXYGEN"),TEXT("Oxygen"),TEXT("اکسیژن"),TEXT("氧气"));Add(TEXT("BATTERY"),TEXT("Battery"),TEXT("باتری"),TEXT("电池"));Add(TEXT("SONAR"),TEXT("Sonar"),TEXT("سونار"),TEXT("声呐"));}
FText UDZLocalizationSubsystem::Text(FName Key)const{const TArray<FText>*V=Dict.Find(Key);return(!V||V->Num()<3)?FText::FromName(Key):(*V)[static_cast<uint8>(Language)];}

bool UDZSaveGame::Migrate(){while(Version<CurrentVersion){switch(Version){case 1:DesiredDepth=FMath::Max(DesiredDepth,100.f);Version=2;break;case 2:Battery=FMath::Max(Battery,1.f);Version=3;break;case 3:Version=4;break;default:return false;}}return Version==CurrentVersion;}
FString UDZSaveGame::IntegrityToken()const{FString P=FString::Printf(TEXT("%d|%s|%.3f|%.3f|%.3f|%.3f"),Version,*SlotGuid,Hull,Battery,Oxygen,DesiredDepth);FSHAHash Hash;FTCHARToUTF8 C(*P);FSHA1::HashBuffer(C.Get(),C.Length(),Hash.Hash);return Hash.ToString();}
bool UDZSaveSubsystem::Save(int32 Slot,UDZSaveGame*Data){if(!Data||Slot<0)return false;Data->Version=UDZSaveGame::CurrentVersion;Data->SavedAt=FDateTime::UtcNow();return UGameplayStatics::SaveGameToSlot(Data,SlotName(Slot),0);}
UDZSaveGame*UDZSaveSubsystem::Load(int32 Slot){if(Slot<0)return nullptr;UDZSaveGame*Data=Cast<UDZSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName(Slot),0));if(Data&&!Data->Migrate())return nullptr;return Data;}
bool UDZSaveSubsystem::Remove(int32 Slot){return Slot>=0&&UGameplayStatics::DeleteGameInSlot(SlotName(Slot),0);}
bool UDZSaveSubsystem::Exists(int32 Slot)const{return Slot>=0&&UGameplayStatics::DoesSaveGameExist(SlotName(Slot),0);}
