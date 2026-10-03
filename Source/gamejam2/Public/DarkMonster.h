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

	// Игрок вошёл в территорию монстра
	UFUNCTION(BlueprintCallable, Category = "Monster|Zone")
	void PlayerEnteredMonsterZone();

	// Игрок вышел из территории монстра
	UFUNCTION(BlueprintCallable, Category = "Monster|Zone")
	void PlayerLeftMonsterZone();

	// Игрок вошёл в активную световую безопасную зону
	UFUNCTION(BlueprintCallable, Category = "Monster|Light")
	void PlayerEnteredLightZone();

	// Игрок вышел из световой зоны
	// ИЛИ свет в этой зоне был выключен/забран
	UFUNCTION(BlueprintCallable, Category = "Monster|Light")
	void PlayerLeftLightZone();

	// Уменьшение света игрока реализовано в BP_DarkMonster
	UFUNCTION(BlueprintImplementableEvent, Category = "Monster|Drain")
	void DrainPlayerLight(AActor* Player, float Amount);

protected:

	virtual void BeginPlay() override;

private:

	// ==============================
	// ПОГОНЯ
	// ==============================

	void UpdateChase();

	UPROPERTY(EditAnywhere, Category = "Monster|Chase")
	float MonsterSpeed = 180.0f;

	UPROPERTY(EditAnywhere, Category = "Monster|Chase")
	float MoveUpdateInterval = 0.25f;

	UPROPERTY(EditAnywhere, Category = "Monster|Chase")
	float ChaseAcceptanceRadius = 20.0f;

	float MoveUpdateTimer = 0.0f;


	// ==============================
	// ВЫСАСЫВАНИЕ СВЕТА
	// ==============================

	void UpdateDrain(float DeltaTime);

	UPROPERTY(EditAnywhere, Category = "Monster|Drain")
	float DrainRadius = 120.0f;

	UPROPERTY(EditAnywhere, Category = "Monster|Drain")
	float DrainAmount = 5.0f;

	UPROPERTY(EditAnywhere, Category = "Monster|Drain")
	float DrainInterval = 1.0f;

	float DrainTimer = 0.0f;


	// ==============================
	// ВОЗВРАЩЕНИЕ ДОМОЙ
	// ==============================

	void StartReturnHome();
	void UpdateReturnHome();
	bool IsAtHome() const;

	UPROPERTY(EditAnywhere, Category = "Monster|Return")
	float SpawnAcceptanceRadius = 40.0f;

	FVector SpawnLocation = FVector::ZeroVector;

	bool bReturningHome = false;


	// ==============================
	// СОСТОЯНИЕ
	// ==============================

	// Игрок физически находится внутри BP_MonsterZone
	bool bPlayerInsideMonsterZone = false;

	// Игрок сейчас защищён активным светом
	bool bPlayerProtectedByLight = false;


	// ==============================
	// ИГРОК
	// ==============================

	AActor* PlayerActor = nullptr;
};