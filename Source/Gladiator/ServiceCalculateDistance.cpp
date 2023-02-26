// Fill out your copyright notice in the Description page of Project Settings.


#include "ServiceCalculateDistance.h"
#include "Enemy.h"
#include "GladiatorCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include <Kismet/GameplayStatics.h>


UServiceCalculateDistance::UServiceCalculateDistance(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bNotifyTick = true;
}

void UServiceCalculateDistance::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

	AGladiatorCharacter* PlayerPawn = Cast<AGladiatorCharacter>(Blackboard->GetValueAsObject(player.SelectedKeyName));
	if (PlayerPawn == nullptr)
		return;

	AEnemy* AIPawn = Cast<AEnemy>(Blackboard->GetValueAsObject(ai.SelectedKeyName));
	if (AIPawn == nullptr)
		return;

	float dist = FVector::Dist(AIPawn->GetActorLocation(), PlayerPawn->GetActorLocation());
	
	Blackboard->SetValueAsFloat(distance.SelectedKeyName, dist);
}