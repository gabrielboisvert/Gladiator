// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BT_InRange.generated.h"

/**
 *
 */
UCLASS()
class GLADIATOR_API UBT_InRange : public UBTDecorator
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, Category = Blackboard)
		FBlackboardKeySelector MaxRange;

	UPROPERTY(EditAnywhere, Category = Blackboard)
		FBlackboardKeySelector Distance;

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};