// Fill out your copyright notice in the Description page of Project Settings.

#include "SMagicProjectile.h"

#include "SActionComponent.h"
#include "SGameplayFunctionLibrary.h"
#include "Components/AudioComponent.h"
#include "components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "SActionEffect.h"

ASMagicProjectile::ASMagicProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	Damage = -10.0f;
	
	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ASMagicProjectile::OnActorOverlap);
}

void ASMagicProjectile::OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("%s Overlap OtherActor:%s, Damage:%f"), *GetNameSafe(this), *GetNameSafe(OtherActor), Damage);
	
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

			if(ActionComp && BuringActionClass && HasAuthority())
			{
				ActionComp->AddAction(GetInstigator(), BuringActionClass);
			}
		}
	}
}
