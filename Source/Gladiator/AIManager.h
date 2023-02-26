// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enemy.h"
#include "Delegates/Delegate.h"
#include "AIManager.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAllEnemyDeath);
#define AI_MAX_ENEMIES 5

UCLASS()
class GLADIATOR_API AAIManager : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable)
	FAllEnemyDeath OnAllEnemyDeath;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Stats)
	float TimeBetweenAttack = 2;

	UPROPERTY(EditAnywhere, Category = ai)
	AEnemy* Enemies[AI_MAX_ENEMIES] = {nullptr, nullptr, nullptr, nullptr, nullptr};
	int Counter = AI_MAX_ENEMIES;
	float AttackTime = 0;

	// Sets default values for this actor's properties
	AAIManager();

protected:
	AEnemy* EnemyAttacking = nullptr;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnEnemyDeath();

	UFUNCTION()
	void OnEnemyEndAttack();

	bool IsGameOver();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	AEnemy* ChooseRandomEnemy();
};
