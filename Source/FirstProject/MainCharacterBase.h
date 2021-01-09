#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MainCharacterBase.generated.h"

class USpringArmComponent;
class UCameraComponent;

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

};
