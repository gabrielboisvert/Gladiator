// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameStateBase.h"
#include <Kismet/GameplayStatics.h>
#include "GladiatorCharacter.h"

AMyGameStateBase::AMyGameStateBase()
{
	PlayerSound = CreateDefaultSubobject<UAudioComponent>(TEXT("PlayerSound"));
}

void AMyGameStateBase::BeginPlay()
{
	Super::BeginPlay();

	AGladiatorCharacter* player = (AGladiatorCharacter*)(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	player->OnPlayerDeath.AddDynamic(this, &AMyGameStateBase::PlayerDeathEvent);

	InputComponent = GetWorld()->GetFirstPlayerController()->InputComponent;

	TArray<AActor*> FoundEnemies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAIManager::StaticClass(), FoundEnemies);

	AIManager = (AAIManager*)FoundEnemies[0];
	AIManager->OnAllEnemyDeath.AddDynamic(this, &AMyGameStateBase::PlayerWinEvent);

	if (PlayerSound != nullptr)
		PlayerSound->Stop();
}



void AMyGameStateBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMyGameStateBase::PlayerDeathEvent()
{
	DeadScreen = CreateWidget<UUIScreen>(GetWorld()->GetFirstPlayerController(), DeadWidgetClass);
	DeadScreen->AddToViewport(9999);
	InputComponent->BindAction(TEXT("Any Key"), IE_Pressed, this, &AMyGameStateBase::Restart);

	PlayerSound->SetSound(DeadSound);
	PlayerSound->Play();
}

void AMyGameStateBase::PlayerWinEvent()
{
	WinScreen = CreateWidget<UUIScreen>(GetWorld()->GetFirstPlayerController(), WinWidgetClass);
	WinScreen->AddToViewport(9999);
	InputComponent->BindAction(TEXT("Any Key"), IE_Pressed, this, &AMyGameStateBase::Restart);

	PlayerSound->SetSound(WinSound);
	PlayerSound->Play();
}

void AMyGameStateBase::Restart()
{
	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
}