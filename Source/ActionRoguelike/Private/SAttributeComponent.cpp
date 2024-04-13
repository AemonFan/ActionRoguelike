// Fill out your copyright notice in the Description page of Project Settings.


#include "SAttributeComponent.h"

static TAutoConsoleVariable<float> CVarDamageMultiplier(TEXT("su.DamageMultiplier"), 1.0f, TEXT("Global Damage Modifier for Attribute Component."), ECVF_Cheat);

USAttributeComponent::USAttributeComponent()
{
	Health = 100.0f;
	MaxHealth = 100.0f;
	MinHealth = 0.0f;
	DangerousHealth = 20.0f;
}

bool USAttributeComponent::ApplyHealthChange(AActor* InstigatorActor, float Delta)
{
	if(!GetOwner()->CanBeDamaged())
		return false;
	
	if(Health <= 0 && Delta < 0.0f)
		return false;

	if(Health >= MaxHealth && Delta > 0.0f)
		return false;

	// 伤害倍数
	if(Delta < 0.0f)
	{
		float damageMultply = CVarDamageMultiplier.GetValueOnGameThread();
		Delta *= damageMultply;
		UE_LOG(LogTemp, Warning, TEXT("damageMultply: %f, Delta:%f"), damageMultply, Delta);
	}
	
	Health = FMath::Clamp(Health + Delta, 0.0f, MaxHealth);

	UE_LOG(LogTemp, Warning, TEXT("Health is %f"), Health);

	OnHealthChanged.Broadcast(InstigatorActor, this, Health, Delta);
	
	return true;
}

bool USAttributeComponent::KillSelf(AActor* InstigatorActor)
{
	return ApplyHealthChange(InstigatorActor, GetMaxHealth() * (-1));
}

bool USAttributeComponent::HealSelf(AActor* InstigatorActor, float HealValue /*= 0.0f*/)
{
	if(HealValue == 0.0f)
	{
		HealValue = GetMaxHealth();
	}
	return ApplyHealthChange(InstigatorActor, HealValue);
}

USAttributeComponent* USAttributeComponent::GetAttributes(AActor* FromActor)
{
	if(FromActor)
	{
		return Cast<USAttributeComponent>(FromActor->GetComponentByClass(USAttributeComponent::StaticClass()));
	}

	return nullptr;
}

bool USAttributeComponent::IsActorAlive(AActor* FromActor)
{
	USAttributeComponent* AttributeComp = GetAttributes(FromActor);
	if(AttributeComp)
	{
		return AttributeComp->IsAlive();
	}

	return false;
}
