#pragma once

#define TRACE_LENGTH 80000.f

#define CUSTOM_DEPTH_PURPLE 250
#define CUSOM_DEPTH_BLUE 251
#define CUSTOM_DEPTH_BLUE 252

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_AR UMETA(DisplayName = "Assault Rifle"),
	EWT_RocketLauncher UMETA(DisplayName = "Rocket Launcher"),
	EWT_Pistol UMETA(DisplayName = "Pistol"),
	EWT_SMG UMETA(DisplayName = "SMG"),
	EWT_Shotgun UMETA(DisplayName = "Shotgun"),
	EWT_SR UMETA(DisplayName = "Sniper Rifle"),
	EWT_GranadeLauncher UMETA(DisplayName = "Granade Launcher"),

	EWT_MAX UMETA(DisplayName = "DefaultMAX")
};