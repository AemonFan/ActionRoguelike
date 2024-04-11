// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SInteractionComponent.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class USInteractionComponent;
class USAttributeComponent;

UCLASS()
class ACTIONROGUELIKE_API ASCharacter : public ACharacter
{
 	GENERATED_BODY()

public:

	ASCharacter();

protected:

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* CameraComp;

	UPROPERTY(EditAnywhere, Category="Attack")
	TSubclassOf<AActor> ProjectileClass;

	UPROPERTY(EditAnywhere, Category="Attack")
	TSubclassOf<AActor> BlackHoleProjectileClass;

	UPROPERTY(EditAnywhere, Category="Attack")
	TSubclassOf<AActor> DashProjectileClass;
	
	UPROPERTY(EditAnywhere, Category="Attack") 
	UAnimMontage* AttackAnim;
	
	FTimerHandle TimerHandle_PrimaryAttack;
	
	FTimerHandle TimerHandle_BlackHoleAttack;

	FTimerHandle TimerHanle_Dash;

	FTimerHandle TimerHandle_CharacterDead;
	
	float AttackAnimDelay;
	
	USInteractionComponent* InteractionComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Components")
	USAttributeComponent* AttributeComp;

	void MoveForward(float value);

	void MoveRight(float value);

	void JumpStart();

	void JumpEnd();

	void OpenTreasureChest();

	void SpawnProjectile(TSubclassOf<AActor> ClassToSpawn);
	
	// MagicProjectile
	void PrimaryAttack();

	void PrimaryAttack_TimerElapsed();

	// BlackHoleProjectile
	void BlackHoleAttack();

	void BlackHole_TimeElapsed();

	// DashProjectile
	void Dash();

	void Dash_TimeElapsed();

public:
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void PostInitializeComponents() override;

	UFUNCTION()
	void OnHealthValueChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta);
	
	UFUNCTION()
	void NotifyCharacterDead();
};
