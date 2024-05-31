// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SMonsterData.h"
#include "Engine/DataTable.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "GameFramework/GameModeBase.h"
#include "SGameModeBase.generated.h"

class UDataTable;
class USSaveGame;
class ASPowerup_Coin;
class UEnvQueryInstanceBlueprintWrapper;
class UEnvQuery;

USTRUCT(BlueprintType)
struct FMonsterInfoRow : public FTableRowBase
{
	GENERATED_BODY()

public:

	FMonsterInfoRow()
	{
		Weight = 1.0;
		SpawnCost = 10.0f;
		KillReward = 5.0f;
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USMonsterData* MonsterData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Weight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SpawnCost;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float KillReward;
};

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API ASGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	ASGameModeBase();

protected:

	UPROPERTY()
	USSaveGame* CurrentSaveGame;

	FString SlotName;
	
public:
	
	UPROPERTY(EditAnywhere, Category="AI")
	float SpawnBotMax;
	
	UPROPERTY(EditAnywhere, Category="AI")
	float SpawnTimerInterval;

	UPROPERTY(EditDefaultsOnly, Category="AI")
	UDataTable* MonsterTable;
	
	/*UPROPERTY(EditAnywhere, Category="AI")
	TSubclassOf<AActor> MinionClass;*/

	UPROPERTY(EditAnywhere, Category="AI")
	UEnvQuery* SpawnQueryBot;

	UPROPERTY(EditAnywhere, Category="AI")
	UCurveFloat* DifficultyCurve;
	
	FTimerHandle TimerHandle_SpawnMinion;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="AI")
	int CreditsPerKill;

	UPROPERTY(EditAnywhere, Category="Powerups")
	TArray<TSubclassOf<AActor>> SpawnPowerupClass;

	UPROPERTY(EditAnywhere, Category="Powerups")
	UEnvQuery* SpawnQueryCoin;

	UPROPERTY(EditDefaultsOnly, Category="Powerups")
	float RequiredPowerupDistance;

	UPROPERTY(EditDefaultsOnly, Category="Powerups")
	int DesiredPowerupCount;

public:
	
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	virtual void StartPlay() override;
	
	/** Signals that a player is ready to enter the game, which may start it up */
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

	UFUNCTION(BlueprintCallable, Category="SaveGame")
	void WriteSaveGame();

	void LoadSaveGame();

	UFUNCTION(Exec, BlueprintCallable)
	void KillAllAI();

	void OnActorKilled(AActor* VictimActor, AActor* Killer);

protected:

	UFUNCTION()
	void OnRespawnActorElapsed(AController* RespawnController);
	
	UFUNCTION()
	void OnSpawnBotTimerElapsed();

	UFUNCTION()
	void OnSpawnBotQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);

	UFUNCTION()
	void OnSpawnPowerupQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);
};
