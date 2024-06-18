// Fill out your copyright notice in the Description page of Project Settings.

#include "SCharacter.h"

#include <string>

#include "PhysXInterfaceWrapperCore.h"
#include "SActionComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SInteractionComponent.h"
#include "SAttributeComponent.h"
#include "SGameModeBase.h"
#include "SMagicProjectile.h"
#include "Blueprint/UserWidget.h"
#include "Components/CapsuleComponent.h"

// Sets default values
ASCharacter::ASCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>("SpringArmComp");
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->bUsePawnControlRotation = true;
	
	CameraComp = CreateDefaultSubobject<UCameraComponent>("CameraComp");
	CameraComp->SetupAttachment(SpringArmComp);

	InteractionComp = CreateDefaultSubobject<USInteractionComponent>("InteractionComp");

	AttributeComp = CreateDefaultSubobject<USAttributeComponent>("AttributeComp");

	ActionComp = CreateDefaultSubobject<USActionComponent>("ActionComp");
	
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	
	SwitchProjectileClass = "PrimaryAttack";
	NeedRageForBlackHole = 1.0f;
}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASCharacter::MoveRight);

	PlayerInputComponent->BindAxis("TurnAround", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	/*
	BindAction() 绑定的函数会在输入事件发生时执行，BindAxis() 绑定的函数每帧都会执行
	BindAction() 用来监听外设是否到达某个状态，BindAxis() 监听的是外设状态的变化量
	BindAction() 绑定的函数无参数 BindAxis() 绑定的函数有一个参数，该参数就是外设状态的变化量
	*/
	
	PlayerInputComponent->BindAction("PrimaryAttack", IE_Pressed, this, &ASCharacter::ProjectileAttack);
	PlayerInputComponent->BindAction("MagicProticleAttack", IE_Pressed, this, &ASCharacter::SwitchAttack_MagicProjectile);
	PlayerInputComponent->BindAction("BlackHoleProticleAttack", IE_Pressed, this, &ASCharacter::SwitchAttack_BlackHoleProjectile);
	PlayerInputComponent->BindAction("DashProticleAttack", IE_Pressed, this, &ASCharacter::SwitchAttack_DashProjectile);
	
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASCharacter::JumpStart);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ASCharacter::JumpEnd);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ASCharacter::StartSprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ASCharacter::StopSprint);

	PlayerInputComponent->BindAction("OpenTreasure", IE_Pressed, this, &ASCharacter::OpenTreasureChest);
}

void ASCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	AttributeComp->OnHealthChanged.AddDynamic(this, &ASCharacter::OnHealthValueChanged);
	AttributeComp->OnRageChanged.AddDynamic(this, &ASCharacter::OnRageChanged);
}

void ASCharacter::OnHealthValueChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta)
{
	if(Delta < 0.0f)
	{
		// Show Hit Flash Material
		GetMesh()->SetScalarParameterValueOnMaterials("TimeToHit", GetWorld()->GetTimeSeconds());

		// Add Rage
		AttributeComp->RageGained(InstigatorActor, AttributeComp->GetRageRate() * Delta * (-1));
	}

	// Player Died
	if (NewHealth <= 0.0f && Delta < 0.0f)
	{
		// Disable Player Input
		APlayerController* PC = Cast<APlayerController>(GetController());
		if(PC)
		{
			DisableInput(PC);
		}

		SetLifeSpan(5.0f);
	}
}

void ASCharacter::HealSelf(float Amount /* = 100 */)
{
	AttributeComp->HealSelf(this, Amount);
}

void ASCharacter::OnRageChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewRage, float Delta)
{
	
}

FVector ASCharacter::GetPawnViewLocation() const
{
	return CameraComp->GetComponentLocation();
}

void ASCharacter::ShowCommonTipsWidget(FText ShowText)
{
	if(ensure(CommonTipsWidgetClass))
	{
		UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(), CommonTipsWidgetClass);
		
		if(Widget)
		{
			Widget->AddToViewport();

			CommonTips = ShowText;
		}
	}
}

FText ASCharacter::GetCommonTipsText()
{
	return CommonTips;
}

void ASCharacter::MoveForward(float value)
{
	FRotator ControlRot = GetControlRotation();
	//ControlRot.Yaw
	ControlRot.Pitch = 0.0f;
	ControlRot.Roll = 0.0f;
	// AddMovementInput: ScaleValue – 适用于输入的比例。这可以用于模拟输入，即0.5的值应用正常值的一半，而-1.0将反转方向。
	AddMovementInput(ControlRot.Vector(), value);
	// AddMovementInput(GetActorForwardVector(), value);
}

void ASCharacter::MoveRight(float value)
{
	FRotator ControlRot = GetControlRotation();
	// Yaw 左右摇摆; Pitch 上下摇摆; Roll 左右翻转
	ControlRot.Pitch = 0.0f; 
	ControlRot.Roll = 0.0f;

	// 虚幻世界坐标系为左手坐标系，即：
	// x = Forward (Red); Y = Right (Green);  Z = Up (Blue);
	const FVector RightVector = FRotationMatrix(ControlRot).GetScaledAxis(EAxis::Y); // 获取旋转矩阵ControlRot在Y轴上的向量
	AddMovementInput(RightVector, value);
}

void ASCharacter::JumpStart()
{
	bPressedJump = true;
}

void ASCharacter::JumpEnd()
{
	bPressedJump = false;
}

void ASCharacter::StartSprint()
{
	ActionComp->StartActionByName(this, "Sprint");
}

void ASCharacter::StopSprint()
{
	ActionComp->StopAction(this, "Sprint");
}

void ASCharacter::OpenTreasureChest()
{
	if (InteractionComp)
	{
		InteractionComp->PrimaryInteract();
	}
}

void ASCharacter::ProjectileAttack()
{
	ActionComp->StartActionByName(this, SwitchProjectileClass);
}

void ASCharacter::SwitchAttack_BlackHoleProjectile()
{
	if(AttributeComp->GetRage() > NeedRageForBlackHole)
	{
		AttributeComp->RageRequired(this, NeedRageForBlackHole);
		SwitchProjectileClass = "BlackHole";
	}
	else
	{
		FString text = FString::Printf(TEXT("Can not Use BlackHoleProjectile Unitl Your Rage More Than %f."), NeedRageForBlackHole); //FString::Printf(TEXT("I am %d years old"), 18)
		ShowCommonTipsWidget(FText::FromString(text)); 
	}
}
