#include "MainCharacterBase.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Items/Weapon.h"
#include "Animation/AnimInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Kismet/KismetMathLibrary.h"
#include "Enemies/EnemyBase.h"
#include "MainPlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AMainCharacterBase::AMainCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// create camera boom (pulls towards the character in the case of collision
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 600.f; // distance to character
	CameraBoom->bUsePawnControlRotation = true;

	// Set size for collision capsule
	GetCapsuleComponent()->SetCapsuleSize(46.f, 100.f);

	// create follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// Attach the camera to the end of the boom, and let the boom adjust to match 
	// the controller orientation
	FollowCamera->bUsePawnControlRotation = false;

	// Set turn rates for input
	BaseTurnRate = 65.f;
	BaseLookUpRate = 65.f;

	// Don't rotate when controller rotates
	// Let that just affect camera
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	// configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // character moves in the dirrection of input...
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f); // ... at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 650.f;
	GetCharacterMovement()->AirControl = 0.2f;

	MaxHealth = 100.f;
	Health = MaxHealth;

	MaxStamina = 150.f;
	Stamina = MaxStamina;

	Coins = 5;

	RunningSpeed = 650.f;
	SprintingSpeed = 950.f;

	bSprintEnabled = false;

	// Initialize enums
	MovementStatus = EMovementStatus::EMS_Normal;
	StaminaStatus = EStaminaStatus::ESS_Normal;

	StaminaDrainRate = 25.f;
	MinSprintStamina = 50.f;

	bActionEnabled = false;

	InterpSpeed = 15.f;
	bInterpToEnemy = false;

	bMovingForward = false;
	bMovingRight = false;
}

// Called when the game starts or when spawned
void AMainCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	PlayerController = Cast<AMainPlayerController>(GetController());	
}

// Called every frame
void AMainCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float DeltaStamina = StaminaDrainRate * DeltaTime;

	switch (StaminaStatus)
	{
	case EStaminaStatus::ESS_Normal:
		if (bSprintEnabled)
		{
			if ((Stamina - DeltaStamina) <= MinSprintStamina)
				SetStaminaStatus(EStaminaStatus::ESS_BelowMinimum);
			
			if (bMovingForward || bMovingRight)
				SetMovementStatus(EMovementStatus::EMS_Sprinting);
			else
				SetMovementStatus(EMovementStatus::EMS_Normal);

			if (IsSprinting())
				Stamina -= DeltaStamina;
		}
		else
		{
			SetMovementStatus(EMovementStatus::EMS_Normal);

			if (!IsSprinting())
			{
				if ((Stamina + DeltaStamina) >= MaxStamina)
					Stamina = MaxStamina;
				else
					Stamina += DeltaStamina;
			}			
		}
		break;
	case EStaminaStatus::ESS_BelowMinimum:
		if (bSprintEnabled)
		{
			if ((Stamina - DeltaStamina) <= 0.f)
			{
				SetStaminaStatus(EStaminaStatus::ESS_Exhausted);
				Stamina = 0.f;
				SetMovementStatus(EMovementStatus::EMS_Normal);
			}
			else
			{
				if (bMovingForward || bMovingRight)
					SetMovementStatus(EMovementStatus::EMS_Sprinting);
				else
					SetMovementStatus(EMovementStatus::EMS_Normal);

				if (IsSprinting())
					Stamina -= DeltaStamina;
			}
		}
		else
		{
			if ((Stamina + DeltaStamina) >= MinSprintStamina)
				SetStaminaStatus(EStaminaStatus::ESS_Normal);

			SetMovementStatus(EMovementStatus::EMS_Normal);

			if (!IsSprinting())
				Stamina += DeltaStamina;
		}
		break;
	case EStaminaStatus::ESS_Exhausted:
		if (bSprintEnabled)
		{
			Stamina = 0.f;
		}
		else
		{
			SetStaminaStatus(EStaminaStatus::ESS_ExhaustedRecovering);

			if (!IsSprinting())
				Stamina += DeltaStamina;
		}
		SetMovementStatus(EMovementStatus::EMS_Normal);
		break;
	case EStaminaStatus::ESS_ExhaustedRecovering:
		if ((Stamina + DeltaStamina) >= MinSprintStamina)
			SetStaminaStatus(EStaminaStatus::ESS_Normal);
		
		SetMovementStatus(EMovementStatus::EMS_Normal);

		if (!IsSprinting())
			Stamina += DeltaStamina;
		
		break;
	default:
		;
	}
		

	if (bInterpToEnemy && CombatTarget)
	{
		FRotator LookAtYaw = GetLookAtRotationYaw(CombatTarget->GetActorLocation());
		FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), LookAtYaw, DeltaTime, InterpSpeed);

		SetActorRotation(InterpRotation);
	}

	if (CombatTarget)
	{
		CombatTargetLocation = CombatTarget->GetActorLocation();
		if (PlayerController)
			PlayerController->SetEnemyLocation(CombatTargetLocation);
	}
		
}

// Called to bind functionality to input
void AMainCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &AMainCharacterBase::Jump);
	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Sprint", EInputEvent::IE_Pressed, this, &AMainCharacterBase::SprintEnabled);
	PlayerInputComponent->BindAction("Sprint", EInputEvent::IE_Released, this, &AMainCharacterBase::SprintDisabled);

	PlayerInputComponent->BindAction("Action", EInputEvent::IE_Pressed, this, &AMainCharacterBase::ActionEnabled);
	PlayerInputComponent->BindAction("Action", EInputEvent::IE_Released, this, &AMainCharacterBase::ActionDisabled);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMainCharacterBase::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMainCharacterBase::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAxis("TurnRate", this, &AMainCharacterBase::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMainCharacterBase::LookUpAtRate);

}

void AMainCharacterBase::MoveForward(float InValue)
{
	bMovingForward = false;

	if (Controller && (InValue != 0.f) && !bAttacking )
	{
		// Find forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, InValue);

		bMovingForward = true;
	}
}

void AMainCharacterBase::MoveRight(float InValue)
{
	bMovingRight = false;

	if (Controller && (InValue != 0.f) && !bAttacking)
	{
		// Find right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, InValue);

		bMovingRight = true;
	}
}

void AMainCharacterBase::SprintEnabled()
{
	bSprintEnabled = true;
}

void AMainCharacterBase::SprintDisabled()
{
	bSprintEnabled = false;
}

void AMainCharacterBase::TurnAtRate(float InRate)
{
	AddControllerYawInput(InRate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMainCharacterBase::LookUpAtRate(float InRate)
{
	AddControllerPitchInput(InRate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMainCharacterBase::ActionEnabled()
{
	bActionEnabled = true;

	if (ActiveOverlappingItem)
	{
		AWeapon* Weapon = Cast<AWeapon>(ActiveOverlappingItem);
		if (Weapon)
		{
			Weapon->Equip(this);
			SetActiveOverlappingItem(nullptr);
		}			
	}
	else if (EquippedWeapon)
	{
		Attack();
	}
}

void AMainCharacterBase::ActionDisabled()
{
	bActionEnabled = false;
}

FText AMainCharacterBase::CoinsToText()
{
	FString CoinsString = FString::FromInt(Coins);
	return FText::FromString(CoinsString);
}

void AMainCharacterBase::DecrementHealth(float Amount)
{
	if (Health - Amount <= 0.f)
	{
		Health = 0.f;
		Die();
	}	
	else
	{
		Health -= Amount;
	}		
}

float AMainCharacterBase::TakeDamage(float DamageAmount, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	DecrementHealth(DamageAmount);

	if (!IsAlive() && DamageCauser)
	{
		AEnemyBase* Enemy = Cast<AEnemyBase>(DamageCauser);
		if (Enemy && Enemy->GetCombatTarget())
		{
			Enemy->SetCombatTarget(nullptr);
		}
	}

	return DamageAmount;
}

void AMainCharacterBase::IncrementCoins(int32 Amount)
{
	Coins += Amount;
}

void AMainCharacterBase::IncrementHealth(float Amount)
{
	if ((Health + Amount) >= MaxHealth)
		Health = MaxHealth;
	else
		Health += Amount;
}

void AMainCharacterBase::Die()
{
	if (!IsAlive()) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && CombatMontage)
	{
		AnimInstance->Montage_Play(CombatMontage, 1.f);
		AnimInstance->Montage_JumpToSection(FName("Death"));
	}
	SetMovementStatus(EMovementStatus::EMS_Dead);
	DestroyPlayerInputComponent();
	//InputComponent->ClearBindingValues();
}

void AMainCharacterBase::DeathEnd()
{
	GetMesh()->bPauseAnims = true;
	GetMesh()->bNoSkeletonUpdate = true;
}

void AMainCharacterBase::SetMovementStatus(EMovementStatus Status)
{
	MovementStatus = Status;
	if (MovementStatus == EMovementStatus::EMS_Sprinting)
		GetCharacterMovement()->MaxWalkSpeed = SprintingSpeed;
	else
		GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
}

void AMainCharacterBase::SetEquippedWeapon(AWeapon* InWeapon)
{
	if (EquippedWeapon)
		EquippedWeapon->Destroy();

	EquippedWeapon = InWeapon; 
}

void AMainCharacterBase::Attack()
{
	if (!bAttacking) 
	{
		bAttacking = true;
		SetInterpToEnemy(true);
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && CombatMontage)
		{
			int32 Selection = FMath::RandRange(0, 1);
			switch (Selection)
			{
			case 0:
				AnimInstance->Montage_Play(CombatMontage, 2.2f);
				AnimInstance->Montage_JumpToSection(FName("Attack_1"), CombatMontage);
				break;
			case 1:
				AnimInstance->Montage_Play(CombatMontage, 1.8f);
				AnimInstance->Montage_JumpToSection(FName("Attack_2"), CombatMontage);
				break;
			default:
				;
			}			
		}
	}
}

void AMainCharacterBase::AttackEnd()
{
	bAttacking = false;
	SetInterpToEnemy(false);
	if (bActionEnabled)
		Attack();
}

void AMainCharacterBase::PlaySwingSound()
{
	if (EquippedWeapon->GetSwingSound())
		UGameplayStatics::PlaySound2D(this, EquippedWeapon->GetSwingSound());
}

void AMainCharacterBase::SetInterpToEnemy(bool InInterp)
{
	bInterpToEnemy = InInterp;
}

FRotator AMainCharacterBase::GetLookAtRotationYaw(FVector Target)
{
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target);
	FRotator LookAtRotationYaw(0.f, LookAtRotation.Yaw, 0.f);
	return LookAtRotationYaw;
}

bool AMainCharacterBase::IsAlive()
{
	return MovementStatus != EMovementStatus::EMS_Dead;
}

bool AMainCharacterBase::IsSprinting()
{
	return MovementStatus == EMovementStatus::EMS_Sprinting;
}

void AMainCharacterBase::UpdateCombatTarget()
{
	TArray<AActor*> OverlappingActors;
	
	GetOverlappingActors(OverlappingActors, EnemyFilter); //AEnemyBase::StaticClass()

	if (OverlappingActors.Num() == 0)
	{
		if (GetPlayerController())
			GetPlayerController()->RemoveEnemyHealthBar();

		return;
	}

	float MinDistance = 1000.f;
	float CurrentDistance = 0.f;
	AActor* ClosestTarget = nullptr;

	for (AActor* Target : OverlappingActors)
	{
		CurrentDistance = (Target->GetActorLocation() - GetActorLocation()).Size();
		if (CurrentDistance < MinDistance) 
		{
			MinDistance = CurrentDistance;
			ClosestTarget = Target;
		}
	}

	SetCombatTarget(Cast<AEnemyBase>(ClosestTarget));

	if (PlayerController)
		PlayerController->DisplayEnemyHealthBar();	
	
}

void AMainCharacterBase::SwitchLevel(FName LevelName)
{
	UWorld* World = GetWorld();

	if (World)
	{
		FString CurrentLevel = World->GetMapName();
		FName CurrentLevelName(*CurrentLevel);
		
		if (CurrentLevelName != LevelName)
			UGameplayStatics::OpenLevel(World, LevelName);

	}
}