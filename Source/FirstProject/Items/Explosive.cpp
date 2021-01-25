#include "Explosive.h"
#include "FirstProject/MainCharacterBase.h"
#include "FirstProject/Enemies/EnemyBase.h"
#include "Kismet/GameplayStatics.h"

AExplosive::AExplosive()
{
	Damage = 25.f;
}

void AExplosive::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult
)
{
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if (OtherActor)
	{
		AMainCharacterBase* MainCharacter = Cast<AMainCharacterBase>(OtherActor);
		AEnemyBase* Enemy = Cast<AEnemyBase>(OtherActor);

		if (MainCharacter || Enemy)
			UGameplayStatics::ApplyDamage(OtherActor, Damage, nullptr, this, DamageTypeClass);
		
		Destroy();
	}
}

void AExplosive::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}
