#pragma once

#include "EditorPanel.h"
#include "Outline.h"
#include "EditorContext.h"

class FEditorUI
{
public:
	bool Init(const FEditorContext& InContext);
	void Tick(float DeltaTime);
	void OnRender();

	template <typename T>
	T* AddEditorPanel()
	{
		TUniquePtr<T> newPanel = MakeUnique<T>();
		T* Ret = newPanel.get();
		Ret->Init();
		Panels.push_back(std::move(newPanel));
		
		return Ret;
	}

	template<typename T>
	T* GetEditorPanel()
	{
		for (const TUniquePtr<IEditorPanel>& Panel : Panels)
		{
			if (T* Result = dynamic_cast<T*>(Panel.get()))
			{
				return Result;
			}
		}
	}


private:
	TArray<TUniquePtr<IEditorPanel>> Panels;
	FEditorContext Context;
};