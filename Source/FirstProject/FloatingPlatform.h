#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FloatingPlatform.generated.h"

class UStaticMeshComponent;

UCLASS()
class FIRSTPROJECT_API AFloatingPlatform : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFloatingPlatform();

	/** Mesh for platform */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Platform)
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere)
	FVector StartPoint;
	UPROPERTY(EditAnywhere, meta = (MakeEditWidget = "true"))
	FVector EndPoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Platform)
	float InterpSpeed;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Platform)
	float InterpTime;

	FTimerHandle InterpTimer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Platform)
	bool bInterping;

	float Distance;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void ToggleInterping();

	void SwapVectors(FVector& VecOne, FVector& VecTwo);

};
