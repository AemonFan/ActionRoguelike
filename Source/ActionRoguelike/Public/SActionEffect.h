// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SAction.h"
#include "SActionEffect.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API USActionEffect : public USAction
{
	GENERATED_BODY()

public:

	USActionEffect();

	virtual void StartAction_Implementation(AActor* InstigatorActor) override;

	virtual void StopAction_Implementation(AActor* InstigatorActor) override;

protected:

	UPROPERTY(EditDefaultsOnly, Category="Action")
	float Duration;

	UPROPERTY(EditDefaultsOnly, Category="Action")
	float Period;

	FTimerHandle Duration_TimerHandle;

	FTimerHandle Period_TimerHandle;
	
	UFUNCTION(BlueprintNativeEvent, Category="Action")
	void ExecutePeriodicEffect(AActor* InstigatorActor);
};
