// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include <Kismet/GameplayStatics.h>
#include "Enemy.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "Navigation/CrowdManager.h"

AEnemyAIController::AEnemyAIController(FObjectInitializer const& ObjectInitializer) : Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
{
	ConstructorHelpers::FObjectFinder<UBehaviorTree> obj(TEXT("/Game/Characters/AI_Enemy_V2/BT_Enemy_V2.BT_Enemy_V2"));
	if (obj.Succeeded())
	{
		behaviorTree = obj.Object;
	}
	behaviorTreeComponent = ObjectInitializer.CreateDefaultSubobject<UBehaviorTreeComponent>(this, TEXT("BehaviorComp"));
	blackBoard = ObjectInitializer.CreateDefaultSubobject<UBlackboardComponent>(this, TEXT("BlackBoardComp"));
}

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

	AEnemy* self = Cast<AEnemy>(behaviorTreeComponent->GetAIOwner()->GetPawn());

	if (self != nullptr)
	{
		blackBoard->SetValueAsObject("SelfActor", self);
		blackBoard->SetValueAsObject("Target", UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
		blackBoard->SetValueAsFloat("MinRange", self->GetMinRange());
		blackBoard->SetValueAsFloat("MaxRange", self->GetMaxRange());
		blackBoard->SetValueAsFloat("VisionRange", self->GetVisionDetection());
		blackBoard->SetValueAsBool("IsAttacking", false);
		blackBoard->SetValueAsBool("IsAlive", true);
	}

	RunBehaviorTree(behaviorTree);
	behaviorTreeComponent->StartTree(*behaviorTree);



	UCrowdManager* CrowdManager = UCrowdManager::GetCurrent(GetWorld());
	if (CrowdManager)
	{
		const ICrowdAgentInterface* IAgent = Cast<ICrowdAgentInterface>(this);
		CrowdManager->UpdateAgentParams(IAgent);
	}

	UCrowdFollowingComponent* pathfol = Cast<UCrowdFollowingComponent>(GetPathFollowingComponent());
	if (pathfol)
	{
		pathfol->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::Low);
		pathfol->SetCrowdSeparationWeight(1000, true);
		pathfol->SetCrowdSeparation(true, true);
	}
}

void AEnemyAIController::OnPossess(APawn* const pawn)
{
	Super::OnPossess(pawn);
	if (blackBoard)
	{
		blackBoard->InitializeBlackboard(*behaviorTree->BlackboardAsset);
	}
}

UBlackboardComponent* AEnemyAIController::getBlackBoard() const
{
	return blackBoard;
}
