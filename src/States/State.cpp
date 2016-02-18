#include "State.hpp"
#include "StateStack.hpp"


namespace BrewMan {

State::Context::Context(cpp3ds::String& name)
: name(name)
{
	//
}

State::State(StateStack& stack, Context& context)
: m_stack(&stack)
, m_context(context)
{
}

State::~State()
{
}

void State::requestStackPush(States::ID stateID)
{
	m_stack->pushState(stateID);
}

void State::requestStackPop(States::ID stateID)
{
	m_stack->popState();
}

void State::requestStackClear()
{
	m_stack->clearStates();
}

State::Context State::getContext() const
{
	return m_context;
}

} // namespace BrewMan
