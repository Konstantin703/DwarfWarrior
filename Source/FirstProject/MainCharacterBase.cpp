#include "MainCharacterBase.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AMainCharacterBase::AMainCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

	// create camera boom (pulls towards the character in the case of collision
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 600.f; // distance to character
	CameraBoom->bUsePawnControlRotation = true;

	// Set size for collision capsule
	GetCapsuleComponent()->SetCapsuleSize(46.f, 100.f);

	// create follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// Attach the camera to the end of the boom, and let the boom adjust to match 
	// the controller orientation
	FollowCamera->bUsePawnControlRotation = false;

	// Set turn rates for input
	BaseTurnRate = 65.f;
	BaseLookUpRate = 65.f;

	// Don't rotate when controller rotates
	// Let that just affect camera
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	// configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // character moves in the dirrection of input...
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f); // ... at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 650.f;
	GetCharacterMovement()->AirControl = 0.2f;
}

// Called when the game starts or when spawned
void AMainCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMainCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMainCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMainCharacterBase::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMainCharacterBase::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAxis("TurnRate", this, &AMainCharacterBase::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMainCharacterBase::LookUpAtRate);

}

void AMainCharacterBase::MoveForward(float InValue)
{
	if (Controller && InValue != 0.f)
	{
		// Find forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, InValue);
	}
}

void AMainCharacterBase::MoveRight(float InValue)
{
	if (Controller && InValue != 0.f)
	{
		// Find right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, InValue);
	}
}

void AMainCharacterBase::TurnAtRate(float InRate)
{
	AddControllerYawInput(InRate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMainCharacterBase::LookUpAtRate(float InRate)
{
	AddControllerPitchInput(InRate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}
