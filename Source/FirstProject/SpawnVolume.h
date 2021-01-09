

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnVolume.generated.h"

class UBoxComponent;
class ACritter;

UCLASS()
class FIRSTPROJECT_API ASpawnVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnVolume();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Spawning)
	UBoxComponent* SpawnBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawning)
	TSubclassOf<ACritter> PawnToSpawn;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintPure, Category = "Spawning")
	FVector GetSpawnPoint();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = Spawning)
	void SpawnPawn(UClass* ToSpawn, const FVector& SpawnLocation);

};
