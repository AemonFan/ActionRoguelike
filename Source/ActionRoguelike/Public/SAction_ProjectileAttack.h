// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SAction.h"
#include "SAction_ProjectileAttack.generated.h"

class ASCharacter;
/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API USAction_ProjectileAttack : public USAction
{
	GENERATED_BODY()

public:
	USAction_ProjectileAttack();

	void StartAction_Implementation(AActor* InstigatorActor) override;
	
protected:
	
	UPROPERTY(EditAnywhere, Category="Attack")
	TSubclassOf<AActor> ProjectileClass;

	UPROPERTY(EditAnywhere, Category="Attack") 
	UAnimMontage* AttackAnim;

	float AttackAnimDelay;

	FTimerHandle TimerHandle_AttackDelay;

	UFUNCTION()
	void OnAttackDelay_TimerElapsed(ACharacter* InstigatorCharacter);
};
