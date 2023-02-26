// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "UIScreen.h"
#include "AIManager.h"
#include "Components/InputComponent.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"
#include "MyGameStateBase.generated.h"


/**
 * 
 */
UCLASS()
class GLADIATOR_API AMyGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

protected:
	UUIScreen* WinScreen = nullptr;
	UUIScreen* DeadScreen = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = HUD)
	TSubclassOf<UUIScreen> WinWidgetClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = HUD)
	TSubclassOf<UUIScreen> DeadWidgetClass;

	UInputComponent* InputComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sound)
	UAudioComponent* PlayerSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sound)
	USoundBase* WinSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sound)
	USoundBase* DeadSound;

public:
	AMyGameStateBase();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = ai)
	AAIManager* AIManager;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void PlayerDeathEvent();

	UFUNCTION()
	void PlayerWinEvent();

	void Restart();
};
