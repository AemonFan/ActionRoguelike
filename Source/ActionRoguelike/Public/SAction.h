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
UCLASS(Blueprintable)
class ACTIONROGUELIKE_API USAction : public UObject
{
	GENERATED_BODY()

public:

	USAction();

protected:

	bool bIsRunning;

	UPROPERTY(EditDefaultsOnly, Category="Tags")
	FGameplayTagContainer GrantsTags;

	UPROPERTY(EditDefaultsOnly, Category="Tags")
	FGameplayTagContainer BlockedTags;
	
public:

	UPROPERTY(EditDefaultsOnly, Category="Action")
	bool bIsAutoStart;

	UPROPERTY(EditDefaultsOnly, Category="Action")
	FName ActionName;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Action")
	void StartAction(AActor* InstigatorActor);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Action")
	void StopAction(AActor* InstigatorActor);

	UFUNCTION(BlueprintCallable, Category="Action")
	bool IsCanStartAction(AActor* InstigatorActor);

	UFUNCTION(BlueprintCallable, Category="Action")
	bool IsRunning() const
	{
		return bIsRunning;
	}

	UFUNCTION(BlueprintCallable, Category="Action")
	USActionComponent* GetOwningComponent() const;

	UWorld* GetWorld() const override;
};
