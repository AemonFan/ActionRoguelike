// Fill out your copyright notice in the Description page of Project Settings.


#include "SActionComponent.h"

#include "SAction.h"
#include "Engine/ActorChannel.h"
#include "Net/UnrealNetwork.h"

USActionComponent::USActionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
}

void USActionComponent::BeginPlay()
{
	Super::BeginPlay();

	if(GetOwner()->HasAuthority())
	{
		for (TSubclassOf<USAction> ActionClass : DefaultActions)
		{
			AddAction(GetOwner(), ActionClass);
		}		
	}
}

void USActionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FString DebugMessage = GetNameSafe(GetOwner()) + " : " + ActiveGameplayTags.ToStringSimple();
	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::White, DebugMessage);
}

void USActionComponent::AddAction(AActor* InstigatorActor, TSubclassOf<USAction> ActionClass)
{
	if(!ensure(ActionClass))
	{
		return;
	}
	
	USAction* NewAction = NewObject<USAction>(GetOwner(), ActionClass);
	if(ensure(NewAction))
	{
		NewAction->Initialize(this);
		
		Actions.Add(NewAction);

		if(NewAction->bIsAutoStart && ensure(NewAction->IsCanStartAction(InstigatorActor)))
		{
			NewAction->StartAction(InstigatorActor);
		}
	}
}

void USActionComponent::RemoveAction(USAction* Action)
{
	ensure(!Action->IsRunning());

	Actions.Remove(Action);
}

bool USActionComponent::StartActionByName(AActor* InstigatorActor, FName ActionName)
{
	for (auto Action : Actions)
	{
		if(Action && Action->ActionName == ActionName)
		{
			if(!Action->IsCanStartAction(InstigatorActor))
			{
				GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::White, GetNameSafe(GetOwner()) + " StartAction :" + ActionName.ToString());
				
				continue;
			}

			if(!GetOwner()->HasAuthority())
			{
				// Notify Client 
				ServerStartAction(InstigatorActor, ActionName);
			}
			
			Action->StartAction(InstigatorActor);
			
			return true;
		}
	}
	return false;
}

void USActionComponent::ServerStartAction_Implementation(AActor* InstigatorActor, FName ActionName)
{
	StartActionByName(InstigatorActor, ActionName);
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

bool USActionComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
	for (USAction* Action : Actions)
	{
		if (Action)
		{
			WroteSomething |= Channel->ReplicateSubobject(Action, *Bunch, *RepFlags);
		}
	}

	return WroteSomething;
}

void USActionComponent::GetLifetimeReplicatedProps(class TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USActionComponent, Actions);
}

