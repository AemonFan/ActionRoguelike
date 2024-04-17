// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameModeBase.h"

#include <Actor.h>

#include "EngineUtils.h"
#include "SAttributeComponent.h"
#include "SCharacter.h"
#include "SPlayerState.h"
#include "AI/SAICharacter.h"
#include "EnvironmentQuery/EnvQuery.h"
#include "EnvironmentQuery/EnvQueryManager.h"

static TAutoConsoleVariable<bool> CVarSpawnBots(TEXT("su.SpawnBots"), true, TEXT("Enable spawning of bots via timer."), ECVF_Cheat);

ASGameModeBase::ASGameModeBase()
{
	SpawnBotMax = 10;
	
	SpawnTimerInterval = 2.0f;

	CreditsPerKill = 10; // kill a minion get CreditsPerKill Credits

	RequiredPowerupDistance = 300.0f;

	DesiredPowerupCount = 50;
}

void ASGameModeBase::StartPlay()
{
	Super::StartPlay();

	// Spawn Minions Later
	GetWorldTimerManager().SetTimer(TimerHandle_SpawnMinion, this, &ASGameModeBase::OnSpawnBotTimerElapsed, SpawnTimerInterval, true);

	// Spawn Powerup Actor
	if(ensure(SpawnPowerupClass.Num() > 0))
	{
		UEnvQueryInstanceBlueprintWrapper* EnvQueryInst = UEnvQueryManager::RunEQSQuery(this, SpawnQueryCoin, this, EEnvQueryRunMode::AllMatching, nullptr);
		if(ensure(EnvQueryInst))
		{
			EnvQueryInst->GetOnQueryFinishedEvent().AddDynamic(this, &ASGameModeBase::OnSpawnPowerupQueryFinished);
		}
	}
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

void ASGameModeBase::OnAIActorKilled(AActor* Killer)
{
	ASCharacter* Pawn = Cast<ASCharacter>(Killer);
	if(Pawn)
	{
		ASPlayerState* PlayerState = ASPlayerState::GetPlayerState(Pawn);
		if(PlayerState)
		{
			PlayerState->AddCredits(CreditsPerKill);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("OnAIActorKilled: AI be killed by %s"), *GetNameSafe(Killer));
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
		EnvQueryInst->GetOnQueryFinishedEvent().AddDynamic(this, &ASGameModeBase::OnSpawnBotQueryFinished);
	}
}

void ASGameModeBase::OnSpawnBotQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	if(QueryStatus != EEnvQueryStatus::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawn Bots EQSQuery Failed!"));
		return;
	}
	
	TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();
	if(Locations.IsValidIndex(0))
	{
		GetWorld()->SpawnActor<AActor>(MinionClass, Locations[0], FRotator(0, FMath::FRandRange(0, 360), 0));
	}
}

void ASGameModeBase::OnSpawnPowerupQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance,	EEnvQueryStatus::Type QueryStatus)
{
	if(QueryStatus != EEnvQueryStatus::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawn Coins EQSQuery Failed!"));
		return;
	}

	TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();
	TArray<FVector> UsedLocations;
	TSubclassOf<AActor> PowerupCoinClass;
	
	int index;
	int SpawnCount = 0;
	bool bSpawnPowerup;
	float DistanceTo;
	while (SpawnCount < DesiredPowerupCount && Locations.Num() > 0)
	{
		bSpawnPowerup = true;
		
		index = FMath::RandRange(0, Locations.Num() - 1);
		
		for (auto TempLocation : UsedLocations)
		{
			DistanceTo = (Locations[index] - TempLocation).Size();
			if(DistanceTo < RequiredPowerupDistance)
			{
				bSpawnPowerup = false;
				break;
			}
		}
		
		if(bSpawnPowerup)
		{
			PowerupCoinClass = SpawnPowerupClass[FMath::RandRange(0, SpawnPowerupClass.Num() - 1)];
			
			GetWorld()->SpawnActor<AActor>(PowerupCoinClass, Locations[index], FRotator::ZeroRotator);

			SpawnCount++;
			
			UsedLocations.Add(Locations[index]);
		}
		
		Locations.RemoveAt(index);
	}
}
