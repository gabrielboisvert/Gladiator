// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Enemy.h"
#include "BT_Attack.generated.h"

/**
 * 
 */
UCLASS()
class GLADIATOR_API UBT_Attack : public UBTTaskNode
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, Category = Blackboard)
	FBlackboardKeySelector actor;

	UPROPERTY(EditAnywhere, Category = Blackboard)
	FBlackboardKeySelector bIsAttacking;

	AEnemy* enemy;
	bool bHasBegin = false;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

public:
	UBT_Attack(const FObjectInitializer& ObjectInitializer);
};
