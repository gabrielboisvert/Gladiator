// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UIScreen.generated.h"

/**
 * 
 */
UCLASS()
class GLADIATOR_API UUIScreen : public UUserWidget
{
	GENERATED_BODY()
	virtual void NativeConstruct() override;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UImage* image;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* text;
	
};
