// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SProjectileBase.h"
#include "SDashProjectile.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API ASDashProjectile : public ASProjectileBase
{
	GENERATED_BODY()

public:

	ASDashProjectile();
	
protected:

	// 传送延迟
	UPROPERTY(EditDefaultsOnly, Category="Teleport")
	float TeleportDelay;

	// 爆炸延迟
	UPROPERTY(EditDefaultsOnly, Category="Teleport")
	float DetonateDelay;

	FTimerHandle TimerHandle_DelayedDetonate;

	FTimerHandle TimerHandle_DelayedTeleport;
	
	virtual void Explode_Implementation() override;

	virtual void BeginPlay() override;

	void TeleportInstigator();
};
