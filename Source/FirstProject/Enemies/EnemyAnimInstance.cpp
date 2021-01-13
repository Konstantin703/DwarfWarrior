#include "EnemyAnimInstance.h"
#include "EnemyBase.h"

void UEnemyAnimInstance::NativeInitializeAnimation()
{
	if (!Pawn)
	{
		Pawn = TryGetPawnOwner();
		if (Pawn)
		{
			Enemy = Cast<AEnemyBase>(Pawn);
		}
	}
}


void UEnemyAnimInstance::UpdateAnimationProperties()
{
	if (!Pawn)
	{
		Pawn = TryGetPawnOwner();
		if (Pawn)
		{
			FVector Speed = Pawn->GetVelocity();
			FVector LateralSpeed = FVector(Speed.X, Speed.Y, 0.f);
			MovementSpeed = LateralSpeed.Size();

			if (!Enemy)
				Enemy = Cast<AEnemyBase>(Pawn);
		}
	}
}