// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlueprintBase.h"
#include "BT_SetPosition.generated.h"

/**
 * 
 */
UCLASS()
class GLADIATOR_API UBT_SetPosition : public UBTService_BlueprintBase
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, Category = Blackboard)
	FBlackboardKeySelector actor;

	UPROPERTY(EditAnywhere, Category = Blackboard)
	FBlackboardKeySelector vector;

	UBT_SetPosition(const FObjectInitializer& ObjectInitializer);

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
