// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerState.h"

#include "SCharacter.h"
#include "SSaveGame.h"
#include "Net/UnrealNetwork.h"

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

		OnCreditsValueChange.Broadcast(this, Credits, Delat);
		
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

void ASPlayerState::OnRep_Credits(int32 OldCredits)
{
	OnCreditsValueChange.Broadcast(this, Credits, Credits - OldCredits);
}

void ASPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASPlayerState, Credits);
}

void ASPlayerState::SavePlayerState_Implementation(USSaveGame* InSaveGameObject)
{
	if(InSaveGameObject)
	{
		InSaveGameObject->Credits = Credits;
	}
}

void ASPlayerState::LoadPlayerState_Implementation(USSaveGame* InSaveGameObject)
{
	if(InSaveGameObject)
	{
		//Credits = InSaveGameObject->Credits;

		AddCredits(InSaveGameObject->Credits);
	}
}
