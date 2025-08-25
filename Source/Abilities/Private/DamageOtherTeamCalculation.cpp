// Fill out your copyright notice in the Description page of Project Settings.
#include "DamageOtherTeamCalculation.h"

#include "CaptureTheFlag/HealthAttributeSet.h"

UDamageOtherTeamCalculation::UDamageOtherTeamCalculation()
{
	const FCapturedAttributes Attributes;
	RelevantAttributesToCapture.Add(Attributes.CalculatedDamageDef);
}

void UDamageOtherTeamCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                                         FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	Super::Execute_Implementation(ExecutionParams, OutExecutionOutput);

	const FCapturedAttributes Attributes;

	bool bIsTargetAnEnemy = false;

	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	if (TargetASC && SourceASC)
	{
		const FGameplayTag BlueTeamTag = FGameplayTag::RequestGameplayTag("Character.Team.Blue");
		const FGameplayTag RedTeamTag = FGameplayTag::RequestGameplayTag("Character.Team.Red");
		// if we wanted to make something always take damage unconditionally, like environment, could make something like this
		// FGameplayTag NoTeamTag = FGameplayTag::RequestGameplayTag("Character.Team.NoTeam");

		const FGameplayTagContainer SourceTags = SourceASC->GetOwnedGameplayTags();
		const FGameplayTagContainer TargetTags = TargetASC->GetOwnedGameplayTags();

		bIsTargetAnEnemy = (SourceTags.HasTag(RedTeamTag) && TargetTags.HasTag(BlueTeamTag)) ||
					       (SourceTags.HasTag(BlueTeamTag) && TargetTags.HasTag(RedTeamTag));
	}


	float Damage = 0.0f;
	FGameplayModifierEvaluatedData CalcDamageOutput;
	if (bIsTargetAnEnemy)
	{
		const FAggregatorEvaluateParameters EvalParams;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
			Attributes.CalculatedDamageDef,
			EvalParams,
			Damage);

		CalcDamageOutput = FGameplayModifierEvaluatedData(Attributes.CalculatedDamageProperty, EGameplayModOp::Additive, Damage);
	}
	else
	{
		CalcDamageOutput = FGameplayModifierEvaluatedData(Attributes.CalculatedDamageProperty, EGameplayModOp::Override, 0.0f);
	}
	
	OutExecutionOutput.AddOutputModifier(CalcDamageOutput);
}
