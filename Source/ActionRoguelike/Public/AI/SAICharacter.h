// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SAICharacter.generated.h"

class USActionComponent;
class USWorldUserWidget;
class USAttributeComponent;
class UPawnSensingComponent;

UCLASS()
class ACTIONROGUELIKE_API ASAICharacter : public ACharacter
{
	GENERATED_BODY()

public:
	
	ASAICharacter();

protected:

	UPROPERTY(EditAnywhere, Category="Components")
	UPawnSensingComponent* PawnSensingComp;

	UPROPERTY(EditAnywhere, Category="Components")
	USAttributeComponent* AttributeComp;

	UPROPERTY(EditAnywhere, Category="Action")
	USActionComponent* ActionComp;
	
	UPROPERTY(EditAnywhere, Category="Health")
	float CheckLowHealthInterval;

	UPROPERTY(VisibleAnywhere, Category="Effets")
	FName HitFlashParamName;

	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UUserWidget> UserWidgetClass;
	
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UUserWidget> PlayerSpottedWidgetClass;
	
	FTimerHandle TimerHandle_CheckLowHealth;

	UPROPERTY()
	USWorldUserWidget* UserWidgetUI;

	FName TargetActorKey;

public:
	
	UFUNCTION()
	void HealAISelf(float HealValue = 0.0f);

protected:
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	virtual void PostInitializeComponents() override;
	
	UFUNCTION()
	virtual void OnSeePawn(APawn* Pawn);

	UFUNCTION()
	void OnHealthValueChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastPawnSeen();
	
	void SetTargetActor(AActor* NewTargetActor);

	AActor* GetTargetActor() const;

	void ActorDead(AActor* Killer);
};
