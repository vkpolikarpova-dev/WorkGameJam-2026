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

	// Точка, куда монстр всегда возвращается
	SpawnLocation = GetActorLocation();

	PlayerActor = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	GetCharacterMovement()->MaxWalkSpeed = MonsterSpeed;

	bPlayerInsideMonsterZone = false;
	bPlayerProtectedByLight = false;
	bReturningHome = false;

	DrainTimer = 0.0f;
	MoveUpdateTimer = 0.0f;
}


// =====================================================
// ИГРОК ВОШЁЛ В ЗОНУ МОНСТРА
// =====================================================

void ADarkMonster::PlayerEnteredMonsterZone()
{
	bPlayerInsideMonsterZone = true;

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("PLAYER ENTERED MONSTER ZONE")
	);

	// Если игрок не защищён светом,
	// монстр должен сразу снова стать активным.
	if (!bPlayerProtectedByLight)
	{
		bReturningHome = false;

		AAIController* MonsterAIController =
			Cast<AAIController>(GetController());

		if (MonsterAIController)
		{
			MonsterAIController->StopMovement();
		}

		MoveUpdateTimer = MoveUpdateInterval;
	}
}


// =====================================================
// ИГРОК ВЫШЕЛ ИЗ ЗОНЫ МОНСТРА
// =====================================================

void ADarkMonster::PlayerLeftMonsterZone()
{
	bPlayerInsideMonsterZone = false;

	DrainTimer = 0.0f;

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("PLAYER LEFT MONSTER ZONE")
	);

	// Игрок ушёл с территории —
	// монстр возвращается домой.
	StartReturnHome();
}


// =====================================================
// ИГРОК ВОШЁЛ В АКТИВНУЮ СВЕТОВУЮ ЗОНУ
// =====================================================

void ADarkMonster::PlayerEnteredLightZone()
{
	bPlayerProtectedByLight = true;

	DrainTimer = 0.0f;

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("PLAYER ENTERED LIGHT ZONE")
	);

	// Свет отпугивает монстра.
	StartReturnHome();
}


// =====================================================
// ИГРОК ВЫШЕЛ ИЗ СВЕТА / СВЕТ ВЫКЛЮЧИЛСЯ
// =====================================================

void ADarkMonster::PlayerLeftLightZone()
{
	bPlayerProtectedByLight = false;

	DrainTimer = 0.0f;

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("PLAYER LEFT LIGHT ZONE")
	);

	// Если игрок всё ещё на территории монстра,
	// монстр прекращает возвращение домой
	// и снова начинает погоню.
	if (bPlayerInsideMonsterZone)
	{
		bReturningHome = false;

		AAIController* MonsterAIController =
			Cast<AAIController>(GetController());

		if (MonsterAIController)
		{
			MonsterAIController->StopMovement();
		}

		MoveUpdateTimer = MoveUpdateInterval;
	}
	else
	{
		// Игрок вообще ушёл из MonsterZone,
		// поэтому продолжаем возвращаться домой.
		StartReturnHome();
	}
}


// =====================================================
// TICK
// =====================================================

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


	// =================================================
	// ЕСЛИ ИГРОК ВНЕ ЗОНЫ ИЛИ ЗАЩИЩЁН СВЕТОМ
	// =================================================

	if (!bPlayerInsideMonsterZone || bPlayerProtectedByLight)
	{
		// Если монстр ещё не дома — возвращаем.
		if (!IsAtHome())
		{
			if (!bReturningHome)
			{
				StartReturnHome();
			}

			UpdateReturnHome();
		}
		else
		{
			AAIController* MonsterAIController =
				Cast<AAIController>(GetController());

			if (MonsterAIController)
			{
				MonsterAIController->StopMovement();
			}

			bReturningHome = false;
		}

		return;
	}


	// =================================================
	// ИГРОК В MONSTER ZONE И НЕ ЗАЩИЩЁН
	// =================================================

	// Если до этого монстр возвращался домой —
	// отменяем возврат.
	if (bReturningHome)
	{
		bReturningHome = false;

		AAIController* MonsterAIController =
			Cast<AAIController>(GetController());

		if (MonsterAIController)
		{
			MonsterAIController->StopMovement();
		}

		MoveUpdateTimer = MoveUpdateInterval;
	}


	// ==============================
	// ПОГОНЯ
	// ==============================

	MoveUpdateTimer += DeltaTime;

	if (MoveUpdateTimer >= MoveUpdateInterval)
	{
		MoveUpdateTimer = 0.0f;

		UpdateChase();
	}


	// ==============================
	// ВЫСАСЫВАНИЕ
	// ==============================

	UpdateDrain(DeltaTime);
}


// =====================================================
// ПОГОНЯ
// =====================================================

void ADarkMonster::UpdateChase()
{
	if (!PlayerActor)
	{
		return;
	}

	if (!bPlayerInsideMonsterZone)
	{
		return;
	}

	if (bPlayerProtectedByLight)
	{
		return;
	}

	AAIController* MonsterAIController =
		Cast<AAIController>(GetController());

	if (!MonsterAIController)
	{
		return;
	}

	MonsterAIController->MoveToActor(
		PlayerActor,
		ChaseAcceptanceRadius,
		false,
		true,
		true
	);
}


// =====================================================
// ВЫСАСЫВАНИЕ СВЕТА
// =====================================================

void ADarkMonster::UpdateDrain(float DeltaTime)
{
	if (!PlayerActor)
	{
		return;
	}

	if (!bPlayerInsideMonsterZone || bPlayerProtectedByLight)
	{
		DrainTimer = 0.0f;
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


// =====================================================
// НАЧАТЬ ВОЗВРАЩЕНИЕ ДОМОЙ
// =====================================================

void ADarkMonster::StartReturnHome()
{
	// Если уже почти дома — просто останавливаемся.
	if (IsAtHome())
	{
		AAIController* MonsterAIController =
			Cast<AAIController>(GetController());

		if (MonsterAIController)
		{
			MonsterAIController->StopMovement();
		}

		bReturningHome = false;

		return;
	}

	AAIController* MonsterAIController =
		Cast<AAIController>(GetController());

	if (!MonsterAIController)
	{
		return;
	}

	bReturningHome = true;

	DrainTimer = 0.0f;

	// Останавливаем текущую погоню.
	MonsterAIController->StopMovement();

	// И отправляем домой.
	MonsterAIController->MoveToLocation(
		SpawnLocation,
		SpawnAcceptanceRadius,
		false,
		true,
		true,
		false
	);

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("MONSTER RETURNING HOME")
	);
}


// =====================================================
// ОБНОВЛЕНИЕ ВОЗВРАТА
// =====================================================

void ADarkMonster::UpdateReturnHome()
{
	if (!bReturningHome)
	{
		return;
	}

	AAIController* MonsterAIController =
		Cast<AAIController>(GetController());

	if (!MonsterAIController)
	{
		bReturningHome = false;
		return;
	}

	if (IsAtHome())
	{
		MonsterAIController->StopMovement();

		bReturningHome = false;

		UE_LOG(
			LogTemp,
			Warning,
			TEXT("MONSTER RETURNED HOME")
		);

		return;
	}
}


// =====================================================
// ПРОВЕРКА: МОНСТР ДОМА?
// =====================================================

bool ADarkMonster::IsAtHome() const
{
	const float DistanceToSpawn =
		FVector::Dist2D(
			GetActorLocation(),
			SpawnLocation
		);

	return DistanceToSpawn <= SpawnAcceptanceRadius;
}