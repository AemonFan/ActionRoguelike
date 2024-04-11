// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SWorldUserWidget.generated.h"

class USizeBox;
/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API USWorldUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadOnly, Category="UI")
	AActor* AttachToActor;

	UPROPERTY(meta=(BindWidget))
	USizeBox* ParentSizeBox;
	
	UPROPERTY(EditAnywhere, Category="UI")
	FVector PositionOffset = FVector(0, 0, 50);
	
protected:
	
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

};
