// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SInteractionComponent.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

class USActionComponent;
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
	
	UPROPERTY(VisibleAnywhere, Category="Components")
	USInteractionComponent* InteractionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	USAttributeComponent* AttributeComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	USActionComponent* ActionComp;

	FTimerHandle TimerHandle_CharacterDead;

	UPROPERTY(VisibleAnywhere, Category="Projectile")
	FName SwitchProjectileClass;
		
	void MoveForward(float value);

	void MoveRight(float value);

	void JumpStart();

	void JumpEnd();

	void StartSprint();

	void StopSprint();

	void OpenTreasureChest();

	void ProjectileAttack();
	
	void SwitchAttack_MagicProjectile()
	{
		SwitchProjectileClass = "PrimaryAttack";
	}

	void SwitchAttack_BlackHoleProjectile()
	{
		SwitchProjectileClass = "BlackHole";
	}

	void SwitchAttack_DashProjectile()
	{
		SwitchProjectileClass = "Dash";
	}

public:
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void PostInitializeComponents() override;

	UFUNCTION()
	void OnHealthValueChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta);
	
	UFUNCTION()
	void OnActorDead(AActor* Killer);

	UFUNCTION(Exec)
	void HealSelf(float Amount = 100.0f);

	virtual FVector GetPawnViewLocation() const override;
};
