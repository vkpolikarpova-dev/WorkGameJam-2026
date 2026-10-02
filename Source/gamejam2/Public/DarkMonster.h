#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DarkMonster.generated.h"

UCLASS()
class GAMEJAM2_API ADarkMonster : public ACharacter
{
	GENERATED_BODY()

public:
	ADarkMonster();

	virtual void Tick(float DeltaTime) override;

	// Вызывается из BP_LightSource, когда монстр попадает в активный свет
	UFUNCTION(BlueprintCallable, Category = "Monster|Light")
	void FleeFromLight(AActor* LightSource);

protected:
	virtual void BeginPlay() override;

	// ===== ПОГОНЯ =====

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|Chase")
	float DetectionRadius = 800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|Chase")
	float MonsterSpeed = 180.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|Chase")
	float MoveUpdateInterval = 0.25f;


	// ===== ВЫСАСЫВАНИЕ СВЕТА =====

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|Drain")
	float DrainRadius = 120.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|Drain")
	float DrainAmount = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|Drain")
	float DrainInterval = 1.0f;

	UFUNCTION(BlueprintImplementableEvent, Category = "Monster|Drain")
	void DrainPlayerLight(AActor* Player, float Amount);


	// ===== ВОЗВРАТ НА СПАВН =====

	// Насколько близко нужно подойти к стартовой точке,
	// чтобы считать, что монстр вернулся
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|Light")
	float SpawnAcceptanceRadius = 80.0f;

private:
	UPROPERTY()
	AActor* PlayerActor = nullptr;

	FVector SpawnLocation;

	float DrainTimer = 0.0f;
	float MoveUpdateTimer = 0.0f;

	bool bReturningToSpawn = false;

	void UpdateChase();
	void UpdateDrain(float DeltaTime);
	void UpdateReturnToSpawn();
};