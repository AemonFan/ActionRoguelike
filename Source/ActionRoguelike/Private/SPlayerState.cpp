// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerState.h"

#include "SCharacter.h"

ASPlayerState::ASPlayerState()
{
	Credits = 0;

	MaxDelat = 100;
}

bool ASPlayerState::RemoveCredits(int Delat)
{
	if(Credits >= Delat)
	{
		Credits -= Delat;

		OnCreditsValueChange.Broadcast(this, Credits, Delat * (-1));
		
		return true;
	}

	return false;
}

bool ASPlayerState::AddCredits(int Delat)
{
	if(Credits < MaxDelat)
	{
		Credits += Delat;

		OnCreditsValueChange.Broadcast(this, Credits, Delat);
		
		return true;
	}

	return false;
}

ASPlayerState* ASPlayerState::GetPlayerState(APawn* Pawn)
{
	if(ensure(Pawn))
	{ 
		return Pawn->GetPlayerState<ASPlayerState>();
	}

	return nullptr;
}
