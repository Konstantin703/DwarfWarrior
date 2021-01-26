#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Pickup.generated.h"

class AMainCharacterBase;

/**
 *  Object adding coins to the main charater
 */
UCLASS()
class FIRSTPROJECT_API APickup : public AItem
{
	GENERATED_BODY()
public:
	APickup();

	virtual void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult
	) override;

	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Pickup")
	void OnPickupBP(AMainCharacterBase* Target);
};
