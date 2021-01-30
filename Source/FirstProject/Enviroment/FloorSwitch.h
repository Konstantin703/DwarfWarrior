

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FloorSwitch.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
// TODO: make logic for two floor switches 
UCLASS()
class FIRSTPROJECT_API AFloorSwitch : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFloorSwitch();

	/** Overlap volume for rlogic to be triggered */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "FloorSwitch")
	UBoxComponent* TriggerBox;
	/** Switch for character to step on */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "FloorSwitch")
	UStaticMeshComponent* FloorSwitch;
	/** Door to move when floor switch steped on*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "FloorSwitch")
	UStaticMeshComponent* Door;
	/** Initial location for the door */
	UPROPERTY(BlueprintReadWrite, Category = "FloorSwitch")
	FVector InitialDoorLocation;
	/** Initial location for switch*/
	UPROPERTY(BlueprintReadWrite, Category = "FloorSwitch")
	FVector InitialSwitchLocation;

	UPROPERTY(EditAnywhere, Category = "FloorSwitch")
	float SwitchTime;

	bool bCharacterOnSwitch;

	FTimerHandle SwitchHandle;

	void CloseDoor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	UFUNCTION()
	void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep,const FHitResult& SweepResult
	);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	UFUNCTION(BlueprintImplementableEvent, Category = "FloorSwitch")
	void RaiseDoor();
	UFUNCTION(BlueprintImplementableEvent, Category = "FloorSwitch")
	void LowerDoor();
	UFUNCTION(BlueprintImplementableEvent, Category = "FloorSwitch")
	void RaiseFloorSwitch();
	UFUNCTION(BlueprintImplementableEvent, Category = "FloorSwitch")
	void LowerFloorSwitch();

	UFUNCTION(BlueprintCallable, Category = "FloorSwitch")
	void UpdateDoorLocation(float Z);
	UFUNCTION(BlueprintCallable, Category = "FloorSwitch")
	void UpdateFloorSwitchLocation(float Z);

};
