#include "DZSubmarine.h"
#include "DZCoreSimulation.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
ADZSubmarine::ADZSubmarine(){
 PrimaryActorTick.bCanEverTick=true;Root=CreateDefaultSubobject<USceneComponent>(TEXT("Root"));RootComponent=Root;
 Hull=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Hull"));Hull->SetupAttachment(Root);Hull->SetCollisionProfileName(TEXT("Pawn"));
 CameraArm=CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraArm"));CameraArm->SetupAttachment(Root);CameraArm->TargetArmLength=0;CameraArm->bUsePawnControlRotation=true;
 Camera=CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));Camera->SetupAttachment(CameraArm);Camera->FieldOfView=92;
 Ocean=CreateDefaultSubobject<UDZOceanSimulationComponent>(TEXT("Ocean"));Systems=CreateDefaultSubobject<UDZSubmarineSimulationComponent>(TEXT("Systems"));Flooding=CreateDefaultSubobject<UDZFloodingComponent>(TEXT("Flooding"));Sonar=CreateDefaultSubobject<UDZSonarComponent>(TEXT("Sonar"));Crew=CreateDefaultSubobject<UDZCrewComponent>(TEXT("Crew"));AutoPossessPlayer=EAutoReceiveInput::Player0;}
void ADZSubmarine::BeginPlay(){Super::BeginPlay();BuildHull();}
void ADZSubmarine::Tick(float Dt){Super::Tick(Dt);if(Ocean&&Systems){Ocean->Resample();Systems->SetOcean(Ocean->Sample);const FVector Current=Ocean->CurrentAtDepth(DepthMeters(),GetWorld()->GetTimeSeconds());const float V=Vertical*MaxVerticalSpeed;const float Speed=FMath::Abs(Forward)*MaxForwardSpeed*Systems->PropulsionEfficiency;AddActorWorldOffset(GetActorForwardVector()*Forward*Speed*Dt+FVector::UpVector*V*Dt+Current*Dt,true);}}
void ADZSubmarine::SetupPlayerInputComponent(UInputComponent*IC){Super::SetupPlayerInputComponent(IC);IC->BindAxis(TEXT("MoveForward"),this,&ADZSubmarine::MoveForward);IC->BindAxis(TEXT("MoveUp"),this,&ADZSubmarine::MoveVertical);IC->BindAxis(TEXT("Turn"),this,[this](float V){AddControllerYawInput(V);});IC->BindAxis(TEXT("LookUp"),this,[this](float V){AddControllerPitchInput(V);});}
void ADZSubmarine::MoveForward(float Value){Forward=FMath::Clamp(Value,-1.f,1.f);if(Systems)Systems->SetThrottle(Forward);}
void ADZSubmarine::MoveVertical(float Value){Vertical=FMath::Clamp(Value,-1.f,1.f);if(Systems)Systems->AdjustBallast(Value*.0025f);}
void ADZSubmarine::SetDepth(float DepthMetersValue){if(Systems)Systems->SetDesiredDepth(DepthMetersValue);}
float ADZSubmarine::DepthMeters()const{return Ocean?Ocean->Sample.DepthMeters:0;}
void ADZSubmarine::BuildHull(){if(!Hull->GetStaticMesh()){static ConstructorHelpers::FObjectFinder<UStaticMesh>Sphere(TEXT("/Engine/BasicShapes/Sphere.Sphere"));if(Sphere.Succeeded()){Hull->SetStaticMesh(Sphere.Object);Hull->SetRelativeScale3D(FVector(3.5,1.8,1.8));}}}
