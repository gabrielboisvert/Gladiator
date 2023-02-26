// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/SkeletalMesh.h"
#include "Kismet/KismetMathLibrary.h"
#include <Kismet/GameplayStatics.h>
#include "BehaviorTree/BlackboardComponent.h"
#include "EnemyAIController.h"
#include "GladiatorCharacter.h"

#include "NavAreas/NavArea_Null.h"

// Sets default values
AEnemy::AEnemy(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));
	Shield = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Shield"));

	ConstructorHelpers::FObjectFinder<USkeletalMesh> body(TEXT("/Game/assets/Characters/DwarfGrunt/SkelMesh/DwarfGrunt_R_new.DwarfGrunt_R_new"));
	if (body.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(body.Object);

		if (Weapon != nullptr)
		{
			Weapon->SetupAttachment(GetMesh(), FName(TEXT("WeaponPoint")));
			Weapon->SetRelativeLocation(FVector());
		}

		if (Shield != nullptr)
		{
			Shield->SetupAttachment(GetMesh(), FName(TEXT("DualWeaponPoint")));
			Shield->SetRelativeLocation(FVector());
		}
	}

	Widget = CreateDefaultSubobject<UWidgetComponent>(TEXT("UWidget"));
	Widget->SetupAttachment(GetMesh());
	Widget->RegisterComponent();

	HammerHitSphere = CreateDefaultSubobject<USphereComponent>(TEXT("sphere"));
	HammerHitSphere->SetupAttachment(Weapon);
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	DynTop = GetMesh()->CreateDynamicMaterialInstance(0);
	DynBottom = GetMesh()->CreateDynamicMaterialInstance(1);

	GetMesh()->SetMaterial(0, DynTop);
	GetMesh()->SetMaterial(1, DynBottom);

	AnimInstance = GetMesh()->GetAnimInstance();

	EndAttackDelegate.BindUObject(this, &AEnemy::OnAnimationAttackEnded);
	EndDeathDelegate.BindUObject(this, &AEnemy::OnAnimationDeathEnded);

	HUD = (UEnemyWidget*)Widget->GetWidget();
	UpdateHUD();

	HammerHitSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnOverlapBegin);
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasBeenHit)
	{
		HitEffectDurationTimer += DeltaTime;
		if (HitEffectDurationTimer > HitEffectDuration)
		{
			DynTop->SetVectorParameterValue("HitColor", FLinearColor(0.0, 0.0, 0.0, 1.0));
			DynBottom->SetVectorParameterValue("HitColor", FLinearColor(0.0, 0.0, 0.0, 1.0));
			HasBeenHit = false;
		}
	}

	if (!bAlive)
		return;

	HitTimer += DeltaTime;
	if (HittedEnemy != nullptr)
	{
		if (HitTimer >= AnimationDuration * 0.30f && HitTimer < AnimationDuration * 0.45f)
		{
			UGameplayStatics::ApplyDamage(HittedEnemy, 1, GetController(), this, UDamageType::StaticClass());
			HittedEnemy = nullptr;
			HitTimer = 0;
			bHasHitDuringFrame = true;
		}
	}

	FRotator PlayerRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->GetActorLocation());
	SetActorRotation(PlayerRot);

	PlayerRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), GetWorld()->GetFirstPlayerController()->PlayerCameraManager->GetCameraLocation());
	PlayerRot.Pitch = 0;
	PlayerRot.Roll = 0;
	Widget->SetWorldRotation(PlayerRot);
}

void AEnemy::UpdateHUD()
{
	float lifeRatio = (float)this->Life / (float)this->MAX_LIFE;
	HUD->SetRatio(lifeRatio);
	HUD->SetLife(this->Life);
	HUD->SetLifeColor(lifeRatio);
}

float AEnemy::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (this->Life <= 0)
		return Damage;

	this->Life -= Damage;
	UpdateHUD();

	HitEffectDurationTimer = 0;
	DynTop->SetVectorParameterValue("HitColor", FLinearColor(1, 0.0, 0.0, 1.0));
	DynBottom->SetVectorParameterValue("HitColor", FLinearColor(1, 0.0, 0.0, 1.0));
	HasBeenHit = true;

	if (this->Life <= 0)
	{
		bAlive = false;

		Widget->SetVisibility(false);
		SetActorEnableCollision(false);

		AnimInstance->Montage_Play(this->DeathAnim, 1.f);
		AnimInstance->Montage_SetEndDelegate(EndDeathDelegate);

		Cast<AEnemyAIController>(GetController())->getBlackBoard()->SetValueAsBool("IsAlive", false);

		if (Cast<AEnemyAIController>(GetController())->getBlackBoard()->GetValueAsBool("IsAttacking"))
		{
			Cast<AEnemyAIController>(GetController())->getBlackBoard()->SetValueAsBool("IsAttacking", false);
			OnEndAttack.Broadcast();
		}
	}

	return Damage;
}

void AEnemy::OnAnimationAttackEnded(UAnimMontage* Montage, bool bInterrupted)
{
	bCanAttack = true;
	bCanMove = true;
	bIsAttacking = false;
	bHasHitDuringFrame = false;

	Cast<AEnemyAIController>(GetController())->getBlackBoard()->SetValueAsBool("IsAttacking", false);
	OnEndAttack.Broadcast();
}

void AEnemy::OnAnimationDeathEnded(UAnimMontage* Montage, bool bInterrupted)
{
	OnEnemyDeath.Broadcast();
}

void AEnemy::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!bIsAttacking || HittedEnemy != nullptr || bHasHitDuringFrame)
		return;

	HittedEnemy = Cast<AGladiatorCharacter>(OtherActor);
	if (HittedEnemy == nullptr)
		return;

	bIsAttacking = false;
}

void AEnemy::Attack()
{
	if (!bCanAttack)
		return;

	AnimationDuration = AnimInstance->Montage_Play(this->AttackAnim, 1.f);
	AnimInstance->Montage_SetEndDelegate(EndAttackDelegate);

	bCanMove = false;
	bCanAttack = false;
	HitTimer = 0;
	bIsAttacking = true;
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}