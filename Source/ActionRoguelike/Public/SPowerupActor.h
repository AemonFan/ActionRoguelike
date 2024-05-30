// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SGamePlayInterface.h"
#include "GameFramework/Actor.h"
#include "SPowerupActor.generated.h"

class USphereComponent;

UCLASS(ABSTRACT)
class ACTIONROGUELIKE_API ASPowerupActor : public AActor, public ISGamePlayInterface
{
	GENERATED_BODY()
	
public:	

	ASPowerupActor();

protected:

	UPROPERTY(VisibleAnywhere, Category="Components")
	USphereComponent* SphereComp;

	UPROPERTY(VisibleAnywhere, Category="Components")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(EditAnywhere, ReplicatedUsing="OnRep_IsActive", Category="Powerup");
	bool bIsActive;
	
	UPROPERTY(EditAnywhere, Category="Powerup");
	float RespawnTime;

	FTimerHandle TimerHandle_RespawnTime;
	
	UFUNCTION()
	void ShowPowerup();

	void HideAndCooldownPowerup();

	void SetPowerupState(bool bNewIsActive);
	
public:	

	virtual void Interact_Implementation(APawn* InstigatorPawn) override;

	virtual FText GetInteractMessageText_Implementation(APawn* InstigatorPawn) override;
	
	UFUNCTION()
	void OnRep_IsActive();
};
