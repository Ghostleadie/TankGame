// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "TankGameplayTagStack.generated.h"

struct FTankGameplayTagStackContainer;

/** A single gameplay tag and its stack count, replicated as an entry in the container. */
USTRUCT(BlueprintType)
struct FTankGameplayTagStack : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FTankGameplayTagStack()
	{}

	FTankGameplayTagStack(FGameplayTag InTag, int32 InCount)
		: Tag(InTag)
		, Count(InCount)
	{}

private:
	friend FTankGameplayTagStackContainer;

	/** The tag this stack is for. */
	UPROPERTY()
	FGameplayTag Tag;

	/** Current stack count of the tag. */
	UPROPERTY()
	int32 Count = 0;
};

/** Replicated list of tag stacks with a local map for fast queries. Modify on the server only. */
USTRUCT(BlueprintType)
struct FTankGameplayTagStackContainer : public FFastArraySerializer
{
	GENERATED_BODY()

	FTankGameplayTagStackContainer()
	{}

	/** Adds Count to the tag's stack, creating the stack if needed. Server only. */
	void AddStackCount(FGameplayTag Tag, int32 Count);

	/** Removes Count from the tag's stack, removing the stack when it reaches zero. Server only. */
	void RemoveStackCount(FGameplayTag Tag, int32 Count);

	/** Returns true if there is a stack for the tag. */
	bool HasTag(FGameplayTag Tag) const
	{
		return TagCountMap.Contains(Tag);
	}

	/** Returns the stack count for the tag, or 0 if there is none. */
	int32 GetStackCount(FGameplayTag Tag) const
	{
		return TagCountMap.FindRef(Tag);
	}

	//~ FFastArraySerializer contract
	void PreReplicatedRemove(const TArrayView<int32>& RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize);

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FastArrayDeltaSerialize<FTankGameplayTagStack, FTankGameplayTagStackContainer>(Stacks, DeltaParms, *this);
	}

private:
	/** Replicated list of gameplay tag stacks. Use TagCountMap for queries. */
	UPROPERTY()
	TArray<FTankGameplayTagStack> Stacks;

	/** Non-replicated lookup rebuilt from Stacks on both server and clients. */
	TMap<FGameplayTag, int32> TagCountMap;
};

template<>
struct TStructOpsTypeTraits<FTankGameplayTagStackContainer> : public TStructOpsTypeTraitsBase2<FTankGameplayTagStackContainer>
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};