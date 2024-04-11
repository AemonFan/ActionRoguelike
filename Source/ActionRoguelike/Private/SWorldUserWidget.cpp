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

	FVector2D ScreenPosition;
	if(UGameplayStatics::ProjectWorldToScreen(GetOwningPlayer(), AttachToActor->GetActorLocation() + PositionOffset, ScreenPosition))
	{
		float Scale = UWidgetLayoutLibrary::GetViewportScale(this);
		
		ScreenPosition /= Scale;

		ParentSizeBox->SetRenderTranslation(ScreenPosition);
	}
}
