#include "DarkMonster.h"

#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"

ADarkMonster::ADarkMonster()
{
	PrimaryActorTick.bCanEverTick = true;

	AIControllerClass = AAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void ADarkMonster::BeginPlay()
{
	Super::BeginPlay();

	// Запоминаем место, где монстр стоял при запуске игры
	SpawnLocation = GetActorLocation();

	PlayerActor = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	GetCharacterMovement()->MaxWalkSpeed = MonsterSpeed;
}

void ADarkMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!PlayerActor)
	{
		PlayerActor = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

		if (!PlayerActor)
		{
			return;
		}
	}

	// Если монстр был отпугнут светом,
	// погоня и высасывание света отключаются
	if (bReturningToSpawn)
	{
		UpdateReturnToSpawn();
		return;
	}

	// ===== ПОГОНЯ =====

	MoveUpdateTimer += DeltaTime;

	if (MoveUpdateTimer >= MoveUpdateInterval)
	{
		MoveUpdateTimer = 0.0f;
		UpdateChase();
	}

	// ===== ВЫСАСЫВАНИЕ СВЕТА =====

	UpdateDrain(DeltaTime);
}

void ADarkMonster::UpdateChase()
{
	if (!PlayerActor)
	{
		return;
	}

	const float DistanceToPlayer =
		FVector::Dist(
			GetActorLocation(),
			PlayerActor->GetActorLocation()
		);

	AAIController* MonsterAIController =
		Cast<AAIController>(GetController());

	if (!MonsterAIController)
	{
		return;
	}

	if (DistanceToPlayer <= DetectionRadius)
	{
		MonsterAIController->MoveToActor(
			PlayerActor,
			DrainRadius * 0.75f,
			true,
			true,
			true
		);
	}
	else
	{
		MonsterAIController->StopMovement();
	}
}

void ADarkMonster::UpdateDrain(float DeltaTime)
{
	if (!PlayerActor)
	{
		return;
	}

	const float DistanceToPlayer =
		FVector::Dist(
			GetActorLocation(),
			PlayerActor->GetActorLocation()
		);

	if (DistanceToPlayer > DrainRadius)
	{
		DrainTimer = 0.0f;
		return;
	}

	DrainTimer += DeltaTime;

	if (DrainTimer >= DrainInterval)
	{
		DrainTimer = 0.0f;

		DrainPlayerLight(
			PlayerActor,
			DrainAmount
		);
	}
}

void ADarkMonster::FleeFromLight(AActor* LightSource)
{
	if (!LightSource)
	{
		return;
	}

	AAIController* MonsterAIController =
		Cast<AAIController>(GetController());

	if (!MonsterAIController)
	{
		return;
	}

	// Включаем режим возвращения домой
	bReturningToSpawn = true;

	// На всякий случай сбрасываем таймер высасывания
	DrainTimer = 0.0f;

	// Отменяем погоню за игроком
	MonsterAIController->StopMovement();

	// Отправляем монстра туда, где он появился
	MonsterAIController->MoveToLocation(
		SpawnLocation,
		SpawnAcceptanceRadius,
		true,
		true,
		true,
		false
	);
}

void ADarkMonster::UpdateReturnToSpawn()
{
	AAIController* MonsterAIController =
		Cast<AAIController>(GetController());

	if (!MonsterAIController)
	{
		return;
	}

	const float DistanceToSpawn =
		FVector::Dist2D(
			GetActorLocation(),
			SpawnLocation
		);

	// Уже вернулся домой
	if (DistanceToSpawn <= SpawnAcceptanceRadius)
	{
		MonsterAIController->StopMovement();

		bReturningToSpawn = false;

		return;
	}

	// Продолжаем идти домой.
	// Повторяем команду на случай перестроения NavMesh/маршрута.
	MonsterAIController->MoveToLocation(
		SpawnLocation,
		SpawnAcceptanceRadius,
		true,
		true,
		true,
		false
	);
}