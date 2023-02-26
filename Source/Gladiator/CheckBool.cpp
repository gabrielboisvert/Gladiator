// Fill out your copyright notice in the Description page of Project Settings.
#include "CheckBool.h"
#include "BehaviorTree/BlackboardComponent.h"

bool UCheckBool::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	UBlackboardComponent* bb = OwnerComp.GetBlackboardComponent();

	return bb->GetValueAsBool(bValue.SelectedKeyName);
}
