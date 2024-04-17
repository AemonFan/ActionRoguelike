// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FCreditsValueChanged, APlayerState*, PlayerState, int, NewCredits, int, Delat);

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API ASPlayerState : public APlayerState
{
	GENERATED_BODY()

public:

	ASPlayerState();

protected:

	UPROPERTY(VisibleAnywhere, Category="Credits")
	int Credits;

	UPROPERTY(EditAnywhere, Category="Credits")
	int MaxDelat;

public:

	UPROPERTY(BlueprintAssignable, Category="Events")
	FCreditsValueChanged OnCreditsValueChange;
	
	UFUNCTION(BlueprintCallable, Category="Credits")
	int GetCredits()
	{
		return Credits;
	}

	UFUNCTION(BlueprintCallable, Category="Credits")
	bool RemoveCredits(int Delat);

	UFUNCTION(BlueprintCallable, Category="Credits")
	bool AddCredits(int Delat);

	UFUNCTION(BlueprintCallable, Category="Credits")
	static ASPlayerState* GetPlayerState(APawn* Pawn);
};
