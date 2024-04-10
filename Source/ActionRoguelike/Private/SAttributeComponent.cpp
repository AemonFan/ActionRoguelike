// Fill out your copyright notice in the Description page of Project Settings.


#include "SAttributeComponent.h"

USAttributeComponent::USAttributeComponent()
{
	Health = 100.0f;
	MaxHealth = 100.0f;
	MinHealth = 0.0f;
	DangerousHealth = 20.0f;
}

bool USAttributeComponent::ApplyHealthChange(float Delta)
{
	if(Health <= 0 && Delta < 0.0f)
		return false;

	if(Health >= MaxHealth && Delta > 0.0f)
		return false;

	Health = FMath::Clamp(Health + Delta, 0.0f, MaxHealth);

	UE_LOG(LogTemp, Warning, TEXT("Health is %f"), Health);

	OnHealthChanged.Broadcast(nullptr, this, Health, Delta, MaxHealth, DangerousHealth);
	
	return true;
}

USAttributeComponent* USAttributeComponent::GetAttributes(AActor* FromActor)
{
	if(FromActor)
	{
		return Cast<USAttributeComponent>(FromActor->GetComponentByClass(USAttributeComponent::StaticClass()));
	}

	return nullptr;
}
