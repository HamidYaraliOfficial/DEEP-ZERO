#pragma once
#include "CoreMinimal.h"
class IDZSimulationContract
{
public:
 virtual ~IDZSimulationContract()=default;
 virtual void Step(float DeltaSeconds)=0;
 virtual void Reset()=0;
 virtual bool Healthy()const=0;
};
