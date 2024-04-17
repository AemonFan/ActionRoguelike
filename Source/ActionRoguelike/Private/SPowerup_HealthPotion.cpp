// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerup_HealthPotion.h"
#include "SAttributeComponent.h"
#include "SPlayerState.h"

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