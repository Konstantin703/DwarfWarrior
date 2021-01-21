#include "MainPlayerController.h"
#include "Blueprint/UserWidget.h"

void AMainPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (HUDOverlayAsset)
		HUDOverlay = CreateWidget<UUserWidget>(this, HUDOverlayAsset);

	HUDOverlay->AddToViewport();
	HUDOverlay->SetVisibility(ESlateVisibility::Visible);

	if (WEnemyHealthBar)
	{
		EnemyHealthBar = CreateWidget<UUserWidget>(this, WEnemyHealthBar);
		if (EnemyHealthBar)
		{
			EnemyHealthBar->AddToViewport();
			EnemyHealthBar->SetVisibility(ESlateVisibility::Hidden);
		}
		FVector2D Alignment(0.f, 0.f);
		EnemyHealthBar->SetAlignmentInViewport(Alignment);
	}

	WidgetLength = 150.f;
	WidgetWidth = 10.f;

	WidgetShiftX = WidgetLength / 2;
	WidgetShiftY = 100.f;	

	SizeInViewPort = FVector2D(WidgetLength, WidgetWidth);
}

void AMainPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (EnemyHealthBar)
	{
		FVector2D ViewportPosition;
		ProjectWorldLocationToScreen(EnemyLocation, ViewportPosition);
		ViewportPosition.X -= WidgetShiftX;
		ViewportPosition.Y -= WidgetShiftY;

		EnemyHealthBar->SetPositionInViewport(ViewportPosition);
		EnemyHealthBar->SetDesiredSizeInViewport(SizeInViewPort);
	}
}

void AMainPlayerController::DisplayEnemyHealthBar()
{
	if (EnemyHealthBar)
	{
		bEnemyHealthBarVisible = true;
		EnemyHealthBar->SetVisibility(ESlateVisibility::Visible);
	}
}

void AMainPlayerController::RemoveEnemyHealthBar()
{
	if (EnemyHealthBar)
	{
		bEnemyHealthBarVisible = false;
		EnemyHealthBar->SetVisibility(ESlateVisibility::Hidden);
	}
}
