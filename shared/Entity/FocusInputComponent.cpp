#include "PlatformPrecomp.h"

#include "FocusInputComponent.h"
#include "BaseApp.h"

FocusInputComponent::FocusInputComponent()
{
	SetName("FocusInput");
}

FocusInputComponent::~FocusInputComponent()
{
}

void FocusInputComponent::OnAdd(Entity *pEnt)
{
	EntityComponent::OnAdd(pEnt);

	if (GetVar("mode")->GetUINT32() == MODE_START_NORMAL)
	{
		BaseApp::GetBaseApp()->m_sig_input.connect(1, boost::bind(&FocusInputComponent::OnInput, this, _1));
	}

	GetFunction("LinkMoveMessages")->m_sig_function.connect(1, boost::bind(&FocusInputComponent::LinkMoveMessages, this, _1));
	GetFunction("LinkRawMessages")->m_sig_function.connect(1, boost::bind(&FocusInputComponent::LinkRawMessages, this, _1));
}

void FocusInputComponent::LinkMoveMessages(VariantList *pVList)
{
	if (BaseApp::GetBaseApp()->GetInputMode() == INPUT_MODE_SEPARATE_MOVE_TOUCHES)
	{
		BaseApp::GetBaseApp()->m_sig_input_move.connect(1, boost::bind(&FocusInputComponent::OnInput, this, _1));
	}
}

void FocusInputComponent::LinkRawMessages(VariantList *pVList)
{
	BaseApp::GetBaseApp()->m_sig_raw_keyboard.connect(1, boost::bind(&FocusInputComponent::OnInputRaw, this, _1));
}

void FocusInputComponent::OnRemove()
{
	EntityComponent::OnRemove();
}

void FocusInputComponent::OnInput(VariantList *pVList)
{
	//the 1 is because the pt we need modified is index 1 of the VariantList
	//GetParent()->CallFunctionRecursivelyWithUpdatedVar("OnInput", pVList, string("pos2d"), 1, Entity::RECURSIVE_VAR_OP_SUBTRACTION_PLUS_ALIGNMENT_OFFSET);

	//new way, which calls things in reverse order
	GetParent()->CallFunctionRecursivelyWithUpdatedVarBackwards("OnInput", pVList, string("pos2d"), 1, Entity::RECURSIVE_VAR_OP_SUBTRACTION_PLUS_ALIGNMENT_OFFSET);

}

void FocusInputComponent::OnInputRaw(VariantList *pVList)
{
	GetParent()->CallFunctionRecursively("OnInputRaw", pVList);
}
