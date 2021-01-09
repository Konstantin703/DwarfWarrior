#include "MainAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"

void UMainAnimInstance::NativeInitializeAnimation()
{
	if (!Pawn)
		Pawn = TryGetPawnOwner();
}

void UMainAnimInstance::UpdateAnimationProperties()
{
	if (!Pawn)
	{
		Pawn = TryGetPawnOwner();
	}
	else
	{
		FVector Speed = Pawn->GetVelocity();
		FVector LateralSpeed = FVector(Speed.X, Speed.Y, 0.f);
		MovementSpeed = LateralSpeed.Size();

		bIsInAir = Pawn->GetMovementComponent()->IsFalling();
	}
}