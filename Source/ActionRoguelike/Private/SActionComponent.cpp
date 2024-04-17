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

}

void USActionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void USActionComponent::AddAction(TSubclassOf<USAction> ActionClass)
{
	USAction* NewAction = NewObject<USAction>(this, ActionClass);
	if(ensure(NewAction))
	{
		Actions.Add(NewAction);
	}
}

bool USActionComponent::StartAction(AActor* InstigatorActor, FName ActionName)
{
	for (auto Action : Actions)
	{
		if(Action->ActionName == ActionName)
		{
			Action->StartAction(InstigatorActor);
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
			return true;
		}
	}
	return false;
}
