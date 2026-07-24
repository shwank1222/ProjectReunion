// Fill out your copyright notice in the Description page of Project Settings.


#include "OSMKStateTreeUtility.h"

#include "EnemyCharacter.h"
#include "StateTreeExecutionContext.h"
#include "Kismet/GameplayStatics.h"

EStateTreeRunStatus FStateTreeShootAtTargetTask::EnterState(FStateTreeExecutionContext& Context,
                                                            const FStateTreeTransitionResult& Transition) const
{
	auto [Character] = Context.GetInstanceData(*this);
	if (!IsValid(Character))
	{
		return EStateTreeRunStatus::Failed;
	}
	
	Character->Fire();
	
	return EStateTreeRunStatus::Succeeded;
}

EStateTreeRunStatus FStateTreeGetPlayerInfoTask::EnterState(FStateTreeExecutionContext& Context,
                                                            const FStateTreeTransitionResult& Transition) const
{
	auto& [Character, TargetActor, DistanceToTarget] = Context.GetInstanceData(*this);
	
	if (!IsValid(Character))
	{
		return EStateTreeRunStatus::Failed;
	}
	
	TargetActor = UGameplayStatics::GetPlayerCharacter(Character->GetWorld(), 0);
	
	if (!IsValid(TargetActor))
	{
		return EStateTreeRunStatus::Failed;
	}
	
	DistanceToTarget = FVector::Dist(Character->GetActorLocation(), TargetActor->GetActorLocation());
	
	return EStateTreeRunStatus::Running;
}
