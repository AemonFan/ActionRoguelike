// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerup_HealthPotion.h"
#include "SAttributeComponent.h"
#include "SPlayerState.h"

#define LOCTEXT_NAMESPACE "InteractableActors"

ASPowerup_HealthPotion::ASPowerup_HealthPotion()
{
	CreditCost = 10;

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
	if(!ensure(AttributeComp) || AttributeComp->IsFullHeath())
	{
		return;
	}

	ASPlayerState* PlayerState = ASPlayerState::GetPlayerState(InstigatorPawn);
	if(!PlayerState || PlayerState->GetCredits() < CreditCost)
	{
		return;
	}
	
	if(AttributeComp->ApplyHealthChange(InstigatorPawn, HealthHeal))
	{
		PlayerState->RemoveCredits(CreditCost);
		HideAndCooldownPowerup();
	}
}

FText ASPowerup_HealthPotion::GetInteractMessageText_Implementation(APawn* InstigatorPawn)
{
	USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(InstigatorPawn);
	if(!AttributeComp || AttributeComp->IsFullHeath())
	{
		return LOCTEXT("HealthPotion_FullHealthWarning", "Already at full health.");
	}
	return FText::Format(LOCTEXT("HealthPotion_InteractMessage", "Cost {0} Credits. Add health {1}."), CreditCost, HealthHeal);
}


#undef LOCTEXT_NAMESPACE
