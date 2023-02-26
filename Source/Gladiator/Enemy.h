// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/TextRenderComponent.h"
#include "Delegates/Delegate.h"
#include "NavModifierComponent.h"
#include <Components/SphereComponent.h>
#include "Components/WidgetComponent.h"
#include "EnemyWidget.h"
#include "Enemy.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEnemyDeath);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEndAttack);

UCLASS()
class GLADIATOR_API AEnemy : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Stats)
		int Life = 3;
	UPROPERTY(EditAnywhere, Category = Stats)
		int MAX_LIFE = 3;
	UPROPERTY(EditAnywhere, Category = Stats)
		float MinRangeDetection = 300;
	UPROPERTY(EditAnywhere, Category = Stats)
		float MaxRangeDetection = 500;
	UPROPERTY(EditAnywhere, Category = Stats)
		float VisionDetection = 60;

	UPROPERTY(EditAnywhere)
		USkeletalMeshComponent* Weapon;

	UPROPERTY(EditAnywhere)
		USkeletalMeshComponent* Shield;

	//UPROPERTY(EditAnywhere)
	//	UNavModifierComponent* NavModifier;

protected:
	class UMaterialInstanceDynamic* DynTop;
	class UMaterialInstanceDynamic* DynBottom;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Stats)
	float HitEffectDuration = 0.25f;
	float HitEffectDurationTimer = 0;
	bool HasBeenHit = false;

	UAnimInstance* AnimInstance;
	FOnMontageEnded EndAttackDelegate;
	FOnMontageEnded EndDeathDelegate;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = collision)
	USphereComponent* HammerHitSphere;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UWidgetComponent* Widget;

	UEnemyWidget* HUD;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Animation)
	UAnimMontage* AttackAnim;
	float AnimationDuration;
	float HitTimer = 0;

	bool bCanMove = true;
	bool bCanAttack = true;
	bool bIsAttacking = false;
	bool bHasHitDuringFrame = false;

	class AGladiatorCharacter* HittedEnemy = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Animation)
	UAnimMontage* DeathAnim;

	//UPROPERTY(BlueprintReadWrite, EditAnywhere)
	//UTextRenderComponent* TextRenderer;

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Stats)
	bool bAlive = true;

	UPROPERTY(BlueprintAssignable)
	FEnemyDeath OnEnemyDeath;
	
	UPROPERTY(BlueprintAssignable)
	FEndAttack OnEndAttack;

	AEnemy(const FObjectInitializer& ObjectInitializer);
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void UpdateHUD();

	UFUNCTION()
	virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION()
	void OnAnimationAttackEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnAnimationDeathEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION(BlueprintCallable)
	void Attack();

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	float GetMinRange() { return MinRangeDetection; }
	float GetMaxRange() { return MaxRangeDetection; }
	float GetVisionDetection() { return VisionDetection; }
	bool IsAttacking() { return bIsAttacking; }

};