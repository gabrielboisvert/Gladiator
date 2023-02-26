// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "CheckBool.generated.h"

/**
 * 
 */
UCLASS()
class GLADIATOR_API UCheckBool : public UBTDecorator
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, Category = Blackboard)
	FBlackboardKeySelector bValue;

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
