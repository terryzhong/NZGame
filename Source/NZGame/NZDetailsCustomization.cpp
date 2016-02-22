// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZDetailsCustomization.h"

#if WITH_EDITOR
#include "SlateBasics.h"
#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "PropertyHandle.h"
#include "PropertyCustomizationHelpers.h"


struct FMuzzleFlashChoice
{
    TWeakObjectPtr<UParticleSystemComponent> PSC;
    FName DisplayName;
    
    FMuzzleFlashChoice()
        : PSC(NULL)
        , DisplayName(NAME_None)
    {}
    
    FMuzzleFlashChoice(const TWeakObjectPtr<UParticleSystemComponent>& InPSC, const FName& InName)
        : PSC(InPSC)
        , DisplayName(InName)
    {}
    
    bool operator==(const FMuzzleFlashChoice& Other)
    {
        return PSC == Other.PSC;
    }
    
    bool operator!=(const FMuzzleFlashChoice& Other)
    {
        return PSC != Other.PSC;
    }
};

struct FMuzzleFlashItem : public TSharedFromThis<FMuzzleFlashItem>
{
    /** Index in muzzle flash array */
    uint32 Index;
    
    /** Object being modified */
    TWeakObjectPtr<UObject> Obj;
    
    /** Choices */
    const TArray<TSharedPtr<FMuzzleFlashChoice>>& Choices;
    
    TSharedPtr<STextBlock> TextBlock;
    
    FMuzzleFlashItem(uint32 InIndex, TWeakObjectPtr<UObject> InObj, const TArray<TSharedPtr<FMuzzleFlashChoice>>& InChoices)
        : Index(InIndex)
        , Obj(InObj)
        , Choices(InChoices)
    {}
    
    TSharedRef<SComboBox<TSharedPtr<FMuzzleFlashChoice>>> CreateWidget()
    {
		FString CurrentText;
		{
			UParticleSystemComponent* CurrentValue = NULL;
			ANZWeapon* Weap = Cast<ANZWeapon>(Obj.Get());
			if (Weap != NULL)
			{
				CurrentValue = Weap->MuzzleFlash[Index];
			}
			else
			{

			}
		}


    }
    
    TSharedRef<SWidget> GenerateWidget(TSharedPtr<FMuzzleFlashChoice> InItem)
    {
        return SNew(SBox).Padding(5)[SNew(STextBlock).Text(FText::FromName(InItem->DisplayName))];
    }
};

class NZGAME_API FMFArrayBuilder : public FDetailArrayBuilder
{
public:
    FMFArrayBuilder(TWeakObjectPtr<UObject> InObj, TSharedRef<IPropertyHandle> InBaseProperty, TArray<TSharedPtr<FMuzzleFlashChoice>>& InChoices, bool InGenerateHeader = true)
        : FDetailArrayBuilder(InBaseProperty, InGenerateHeader)
        , Obj(InObj)
        , Choices(InChoices)
        , MyArrayProperty(InBaseProperty->AsArray())
    {}
    
    TWeakObjectPtr<UObject> Obj;
    TArray<TSharedPtr<FMuzzleFlashChoice>> Choices;
    TArray<TSharedPtr<FMuzzleFlashItem>> MFEntries;
    TSharedPtr<IPropertyHandleArray> MyArrayProperty;
    
    virtual void GenerateChildContent(IDetailChildrenBuilder& ChildrenBuilder) override
    {
        uint32 NumChildren = 0;
        MyArrayProperty->GetNumElements(NumChildren);
        
        MFEntries.SetNum(NumChildren);
        for (uint32 ChildIndex = 0; ChildIndex < NumChildren; ++ChildIndex)
        {
            TSharedPtr<FMuzzleFlashItem> NewEntry = MakeShareable(new FMuzzleFlashItem(ChildIndex, Obj, Choices));
            MFEntries[ChildIndex] = NewEntry;
            
            TSharedRef<IPropertyHandle> ElementHandle = MyArrayProperty->GetElement(ChildIndex);
            ChildrenBuilder.AddChildContent(FText::FromString(TEXT("MuzzleFlash")))
                .NameContent()[ElementHandle->CreatePropertyNameWidget()]
                .ValueContent()[NewEntry->CreateWidget()];
        }
    }
};


FNZDetailsCustomization::FNZDetailsCustomization()
{
}

FNZDetailsCustomization::~FNZDetailsCustomization()
{
}

void FNZDetailsCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailLayout)
{
    TArray<TWeakObjectPtr<UObject>> Objects;
    DetailLayout.GetObjectsBeingCustomized(Objects);
    if (Objects.Num() == 1 && Objects[0].IsValid())
    {
        IDetailCategoryBuilder& WeaponCategory = DetailLayout.EditCategory("Weapon");
        
        TSharedRef<IPropertyHandle> MuzzleFlash = DetailLayout.GetProperty(TEXT("MuzzleFlash"));
        
        uint32 NumChildren = 0;
        MuzzleFlash->GetNumChildren(NumChildren);
        
        TArray<TSharedPtr<FMuzzleFlashChoice>> Choices;
        Choices.Add(MakeShareable(new FMuzzleFlashChoice(NULL, NAME_None)));
        {
            TArray<USCS_Node*> ConstructionNodes;
            {
                // The components editor uses names from the SCS instead so that's what we need to use
                TArray<const UBlueprintGeneratedClass*> ParentBPClassStack;
                UBlueprintGeneratedClass::GetGeneratedClassesHierarchy(Objects[0].Get()->GetClass(), ParentBPClassStack);
                for (int32 i = ParentBPClassStack.Num() - 1; i >= 0; i--)
                {
                    const UBlueprintGeneratedClass* CurrentBPGClass = ParentBPClassStack[i];
                    if (CurrentBPGClass->SimpleConstructionScript)
                    {
                        ConstructionNodes += CurrentBPGClass->SimpleConstructionScript->GetAllNodes();
                    }
                }
            }
            TArray<UObject*> Children;
            for (UClass* TestClass = Objects[0].Get()->GetClass(); TestClass != NULL; TestClass = TestClass->GetSuperClass())
            {
                GetObjectsWithOuter(TestClass, Children, true, RF_NoFlags, EInternalObjectFlags::PendingKill);
            }
            for (int32 i = 0; i < Children.Num(); i++)
            {
                UParticleSystemComponent* PSC = Cast<UParticleSystemComponent>(Children[i]);
                if (PSC != NULL)
                {
                    FName DisplayName = PSC->GetFName();
                    for (int32 j = 0; j < ConstructionNodes.Num(); j++)
                    {
                        if (ConstructionNodes[j]->ComponentTemplate == PSC)
                        {
                            DisplayName = ConstructionNodes[j]->VariableName;
                            break;
                        }
                    }
                    Choices.Add(MakeShareable(new FMuzzleFlashChoice(PSC, DisplayName)));
                }
            }
        }
        
        WeaponCategory.AddCustomBuilder(MakeShareable(new FMFArrayBuilder(Objects[0], MuzzleFlash, Choices, true)), false);
    }
}

#endif