// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Blueprint/UserWidget.h"
#include "PlayerWidget.h"
#include "Animation/AnimMontage.h"
#include "Delegates/Delegate.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Enemy.h"
#include "GladiatorCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerDeath);

UCLASS(config=Game)
class AGladiatorCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = collision)
	USphereComponent* HammerHitSphere;

	UPROPERTY(EditAnywhere)
	USkeletalMeshComponent* Weapon;

	UPROPERTY(EditAnywhere)
	USkeletalMeshComponent* Shield;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Animation)
	UAnimMontage* AttackAnim;
	float AnimationDuration;

	AEnemy* HittedEnemy = nullptr;
	float HitTimer = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Animation)
	UAnimMontage* DeathAnim;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Stats)
	int Life = 2;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Stats)
	int  MAX_LIFE = 5;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Stats)
	bool bAlive = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Stats)
	float HitEffectDuration = 0.5f;
	float HitEffectDurationTimer = 0;
	bool nHasBeenHit = false;

	UPlayerWidget* PlayerHUD;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = HUD)
	TSubclassOf<UPlayerWidget> PlayerWidgetClass;
	
	class UMaterialInstanceDynamic* DynTop;
	class UMaterialInstanceDynamic* DynBottom;

	bool bCanMove = true;
	bool bCanAttack = true;
	bool bIsAttacking = false;
	bool bHasHitDuringFrame = false;
	UAnimInstance* AnimInstance;
	FOnMontageEnded EndAttackDelegate;
	FOnMontageEnded EndDeathDelegate;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input)
	float TurnRateGamepad;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sound)
	UAudioComponent* PlayerSound;
public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UPROPERTY(BlueprintAssignable)
	FPlayerDeath OnPlayerDeath;

	AGladiatorCharacter();
	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;
	void UpdateHUD();

	UFUNCTION()
	virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION()
	void OnAnimationAttackEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnAnimationDeathEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION(BlueprintCallable)
	void Attack();

	UFUNCTION(BlueprintCallable)
	void PlaySound();

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	int& GetLife();
	int& GetMaxLife();



	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface
};