#include "SpawnVolume.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
//#include "Critter.h"

// Sets default values
ASpawnVolume::ASpawnVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpawnBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnBox"));

}

// Called when the game starts or when spawned
void ASpawnVolume::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASpawnVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FVector ASpawnVolume::GetSpawnPoint()
{
	FVector Extent = SpawnBox->GetScaledBoxExtent();
	FVector Origin = SpawnBox->GetComponentLocation();

	FVector SpawnPoint = UKismetMathLibrary::RandomPointInBoundingBox(Origin, Extent);
	return SpawnPoint;
}

void ASpawnVolume::SpawnPawn_Implementation(UClass* ToSpawn, const FVector& SpawnLocation)
{
	if (ToSpawn)
	{
		UWorld* World = GetWorld();
		FActorSpawnParameters SpawnParams;

		/*if (World)
			ACritter* SpawnedCritter = World->SpawnActor<ACritter>(ToSpawn, SpawnLocation, FRotator(0.f), SpawnParams);*/

	}
}
