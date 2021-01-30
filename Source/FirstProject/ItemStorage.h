#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemStorage.generated.h"

class AWeapon;

UCLASS()
class FIRSTPROJECT_API AItemStorage : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemStorage();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	UPROPERTY(EditDefaultsOnly, Category = "SaveData")
	TMap<FString, TSubclassOf<AWeapon>> WeaponMap;

	FORCEINLINE TMap<FString, TSubclassOf<AWeapon>> GetWeaponMap() { return WeaponMap; }
};
