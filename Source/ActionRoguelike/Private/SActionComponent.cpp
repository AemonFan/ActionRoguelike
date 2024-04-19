// Fill out your copyright notice in the Description page of Project Settings.


#include "SActionComponent.h"

#include "SAction.h"

USActionComponent::USActionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void USActionComponent::BeginPlay()
{
	Super::BeginPlay();

	for (auto ActionClass : DefaultActions)
	{
		AddAction(ActionClass);
	}
}

void USActionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FString DebugMessage = GetNameSafe(GetOwner()) + " : " + ActiveGameplayTags.ToStringSimple();
	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::White, DebugMessage);
}

void USActionComponent::AddAction(TSubclassOf<USAction> ActionClass)
{
	USAction* NewAction = NewObject<USAction>(this, ActionClass);
	if(ensure(NewAction))
	{
		Actions.Add(NewAction);

		if(NewAction->bIsAutoStart)
		{
			NewAction->StartAction(GetOwner());
		}
	}
}

void USActionComponent::RemoveAction(USAction* Action)
{
	ensure(!Action->IsRunning());

	Actions.Remove(Action);
}

bool USActionComponent::StartAction(AActor* InstigatorActor, FName ActionName)
{
	for (auto Action : Actions)
	{
		if(Action->ActionName == ActionName)
		{
			if(!Action->IsCanStartAction(InstigatorActor))
			{
				continue;
			}
			
			Action->StartAction(InstigatorActor);
			
			GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::White, GetNameSafe(GetOwner()) + " StartAction :" + ActionName.ToString());
			
			return true;
		}
	}
	return false;
}

bool USActionComponent::StopAction(AActor* InstigatorActor, FName ActionName)
{
	for (auto Action : Actions)
	{
		if(Action->ActionName == ActionName)
		{
			Action->StopAction(InstigatorActor);
			
			GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::White, GetNameSafe(GetOwner()) + " StopAction :" + ActionName.ToString());
			
			return true;
		}
	}
	return false;
}
