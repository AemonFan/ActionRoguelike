// Fill out your copyright notice in the Description page of Project Settings.

#include "SCharacter.h"

#include "PhysXInterfaceWrapperCore.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SInteractionComponent.h"
#include "SAttributeComponent.h"

// Sets default values
ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>("SpringArmComp");
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->bUsePawnControlRotation = true;
	
	CameraComp = CreateDefaultSubobject<UCameraComponent>("CameraComp");
	CameraComp->SetupAttachment(SpringArmComp);

	InteractionComp = CreateDefaultSubobject<USInteractionComponent>("InteractionComp");

	AttributeComp = CreateDefaultSubobject<USAttributeComponent>("AttributeComp");
	
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASCharacter::MoveRight);

	PlayerInputComponent->BindAxis("TurnAround", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction("PrimaryAttack", IE_Pressed, this, &ASCharacter::PrimaryAttack);
	
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASCharacter::JumpStart);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ASCharacter::JumpEnd);

	PlayerInputComponent->BindAction("OpenTreasure", IE_Pressed, this, &ASCharacter::OpenTreasureChest);
}

void ASCharacter::MoveForward(float value)
{
	FRotator ControlRot = GetControlRotation();
	//ControlRot.Yaw
	ControlRot.Pitch = 0.0f;
	ControlRot.Roll = 0.0f;
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

void ASCharacter::PrimaryAttack()
{
	PlayAnimMontage(AttackAnim);

	GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &ASCharacter::PrimaryAttack_TimerElapsed, 0.2f);

	// 角色死亡后应该清理定时器句柄Handle
	// GetWorldTimerManager().ClearTimer(AttackTimerHandle);
}

void ASCharacter::PrimaryAttack_TimerElapsed()
{
	// ensure ： 确保 ProjectileClass 存在，否则出发断言，仅在第一次执行时出发
	if(ensure(ProjectileClass))
	{
		const FVector HandLocation = GetMesh()->GetSocketLocation("Muzzle_01");

		// 投射物生成位置及方向
		const FTransform SpawnTM = FTransform(GetControlRotation(), HandLocation);

		// 指定投射物生成方式
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Instigator = this; // Instigator : 对生成的Actor造成的伤害负责的 APawn

		// 生成投射物
		GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnTM, SpawnParams);
	}
}

void ASCharacter::OpenTreasureChest()
{
	if (InteractionComp)
	{
		InteractionComp->PrimaryInteract();
	}
}