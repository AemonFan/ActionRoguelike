// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "GameFramework/GameModeBase.h"
#include "SGameModeBase.generated.h"

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
	
	virtual void StartPlay() override;

	UFUNCTION(Exec, BlueprintCallable)
	void KillAllAI();

	void OnActorKilled(AActor* RespawnActor, AActor* Killer);

protected:

	UFUNCTION()
	void OnRespawnActorElapsed(AController* RespawnController);
	
	UFUNCTION()
	void OnSpawnBotTimerElapsed();

	UFUNCTION()
	void OnQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus) ;
};
