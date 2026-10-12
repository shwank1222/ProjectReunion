#include "Data/Stage/StageExtractRule.h"
#include "Data/Stage/StageExtractedCache.h"
#include "Data/Stage/StageExtractPCGWaiter.h"
#include "Data/Stage/StageExtractVolumeHelper.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Engine/StaticMesh.h"
#include "PCGComponent.h"
#include "UObject/StructOnScope.h"
#include "TimerManager.h"

#if WITH_EDITOR
#include "EngineUtils.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Engine/SimpleConstructionScript.h"
#include "Engine/SCS_Node.h"
#endif

DEFINE_LOG_CATEGORY_STATIC(LogStageExtract, Log, All);

#if WITH_EDITOR
namespace
{
	struct FOptionTarget
	{
		FString Prefix;
		const UClass* Class = nullptr;
	};

	TArray<FOptionTarget> CollectOptionTargets(UClass* ActorClass)
	{
		TArray<FOptionTarget> Targets;
		if (!ActorClass)
		{
			return Targets;
		}

		Targets.Add({ FString(), ActorClass });

		if (const AActor* CDO = ActorClass->GetDefaultObject<AActor>())
		{
			TInlineComponentArray<UActorComponent*> Components;
			CDO->GetComponents(Components);

			for (const UActorComponent* Comp : Components)
			{
				if (Comp)
				{
					Targets.Add({ Comp->GetName(), Comp->GetClass() });
				}
			}
		}

		for (const UClass* Class = ActorClass; Class; Class = Class->GetSuperClass())
		{
			const UBlueprintGeneratedClass* BPClass = Cast<UBlueprintGeneratedClass>(Class);
			if (!BPClass || !BPClass->SimpleConstructionScript)
			{
				continue;
			}

			for (const USCS_Node* Node : BPClass->SimpleConstructionScript->GetAllNodes())
			{
				if (Node && Node->ComponentClass)
				{
					Targets.Add({ Node->GetVariableName().ToString(), Node->ComponentClass });
				}
			}
		}

		return Targets;
	}

	FString MakeOptionPath(const FString& Prefix, const FString& Name)
	{
		return Prefix.IsEmpty() ? Name : Prefix + TEXT(".") + Name;
	}

	bool IsSyncableProperty(const FProperty* Prop)
	{
		return Prop->HasAnyPropertyFlags(CPF_Edit)
			&& !Prop->HasAnyPropertyFlags(CPF_EditConst | CPF_Deprecated | CPF_Transient)
			&& !Prop->IsA<FDelegateProperty>()
			&& !Prop->IsA<FMulticastDelegateProperty>();
	}

	bool IsCallableFunction(const UFunction* Func)
	{
		return Func->HasAnyFunctionFlags(FUNC_BlueprintCallable)
			&& !Func->HasAnyFunctionFlags(FUNC_Static | FUNC_Delegate | FUNC_EditorOnly);
	}

	bool IsGenericBaseMember(const UStruct* Owner)
	{
		return Owner == UObject::StaticClass()
			|| Owner == AActor::StaticClass()
			|| Owner == UActorComponent::StaticClass();
	}

	struct FClassOptions
	{
		TWeakObjectPtr<const UObject> DefaultObject;
		TArray<FString> Properties;
		TArray<FString> Functions;
	};

	const FClassOptions& GetClassOptions(UClass* ActorClass)
	{
		static TMap<TWeakObjectPtr<const UClass>, FClassOptions> Cache;
		static const FClassOptions Empty;

		if (!ActorClass)
		{
			return Empty;
		}

		const UObject* DefaultObject = ActorClass->GetDefaultObject();
		FClassOptions& Entry = Cache.FindOrAdd(ActorClass);
		if (Entry.DefaultObject.Get() == DefaultObject)
		{
			return Entry;
		}

		TSet<FString> Properties;
		TSet<FString> Functions;

		for (const FOptionTarget& Target : CollectOptionTargets(ActorClass))
		{
			for (TFieldIterator<FProperty> It(Target.Class); It; ++It)
			{
				if (!IsGenericBaseMember(It->GetOwnerStruct()) && IsSyncableProperty(*It))
				{
					Properties.Add(MakeOptionPath(Target.Prefix, It->GetName()));
				}
			}

			for (TFieldIterator<UFunction> It(Target.Class); It; ++It)
			{
				if (!IsGenericBaseMember(It->GetOwnerClass()) && IsCallableFunction(*It))
				{
					Functions.Add(MakeOptionPath(Target.Prefix, It->GetName()));
				}
			}
		}

		Entry.DefaultObject = DefaultObject;
		Entry.Properties = Properties.Array();
		Entry.Properties.Sort();
		Entry.Functions = Functions.Array();
		Entry.Functions.Sort();
		return Entry;
	}
}
#endif

void UStageExtractRule::PostLoad()
{
	Super::PostLoad();

#if WITH_EDITORONLY_DATA
	if (PCGParamNames_DEPRECATED.Num() > 0)
	{
		for (const FName& ParamPath : PCGParamNames_DEPRECATED)
		{
			SyncProperties.AddUnique(ParamPath);
		}
		PCGParamNames_DEPRECATED.Empty();
		Fields |= static_cast<int32>(EStageExtractField::Properties);
	}
#endif
}

#if WITH_EDITOR
void UStageExtractRule::ExtractFromWorld(UWorld* World, FName LevelName, UStageExtractedCache* Cache) const
{
	if (!World || !Cache || !TargetClass || RuleId == NAME_None)
	{
		return;
	}

	FStageExtractedGroup Group;

	for (TActorIterator<AActor> It(World, TargetClass); It; ++It)
	{
		AActor* Actor = *It;
		if (!Actor || !MatchesActor(Actor))
		{
			continue;
		}

		if (bMatchExactClass && Actor->GetClass() != TargetClass)
		{
			continue;
		}

		FStageExtractedItem Item;
		FillItem(Actor, Item);
		Group.Items.Add(Item);
	}

	Cache->Write(RuleId, LevelName, Group);
}

TArray<FString> UStageExtractRule::GetPropertyOptions() const
{
	return GetClassOptions(TargetClass).Properties;
}

TArray<FString> UStageExtractRule::GetFunctionOptions() const
{
	return GetClassOptions(TargetClass).Functions;
}

bool UStageExtractRule::MatchesActor(const AActor* Actor) const
{
	return Actor != nullptr;
}

void UStageExtractRule::FillItem(const AActor* Actor, FStageExtractedItem& OutItem) const
{
	if (!Actor)
	{
		return;
	}

	if (HasField(EStageExtractField::Class))
	{
		OutItem.ActorClass = TSoftClassPtr<AActor>(Actor->GetClass());
	}

	if (HasField(EStageExtractField::Transform))
	{
		OutItem.Transform = Actor->GetActorTransform();
	}

	if (HasField(EStageExtractField::Mesh) || HasField(EStageExtractField::Collision) || HasField(EStageExtractField::Materials))
	{
		if (UStaticMeshComponent* SMComp = Actor->FindComponentByClass<UStaticMeshComponent>())
		{
			if (HasField(EStageExtractField::Mesh))
			{
				OutItem.Mesh = TSoftObjectPtr<UStaticMesh>(SMComp->GetStaticMesh());
			}

			if (HasField(EStageExtractField::Collision))
			{
				OutItem.CollisionProfile = SMComp->GetCollisionProfileName();
			}

			if (HasField(EStageExtractField::Materials))
			{
				const int32 MatCount = SMComp->GetNumMaterials();
				for (int32 i = 0; i < MatCount; ++i)
				{
					OutItem.Materials.Add(TSoftObjectPtr<UMaterialInterface>(SMComp->GetMaterial(i)));
				}
			}
		}
	}

	StageExtractVolume::CaptureBrushBounds(Actor, OutItem);

	if (HasField(EStageExtractField::Properties))
	{
		for (const FName& ParamPath : SyncProperties)
		{
			CaptureParam(Actor, ParamPath, OutItem.PropertyValues);
		}
	}
}

void UStageExtractRule::CaptureParam(const AActor* Actor, FName ParamPath, TMap<FName, FString>& OutParams) const
{
	FName PropertyName = NAME_None;
	const UObject* Target = ResolveParamTarget(Actor, ParamPath, PropertyName);
	if (!Target)
	{
		UE_LOG(LogStageExtract, Warning, TEXT("[%s] Param '%s': target not found on %s"),
			*RuleId.ToString(), *ParamPath.ToString(), *Actor->GetName());
		return;
	}

	const FProperty* Prop = Target->GetClass()->FindPropertyByName(PropertyName);
	if (!Prop)
	{
		UE_LOG(LogStageExtract, Warning, TEXT("[%s] Param '%s': property '%s' not found on %s"),
			*RuleId.ToString(), *ParamPath.ToString(), *PropertyName.ToString(), *Target->GetName());
		return;
	}

	FString ValueText;
	Prop->ExportText_InContainer(0, ValueText, Target, nullptr, nullptr, PPF_None);
	OutParams.Add(ParamPath, ValueText);
}
#endif

void UStageExtractRule::SpawnStage(FStageSpawnContext& Ctx) const
{
	if (!Ctx.World || !Ctx.Cache || RuleId == NAME_None)
	{
		return;
	}

	const FStageExtractedGroup* Group = Ctx.Cache->Read(RuleId, Ctx.LevelName);
	if (!Group)
	{
		return;
	}

	for (const FStageExtractedItem& Item : Group->Items)
	{
		SpawnItem(Ctx, Item);
	}
}

void UStageExtractRule::SpawnItem(FStageSpawnContext& Ctx, const FStageExtractedItem& Item) const
{
	if (!Ctx.World)
	{
		return;
	}

	UClass* ClassToSpawn = nullptr;
	if (HasField(EStageExtractField::Class))
	{
		ClassToSpawn = Item.ActorClass.LoadSynchronous();
	}
	if (!ClassToSpawn && TargetClass)
	{
		ClassToSpawn = TargetClass.Get();
	}
	if (!ClassToSpawn)
	{
		return;
	}

	AActor* Spawned = Ctx.World->SpawnActorDeferred<AActor>(ClassToSpawn, Item.Transform, nullptr, nullptr,
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
	if (!Spawned)
	{
		return;
	}

	const bool bUseProperties = HasField(EStageExtractField::Properties);

	if (bUseProperties)
	{
		ApplyParams(Spawned, Item.PropertyValues, false);
	}

	Spawned->FinishSpawning(Item.Transform);
	StageExtractVolume::ApplyBrushBounds(Spawned, Item);

	ConfigureSpawnedActor(Spawned, Item, Ctx);
	if (bUseProperties)
	{
		ApplyParams(Spawned, Item.PropertyValues, true);
	}

	if (HasField(EStageExtractField::Functions) && PostSpawnFunctions.Num() > 0)
	{
		SchedulePostSpawnFunctions(Spawned, Ctx);
	}
	else
	{
		WaitForPCGGeneration(Spawned, Ctx.BeginAsync, Ctx.EndAsync);
	}

	if (Ctx.TrackActor)
	{
		Ctx.TrackActor(Spawned);
	}
}

void UStageExtractRule::ConfigureSpawnedActor(AActor* SpawnedActor, const FStageExtractedItem& Item, FStageSpawnContext& Ctx) const
{
	if (!SpawnedActor)
	{
		return;
	}

	const bool bNeedsMesh = HasField(EStageExtractField::Mesh);
	const bool bNeedsCollision = HasField(EStageExtractField::Collision);
	const bool bNeedsMaterials = HasField(EStageExtractField::Materials);

	if (!bNeedsMesh && !bNeedsCollision && !bNeedsMaterials)
	{
		return;
	}

	UStaticMeshComponent* SMComp = SpawnedActor->FindComponentByClass<UStaticMeshComponent>();
	if (!SMComp)
	{
		return;
	}

	if (bNeedsMesh)
	{
		if (UStaticMesh* Mesh = Item.Mesh.LoadSynchronous())
		{
			SpawnedActor->SetActorScale3D(Item.Transform.GetScale3D());
			SMComp->SetMobility(EComponentMobility::Movable);
			SMComp->SetStaticMesh(Mesh);
		}
	}

	if (bNeedsCollision && Item.CollisionProfile != NAME_None)
	{
		SMComp->SetCollisionProfileName(Item.CollisionProfile);
	}

	if (bNeedsMaterials)
	{
		for (int32 i = 0; i < Item.Materials.Num(); ++i)
		{
			if (UMaterialInterface* Mat = Item.Materials[i].LoadSynchronous())
			{
				SMComp->SetMaterial(i, Mat);
			}
		}
	}
}

bool UStageExtractRule::ApplyParams(AActor* SpawnedActor, const TMap<FName, FString>& Params, bool bComponentParams) const
{
	if (!SpawnedActor)
	{
		return false;
	}

	bool bApplied = false;

	for (const TPair<FName, FString>& Param : Params)
	{
		const bool bIsComponentParam = Param.Key.ToString().Contains(TEXT("."));
		if (bIsComponentParam != bComponentParams)
		{
			continue;
		}

		FName PropertyName = NAME_None;
		UObject* Target = ResolveParamTarget(SpawnedActor, Param.Key, PropertyName);
		if (!Target)
		{
			UE_LOG(LogStageExtract, Warning, TEXT("[%s] Param '%s': target not found on %s"),
				*RuleId.ToString(), *Param.Key.ToString(), *SpawnedActor->GetName());
			continue;
		}

		const FProperty* Prop = Target->GetClass()->FindPropertyByName(PropertyName);
		if (!Prop)
		{
			UE_LOG(LogStageExtract, Warning, TEXT("[%s] Param '%s': property '%s' not found on %s"),
				*RuleId.ToString(), *Param.Key.ToString(), *PropertyName.ToString(), *Target->GetName());
			continue;
		}

		void* ValuePtr = Prop->ContainerPtrToValuePtr<void>(Target);
		if (!Prop->ImportText_Direct(*Param.Value, ValuePtr, Target, PPF_None))
		{
			UE_LOG(LogStageExtract, Warning, TEXT("[%s] Param '%s': failed to import '%s'"),
				*RuleId.ToString(), *Param.Key.ToString(), *Param.Value);
			continue;
		}

		RefreshAfterParamChange(Target);
		bApplied = true;
	}

	return bApplied;
}

void UStageExtractRule::SchedulePostSpawnFunctions(AActor* SpawnedActor, FStageSpawnContext& Ctx) const
{
	if (!SpawnedActor || !Ctx.World)
	{
		return;
	}

	if (Ctx.BeginAsync)
	{
		Ctx.BeginAsync();
	}

	TWeakObjectPtr<const UStageExtractRule> WeakRule = this;
	TWeakObjectPtr<AActor> WeakActor = SpawnedActor;
	TFunction<void()> BeginAsync = Ctx.BeginAsync;
	TFunction<void()> EndAsync = Ctx.EndAsync;

	Ctx.World->GetTimerManager().SetTimerForNextTick([WeakRule, WeakActor, BeginAsync, EndAsync]()
	{
		const UStageExtractRule* Rule = WeakRule.Get();
		AActor* Actor = WeakActor.Get();

		if (!Rule || !Actor)
		{
			return;
		}

		Rule->CallPostSpawnFunctions(Actor);
		WaitForPCGGeneration(Actor, BeginAsync, EndAsync);

		if (EndAsync)
		{
			EndAsync();
		}
	});
}

void UStageExtractRule::CallPostSpawnFunctions(AActor* SpawnedActor) const
{
	if (!SpawnedActor)
	{
		return;
	}

	for (const FStageFunctionCall& Call : PostSpawnFunctions)
	{
		FName FunctionName = NAME_None;
		UObject* Target = ResolveParamTarget(SpawnedActor, Call.Function, FunctionName);
		UFunction* Func = Target ? Target->FindFunction(FunctionName) : nullptr;

		if (!Func)
		{
			UE_LOG(LogStageExtract, Warning, TEXT("[%s] PostSpawnFunction '%s': not found on %s"),
				*RuleId.ToString(), *Call.Function.ToString(), *SpawnedActor->GetName());
			continue;
		}

		FStructOnScope ParamsScope(Func);
		uint8* ParamsBuffer = ParamsScope.GetStructMemory();

		int32 ArgIndex = 0;
		for (TFieldIterator<FProperty> It(Func); It && It->HasAnyPropertyFlags(CPF_Parm); ++It)
		{
			const FProperty* Param = *It;
			const bool bIsOutput = Param->HasAnyPropertyFlags(CPF_ReturnParm)
				|| (Param->HasAnyPropertyFlags(CPF_OutParm) && !Param->HasAnyPropertyFlags(CPF_ReferenceParm));
			if (bIsOutput)
			{
				continue;
			}

			if (Call.Arguments.IsValidIndex(ArgIndex) && !Call.Arguments[ArgIndex].IsEmpty())
			{
				if (!Param->ImportText_InContainer(*Call.Arguments[ArgIndex], ParamsBuffer, Target, PPF_None))
				{
					UE_LOG(LogStageExtract, Warning, TEXT("[%s] PostSpawnFunction '%s': failed to import argument %d '%s'"),
						*RuleId.ToString(), *Call.Function.ToString(), ArgIndex, *Call.Arguments[ArgIndex]);
				}
			}
			++ArgIndex;
		}

		Target->ProcessEvent(Func, ParamsBuffer);
	}
}

void UStageExtractRule::WaitForPCGGeneration(AActor* SpawnedActor, const TFunction<void()>& BeginAsync, const TFunction<void()>& EndAsync)
{
	if (!SpawnedActor)
	{
		return;
	}

	TArray<UPCGComponent*> PCGComps;
	SpawnedActor->GetComponents<UPCGComponent>(PCGComps);

	for (UPCGComponent* PCG : PCGComps)
	{
		if (!PCG || !PCG->IsGenerating())
		{
			continue;
		}

		if (BeginAsync)
		{
			BeginAsync();
		}

		UStageExtractPCGWaiter* Waiter = NewObject<UStageExtractPCGWaiter>(SpawnedActor);
		Waiter->AddToRoot();
		Waiter->OnComplete = EndAsync;
		PCG->OnPCGGraphGeneratedDelegate.AddUObject(Waiter, &UStageExtractPCGWaiter::HandlePCGGenerated);
	}
}

UObject* UStageExtractRule::ResolveParamTarget(const AActor* Actor, FName ParamPath, FName& OutPropertyName)
{
	OutPropertyName = NAME_None;

	if (!Actor || ParamPath == NAME_None)
	{
		return nullptr;
	}

	FString ComponentName;
	FString PropertyName;
	if (!ParamPath.ToString().Split(TEXT("."), &ComponentName, &PropertyName))
	{
		OutPropertyName = ParamPath;
		return const_cast<AActor*>(Actor);
	}

	OutPropertyName = FName(*PropertyName);

	TArray<UActorComponent*> Components;
	Actor->GetComponents(Components);

	for (UActorComponent* Comp : Components)
	{
		if (Comp && Comp->GetName() == ComponentName)
		{
			return Comp;
		}
	}

	for (UActorComponent* Comp : Components)
	{
		if (!Comp)
		{
			continue;
		}

		const FString InstanceName = Comp->GetName();
		if (InstanceName.Contains(ComponentName) || ComponentName.Contains(InstanceName))
		{
			return Comp;
		}
	}

	return nullptr;
}

void UStageExtractRule::RefreshAfterParamChange(UObject* Target)
{
	if (UBoxComponent* Box = Cast<UBoxComponent>(Target))
	{
		Box->SetBoxExtent(Box->GetUnscaledBoxExtent(), true);
		return;
	}

	if (USceneComponent* SceneComp = Cast<USceneComponent>(Target))
	{
		SceneComp->UpdateBounds();
		SceneComp->MarkRenderStateDirty();
	}
}
