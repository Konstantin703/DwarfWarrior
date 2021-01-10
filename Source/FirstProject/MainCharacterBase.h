#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MainCharacterBase.generated.h"

class USpringArmComponent;
class UCameraComponent;

UENUM(BlueprintType)
enum class EMovementStatus : uint8
{
	EMS_Normal UMETA(DisplayName = "Normal"),
	EMS_Sprinting UMETA(DisplayName = "Sprinting"),
	EMS_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EStaminaStatus : uint8
{
	ESS_Normal UMETA(DisplayName = "Normal"),
	ESS_BelowMinimum UMETA(DisplayName = "BelowMinimum"),
	ESS_Exhausted UMETA(DisplayName = "Exhausted"),
	ESS_ExhaustedRecovering UMETA(DisplayName = "ExhaustedRecovering"),
	ESS_MAX UMETA(DisplayName = "DefaultMax")
};

UCLASS()
class FIRSTPROJECT_API AMainCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMainCharacterBase();

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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

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

	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UFUNCTION(BlueprintCallable, Category = "PlayerStats")
	FORCEINLINE float GetHealthPercent() { return Health / MaxHealth; }

	UFUNCTION(BlueprintCallable, Category = "PlayerStats")
	FORCEINLINE float GetStaminaPercent() { return Stamina / MaxStamina; }

	UFUNCTION(BlueprintCallable, Category = "PlayerStats")
	FText CoinsToText();

	void DecrementHealth(float Amount);
	void IncrementCoins(int32 Amount);

	void Die();

	/** Set movemet status and running speed */
	void SetMovementStatus(EMovementStatus Status);

	FORCEINLINE void SetStaminaStatus(EStaminaStatus Status) { StaminaStatus = Status; }
};
