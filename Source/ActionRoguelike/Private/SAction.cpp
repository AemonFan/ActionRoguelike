// Fill out your copyright notice in the Description page of Project Settings.


#include "SAction.h"

#include "SActionComponent.h"

void USAction::StartAction_Implementation(AActor* InstigatorActor)
{
	if(!IsCanStartAction(InstigatorActor))
	{
		return;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("%s Runnning Action %s"), *GetNameSafe(InstigatorActor), *GetNameSafe(this));

	USActionComponent* ActionComp = GetOwningComponent();
	ActionComp->ActiveGameplayTags.AppendTags(GrantsTags);

	bIsRunning = true;
}

void USAction::StopAction_Implementation(AActor* InstigatorActor)
{
	if(!bIsRunning)
	{
		return;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("%s Stop Action %s"), *GetNameSafe(InstigatorActor), *GetNameSafe(this));

	USActionComponent* ActionComp = GetOwningComponent();
	ActionComp->ActiveGameplayTags.RemoveTags(GrantsTags);
	
	bIsRunning = false;
}

bool USAction::IsCanStartAction(AActor* InstigatorActor)
{
	if(bIsRunning)
	{
		return false;
	}
	
	USActionComponent* ActionComp = GetOwningComponent();
	if(ActionComp->ActiveGameplayTags.HasAnyExact(BlockedTags))
	{
		return false;
	}

	return true;
}

USActionComponent* USAction::GetOwningComponent() const
{
	return Cast<USActionComponent>(GetOuter());
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
