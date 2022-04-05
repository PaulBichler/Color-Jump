#include "Label.hpp"

#include "ResourceHolder.hpp"
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

namespace GUI
{
	Label::Label(const std::string& text, const FontHolder& fonts, const int font_size)
	: m_text(text, fonts.Get(Fonts::Main), font_size)
	{
	}

	bool Label::IsSelectable() const
	{
		return false;
	}

	void Label::SetText(const std::string& text)
	{
		m_text.setString(text);
	}

	void Label::HandleEvent(const sf::Event& event)
	{
	}

	sf::Text& Label::GetText()
	{
		return m_text;
	}

	void Label::draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		states.transform *= getTransform();
		target.draw(m_text, states);
	}
}

