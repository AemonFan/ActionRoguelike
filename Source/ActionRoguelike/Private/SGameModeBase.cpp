// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameModeBase.h"

#include "EngineUtils.h"
#include "SAttributeComponent.h"
#include "SCharacter.h"
#include "AI/SAICharacter.h"
#include "EnvironmentQuery/EnvQueryManager.h"

static TAutoConsoleVariable<bool> CVarSpawnBots(TEXT("su.SpawnBots"), true, TEXT("Enable spawning of bots via timer."), ECVF_Cheat);

ASGameModeBase::ASGameModeBase()
{
	SpawnBotMax = 10;
	
	SpawnTimerInterval = 2.0f;
	
}

void ASGameModeBase::StartPlay()
{
	Super::StartPlay();

	GetWorldTimerManager().SetTimer(TimerHandle_SpawnMinion, this, &ASGameModeBase::OnSpawnBotTimerElapsed, SpawnTimerInterval, true);
}

void ASGameModeBase::KillAllAI()
{
	for(TActorIterator<ASAICharacter> It(GetWorld()); It; ++It)
	{
		ASAICharacter* AICharacter = *It;
		USAttributeComponent* Comp = USAttributeComponent::GetAttributes(AICharacter);
		if(Comp && Comp->IsAlive())
		{
			Comp->KillSelf(this); // @fixme: maybe pass in player? for kill credit
		}
	}
}

void ASGameModeBase::OnActorKilled(AActor* RespawnActor, AActor* Killer)
{
	ASCharacter* RespawnCharacter = Cast<ASCharacter>(RespawnActor);
	if(RespawnCharacter)
	{
		FTimerDelegate RespawnDelegate;
		RespawnDelegate.BindUFunction(this, "OnRespawnActorElapsed", RespawnCharacter->GetController());
	
		FTimerHandle TimerHandle_RespawnDelay;
		GetWorldTimerManager().SetTimer(TimerHandle_RespawnDelay, RespawnDelegate, 2.0f, false);
	}

	UE_LOG(LogTemp, Warning, TEXT("OnActorKiller: %s be killed by %s"), *GetNameSafe(RespawnActor), *GetNameSafe(Killer));
}

void ASGameModeBase::OnRespawnActorElapsed(AController* RespawnController)
{
	if(ensure(RespawnController))
	{
		RespawnController->UnPossess();
		RestartPlayer(RespawnController);
	}
}

void ASGameModeBase::OnSpawnBotTimerElapsed()
{
	if(!CVarSpawnBots.GetValueOnGameThread())
	{
		UE_LOG(LogTemp, Warning, TEXT("CVarSpawnBots false"));
		return;	
	}
	
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
