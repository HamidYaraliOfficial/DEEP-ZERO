#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "DZSubmarine.generated.h"
class UCameraComponent; class USpringArmComponent; class UStaticMeshComponent;
class UDZOceanSimulationComponent; class UDZSubmarineSimulationComponent; class UDZFloodingComponent; class UDZSonarComponent; class UDZCrewComponent;
UCLASS()
class DEEPZERO_API ADZSubmarine:public APawn
{
 GENERATED_BODY()
public:
 ADZSubmarine();
 UPROPERTY(VisibleAnywhere) USceneComponent* Root;
 UPROPERTY(VisibleAnywhere) UStaticMeshComponent* Hull;
 UPROPERTY(VisibleAnywhere) UCameraComponent* Camera;
 UPROPERTY(VisibleAnywhere) USpringArmComponent* CameraArm;
 UPROPERTY(VisibleAnywhere) UDZOceanSimulationComponent* Ocean;
 UPROPERTY(VisibleAnywhere) UDZSubmarineSimulationComponent* Systems;
 UPROPERTY(VisibleAnywhere) UDZFloodingComponent* Flooding;
 UPROPERTY(VisibleAnywhere) UDZSonarComponent* Sonar;
 UPROPERTY(VisibleAnywhere) UDZCrewComponent* Crew;
 UPROPERTY(EditAnywhere,BlueprintReadWrite) float MaxForwardSpeed=1500;
 UPROPERTY(EditAnywhere,BlueprintReadWrite) float MaxVerticalSpeed=450;
 UFUNCTION(BlueprintCallable) void MoveForward(float Value);
 UFUNCTION(BlueprintCallable) void MoveVertical(float Value);
 UFUNCTION(BlueprintCallable) void SetDepth(float DepthMeters);
 UFUNCTION(BlueprintPure) float DepthMeters()const;
protected:
 virtual void Tick(float DeltaSeconds)override;
 virtual void SetupPlayerInputComponent(UInputComponent*InputComponent)override;
private:
 float Forward=0; float Vertical=0; void BuildHull();
};
