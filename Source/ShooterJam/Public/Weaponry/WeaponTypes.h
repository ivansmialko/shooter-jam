#pragma once

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_AR UMETA(DisplayNames = "Assault Rifle"),

	EWT_MAX UMETA(DisplayName = "DefaultMAX")
};