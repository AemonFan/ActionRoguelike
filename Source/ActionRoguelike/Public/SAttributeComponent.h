// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SAttributeComponent.generated.h"

// 声明动态多播委托
DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FOnHealthChanged, AActor*, InstigatorActor, USAttributeComponent*, OwningComp, float, NewHealth, float, Delta, float, MaxHealth, float, DangerousHealth);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONROGUELIKE_API USAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	USAttributeComponent();

protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attributes")
	float Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attributes")
	float MinHealth;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attributes")
	float MaxHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attributes")
	float DangerousHealth;
	
public:
	
	UPROPERTY(BlueprintAssignable)
	FOnHealthChanged OnHealthChanged;
	
	UFUNCTION(BlueprintCallable, Category="Attributes")
	bool ApplyHealthChange(float Delta);

	UFUNCTION(BlueprintCallable, Category="Attributs")
	static USAttributeComponent* GetAttributes(AActor* FromActor);
	
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
};
