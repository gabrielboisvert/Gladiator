// Copyright Epic Games, Inc. All Rights Reserved.

#include "GladiatorCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"


AGladiatorCharacter::AGladiatorCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rate for input
	TurnRateGamepad = 50.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
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

	HammerHitSphere = CreateDefaultSubobject<USphereComponent>(TEXT("sphere"));
	HammerHitSphere->SetupAttachment(Weapon);

	PlayerSound = CreateDefaultSubobject<UAudioComponent>(TEXT("PlayerSound"));
}

void AGladiatorCharacter::BeginPlay()
{
	Super::BeginPlay();

	PlayerHUD = CreateWidget<UPlayerWidget>((APlayerController*)GetController(), PlayerWidgetClass);
	if (!PlayerHUD)
		return;

	PlayerHUD->AddToViewport(9999);
	UpdateHUD();

	DynTop = GetMesh()->CreateDynamicMaterialInstance(0);
	DynBottom = GetMesh()->CreateDynamicMaterialInstance(1);
	
	GetMesh()->SetMaterial(0, DynTop);
	GetMesh()->SetMaterial(1, DynBottom);

	EndAttackDelegate.BindUObject(this, &AGladiatorCharacter::OnAnimationAttackEnded);
	EndDeathDelegate.BindUObject(this, &AGladiatorCharacter::OnAnimationDeathEnded);

	AnimInstance = GetMesh()->GetAnimInstance();

	HammerHitSphere->OnComponentBeginOverlap.AddDynamic(this, &AGladiatorCharacter::OnOverlapBegin);
	HammerHitSphere->OnComponentEndOverlap.AddDynamic(this, &AGladiatorCharacter::OnOverlapEnd);

	if (PlayerSound != nullptr)
		PlayerSound->Stop();

}

void AGladiatorCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (nHasBeenHit)
	{
		HitEffectDurationTimer += DeltaTime;
		if (HitEffectDurationTimer > HitEffectDuration)
		{
			DynTop->SetVectorParameterValue("HitColor", FLinearColor(0.0, 0.0, 0.0, 1.0));
			DynBottom->SetVectorParameterValue("HitColor", FLinearColor(0.0, 0.0, 0.0, 1.0));
			nHasBeenHit = false;
		}
	}

	HitTimer += DeltaTime;
	if (HittedEnemy != nullptr)
	{
		if (HitTimer >= 0.30f && HitTimer <=  0.40f)
		{
			UGameplayStatics::ApplyDamage(HittedEnemy, 1, GetController(), this, UDamageType::StaticClass());
			HittedEnemy = nullptr;
			bHasHitDuringFrame = true;
			HitTimer = 0;
		}
	}
}

void AGladiatorCharacter::UpdateHUD()
{
	float lifeRatio = (float)this->Life / (float)this->MAX_LIFE;
	PlayerHUD->SetRatio(lifeRatio);
	PlayerHUD->SetLife(this->Life);
	PlayerHUD->SetLifeColor(lifeRatio);
}

float AGladiatorCharacter::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (this->Life <= 0)
		return Damage;

	this->Life -= Damage;
	UpdateHUD();

	HitEffectDurationTimer = 0;
	DynTop->SetVectorParameterValue("HitColor", FLinearColor(1, 0.0, 0.0, 1.0));
	DynBottom->SetVectorParameterValue("HitColor", FLinearColor(1, 0.0, 0.0, 1.0));
	nHasBeenHit = true;

	if (this->Life <= 0)
	{
		bCanMove = false;
		bCanAttack = false;
		bAlive = false;
		
		PlayerHUD->RemoveFromViewport();
		SetActorEnableCollision(false);

		AnimInstance->Montage_Play(this->DeathAnim, 1.f);
		AnimInstance->Montage_SetEndDelegate(EndDeathDelegate);
	}

	return Damage;
}

void AGladiatorCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!bIsAttacking || bHasHitDuringFrame)
		return;

	HittedEnemy = Cast<AEnemy>(OtherActor);
	if (HittedEnemy == nullptr)
		return;
}

void AGladiatorCharacter::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AEnemy* enemy = Cast<AEnemy>(OtherActor);
	if (enemy == HittedEnemy)
		HittedEnemy = nullptr;
}

void AGladiatorCharacter::OnAnimationAttackEnded(UAnimMontage* Montage, bool bInterrupted)
{
	bCanAttack = true;
	bCanMove = true;
	bIsAttacking = false;
	bHasHitDuringFrame = false;
	HittedEnemy = nullptr;
}

void AGladiatorCharacter::OnAnimationDeathEnded(UAnimMontage* Montage, bool bInterrupted)
{
	OnPlayerDeath.Broadcast();
}

void AGladiatorCharacter::Attack()
{
	if (!bCanAttack)
		return;

	AnimationDuration = AnimInstance->Montage_Play(this->AttackAnim, 1.f);
	AnimInstance->Montage_SetEndDelegate(EndAttackDelegate);

	bCanMove = false;
	bCanAttack = false;
	bIsAttacking = true;
	HitTimer = 0;
}

void AGladiatorCharacter::PlaySound()
{
	if (PlayerSound == nullptr)
		return;

	if (PlayerSound->IsPlaying())
		PlayerSound->Stop();
	else if (!PlayerSound->IsPlaying())
		PlayerSound->Play();
}

void AGladiatorCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	//PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	//PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &AGladiatorCharacter::Attack);
	PlayerInputComponent->BindAction("PlaySound", IE_Pressed, this, &AGladiatorCharacter::PlaySound);

	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &AGladiatorCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &AGladiatorCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &AGladiatorCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &AGladiatorCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AGladiatorCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AGladiatorCharacter::TouchStopped);
}

int& AGladiatorCharacter::GetLife()
{
	return Life;
}

int& AGladiatorCharacter::GetMaxLife()
{
	return MAX_LIFE;
}

void AGladiatorCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void AGladiatorCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}

void AGladiatorCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void AGladiatorCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void AGladiatorCharacter::MoveForward(float Value)
{
	if (!bCanMove)
		return;

	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AGladiatorCharacter::MoveRight(float Value)
{
	if (!bCanMove)
		return;

	if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}