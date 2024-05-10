// Fill out your copyright notice in the Description page of Project Settings.

#include "SMagicProjectile.h"

#include "SActionComponent.h"
#include "SAttributeComponent.h"
#include "SGameplayFunctionLibrary.h"
#include "Components/AudioComponent.h"
#include "components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

ASMagicProjectile::ASMagicProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	Damage = -10.0f;
	
	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ASMagicProjectile::OnActorOverlap);
}

void ASMagicProjectile::OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("ASMagicProjectile::OnActorOverlap OtherActor:%s, Damage:%f"), *GetNameSafe(OtherActor), Damage);
	
	if(OtherActor && OtherActor != GetInstigator()) // 确保角色自己生成的投射物不会对自己造成伤害
	{
		USActionComponent* ActionComp = Cast<USActionComponent>(OtherActor->GetComponentByClass(USActionComponent::StaticClass()));
		
		if(ActionComp && ActionComp->ActiveGameplayTags.HasTag(ParryTag))
		{
			MovementComp->Velocity = MovementComp->Velocity * (-1);
			
			SetInstigator(Cast<APawn>(OtherActor));

			return;
		}
		
		if(USGameplayFunctionLibrary::ApplyDirectionalDamage(GetInstigator(), OtherActor, Damage, SweepResult))
		{
			Explode();

			if(ActionComp)
			{
				ActionComp->AddAction(GetInstigator(), ActionClass);
			}
		}
	}
}
