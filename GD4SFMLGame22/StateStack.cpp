#include "StateStack.hpp"

#include <cassert>

StateStack::StateStack(Context context)
:m_context(context)
{
}

void StateStack::Update(sf::Time dt)
{
	for (auto itr = m_stack.rbegin(); itr != m_stack.rend(); ++itr)
	{
		if (!(*itr)->Update(dt))
		{
			break;
		}
	}
	ApplyPendingChanges();
}

void StateStack::Draw()
{
	for(State::Ptr& state:m_stack)
	{
		state->Draw();
	}
}

void StateStack::HandleEvent(const sf::Event& event)
{
	for (auto itr = m_stack.rbegin(); itr != m_stack.rend(); ++itr)
	{
		if (!(*itr)->HandleEvent(event))
		{
			break;
		}
	}
	ApplyPendingChanges();
}

void StateStack::PushState(StateID state_id)
{
	m_pending_list.emplace_back(PendingChange(Action::Push, state_id));
}

void StateStack::PopState()
{
	m_pending_list.emplace_back(PendingChange(Action::Pop));
}

void StateStack::ClearStates()
{
	m_pending_list.emplace_back(PendingChange(Action::Clear));
}

bool StateStack::IsEmpty() const
{
	return m_stack.empty();
}

State::Ptr StateStack::CreateState(StateID state_id)
{
	auto found = m_state_factory.find(state_id);
	assert(found != m_state_factory.end());
	return found->second();
}

void StateStack::ApplyPendingChanges()
{
	for(PendingChange change : m_pending_list)
	{
		switch (change.m_action)
		{
			case Action::Push:
				m_stack.emplace_back(CreateState(change.m_state_id));
				break;
			case Action::Pop:
				m_stack.back()->OnStackPopped();
				m_stack.pop_back();
				break;
			case Action::Clear:
				for (const auto& state : m_stack) state->OnStackPopped();
				m_stack.clear();
				break;
		}
	}
	m_pending_list.clear();
}

StateStack::PendingChange::PendingChange(Action action, StateID state_id)
: m_action(action)
, m_state_id(state_id)
{
}
