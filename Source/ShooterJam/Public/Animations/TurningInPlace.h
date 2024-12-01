#pragma once

UENUM(BlueprintType)
enum class ETurningInPlace : uint8
{
	TIP_Left UMETA(DisplayName = "Turning left"),
	TIP_Right UMETA(DisplayName = "Turning right"),
	TIP_NotTurning UMETA(DisplayName = "Not turning"),

	TIP_MAX UMETA(DisplayName = "DefaultMAX")
};