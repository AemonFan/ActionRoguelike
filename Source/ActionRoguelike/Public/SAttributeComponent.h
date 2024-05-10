// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SAttributeComponent.generated.h"

// 声明动态多播委托
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnHealthChanged, AActor*, InstigatorActor, USAttributeComponent*, OwningComp, float, NewHealth, float, Delta);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnRageChanged, AActor*, InstigatorActor, USAttributeComponent*, OwningComp, float, NewRage, float, Delta);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONROGUELIKE_API USAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	USAttributeComponent();

	UFUNCTION(BlueprintCallable, Category="Attributs")
	static USAttributeComponent* GetAttributes(AActor* FromActor);
	
	UFUNCTION(BlueprintCallable, Category="Attributs", meta=(DisplayName="IsAlive"))
	static bool IsActorAlive(AActor* FromActor);
	
protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category="Attributes")
	float Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category="Attributes")
	float MaxHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attributes")
	float DangerousHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attributes")
	float Rage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attributes")
	float RageMax;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attributes")
	float RageRate;
	
public:
	
	UPROPERTY(BlueprintAssignable, Category="Attributes")
	FOnHealthChanged OnHealthChanged;

	UFUNCTION(NetMulticast, Reliable) // @FIXME: mark as unreliable once we moved the 'state' out of scharacter
	void MulticastHealthChanged(AActor* InstigatorActor, float NewHealth, float Delta);
	
	UFUNCTION(BlueprintCallable, Category="Attributes")
	bool ApplyHealthChange(AActor* InstigatorActor, float Delta);

	UFUNCTION(BlueprintCallable, Category="Attributes")
	bool IsAlive() const
	{
		return Health > 0.0f;
	}

	UFUNCTION(BlueprintCallable, Category="Attributes")
	bool IsFullHeath() const
	{
		return Health == MaxHealth;
	}
	
	UFUNCTION(BlueprintCallable, Category="Attributes")
	float GetHealth() const
	{
		return Health;
	}

	UFUNCTION(BlueprintCallable, Category="Attributes")
	bool IsInLowHealth() const
	{
		return Health <= DangerousHealth;
	}

	UFUNCTION(BlueprintCallable, Category="Attributes")
	float GetMaxHealth() const
	{
		return MaxHealth;
	}

	UFUNCTION(BlueprintCallable, Category="Attributes")
	bool KillSelf(AActor* InstigatorActor);

	UFUNCTION(BlueprintCallable, Category="Attributes")
	bool HealSelf(AActor* InstigatorActor, float HealValue = 0.0f);

	UPROPERTY(BlueprintAssignable, Category="Attributes")
	FOnRageChanged OnRageChanged;
	
	UFUNCTION(BlueprintCallable, Category="Attributes")
	bool ApplyRageChange(AActor* InstigatorActor, float Delta);
	
	UFUNCTION(BlueprintCallable, Category="Attributes")
	void RageGained(AActor* InstigatorActor, float Delta);
	
	UFUNCTION(BlueprintCallable, Category="Attributes")
	bool RageRequired(AActor* InstigatorActor, float Delta);

	UFUNCTION(BlueprintCallable, Category="Attributes")
	float GetRage() const
	{
		return Rage;
	}

	UFUNCTION(BlueprintCallable, Category="Attributes")
	float GetRageRate() const
	{
		return RageRate;
	}
};
