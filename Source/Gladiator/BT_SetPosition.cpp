// Fill out your copyright notice in the Description page of Project Settings.


#include "BT_SetPosition.h"
#include "Enemy.h"
#include "BehaviorTree/BlackboardComponent.h"

UBT_SetPosition::UBT_SetPosition(const FObjectInitializer& ObjectInitializer)
{
	bNotifyTick = true;
}

void UBT_SetPosition::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	UBlackboardComponent* bb = OwnerComp.GetBlackboardComponent();

	AEnemy* enemy = (AEnemy*)bb->GetValueAsObject(actor.SelectedKeyName);

	bb->SetValueAsVector(vector.SelectedKeyName, enemy->GetActorLocation());
}