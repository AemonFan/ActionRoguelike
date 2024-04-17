// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SAICharacter.generated.h"

class USWorldUserWidget;
class USAttributeComponent;
class UPawnSensingComponent;

UCLASS()
class ACTIONROGUELIKE_API ASAICharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASAICharacter();

	UFUNCTION()
	void HealAISelf(float HealValue = 0.0f);

protected:

	UPROPERTY(EditAnywhere, Category="Components")
	UPawnSensingComponent* PawnSensingComp;

	UPROPERTY(EditAnywhere, Category="Components")
	USAttributeComponent* AttributeComp;

	UPROPERTY(EditAnywhere, Category="Health")
	float CheckLowHealthInterval;

	UPROPERTY(VisibleAnywhere, Category="Effets")
	FName HitFlashParamName;

	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UUserWidget> UserWidgetClass;
	
	FTimerHandle TimerHandle_CheckLowHealth;
	
	USWorldUserWidget* UserWidgetUI;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	virtual void PostInitializeComponents() override;
	
	UFUNCTION()
	virtual void OnSeePawn(APawn* Pawn);

	UFUNCTION()
	void OnHealthValueChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta);

	void SetTargetActor(AActor* NewTargetActor);

	void ActorDead(AActor* Killer);
};
