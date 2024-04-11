// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameModeBase.h"

#include "EngineUtils.h"
#include "SAttributeComponent.h"
#include "AI/SAICharacter.h"
#include "EnvironmentQuery/EnvQueryManager.h"

ASGameModeBase::ASGameModeBase()
{
	SpawnBotMax = 10;
	
	SpawnTimerInterval = 2.0f;
	
}

void ASGameModeBase::StartPlay()
{
	Super::StartPlay();

	GetWorldTimerManager().SetTimer(TimerHandle_SpawnMinion, this, &ASGameModeBase::SpawnBotTimerElapsed, SpawnTimerInterval, true);
}

void ASGameModeBase::SpawnBotTimerElapsed()
{
	int AliveMinionCount = 0;
	for(TActorIterator<ASAICharacter> It(GetWorld()); It; ++It)
	{
		ASAICharacter* AICharacter = *It;
		if(USAttributeComponent::IsActorAlive(AICharacter))
		{
			AliveMinionCount ++;
		}
	}

	if(DifficultyCurve)
	{
		SpawnBotMax = DifficultyCurve->GetFloatValue(GetWorld()->TimeSeconds);
	}
	
	if(AliveMinionCount >= SpawnBotMax)
	{
		UE_LOG(LogTemp, Log, TEXT("Spawn bot count reach max!"));
		return;
	}
	
	UEnvQueryInstanceBlueprintWrapper* EnvQueryInst = UEnvQueryManager::RunEQSQuery(this, SpawnQueryBot, this, EEnvQueryRunMode::RandomBest25Pct, nullptr);
	if(ensure(EnvQueryInst))
	{
		EnvQueryInst->GetOnQueryFinishedEvent().AddDynamic(this, &ASGameModeBase::OnQueryFinished);
	}
}

void ASGameModeBase::OnQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	if(QueryStatus != EEnvQueryStatus::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawn bot EQSQuery Failed!"));
		return;
	}
	
	TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();
	if(Locations.IsValidIndex(0))
	{
		GetWorld()->SpawnActor<AActor>(MinionClass, Locations[0], FRotator(0, FMath::FRandRange(0, 360), 0));
	}
}
