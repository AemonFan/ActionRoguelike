// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "SActionComponent.generated.h"


class USAction;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONROGUELIKE_API USActionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	USActionComponent();

protected:

	UPROPERTY(EditAnywhere, Category="Action")
	TArray<TSubclassOf<USAction>> DefaultActions;

	UPROPERTY(Replicated)
	TArray<USAction*> Actions;
	
	virtual void BeginPlay() override;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tags")
	FGameplayTagContainer ActiveGameplayTags;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category="Action")
	void AddAction(AActor* InstigatorActor,TSubclassOf<USAction> ActionClass);

	UFUNCTION(BlueprintCallable, Category="Action")
	void RemoveAction(USAction* NewAction);
	
	UFUNCTION(BlueprintCallable, Category="Action")
	bool StartActionByName(AActor* InstigatorActor, FName ActionName);

	UFUNCTION(Server, Reliable)
	void ServerStartAction(AActor* InstigatorActor, FName ActionName);
	
	UFUNCTION(BlueprintCallable, Category="Action")
	bool StopAction(AActor* InstigatorActor, FName ActionName);

protected:

	bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	
};
