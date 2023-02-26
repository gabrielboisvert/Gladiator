// Fill out your copyright notice in the Description page of Project Settings.


#include "AIManager.h"
#include "EnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

// Sets default values
AAIManager::AAIManager()
{
 	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAIManager::BeginPlay()
{
	Super::BeginPlay();
	
	for (unsigned int i = 0; i < 5; i++)
	{
		if (Enemies[i] != nullptr)
			Enemies[i]->OnEnemyDeath.AddDynamic(this, &AAIManager::OnEnemyDeath);
		else
			Counter--;
	}
}

// Called every frame
void AAIManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Counter <= 0)
		return;

	AttackTime += DeltaTime;
	if (EnemyAttacking == nullptr && AttackTime > TimeBetweenAttack && !IsGameOver())
	{
		do
		{
			EnemyAttacking = ChooseRandomEnemy();
		} while (!EnemyAttacking->bAlive);
		
		Cast<AEnemyAIController>(EnemyAttacking->GetController())->getBlackBoard()->SetValueAsBool("IsAttacking", true);
		EnemyAttacking->OnEndAttack.AddDynamic(this, &AAIManager::OnEnemyEndAttack);
	}
}

bool AAIManager::IsGameOver()
{
	for (unsigned int i = 0; i < AI_MAX_ENEMIES; i++)
		if (Enemies[i]->bAlive)
			return false; 
	return true;
}

AEnemy* AAIManager::ChooseRandomEnemy()
{
	int idx = FMath::RandRange(0, AI_MAX_ENEMIES - 1);
	return Enemies[idx];
}

void AAIManager::OnEnemyEndAttack()
{
	Cast<AEnemyAIController>(EnemyAttacking->GetController())->getBlackBoard()->SetValueAsBool("IsAttacking", false);
	EnemyAttacking->OnEndAttack.RemoveDynamic(this, &AAIManager::OnEnemyEndAttack);
	EnemyAttacking = nullptr;

	AttackTime = 0;
}

void AAIManager::OnEnemyDeath()
{
	Counter--;
	if (Counter <= 0)
		OnAllEnemyDeath.Broadcast();
}