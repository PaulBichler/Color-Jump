#include "Component.hpp"
namespace GUI
{
	Component::Component()
	: m_is_selected(false)
	, m_is_active(false),
	m_draw_predicate([]{return true;})
	{
	}


	bool Component::IsSelected() const
	{
		return m_is_selected;
	}

	void Component::Select()
	{
		m_is_selected = true;
	}

	void Component::Deselect()
	{
		m_is_selected = false;
	}

	bool Component::IsActive() const
	{
		return m_is_active;
	}

	void Component::Activate()
	{
		m_is_active = true;
	}

	void GUI::Component::Deactivate()
	{
		m_is_active = false;
	}

	void Component::SetDrawPredicate(const std::function<bool()>& predicate)
	{
		m_draw_predicate = predicate;
	}

	bool Component::CanBeDrawn() const
	{
		return m_draw_predicate();
	}
}
