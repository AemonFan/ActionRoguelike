// Fill out your copyright notice in the Description page of Project Settings.


#include "SWorldUserWidget.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/SizeBox.h"
#include "Kismet/GameplayStatics.h"


void USWorldUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if(!IsValid(AttachToActor))
	{
		RemoveFromParent();

		UE_LOG(LogTemp, Warning, TEXT("AttchToActor is nullptr, so remove WorldUserWidget."));
		
		return;
	}

	if(ParentSizeBox)
	{
		FVector2D ScreenPosition;

		bool bIsVisibleOnScreen = UGameplayStatics::ProjectWorldToScreen(GetOwningPlayer(), AttachToActor->GetActorLocation() + PositionOffset, ScreenPosition);
		if(bIsVisibleOnScreen)
		{
			float Scale = UWidgetLayoutLibrary::GetViewportScale(this);
		
			ScreenPosition /= Scale;

			ParentSizeBox->SetRenderTranslation(ScreenPosition);
		}

		ParentSizeBox->SetVisibility(bIsVisibleOnScreen ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	}
}
