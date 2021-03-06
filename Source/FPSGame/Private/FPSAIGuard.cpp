// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSAIGuard.h"
#include "Perception/PawnSensingComponent.h"
#include "DrawDebugHelpers.h"
//#include "NavigationSystem/Public/NavigationSystem.h"
//#include "NavigationSystem/Public/NavigationPath.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "FPSGameMode.h"

class UNavigationSystemV1;

// Sets default values
AFPSAIGuard::AFPSAIGuard()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));
	PawnSensingComp->OnSeePawn.AddDynamic(this, &AFPSAIGuard::OnPawnSeen);
	PawnSensingComp->OnHearNoise.AddDynamic(this, &AFPSAIGuard::OnNoiseHeard);

	GuardState = EAIState::Idle;

}

// Called when the game starts or when spawned
void AFPSAIGuard::BeginPlay()
{
	Super::BeginPlay();

	OrigianlRotation = GetActorRotation();

	if (bPatrol)
	{
		MoveToNextPatrolPoint();
	}
	
}

void AFPSAIGuard::OnPawnSeen(APawn * SeenPawn)
{
	if (SeenPawn == nullptr)
	{
		return;
	}

	DrawDebugSphere(GetWorld(), SeenPawn->GetActorLocation(), 32.0f, 12, FColor::Red, false, 10.0f);

	AFPSGameMode* GM = Cast<AFPSGameMode>(GetWorld()->GetAuthGameMode());
	if (GM)
	{
		GM->CompleteMission(SeenPawn, false);
	}

	//SetGuardState(EAIState::Alerted);
	// Stop Movement if Patrolling
	AController* Controller = GetController();
	if (Controller)
	{
		Controller->StopMovement();
	}
}

void AFPSAIGuard::OnNoiseHeard(APawn* NoiseInstigator, const FVector & Location, float Volume)
{

	if (GuardState == EAIState::Alerted)
	{
		return;
	}

	DrawDebugSphere(GetWorld(), Location, 32.0f, 12, FColor::Green, false, 10.0f);

	FVector Direction = Location - GetActorLocation();
	Direction.Normalize();

	FRotator NewLookAt =  FRotationMatrix::MakeFromX(Direction).Rotator();
	NewLookAt.Pitch = 0.0f;
	NewLookAt.Roll = 0.0f;

	SetActorRotation(NewLookAt);

	GetWorldTimerManager().ClearTimer(TimerHandle_ResetOrientation);
	GetWorldTimerManager().SetTimer(TimerHandle_ResetOrientation, this, &AFPSAIGuard::ResetOrientation, 3.0f, false);

	SetGuardState(EAIState::Suspicious);

	// Stop Movement if Patrolling
	AController* controller = GetController();
	if (controller)
	{
		controller->StopMovement();
	}
	
}

void AFPSAIGuard::ResetOrientation()
{
	if (GuardState == EAIState::Alerted)
	{
		return;
	}

	SetActorRotation(OrigianlRotation);
	
	SetGuardState(EAIState::Idle);

	// Stopped investigation... if we are a patrolling pawn, pick a new patrol point to move to
	if (bPatrol)
	{
		MoveToNextPatrolPoint();
	}
}

void AFPSAIGuard::SetGuardState(EAIState NewState)
{
	if (GuardState == NewState)
	{
		return;
	}

	GuardState = NewState;

	OnStateChanged(GuardState);
}

// Called every frame
void AFPSAIGuard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Patrol Goal Checks
	if (CurrentPatrolPoint)
	{
		FVector Delta = GetActorLocation() - CurrentPatrolPoint->GetActorLocation();
		float DistanceToGoal = Delta.Size();

		// Check if we are within 50 units of our goal, if so - pick a new patrol point
		if (DistanceToGoal < 50)
		{
			MoveToNextPatrolPoint();
		}
	}

}

void AFPSAIGuard::MoveToNextPatrolPoint()
{
	// Assign next patrol point.

	if (CurrentPatrolPoint == nullptr || CurrentPatrolPoint == SecondPatrolPoint)
	{
		CurrentPatrolPoint = FirstPatrolPoint;
	}
	else
	{
		CurrentPatrolPoint = SecondPatrolPoint;
	}

	// 강좌에서는 UNavigatinSytem을 사용하라고 했지만 디프리케이트 되었고.
	// UNavigationV1을 사용하라고 함 https://docs.unrealengine.com/en-US/API/Runtime/NavigationSystem/UNavigationSystemV1/SimpleMoveToLocation/index.html
	//UNavigationSystem::SimpleMoveToActor(GetController(), CurrentPatrolPoint);

	// 하지만 이것도 안됨
	// 라이브러리 패쓰가 틀리다고 하지만 이렇게 해도 안됨. https://github.com/tomlooman/CoopHordeShooter/issues/1
	//UNavigationSystemV1::SimpleMoveToActor(GetController(), CurrentPatrolPoint);

	// 라이브러리가 .20 이후로 바뀜 AIBlueprintHelperLibrary를 사용해야함 https://github.com/tomlooman/StealthGameUdemy/issues/8
	UAIBlueprintHelperLibrary::SimpleMoveToActor(GetController(), CurrentPatrolPoint);
}


