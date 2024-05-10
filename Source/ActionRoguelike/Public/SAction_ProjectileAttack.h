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

	virtual void StartAction_Implementation(AActor* InstigatorActor) override;
	
protected:
	
	UPROPERTY(EditAnywhere, Category="Attack")
	TSubclassOf<AActor> ProjectileClass;

	UPROPERTY(EditAnywhere, Category="Attack") 
	UAnimMontage* AttackAnim;
	
	UPROPERTY(EditDefaultsOnly, Category="Attack")
	float AttackAnimDelay;

	UPROPERTY(VisibleAnywhere, Category="Effect")
	FName HandSocketName;

	UPROPERTY(EditDefaultsOnly, Category="Effect")
	UParticleSystem* CastingEffect;
	
	UFUNCTION()
	void OnAttackDelay_TimerElapsed(ACharacter* InstigatorCharacter);
};
