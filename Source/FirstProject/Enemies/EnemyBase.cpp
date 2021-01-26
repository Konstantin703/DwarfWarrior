#include "EnemyBase.h"
#include "Components/SphereComponent.h"
#include "AIController.h"
#include "FirstProject/MainCharacterBase.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/BoxComponent.h"
#include "FirstProject/MainCharacterBase.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Sound/SoundCue.h"
#include "Animation/AnimInstance.h"
#include "TimerManager.h"
#include "Components/CapsuleComponent.h"
#include "EnemyAnimInstance.h"
#include "FirstProject/MainPlayerController.h"
#include "Components/CapsuleComponent.h"

// Sets default values
AEnemyBase::AEnemyBase()
{
	//GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);

 	AgroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AgroSphere"));
	AgroSphere->SetupAttachment(GetRootComponent());
	AgroSphere->InitSphereRadius(600.f);

	CombatSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatSphere"));
	CombatSphere->SetupAttachment(GetRootComponent());
	CombatSphere->InitSphereRadius(75.f);

	CombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CombatCollision"));
	CombatCollision->SetupAttachment(GetMesh(), FName("EnemySocket"));
	
	bOverlappingCombatSphere = false;

	MaxHealth = 50.f;
	Health = MaxHealth;
	Damage = 10.f;

	AttackMinTime = 0.5f;
	AttackMaxTime = 3.5f;

	bAttacking = false;

	SetState(EEnemyState::EES_Idle);

	DeathDelay = 3.f;
}

// Called when the game starts or when spawned
void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	AIController = Cast<AAIController>(GetController());

	AgroSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemyBase::AgroOnOverlapBegin);
	AgroSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemyBase::AgroOnOverlapEnd);

	CombatSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemyBase::CombatSphereOnOverlapBegin);
	CombatSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemyBase::CombatSphereOnOverlapEnd);

	CombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemyBase::CombatOnOverlapBegin);
	CombatCollision->OnComponentEndOverlap.AddDynamic(this, &AEnemyBase::CombatOnOverlapEnd);

	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	CombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);

	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	AgroSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);

	CombatSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);
}

// Called every frame
void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemyBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemyBase::AgroOnOverlapBegin(
	UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult
)
{
	if (OtherActor && IsAlive())
	{
		AMainCharacterBase* MainCharacter = Cast<AMainCharacterBase>(OtherActor);
		if (MainCharacter)
		{
			MainCharacter->SetCombatTarget(this);
			MoveToTarget(MainCharacter);
		}			
	}
}

void AEnemyBase::AgroOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && IsAlive())
	{
		AMainCharacterBase* MainCharacter = Cast<AMainCharacterBase>(OtherActor);
		if (MainCharacter)
		{
			if (MainCharacter->GetCombatTarget() == this)
				MainCharacter->SetCombatTarget(nullptr);

			MainCharacter->UpdateCombatTarget();

			SetState(EEnemyState::EES_Idle);
			if (AIController)
				AIController->StopMovement();
		}			
	}
}

void AEnemyBase::CombatSphereOnOverlapBegin(
	UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult
)
{
	if (OtherActor && IsAlive())
	{
		AMainCharacterBase* MainCharacter = Cast<AMainCharacterBase>(OtherActor);
		if (MainCharacter)
		{
			MainCharacter->SetCombatTarget(this);
			MainCharacter->UpdateCombatTarget();

			SetCombatTarget(MainCharacter);
			bOverlappingCombatSphere = true;

			SetAttackDelay();
		}			
	}
}

void AEnemyBase::CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && IsAlive() && OtherComp)
	{
		AMainCharacterBase* MainCharacter = Cast<AMainCharacterBase>(OtherActor);
		if (MainCharacter)
		{
			bOverlappingCombatSphere = false;
			MoveToTarget(MainCharacter);
			SetCombatTarget(nullptr);

			if (MainCharacter->GetCombatTarget() == this)
			{
				MainCharacter->SetCombatTarget(nullptr);
				MainCharacter->UpdateCombatTarget();
			}

			if (MainCharacter->GetPlayerController())
			{
				USkeletalMeshComponent* MainCharacterMesh = Cast<USkeletalMeshComponent>(OtherComp);
				if (MainCharacterMesh)
					MainCharacter->GetPlayerController()->RemoveEnemyHealthBar();
			}
				
			GetWorldTimerManager().ClearTimer(AttackTimer);
		}			
	}
}

void AEnemyBase::MoveToTarget(AMainCharacterBase* Target)
{
	SetState(EEnemyState::EES_MoveToTarget);

	if (AIController && Target->IsAlive())
	{
		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalActor(Target);
		MoveRequest.SetAcceptanceRadius(20.f);

		FNavPathSharedPtr NavPath;

		AIController->MoveTo(MoveRequest, &NavPath);

		/** Debug path
		TArray<FNavPathPoint> PathPoints = NavPath->GetPathPoints();
		for (FNavPathPoint Point : PathPoints)
		{
			FVector Location = Point.Location;
			UKismetSystemLibrary::DrawDebugSphere(this, Location, 25.f, 8, FLinearColor::Green, 10.f, 0.5f);
		}
		*/
	}
}

void AEnemyBase::CombatOnOverlapBegin(
	UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult
)
{
	if (OtherActor && IsAlive())
	{
		AMainCharacterBase* MainCharater = Cast<AMainCharacterBase>(OtherActor);
		if (MainCharater)
		{
			if (MainCharater->GetHitParticles())
			{
				const USkeletalMeshSocket* TipSocket = GetMesh()->GetSocketByName("TipSocket");
				if (TipSocket)
				{
					FVector SocketLocation = TipSocket->GetSocketLocation(GetMesh());
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MainCharater->GetHitParticles(), SocketLocation, FRotator(0.f), false);
				}
			}

			if (MainCharater->GetHitSound())
				UGameplayStatics::PlaySound2D(this, MainCharater->GetHitSound());

			if (DamageTypeClass)
				UGameplayStatics::ApplyDamage(MainCharater, Damage, AIController, this, DamageTypeClass);
		
		}
	}
}

void AEnemyBase::CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void AEnemyBase::ActivateAttackCollision()
{
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AEnemyBase::DeactivateAttackCollision()
{
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemyBase::Attack()
{
	if (IsAlive() && GetCombatTarget())
	{
		if (AIController)
		{
			AIController->StopMovement();
			SetState(EEnemyState::EES_Attacking);
		}

		if (!bAttacking)
		{
			bAttacking = true;
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
			if (AnimInstance)
			{
				AnimInstance->Montage_Play(CombatMontage, 1.35f);
				AnimInstance->Montage_JumpToSection(FName("Attack"), CombatMontage);
			}
		}
	}	
}

void AEnemyBase::AttackEnd()
{
	bAttacking = false;

	if (bOverlappingCombatSphere)
		SetAttackDelay();
}

void AEnemyBase::PlaySwingSound()
{
	if (GetSwingSound())
		UGameplayStatics::PlaySound2D(this, GetSwingSound());
}

float AEnemyBase::TakeDamage(float DamageAmount, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	if (Health - DamageAmount <= 0.f)
	{
		Health = 0.f;
		Die(DamageCauser);
	}
	else
	{
		Health -= DamageAmount;
	}		

	return DamageAmount;
}

void AEnemyBase::Die(AActor* DeathCauser)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Play(CombatMontage, 1.35f);
		AnimInstance->Montage_JumpToSection(FName("Death"), CombatMontage);
	}
		
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AgroSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	bAttacking = false;

	SetState(EEnemyState::EES_Dead);
}

void AEnemyBase::DeathEnd()
{
	GetMesh()->bPauseAnims = true;
	GetMesh()->bNoSkeletonUpdate = true;

	GetWorldTimerManager().SetTimer(DeathTimer, this, &AEnemyBase::Disappear, DeathDelay);
}

bool AEnemyBase::IsAlive()
{
	return GetState() != EEnemyState::EES_Dead;
}

void AEnemyBase::Disappear()
{
	Destroy();
}

void AEnemyBase::SetAttackDelay()
{
	float AttackTime = FMath::RandRange(AttackMinTime, AttackMaxTime);
	GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemyBase::Attack, AttackTime);
}
