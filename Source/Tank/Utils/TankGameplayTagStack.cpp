// Fill out your copyright notice in the Description page of Project Settings.


#include "TankGameplayTagStack.h"


void FTankGameplayTagStackContainer::AddStackCount(FGameplayTag Tag, int32 Count)
{
	if (!Tag.IsValid() || Count <= 0)
	{
		return;
	}

	for (FTankGameplayTagStack& Stack : Stacks)
	{
		if (Stack.Tag == Tag)
		{
			Stack.Count += Count;
			TagCountMap[Tag] = Stack.Count;
			MarkItemDirty(Stack);
			return;
		}
	}

	FTankGameplayTagStack& NewStack = Stacks.Emplace_GetRef(Tag, Count);
	MarkItemDirty(NewStack);
	TagCountMap.Add(Tag, Count);
}

void FTankGameplayTagStackContainer::RemoveStackCount(FGameplayTag Tag, int32 Count)
{
	if (!Tag.IsValid() || Count <= 0)
	{
		return;
	}

	for (auto It = Stacks.CreateIterator(); It; ++It)
	{
		FTankGameplayTagStack& Stack = *It;
		if (Stack.Tag == Tag)
		{
			if (Stack.Count <= Count)
			{
				It.RemoveCurrent();
				TagCountMap.Remove(Tag);
				MarkArrayDirty();
			}
			else
			{
				Stack.Count -= Count;
				TagCountMap[Tag] = Stack.Count;
				MarkItemDirty(Stack);
			}
			return;
		}
	}
}

void FTankGameplayTagStackContainer::PreReplicatedRemove(const TArrayView<int32>& RemovedIndices, int32 FinalSize)
{
	for (const int32 Index : RemovedIndices)
	{
		TagCountMap.Remove(Stacks[Index].Tag);
	}
}

void FTankGameplayTagStackContainer::PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize)
{
	for (const int32 Index : AddedIndices)
	{
		const FTankGameplayTagStack& Stack = Stacks[Index];
		TagCountMap.Add(Stack.Tag, Stack.Count);
	}
}

void FTankGameplayTagStackContainer::PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize)
{
	for (const int32 Index : ChangedIndices)
	{
		const FTankGameplayTagStack& Stack = Stacks[Index];
		TagCountMap.Add(Stack.Tag, Stack.Count);
	}
}