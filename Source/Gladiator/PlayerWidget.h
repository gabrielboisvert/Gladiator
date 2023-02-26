// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerWidget.generated.h"


/**
 * 
 */
UCLASS()
class GLADIATOR_API UPlayerWidget : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;
	
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UProgressBar* lifeBar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* lifeText;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Stats)
	float lowLifeRatio = 0.2f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Stats)
	float highLifeRatio = 0.8f;

	float ratio = 1 / (highLifeRatio - lowLifeRatio);

public:

	UFUNCTION(BlueprintCallable, CallInEditor)
	void SetRatio(float value);

	UFUNCTION(BlueprintCallable, CallInEditor)
	void SetLife(int value);

	UFUNCTION(BlueprintCallable, CallInEditor)
	void SetLifeColor(float value);
};
