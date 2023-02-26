// Fill out your copyright notice in the Description page of Project Settings.


#include "BT_InRange.h"
#include "BehaviorTree/BlackboardComponent.h"


bool UBT_InRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	UBlackboardComponent* bb = OwnerComp.GetBlackboardComponent();

	float max = bb->GetValueAsFloat(MaxRange.SelectedKeyName);
	float dist = bb->GetValueAsFloat(Distance.SelectedKeyName);

	return dist > max;
}