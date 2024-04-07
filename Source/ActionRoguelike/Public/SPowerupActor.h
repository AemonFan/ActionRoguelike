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
	// Sets default values for this actor's properties
	ASPowerupActor();

protected:

	UPROPERTY(VisibleAnywhere, Category="Components")
	USphereComponent* SphereComp;

	UPROPERTY(VisibleAnywhere, Category="Components")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(EditAnywhere, Category="Powerup");
	bool bIsActive;
	
	UPROPERTY(EditAnywhere, Category="Powerup");
	float RespawnTime;

	FTimerHandle TimerHandle_RespawnTime;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void ShowPowerup();

	void HideAndCooldownPowerup();

	void SetPowerupState(bool bNewIsActive);
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Interact_Implementation(APawn* InstigatorPawn) override;

	virtual FText GetInteractText_Implementation(APawn* InstigatorPawn);

};
