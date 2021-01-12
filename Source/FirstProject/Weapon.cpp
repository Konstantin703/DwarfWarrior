#include "Weapon.h"
#include "MainCharacterBase.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

AWeapon::AWeapon()
{
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(GetRootComponent());

	bWeaponParticle = false;

	WeaponState = EWeaponState::EWS_Pickup;
}

void AWeapon::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult
)
{
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if ( (WeaponState == EWeaponState::EWS_Pickup) && OtherActor)
	{
		AMainCharacterBase* MainCharacter = Cast<AMainCharacterBase>(OtherActor);
		if (MainCharacter)
		{
			MainCharacter->SetActiveOverlappingItem(this);
		}			
	}
}

void AWeapon::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);

	if (OtherActor)
	{
		AMainCharacterBase* MainCharacter = Cast<AMainCharacterBase>(OtherActor);
		if (MainCharacter)
		{
			MainCharacter->SetActiveOverlappingItem(nullptr);
		}
	}	
}

void AWeapon::Equip(AMainCharacterBase* Character)
{
	if (Character)
	{
		SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
		SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
		SkeletalMesh->SetSimulatePhysics(false);

		const USkeletalMeshSocket* RightHandSocket = Character->GetMesh()->GetSocketByName("RightHandSocket");

		if (RightHandSocket)
		{			
			bRotate = false;
			RightHandSocket->AttachActor(this, Character->GetMesh());
			Character->SetEquippedWeapon(this);
			Character->SetActiveOverlappingItem(nullptr);
		}

		if (OnEquipSound)
			UGameplayStatics::PlaySound2D(this, OnEquipSound);

		if (!bWeaponParticle)
			IdleParticlesComponent->Deactivate();
	}
}
