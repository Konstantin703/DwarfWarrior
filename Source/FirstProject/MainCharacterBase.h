#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MainCharacterBase.generated.h"

class USpringArmComponent;
class UCameraComponent;
class AWeapon;
class AItem;
class UAnimaMontage;
class UParticleSystem;
class USoundCue;
class AEnemyBase;
class AController;
class AMainPlayerController;
class AItemStorage;

UENUM(BlueprintType)
enum class EMovementStatus : uint8
{
	EMS_Idle		UMETA(DisplayName = "Idle"),
	EMS_Normal		UMETA(DisplayName = "Normal"),
	EMS_Sprinting	UMETA(DisplayName = "Sprinting"),
	EMS_Dead		UMETA(DisplayName = "Dead"),
	EMS_MAX			UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EStaminaStatus : uint8
{
	ESS_Normal					UMETA(DisplayName = "Normal"),
	ESS_BelowMinimum			UMETA(DisplayName = "BelowMinimum"),
	ESS_Exhausted				UMETA(DisplayName = "Exhausted"),
	ESS_ExhaustedRecovering		UMETA(DisplayName = "ExhaustedRecovering"),
	ESS_MAX						UMETA(DisplayName = "DefaultMax")
};

// TODO: some combat releated functions are similar to EnemyBase.h - refactor, make parent class
UCLASS()
class FIRSTPROJECT_API AMainCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMainCharacterBase();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Controller")
	AMainPlayerController* PlayerController;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Items")
	AWeapon* EquippedWeapon;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Items")
	AItem* ActiveOverlappingItem;

	// Camera boom positioning camera behind the character
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;
	// follow camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	/** base turn rates to scale turning function for the camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;
	
	/**
	/* Player Stats
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerStats")
	float MaxHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerStats")
	float Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerStats")
	float MaxStamina;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerStats")
	float Stamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerStats")
	int32 Coins;

	bool bSprintEnabled;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	EMovementStatus MovementStatus;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	EStaminaStatus StaminaStatus;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Running")
	float RunningSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Running")
	float SprintingSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float StaminaDrainRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MinSprintStamina;

	float InterpSpeed;
	bool bInterpToEnemy;

	bool bMovingForward;
	bool bMovingRight;

	bool bActionEnabled;
	bool bPauseMenuEnabled;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anims")
	bool bAttacking;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims")
	UAnimMontage* CombatMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	UParticleSystem* HitParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	USoundCue* HitSound;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	AEnemyBase* CombatTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	FVector CombatTargetLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TSubclassOf<AEnemyBase> EnemyFilter;

	UPROPERTY(EditDefaultsOnly, Category = "SaveData")
	TSubclassOf<AItemStorage> WeaponStorage;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FORCEINLINE AMainPlayerController* GetPlayerController() const { return PlayerController; }

	/** Called for forward/backward input */
	void MoveForward(float InValue);
	/** Called for side to side input */
	void MoveRight(float InValue);

	/** Sprint control */
	void SprintEnabled();
	void SprintDisabled();

	/** Turn via input to turn at a given rate 
	* @param InRate This is a normalized rate, i.e. 1.0 means 100% of desired rate
	*/
	void TurnAtRate(float InRate);

	/** Turn via input to turn at a given rate
	* @param InRate This is a normalized rate, i.e. 1.0 means 100% of desired rate
	*/
	void LookUpAtRate(float InRate);

	void ActionEnabled();
	void ActionDisabled();

	void EnablePauseMenu();
	void DisablePauseMenu();

	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UFUNCTION(BlueprintCallable, Category = "PlayerStats")
	float GetHealthPercent() { return Health / MaxHealth; }

	UFUNCTION(BlueprintCallable, Category = "PlayerStats")
	float GetStaminaPercent() { return Stamina / MaxStamina; }

	UFUNCTION(BlueprintCallable, Category = "PlayerStats")
	FText CoinsToText();

	void DecrementHealth(float Amount);

	virtual float TakeDamage(float DamageAmount, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser) override;

	UFUNCTION(BlueprintCallable)
	void IncrementCoins(int32 Amount);
	UFUNCTION(BlueprintCallable)
	void IncrementHealth(float Amount);

	void Die();

	UFUNCTION(BlueprintCallable)
	void DeathEnd();

	/** Set movemet status and running speed */
	void SetMovementStatus(EMovementStatus Status);

	FORCEINLINE void SetStaminaStatus(EStaminaStatus Status) { StaminaStatus = Status; }

	void SetEquippedWeapon(AWeapon* InWeapon);
	FORCEINLINE AWeapon* GetEquippedWeapon() { return EquippedWeapon; }

	FORCEINLINE void SetActiveOverlappingItem(AItem* InItem) { ActiveOverlappingItem = InItem; }

	void Attack();

	UFUNCTION(BlueprintCallable)
	void AttackEnd();

	UFUNCTION(BlueprintCallable)
	void PlaySwingSound();

	FORCEINLINE UParticleSystem* GetHitParticles() { return HitParticles; }
	FORCEINLINE USoundCue* GetHitSound() { return HitSound; }

	void SetInterpToEnemy(bool InInterp);

	FORCEINLINE void SetCombatTarget(AEnemyBase* Target) { CombatTarget = Target; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE AEnemyBase* GetCombatTarget() { return CombatTarget; }

	FRotator GetLookAtRotationYaw(FVector Target);

	bool IsAlive();
	bool IsSprinting();

	void UpdateCombatTarget();

	void SwitchLevel(FName LevelName);

	UFUNCTION(BlueprintCallable)
	void SaveGame();
	UFUNCTION(BlueprintCallable)
	void LoadGame(bool SetPosition);
};
