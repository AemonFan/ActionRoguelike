// Fill out your copyright notice in the Description page of Project Settings.


#include "SAttributeComponent.h"

// Sets default values for this component's properties
USAttributeComponent::USAttributeComponent()
{
	Health = 100.0f;
	MaxHealth = 100.0f;
	MinHealth = 0.0f;
	DangerousHealth = 20.0f;
}

// Called when the game starts
void USAttributeComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

bool USAttributeComponent::ApplyHealthChange(float Delta)
{
	if(Health <= 0 && Delta < 0.0f)
		return false;

	if(Health >= MaxHealth && Delta > 0.0f)
		return false;

	Health = FMath::Clamp(Health + Delta, 0.0f, MaxHealth);
	/*Health += Delta;
	if(Delta > 0.0f)
	{
		Health = Health > MaxHealth ? MaxHealth : Health;
	}
	else
	{
		Health = Health < 0 ? 0 : Health;
	}*/
	
	UE_LOG(LogTemp, Warning, TEXT("Health is %f"), Health);

	OnHealthChanged.Broadcast(nullptr, this, Health, Delta, MaxHealth, DangerousHealth);
	
	return true;
}

bool USAttributeComponent::IsCharacterAlive() const
{
	return Health > 0.0f;
}

bool USAttributeComponent::IsFullHeath()
{
	return Health == MaxHealth;
}

USAttributeComponent* USAttributeComponent::GetAttributes(AActor* FromActor)
{
	if(FromActor)
	{
		return Cast<USAttributeComponent>(FromActor->GetComponentByClass(USAttributeComponent::StaticClass()));
	}

	return nullptr;
}
