// Fill out your copyright notice in the Description page of Project Settings.


#include "SInteractionComponent.h"

#include "DrawDebugHelpers.h"
#include "SGamePlayInterface.h"
#include "SWorldUserWidget.h"
#include "Blueprint/UserWidget.h"

static TAutoConsoleVariable<bool> CVarShowDrawDebug(TEXT("su.ShowDrawDebug"), false, TEXT("Interaction ShowDrawDebug Switch"), ECVF_Cheat);

USInteractionComponent::USInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	TranceRadius = 50.0f;
	
	TranceDistance = 1000.0f;

	QueryChannel = ECC_WorldDynamic;
}

void USInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	FindBestInteractable();
}

void USInteractionComponent::FindBestInteractable()
{
	AActor* MyOwner = GetOwner();

	FVector EyeLocation;
	FRotator EyeRotation;
	MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation); // 获取Character的Eye位置和角度
	FVector End = EyeLocation + (EyeRotation.Vector() * TranceDistance); // 从眼睛的位置开始到眼睛方向的1000单位

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(QueryChannel); // 设置碰撞查询
	
	bool bUseLogic1 = false;
	bool bDrawDebug = CVarShowDrawDebug.GetValueOnGameThread();
	
	FocusedActor = nullptr;
	
	if (bUseLogic1)
	{
		FHitResult Hit;
		const bool bIsHitObject = GetWorld()->LineTraceSingleByObjectType(Hit, EyeLocation, End, ObjectQueryParams); // 获取碰撞信息

		AActor* HitActor = Hit.GetActor(); // 获取碰撞对象
		if (HitActor)
		{
			// 判断碰撞对象是否实现了交互接口
			if (HitActor->Implements<USGamePlayInterface>())
			{
				FocusedActor = HitActor;
			}
		}
		
		const FColor HitColor = bIsHitObject ? FColor::Green : FColor::Red;

		if(bDrawDebug)
		{
			DrawDebugLine(GetWorld(), EyeLocation, End, HitColor, false, 2.0f, 0, 1.0f);
		}
	}
	else
	{
		TArray<FHitResult> Hits;
		FCollisionShape Shape;
		Shape.SetSphere(TranceRadius);

		// 以Shape为检测范围碰撞检测
		bool bHitObject = GetWorld()->SweepMultiByObjectType(Hits, EyeLocation, End, FQuat::Identity, ObjectQueryParams, Shape);

		FColor HitColor = bHitObject ? FColor::Green : FColor::Red;

		// 遍历碰撞到的所有对象
		for(FHitResult Hit : Hits)
		{
			AActor* HitActor = Hit.GetActor();
			if (HitActor)
			{
				if(HitActor->Implements<USGamePlayInterface>())
				{
					FocusedActor = HitActor;
					
					if(bDrawDebug)
					{
						DrawDebugSphere(GetWorld(), Hit.ImpactPoint, TranceRadius, 32, HitColor, false, 2.0f);
					}
					break;
				}
			}
		}

		if(bDrawDebug)
		{
			DrawDebugLine(GetWorld(), EyeLocation, End, HitColor, false, 2.0f, 0, 1.0f);
		}
	}

	if(FocusedActor && ensure(DefaultWidgetClass))
	{
		if(DefaultWidgetInstance == nullptr)
		{
			DefaultWidgetInstance = CreateWidget<USWorldUserWidget>(GetWorld(), DefaultWidgetClass);
		}

		if(DefaultWidgetInstance)
		{
			DefaultWidgetInstance->AttachToActor = FocusedActor;
			
			if(!DefaultWidgetInstance->IsInViewport())
			{
				DefaultWidgetInstance->AddToViewport();
			}
		}
	}
	else
	{
		if(DefaultWidgetInstance && DefaultWidgetInstance->IsInViewport())
		{
			DefaultWidgetInstance->RemoveFromParent();
		}
	}
}

void USInteractionComponent::PrimaryInteract()
{
	if(FocusedActor == nullptr)
		return;

	AActor* MyOwner = GetOwner();
	APawn* MyPawn = Cast<APawn>(MyOwner);
	
	ISGamePlayInterface::Execute_Interact(FocusedActor, MyPawn);
}