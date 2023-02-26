// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlueprintBase.h"
#include "ServiceCalculateDistance.generated.h"

/**
 * 
 */
UCLASS()
class GLADIATOR_API UServiceCalculateDistance : public UBTService_BlueprintBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Data", meta = (AllowPrivateAccess = "true"))
	FBlackboardKeySelector player;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Data", meta = (AllowPrivateAccess = "true"))
	FBlackboardKeySelector ai;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Data", meta = (AllowPrivateAccess = "true"))
	FBlackboardKeySelector distance;

public:

	UServiceCalculateDistance(const FObjectInitializer& ObjectInitializer);

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
