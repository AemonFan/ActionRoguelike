// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerup_HealthPotion.h"
#include "SAttributeComponent.h"

ASPowerup_HealthPotion::ASPowerup_HealthPotion()
{
	CreditCost = 50.0f;

	HealthHeal = 30.0f;
}

void ASPowerup_HealthPotion::Interact_Implementation(APawn* InstigatorPawn)
{
	Super::Interact_Implementation(InstigatorPawn);

	if(!ensure(InstigatorPawn))
	{
		return;
	}

	USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(InstigatorPawn);
	if(ensure(AttributeComp) && !AttributeComp->IsFullHeath())
	{
		if(AttributeComp->ApplyHealthChange(HealthHeal))
		{
			HideAndCooldownPowerup();
		}
	}
}

FText ASPowerup_HealthPotion::GetInteractText_Implementation(APawn* InstigatorPawn)
{
	/*USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(InstigatorPawn);
	if(AttributeComp && AttributeComp->IsFullHeath())
	{
		return LOCTEXT("HealthPotion_FullHealthWarning", "Already at full health");
	}
	return FText::Format(LOCTEXT("HealtPotion_InteractMessage", "Cost {0} Credits. Restores health to maximum."), CreditCost);*/

	return FText::GetEmpty();
}
