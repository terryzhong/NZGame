// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#if WITH_EDITOR

class UFactory;

#include "PropertyEditorModule.h"

/**
 * 
 */
class NZGAME_API FNZDetailsCustomization : public IDetailCustomization
{
public:
	FNZDetailsCustomization();
	~FNZDetailsCustomization();
    
    /** Make a new instance of this detail layout class for a specific detail view requesting it */
    static TSharedRef<IDetailCustomization> MakeInstance()
    {
        return MakeShareable(new FNZDetailsCustomization);
    }
    
    virtual void CustomizeDetails(IDetailLayoutBuilder& DetailLayout) override;
    
    void OnPropChanged(const FPropertyChangedEvent& Event);
};

#endif