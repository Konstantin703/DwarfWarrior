#include "SpawnVolume.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "FirstProject/Enemies/EnemyBase.h"
#include "AIController.h"
#include "Components/BillboardComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ASpawnVolume::ASpawnVolume()
{
	DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Dummy"));
	SetRootComponent(DummyRoot);

	SpawnBillboard = CreateEditorOnlyDefaultSubobject<UBillboardComponent>(TEXT("SpawnBillboard"));
	SpawnBillboard->SetupAttachment(GetRootComponent());

	SpawnBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnBox"));
	SpawnBox->SetupAttachment(SpawnBillboard);

	TriggerBillboard = CreateEditorOnlyDefaultSubobject<UBillboardComponent>(TEXT("TriggerBillboard"));
	TriggerBillboard->SetupAttachment(GetRootComponent());
	
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetBoxExtent(FVector(100.f));
	TriggerBox->SetupAttachment(TriggerBillboard);

	bSpawnOnBeginPlay = false;
}

// Called when the game starts or when spawned
void ASpawnVolume::BeginPlay()
{
	Super::BeginPlay();	

	if (bSpawnOnBeginPlay)
	{
		SpawnActor(GetSpawnActor(), GetSpawnPoint());
		TriggerBox->DestroyComponent();		
	}		
	else
	{		
		TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ASpawnVolume::OnOverlapBegin);
	}		
}

FVector ASpawnVolume::GetSpawnPoint()
{
	FVector Extent = SpawnBox->GetScaledBoxExtent();
	FVector Origin = SpawnBox->GetComponentLocation();

	FVector SpawnPoint = UKismetMathLibrary::RandomPointInBoundingBox(Origin, Extent);
	return SpawnPoint;
}

void ASpawnVolume::SpawnActor(UClass* ToSpawn, const FVector& SpawnLocation)
{
	if (ToSpawn)
	{
		UWorld* World = GetWorld();
		FActorSpawnParameters SpawnParams;

		if (World)
		{
			AActor* SpawnedActor = World->SpawnActor<AActor>(ToSpawn, SpawnLocation, FRotator(0.f), SpawnParams);
			AEnemyBase* Enemy = Cast<AEnemyBase>(SpawnedActor);
			
			if (Enemy)
			{
				Enemy->SpawnDefaultController();
				AAIController* AIController = Cast<AAIController>(Enemy->GetController());

				if (AIController)
					Enemy->SetAIController(AIController);

				if (SpawnParticle)
					UGameplayStatics::SpawnEmitterAtLocation(World, SpawnParticle, SpawnLocation);
			}			
		}
	}
}

TSubclassOf<AActor> ASpawnVolume::GetSpawnActor()
{
	if (SpawnArray.Num() > 0)
	{
		int32 Selection = FMath::RandRange(0, SpawnArray.Num() - 1);
		return SpawnArray[Selection];
	}

	return nullptr;
}

void ASpawnVolume::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult
)
{
	if (OtherActor)
		SpawnActor(GetSpawnActor(), GetSpawnPoint());
}
