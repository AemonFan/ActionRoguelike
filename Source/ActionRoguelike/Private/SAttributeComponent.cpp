// Fill out your copyright notice in the Description page of Project Settings.


#include "SAttributeComponent.h"

#include "SGameModeBase.h"
#include "Net/UnrealNetwork.h"

static TAutoConsoleVariable<float> CVarDamageMultiplier(TEXT("su.DamageMultiplier"), 1.0f, TEXT("Global Damage Modifier for Attribute Component."), ECVF_Cheat);

USAttributeComponent::USAttributeComponent()
{
	MaxHealth = 100.0f;
	Health = MaxHealth;
	DangerousHealth = 20.0f;

	Rage = 0.0f;
	RageMax = 10.0f;
	RageRate = 0.1f;

	SetIsReplicatedByDefault(true);
}

void USAttributeComponent::MulticastHealthChanged_Implementation(AActor* InstigatorActor, float NewHealth, float Delta)
{
	OnHealthChanged.Broadcast(InstigatorActor, this, Health, Delta);
}

bool USAttributeComponent::ApplyHealthChange(AActor* InstigatorActor, float Delta)
{
	if(!GetOwner()->CanBeDamaged() && Delta < 0.0f)
		return false;
	
	if(Health <= 0 && Delta < 0.0f)
		return false;

	if(Health >= MaxHealth && Delta > 0.0f)
		return false;

	// 伤害倍数
	if(Delta < 0.0f)
	{
		// su.DamageMultiplier : 伤害倍数，由控制台输入，仅用于开发测试使用
		float damageMultply = CVarDamageMultiplier.GetValueOnGameThread();
		Delta *= damageMultply;
		UE_LOG(LogTemp, Log, TEXT("damageMultply: %f, Delta:%f"), damageMultply, Delta);
	}

	float oldHealth = Health;
	float NewHealth = FMath::Clamp(Health + Delta, 0.0f, MaxHealth);

	// Run On Server
	if(GetOwner()->HasAuthority())
	{
		Health = NewHealth;

		UE_LOG(LogTemp, Log, TEXT("Health is %f"), Health);

		if(oldHealth != Health)
		{
			MulticastHealthChanged(InstigatorActor, Health, Delta);
		}

		if(Health == 0.0f)
		{
			ASGameModeBase* GM = GetWorld()->GetAuthGameMode<ASGameModeBase>();
			if (GM)
			{
				GM->OnActorKilled(GetOwner(), InstigatorActor);
			}
		}
	}
	
	return oldHealth != Health;
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

bool USAttributeComponent::ApplyRageChange(AActor* InstigatorActor, float Delta)
{
	if(Rage <= 0 && Delta < 0.0f)
		return false;

	if(Rage >= RageMax && Delta > 0.0f)
		return false;

	float oldRage = Rage;
	Rage = FMath::Clamp(Rage + Delta, 0.0f, RageMax);

	UE_LOG(LogTemp, Log, TEXT("Rage is %f"), Rage);

	if(oldRage != Rage)
	{
		OnRageChanged.Broadcast(InstigatorActor, this, Rage, Delta);
	}
	
	return oldRage != Rage;
}

void USAttributeComponent::RageGained(AActor* InstigatorActor, float Delta)
{
	ApplyRageChange(InstigatorActor, Delta);
}

bool USAttributeComponent::RageRequired(AActor* InstigatorActor, float Delta)
{
	if(Delta > Rage)
	{
		return false;
	}
	
	return ApplyRageChange(InstigatorActor, Delta * (-1));
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

void USAttributeComponent::GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USAttributeComponent, Health);
	DOREPLIFETIME(USAttributeComponent, MaxHealth);
	// DOREPLIFETIME_CONDITION(USAttributeComponent, MaxHealth, COND_InitialOnly);
	
	// DOREPLIFETIME(USAttributeComponent, Rage);
	// DOREPLIFETIME(USAttributeComponent, RageMax);
}