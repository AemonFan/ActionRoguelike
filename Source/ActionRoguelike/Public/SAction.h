// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/NoExportTypes.h"
#include "SAction.generated.h"

class USActionComponent;
/**
 * 
 */

USTRUCT()
struct FRepData
{
	GENERATED_BODY()

public:

	UPROPERTY()
	bool bIsRunning;

	UPROPERTY()
	AActor* Instigator;
};

UCLASS(Blueprintable)
class ACTIONROGUELIKE_API USAction : public UObject
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly, Category="Tags")
	FGameplayTagContainer GrantsTags;

	UPROPERTY(EditDefaultsOnly, Category="Tags")
	FGameplayTagContainer BlockedTags;
	
public:

	UPROPERTY(ReplicatedUsing="OnRep_DataChanged")
	FRepData RepData;

	UPROPERTY(EditDefaultsOnly, Category="Action")
	bool bIsAutoStart;

	UPROPERTY(EditDefaultsOnly, Category="Action")
	FName ActionName;

	UPROPERTY(Replicated)
	USActionComponent* ActionComp;

	void Initialize(USActionComponent* InActionComp);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Action")
	void StartAction(AActor* InstigatorActor);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Action")
	void StopAction(AActor* InstigatorActor);

	UFUNCTION(BlueprintCallable, Category="Action")
	bool IsCanStartAction(AActor* InstigatorActor);

	UFUNCTION(BlueprintCallable, Category="Action")
	bool IsRunning() const
	{
		return RepData.bIsRunning;
	}

	UFUNCTION(BlueprintCallable, Category="Action")
	USActionComponent* GetOwningComponent() const;

	virtual UWorld* GetWorld() const override;

protected:
	
	virtual bool IsSupportedForNetworking() const override
	{
		return true;
	}

	UFUNCTION()
	void OnRep_DataChanged();
};
