// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerup_Coin.h"
#include "SPlayerState.h"

ASPowerup_Coin::ASPowerup_Coin()
{
	CreditAmount = 5;
}

void ASPowerup_Coin::Interact_Implementation(APawn* InstigatorPawn)
{
	Super::Interact_Implementation(InstigatorPawn);

	if(!ensure(InstigatorPawn))
		return;

	ASPlayerState* PlayerState = ASPlayerState::GetPlayerState(InstigatorPawn);
	if(PlayerState)
	{
		PlayerState->AddCredits(CreditAmount);

		HideAndCooldownPowerup();
	}
}
