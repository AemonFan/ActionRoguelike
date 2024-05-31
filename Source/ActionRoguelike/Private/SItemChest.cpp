#include "SItemChest.h"

#include "Net/UnrealNetwork.h"

ASItemChest::ASItemChest()
{
	PrimaryActorTick.bCanEverTick = true;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>("BaseMesh");
	RootComponent = BaseMesh;
	
	LidMesh = CreateDefaultSubobject<UStaticMeshComponent>("LidMesh");
	LidMesh->SetupAttachment(BaseMesh);

	TargetPitchValue = 110.0f;
}

void ASItemChest::BeginPlay()
{
	Super::BeginPlay();
	
	SetReplicates(true); // this actor can be replicate
}

void ASItemChest::Interact_Implementation(APawn* InstigatorPawn)
{
	bLidOpen = !bLidOpen;
}

void ASItemChest::OnActorLoaded_Implementation()
{
	OnRep_LidOpen();
}

void ASItemChest::OnRep_LidOpen()
{
	float Angle = bLidOpen ? TargetPitchValue : 0.0f;
	LidMesh->SetRelativeRotation(FRotator(Angle, 0, 0));
}

// Returns the properties used for network replication, this needs to be overridden by all actor classes with native replicated properties
void ASItemChest::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASItemChest, bLidOpen);
}