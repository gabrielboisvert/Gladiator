// Fill out your copyright notice in the Description page of Project Settings.


#include "BT_Attack.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemy.h"

UBT_Attack::UBT_Attack(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bNotifyTick = true;
}

void UBT_Attack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (!enemy->IsAttacking() && bHasBegin)
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}

EBTNodeResult::Type UBT_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* bb = OwnerComp.GetBlackboardComponent();
	enemy = (AEnemy*)bb->GetValueAsObject(actor.SelectedKeyName);
	
	if (!bHasBegin)
	{
		enemy->Attack();
		bHasBegin = true;
	}
	
	if (!enemy->IsAttacking())
	{
		bHasBegin = false;
		bb->SetValueAsBool(bIsAttacking.SelectedKeyName, false);
		enemy->OnEndAttack.Broadcast();
		return EBTNodeResult::Succeeded;
	}
	else
		return EBTNodeResult::InProgress;

}