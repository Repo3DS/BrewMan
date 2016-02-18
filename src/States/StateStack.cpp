#include "StateStack.hpp"
#include <cassert>


namespace BrewMan {

StateStack::StateStack(State::Context context)
: m_stack()
, m_pendingList()
, m_context(context)
, m_factories()
{
}

void StateStack::update(float delta)
{
	// Iterate from top to bottom, stop as soon as update() returns false
	for (auto itr = m_stack.rbegin(); itr != m_stack.rend(); ++itr)
	{
		if (!itr->pointer->update(delta))
			break;
	}

	applyPendingChanges();
}

void StateStack::renderTopScreen(cpp3ds::Window &window)
{
	// Draw all active states from bottom to top
	for(const StateStackItem& state : m_stack)
		state.pointer->renderTopScreen(window);
}

void StateStack::renderBottomScreen(cpp3ds::Window &window)
{
	// Draw all active states from bottom to top
	for(const StateStackItem& state : m_stack)
		state.pointer->renderBottomScreen(window);
}

void StateStack::processEvent(const cpp3ds::Event& event)
{
	// Iterate from top to bottom, stop as soon as handleEvent() returns false
	for (auto itr = m_stack.rbegin(); itr != m_stack.rend(); ++itr)
	{
		if (!itr->pointer->processEvent(event))
			break;
	}

	applyPendingChanges();
}

void StateStack::pushState(States::ID stateID)
{
	m_pendingList.push_back(PendingChange(Push, stateID));
}

void StateStack::popState()
{
	m_pendingList.push_back(PendingChange(Pop));
}

void StateStack::clearStates()
{
	m_pendingList.push_back(PendingChange(Clear));
}

bool StateStack::isEmpty() const
{
	return m_stack.empty();
}

State::Ptr StateStack::createState(States::ID stateID)
{
	auto found = m_factories.find(stateID);
	assert(found != m_factories.end());

	return found->second();
}

void StateStack::applyPendingChanges()
{
	for(const PendingChange& change : m_pendingList)
	{
		switch (change.action)
		{
			case Push:
				m_stack.push_back({change.stateID, createState(change.stateID)});
				break;

			case Pop:
				m_stack.pop_back();
				break;

			case Clear:
				m_stack.clear();
				break;
		}
	}

	m_pendingList.clear();
}

StateStack::PendingChange::PendingChange(Action action, States::ID stateID)
: action(action)
, stateID(stateID)
{
}

} // namespace BrewMan
