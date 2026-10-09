#include "DZExtendedSystems.h"
#include "Engine/World.h"

UDZInventoryComponent::UDZInventoryComponent(){PrimaryComponentTick.bCanEverTick=false;}
bool UDZInventoryComponent::AddItem(const FDZInventoryItem&Item,int32 Quantity){if(Quantity<=0)return false;FDZInventoryItem Copy=Item;Copy.Quantity=Quantity;const float AddMass=Item.UnitMassKg*Quantity;const float AddVol=Item.VolumeM3*Quantity;if(CurrentMassKg+AddMass>CapacityMassKg||CurrentVolumeM3+AddVol>CapacityVolumeM3)return false;if(FDZInventoryItem*Existing=Items.FindByPredicate([&](FDZInventoryItem&I){return I.Id==Item.Id;})){Existing->Quantity+=Quantity;Existing->Durability=FMath::Max(Existing->Durability,Item.Durability);}else Items.Add(Copy);RecalculateLoad();return true;}
bool UDZInventoryComponent::RemoveItem(FName Id,int32 Quantity){if(Quantity<=0)return false;if(FDZInventoryItem*Item=Items.FindByPredicate([&](FDZInventoryItem&I){return I.Id==Id;})){if(Item->Quantity<Quantity)return false;Item->Quantity-=Quantity;if(Item->Quantity==0)Items.Remove(*Item);RecalculateLoad();return true;}return false;}
int32 UDZInventoryComponent::GetQuantity(FName Id)const{if(const FDZInventoryItem*Item=Items.FindByPredicate([&](const FDZInventoryItem&I){return I.Id==Id;}))return Item->Quantity;return 0;}
bool UDZInventoryComponent::Consume(FName Id,int32 Quantity){return RemoveItem(Id,Quantity);}
float UDZInventoryComponent::MassRatio()const{return FMath::Clamp(CurrentMassKg/FMath::Max(1.f,CapacityMassKg),0.f,1.f);}
float UDZInventoryComponent::VolumeRatio()const{return FMath::Clamp(CurrentVolumeM3/FMath::Max(.001f,CapacityVolumeM3),0.f,1.f);}
void UDZInventoryComponent::RecalculateLoad(){CurrentMassKg=0;CurrentVolumeM3=0;for(const FDZInventoryItem&I:Items){CurrentMassKg+=I.UnitMassKg*I.Quantity;CurrentVolumeM3+=I.VolumeM3*I.Quantity;}}

UDZNavigationComponent::UDZNavigationComponent(){PrimaryComponentTick.bCanEverTick=true;PrimaryComponentTick.TickInterval=.2f;}
void UDZNavigationComponent::AddPoint(const FDZNavPoint&Point){if(Point.Id.IsNone())return;Points.RemoveAll([&](const FDZNavPoint&P){return P.Id==Point.Id;});Points.Add(Point);MapConfidence=FMath::Clamp(MapConfidence+.05f,0.f,1.f);}
void UDZNavigationComponent::UpdateDrift(float DeltaSeconds,float CurrentStrength,float Noise){const float DriftRate=CurrentStrength*(1.f+Noise*.7f);EstimatedDriftMeters+=DriftRate*DeltaSeconds*2.f;GyroHeadingDegrees=FMath::Fmod(GyroHeadingDegrees+Noise*DeltaSeconds*.3f+360.f,360.f);MapConfidence=FMath::Max(0.f,MapConfidence-Noise*DeltaSeconds*.003f);}
void UDZNavigationComponent::CorrectWithReference(FVector ReferenceLocation,float Confidence){if(!GetOwner())return;const float C=FMath::Clamp(Confidence,0.f,1.f);const FVector Error=ReferenceLocation-GetOwner()->GetActorLocation();EstimatedDriftMeters=FMath::Max(0.f,EstimatedDriftMeters*(1.f-C));GetOwner()->SetActorLocation(GetOwner()->GetActorLocation()+Error*C,true);MapConfidence=FMath::Clamp(MapConfidence+C*.25f,0.f,1.f);}
void UDZNavigationComponent::RecordSonarPoint(FVector Location,float Confidence,bool Anomaly){FDZNavPoint P;P.Id=FName(*FString::Printf(TEXT("SONAR_%d"),Points.Num()+1));P.Location=Location;P.EstimatedDepth=GetOwner()?FMath::Max(0.f,-Location.Z/100.f):0;P.Known=Confidence>.5f;P.Anomalous=Anomaly;AddPoint(P);}
FVector UDZNavigationComponent::BestKnownPosition()const{if(!GetOwner())return FVector::ZeroVector;if(Points.IsEmpty())return GetOwner()->GetActorLocation();const FDZNavPoint*Best=&Points[0];float Score=-1;for(const FDZNavPoint&P:Points){const float S=(P.Known?1.f:0.f)+(P.Anomalous?.15f:0.f);if(S>Score){Score=S;Best=&P;}}return Best->Location;}
void UDZNavigationComponent::TickComponent(float Delta,ELevelTick Type,FActorComponentTickFunction*Fn){Super::TickComponent(Delta,Type,Fn);UpdateDrift(Delta,.2f,.1f);}

UDZAtmosphereComponent::UDZAtmosphereComponent(){PrimaryComponentTick.bCanEverTick=true;PrimaryComponentTick.TickInterval=.5f;}
void UDZAtmosphereComponent::AddCrewLoad(int32 CrewCount){Crew=FMath::Clamp(CrewCount,0,64);}
void UDZAtmosphereComponent::AddSmoke(float Amount){Smoke=FMath::Clamp(Smoke+FMath::Abs(Amount),0.f,1.f);}
void UDZAtmosphereComponent::SetVentilation(float Efficiency){Ventilation=FMath::Clamp(Efficiency,0.f,1.f);}
void UDZAtmosphereComponent::SetScrubber(float Efficiency){Scrubber=FMath::Clamp(Efficiency,0.f,1.f);}
bool UDZAtmosphereComponent::IsCritical()const{return OxygenPercent<16.f||CO2Percent>2.5f||PressureKPa<55.f||TemperatureC>60.f||Smoke>.8f;}
void UDZAtmosphereComponent::TickComponent(float Delta,ELevelTick Type,FActorComponentTickFunction*Fn){Super::TickComponent(Delta,Type,Fn);const float CrewLoad=float(Crew);OxygenPercent=FMath::Max(0.f,OxygenPercent-CrewLoad*.00002f*Delta);CO2Percent+=CrewLoad*.000015f*Delta;CO2Percent=FMath::Max(.04f,CO2Percent-Scrubber*.0015f*Delta);OxygenPercent=FMath::Min(20.95f,OxygenPercent+Ventilation*.0006f*Delta);Smoke=FMath::Max(0.f,Smoke-Ventilation*.02f*Delta);Humidity=FMath::Clamp(Humidity+(CrewLoad*.0002f-Smoke*.0001f)*Delta,0.f,1.f);TemperatureC+=CrewLoad*.002f*Delta-Smoke*.001f*Delta;Breathable=OxygenPercent>=16&&CO2Percent<=2.5f&&Smoke<.6f;FireDanger=OxygenPercent>19&&TemperatureC>45;}

void UDZResearchCaptureComponent::Capture(const FDZResearchCapture&Data){if(Data.Id.IsNone())return;Captures.RemoveAll([&](const FDZResearchCapture&C){return C.Id==Data.Id;});Captures.Add(Data);}
bool UDZResearchCaptureComponent::AddNote(FName Id,const FString&Note){if(FDZResearchCapture*C=Captures.FindByPredicate([&](FDZResearchCapture&C){return C.Id==Id;})){C->Notes=Note;return true;}return false;}
float UDZResearchCaptureComponent::AverageQuality()const{if(Captures.IsEmpty())return 0;float Sum=0;for(const FDZResearchCapture&C:Captures)Sum+=C.Quality;return Sum/Captures.Num();}

FString UDZTelemetryFormatter::SystemStateText(EDZSystemState State){switch(State){case EDZSystemState::Offline:return TEXT("OFFLINE");case EDZSystemState::Standby:return TEXT("STANDBY");case EDZSystemState::Nominal:return TEXT("NOMINAL");case EDZSystemState::Degraded:return TEXT("DEGRADED");case EDZSystemState::Failed:return TEXT("FAILED");default:return TEXT("EMERGENCY");}}
FString UDZTelemetryFormatter::SeverityText(EDZAlarmSeverity Severity){switch(Severity){case EDZAlarmSeverity::Info:return TEXT("INFO");case EDZAlarmSeverity::Advisory:return TEXT("ADVISORY");case EDZAlarmSeverity::Warning:return TEXT("WARNING");case EDZAlarmSeverity::Critical:return TEXT("CRITICAL");default:return TEXT("EMERGENCY");}}
FString UDZTelemetryFormatter::BuildDashboard(const TArray<FDZTelemetry>&Telemetry,const TArray<FDZAlarm>&Alarms){TArray<FString>Lines;Lines.Add(TEXT("=== DEEP ZERO TELEMETRY ==="));for(const FDZTelemetry&T:Telemetry){Lines.Add(FString::Printf(TEXT("%-16s %9s HP=%6.1f%% EFF=%6.1f%% LOAD=%6.1f%% | %s"),*T.Name.ToString(),*SystemStateText(T.State),T.Health*100,T.Efficiency*100,T.Load*100,*T.Diagnostic));}Lines.Add(FString::Printf(TEXT("ALARMS: %d"),Alarms.Num()));for(const FDZAlarm&A:Alarms)if(!A.Acknowledged)Lines.Add(FString::Printf(TEXT("[%s] %s: %s"),*SeverityText(A.Severity),*A.Id.ToString(),*A.Message));return FString::Join(Lines,TEXT("\n"));}
FString UDZTelemetryFormatter::FormatPercent(float Ratio){return FString::Printf(TEXT("%.1f%%"),FMath::Clamp(Ratio,0.f,1.f)*100.f);}
FString UDZTelemetryFormatter::FormatDepth(float Meters){return FString::Printf(TEXT("%.0f m"),Meters);}
FString UDZTelemetryFormatter::FormatPressure(float KPa){return FString::Printf(TEXT("%.1f kPa"),KPa);}
