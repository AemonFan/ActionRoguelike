// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "GameFramework/GameModeBase.h"
#include "SGameModeBase.generated.h"

class ASPowerup_Coin;
class UEnvQueryInstanceBlueprintWrapper;
class UEnvQuery;
/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API ASGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	ASGameModeBase();

	UPROPERTY(EditAnywhere, Category="AI")
	float SpawnBotMax;
	
	UPROPERTY(EditAnywhere, Category="AI")
	float SpawnTimerInterval;

	UPROPERTY(EditAnywhere, Category="AI")
	TSubclassOf<AActor> MinionClass;

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

	virtual void StartPlay() override;

	UFUNCTION(Exec, BlueprintCallable)
	void KillAllAI();

	void OnActorKilled(AActor* RespawnActor, AActor* Killer);

	void OnAIActorKilled(AActor* InstigatorActor);
	
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
