// Fill out your copyright notice in the Description page of Project Settings.


#include "SAnimInstance.h"

#include "SActionComponent.h"

void USAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	AActor* OwningActor = GetOwningActor();
	if(OwningActor)
	{
		ActionComp = Cast<USActionComponent>(OwningActor->GetComponentByClass(USActionComponent::StaticClass()));
	}

	bIsStunned = false;
}

void USAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	static FGameplayTag StunnedTag = FGameplayTag::RequestGameplayTag("Status.Stunned");
	if(ActionComp)
	{
		bool newbIsStunned = ActionComp->ActiveGameplayTags.HasTag(StunnedTag);
		if(newbIsStunned != bIsStunned)
		{
			bIsStunned = newbIsStunned;
			FString DebugMessage = FString::Printf(TEXT("bIsStunned: %d"), bIsStunned);
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::White, DebugMessage);
		}
	}
}
