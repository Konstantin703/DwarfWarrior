#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainPlayerController.generated.h"

class UUserWidget;

UCLASS()
class FIRSTPROJECT_API AMainPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	/** Reference to the UMG asset in the editor */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UUserWidget> HUDOverlayAsset;
	/** Variable to hold the widget after creating */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	UUserWidget* HUDOverlay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UUserWidget> WEnemyHealthBar;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Widgets")
	UUserWidget* EnemyHealthBar;

	bool bEnemyHealthBarVisible;

	FVector EnemyLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Widgets")
	float WidgetShift = 100.f;

	void DisplayEnemyHealthBar();
	void RemoveEnemyHealthBar();

	FORCEINLINE void SetEnemyLocation(FVector InLocation) { EnemyLocation = InLocation; }

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
	
};
