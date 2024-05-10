// Fill out your copyright notice in the Description page of Project Settings.


#include "SAction.h"

#include "SActionComponent.h"
#include "Net/UnrealNetwork.h"

void USAction::Initialize(USActionComponent* InActionComp)
{
	ActionComp = InActionComp;
}

void USAction::StartAction_Implementation(AActor* InstigatorActor)
{
	if(!IsCanStartAction(InstigatorActor))
	{
		return;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("%s Runnning Action %s"), *GetNameSafe(InstigatorActor), *GetNameSafe(this));
	
	if(ensure(ActionComp))
	{
		ActionComp->ActiveGameplayTags.AppendTags(GrantsTags);
	}

	RepData.bIsRunning = true;
	RepData.Instigator = InstigatorActor;
}

void USAction::StopAction_Implementation(AActor* InstigatorActor)
{
	if(!IsRunning())
	{
		return;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("%s Stop Action %s"), *GetNameSafe(InstigatorActor), *GetNameSafe(this));

	if(ensure(ActionComp))
	{
		ActionComp->ActiveGameplayTags.RemoveTags(GrantsTags);
	}
	
	RepData.bIsRunning = false;
	RepData.Instigator = nullptr;
}

bool USAction::IsCanStartAction(AActor* InstigatorActor)
{
	if(IsRunning())
	{
		return false;
	}
	
	if(ensure(ActionComp) && ActionComp->ActiveGameplayTags.HasAnyExact(BlockedTags))
	{
		return false;
	}

	return true;
}

USActionComponent* USAction::GetOwningComponent() const
{
	return ActionComp;
}

UWorld* USAction::GetWorld() const
{
	// 获取当前对象绑定的组件，通过组件GetWorld
	UActorComponent* Comp = Cast<UActorComponent>(GetOuter());
	if(Comp)
	{
		return Comp->GetWorld();
	}
	return nullptr;
}

void USAction::OnRep_DataChanged()
{
	if(IsRunning())
	{
		StartAction(RepData.Instigator);
	}
	else
	{
		StopAction(RepData.Instigator);
	}
}

void USAction::GetLifetimeReplicatedProps(class TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USAction, RepData);
}
