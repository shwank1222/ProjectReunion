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
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	
	if (!IsValid(InstanceData.Character))
	{
		return EStateTreeRunStatus::Failed;
	}
	
	InstanceData.TargetActor = UGameplayStatics::GetPlayerCharacter(InstanceData.Character->GetWorld(), 0);
	
	if (!IsValid(InstanceData.TargetActor))
	{
		return EStateTreeRunStatus::Failed;
	}
	
	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FStateTreeGetPlayerInfoTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	auto& [Character, TargetActor,DistanceToTarget, bCanAttackTarget] = Context.GetInstanceData(*this);
	
	if (!IsValid(Character) || !IsValid(TargetActor))
	{
		return EStateTreeRunStatus::Failed;
	}
	
	DistanceToTarget = FVector::Dist(Character->GetActorLocation(), TargetActor->GetActorLocation());
	
	bCanAttackTarget = Character->CanAttackTarget(TargetActor);
	
	return EStateTreeRunStatus::Running;
}
