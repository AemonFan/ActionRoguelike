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
	
	FString DebugMessage = GetNameSafe(InstigatorActor) + " StartAction : " + *GetNameSafe(this);
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, DebugMessage);
	
	if(ensure(ActionComp))
	{
		ActionComp->ActiveGameplayTags.AppendTags(GrantsTags);
		
		GetOwningComponent()->OnActionStarted.Broadcast(ActionComp, this);
	}

	RepData.bIsRunning = true;
	RepData.Instigator = InstigatorActor;

	if(GetOwningComponent()->GetOwnerRole() == ROLE_Authority)
	{
		ActionStartTime = GetWorld()->TimeSeconds;
	}
}

void USAction::StopAction_Implementation(AActor* InstigatorActor)
{
	UE_LOG(LogTemp, Warning, TEXT("%s Stop Action %s"), *GetNameSafe(InstigatorActor), *GetNameSafe(this));

	FString DebugMessage = GetNameSafe(InstigatorActor) + " StopAction : " + *GetNameSafe(this);
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, DebugMessage);
	
	if(ensure(ActionComp))
	{
		ActionComp->ActiveGameplayTags.RemoveTags(GrantsTags);
		
		GetOwningComponent()->OnActionStopped.Broadcast(ActionComp, this);
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
	//UActorComponent* Comp = Cast<UActorComponent>(GetOuter());
	if(ActionComp)
	{
		return ActionComp->GetWorld();
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
	DOREPLIFETIME(USAction, ActionComp);
	DOREPLIFETIME(USAction, ActionStartTime);
}
