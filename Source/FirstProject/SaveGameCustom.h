#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SaveGameCustom.generated.h"

USTRUCT(BlueprintType)
struct FCharacterStats
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	float Health;
	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	float MaxHealth;
	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	float Stamina;
	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	float MaxStamina;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	int32 Coins;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	FVector CharacterLocation;
	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	FRotator CharacterRotation;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	FString WeaponName;

};

UCLASS()
class FIRSTPROJECT_API USaveGameCustom : public USaveGame
{
	GENERATED_BODY()
public:
	USaveGameCustom();

	UPROPERTY(VisibleAnywhere, Category = Basic)
	FString PlayerName;
	UPROPERTY(VisibleAnywhere, Category = Basic)
	uint32 UserIndex;
	UPROPERTY(VisibleAnywhere, Category = Basic)
	FCharacterStats CharacterStats;

	FORCEINLINE FString GetPlayerName() { return PlayerName; }
	FORCEINLINE uint32 GetUserIndex() { return UserIndex; }
	FORCEINLINE FCharacterStats GetCharacterStats() { return CharacterStats; }

};
