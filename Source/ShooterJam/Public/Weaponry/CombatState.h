#pragma once

UENUM(BlueprintType)
enum class ECombatState : uint8
{
	ECS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	ECS_Reloading UMETA(DisplayName = "Reloading"),
	ECS_Throwing UMETA(DisplayName = "Throwing"),

	ECS_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EWeaponSlot : uint8
{
	EWS_Main UMETA(DisplayName = "Main"),
	EWS_Secondary UMETA(DisplayName = "Secondary"),

	EWS_MAX UMETA(Display = "DefaultMAX")
};