// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameModeBase.h"

#include <Actor.h>

#include "EngineUtils.h"
#include "ModuleDescriptor.h"
#include "SAttributeComponent.h"
#include "SCharacter.h"
#include "SGamePlayInterface.h"
#include "SPlayerState.h"
#include "SSaveGame.h"
#include "AI/SAICharacter.h"
#include "EnvironmentQuery/EnvQuery.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"

static TAutoConsoleVariable<bool> CVarSpawnBots(TEXT("su.SpawnBots"), true, TEXT("Enable spawning of bots via timer."), ECVF_Cheat);

ASGameModeBase::ASGameModeBase()
{
	SpawnBotMax = 10;
	
	SpawnTimerInterval = 2.0f;

	CreditsPerKill = 10; // kill a minion get CreditsPerKill Credits

	RequiredPowerupDistance = 300.0f;

	DesiredPowerupCount = 50;

	SlotName = "SaveGame01";
}

void ASGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	LoadSaveGame();
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

void ASGameModeBase::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	ASPlayerState* PS = NewPlayer->GetPlayerState<ASPlayerState>();
	if(PS)
	{
		PS->LoadPlayerState(CurrentSaveGame);
	}
	
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);
}

void ASGameModeBase::WriteSaveGame()
{
	for (int32 i = 0; i < GameState->PlayerArray.Num(); i++)
	{
		ASPlayerState* PS = Cast<ASPlayerState>(GameState->PlayerArray[i]);
		if(PS)
		{
			PS->SavePlayerState(CurrentSaveGame);

			break; // single player only at this point.
		}
	}

	CurrentSaveGame->SavedActors.Empty();
	
	// iterate all actors in the world
	for (FActorIterator It(GetWorld()); It; ++It)
	{
		AActor* Actor = *It;

		// 判断Actor是否实现了USGamePlayInterface接口
		if(!Actor->Implements<USGamePlayInterface>())
		{
			continue;
		}

		FActorSaveData ActorData;
		ActorData.Transform = Actor->GetTransform();
		ActorData.ActorName = Actor->GetName();

		FMemoryWriter MemoryWriter(ActorData.ByteData); 
		FObjectAndNameAsStringProxyArchive Ar(MemoryWriter, true);
		// Find only variables with UPROPERTY(SaveGame)
		Ar.ArIsSaveGame = true;
		Actor->Serialize(Ar);
		
		CurrentSaveGame->SavedActors.Add(ActorData);
	}
	
	UGameplayStatics::SaveGameToSlot(CurrentSaveGame, SlotName, 0);
}

void ASGameModeBase::LoadSaveGame()
{
	if(UGameplayStatics::DoesSaveGameExist(SlotName, 0))
	{
		CurrentSaveGame = Cast<USSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));
		if(!CurrentSaveGame)
		{
			UE_LOG(LogTemp, Log, TEXT("Load Save Game Failed!"));
			return;
		}

		for(FActorIterator It(GetWorld()); It; ++It)
		{
			AActor* Actor = *It;

			// 判断Actor是否实现了USGamePlayInterface接口
			if(!Actor->Implements<USGamePlayInterface>())
			{
				continue;
			}

			for (FActorSaveData ActorData : CurrentSaveGame->SavedActors)
			{
				if(ActorData.ActorName == Actor->GetName())
				{
					UE_LOG(LogTemp, Warning, TEXT("ActorName:%s, ActorPosition:%f, %f, %f "), *(Actor->GetName()), Actor->GetActorLocation().X, Actor->GetActorLocation().Y, Actor->GetActorLocation().Z)
					Actor->SetActorTransform(ActorData.Transform);

					FMemoryReader MemoryReader(ActorData.ByteData); 
					FObjectAndNameAsStringProxyArchive Ar(MemoryReader, true);
					// Find only variables with UPROPERTY(SaveGame)
					Ar.ArIsSaveGame = true;
					// Convert binary array back into actor's variables
					Actor->Serialize(Ar);

					ISGamePlayInterface::Execute_OnActorLoaded(Actor);
					
					break;
				}
			}
		}
		
		UE_LOG(LogTemp, Log, TEXT("Load Save Game From Slot!"));
	}
	else
	{
		CurrentSaveGame = Cast<USSaveGame>(UGameplayStatics::CreateSaveGameObject(USSaveGame::StaticClass()));
		
		UE_LOG(LogTemp, Log, TEXT("Create New Save Game Object!"));
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

void ASGameModeBase::OnActorKilled(AActor* VictimActor, AActor* Killer)
{
	// ASAICharacter kill ASCharacter
	ASCharacter* Player = Cast<ASCharacter>(VictimActor);
	if(Player)
	{
		WriteSaveGame();
		
		FTimerDelegate RespawnDelegate;
		RespawnDelegate.BindUFunction(this, "OnRespawnActorElapsed", Player->GetController());
	
		FTimerHandle TimerHandle_RespawnDelay;
		GetWorldTimerManager().SetTimer(TimerHandle_RespawnDelay, RespawnDelegate, 10.0f, false);
	}

	// ASCharacter kill ASAICharacter
	APawn* KillerPawn = Cast<APawn>(Killer);
	if(KillerPawn && KillerPawn != VictimActor)
	{
		// Only Players will have a 'PlayerState' instance, bots have nullptr
		ASPlayerState* PlayerState = ASPlayerState::GetPlayerState(KillerPawn);
		if(PlayerState)
		{
			PlayerState->AddCredits(CreditsPerKill);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("OnActorKiller: %s be killed by %s"), *GetNameSafe(VictimActor), *GetNameSafe(Killer));
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
