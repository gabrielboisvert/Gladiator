// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyWidget.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UEnemyWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Bind delegates here.
}

void UEnemyWidget::SetRatio(float value)
{
    lifeBar->SetPercent(value);
}

void UEnemyWidget::SetLife(int value)
{
    lifeText->SetText(FText::AsNumber(value));
}

void UEnemyWidget::SetLifeColor(float value)
{
    float result = FMath::Clamp((value - lowLifeRatio) * ratio, 0, 1);

    FLinearColor finalColor = FLinearColor::LerpUsingHSV(FLinearColor(1, 0, 0), FLinearColor(0, 1, 0), result);

    lifeBar->SetFillColorAndOpacity(finalColor);
}