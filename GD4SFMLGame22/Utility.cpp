#define _USE_MATH_DEFINES
#include "Utility.hpp"

#include <cassert>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>


#include <cmath>
#include <iostream>
#include <random>
#include "Animation.hpp"


namespace
{
	std::default_random_engine CreateRandomEngine()
	{
		const auto seed = static_cast<unsigned long>(std::time(nullptr));
		return std::default_random_engine(seed);
	}

	auto random_engine = CreateRandomEngine();
}

void Utility::CreateButton(Context& context, std::shared_ptr<GUI::Button>& play_button, const int x,
                           const int y,
                           const std::string& label, const GUI::Button::Callback& callback,
                           const bool toggle, const std::function<bool()>& predicate)
{
	play_button = std::make_shared<GUI::Button>(context);
	play_button->setPosition(static_cast<float>(x), static_cast<float>(y));
	play_button->SetText(label);
	play_button->SetToggle(toggle);
	if (callback != nullptr)
	{
		play_button->SetCallback(callback);
	}
	if (predicate != nullptr)
	{
		play_button->SetDrawPredicate(predicate);
	}
}

int Utility::RandomInt(const int exclusive_max)
{
	const std::uniform_int_distribution<> dist(0, exclusive_max - 1);
	return dist(random_engine);
}

void Utility::CentreOrigin(sf::Sprite& sprite)
{
	const sf::FloatRect bounds = sprite.getLocalBounds();
	sprite.setOrigin(std::floor(bounds.left + bounds.width / 2.f),
	                 std::floor(bounds.top + bounds.height / 2.f));
}

void Utility::CentreOrigin(sf::Text& text)
{
	const sf::FloatRect bounds = text.getLocalBounds();
	text.setOrigin(std::floor(bounds.left + bounds.width / 2.f),
	               std::floor(bounds.top + bounds.height / 2.f));
}

void Utility::CentreOrigin(Animation& animation)
{
	const sf::FloatRect bounds = animation.GetLocalBounds();
	animation.setOrigin(std::floor(bounds.left + bounds.width / 2.f),
	                    std::floor(bounds.top + bounds.height / 2.f));
}

std::string Utility::ToString(const sf::Keyboard::Key key)
{
#define KEY_TO_STRING_CASE(KEY) case sf::Keyboard::KEY: return #KEY;

	switch (key)
	{
	KEY_TO_STRING_CASE(Unknown)
	KEY_TO_STRING_CASE(A)
	KEY_TO_STRING_CASE(B)
	KEY_TO_STRING_CASE(C)
	KEY_TO_STRING_CASE(D)
	KEY_TO_STRING_CASE(E)
	KEY_TO_STRING_CASE(F)
	KEY_TO_STRING_CASE(G)
	KEY_TO_STRING_CASE(H)
	KEY_TO_STRING_CASE(I)
	KEY_TO_STRING_CASE(J)
	KEY_TO_STRING_CASE(K)
	KEY_TO_STRING_CASE(L)
	KEY_TO_STRING_CASE(M)
	KEY_TO_STRING_CASE(N)
	KEY_TO_STRING_CASE(O)
	KEY_TO_STRING_CASE(P)
	KEY_TO_STRING_CASE(Q)
	KEY_TO_STRING_CASE(R)
	KEY_TO_STRING_CASE(S)
	KEY_TO_STRING_CASE(T)
	KEY_TO_STRING_CASE(U)
	KEY_TO_STRING_CASE(V)
	KEY_TO_STRING_CASE(W)
	KEY_TO_STRING_CASE(X)
	KEY_TO_STRING_CASE(Y)
	KEY_TO_STRING_CASE(Z)
	KEY_TO_STRING_CASE(Num0)
	KEY_TO_STRING_CASE(Num1)
	KEY_TO_STRING_CASE(Num2)
	KEY_TO_STRING_CASE(Num3)
	KEY_TO_STRING_CASE(Num4)
	KEY_TO_STRING_CASE(Num5)
	KEY_TO_STRING_CASE(Num6)
	KEY_TO_STRING_CASE(Num7)
	KEY_TO_STRING_CASE(Num8)
	KEY_TO_STRING_CASE(Num9)
	KEY_TO_STRING_CASE(Escape)
	KEY_TO_STRING_CASE(LControl)
	KEY_TO_STRING_CASE(LShift)
	KEY_TO_STRING_CASE(LAlt)
	KEY_TO_STRING_CASE(LSystem)
	KEY_TO_STRING_CASE(RControl)
	KEY_TO_STRING_CASE(RShift)
	KEY_TO_STRING_CASE(RAlt)
	KEY_TO_STRING_CASE(RSystem)
	KEY_TO_STRING_CASE(Menu)
	KEY_TO_STRING_CASE(LBracket)
	KEY_TO_STRING_CASE(RBracket)
	KEY_TO_STRING_CASE(SemiColon)
	KEY_TO_STRING_CASE(Comma)
	KEY_TO_STRING_CASE(Period)
	KEY_TO_STRING_CASE(Quote)
	KEY_TO_STRING_CASE(Slash)
	KEY_TO_STRING_CASE(BackSlash)
	KEY_TO_STRING_CASE(Tilde)
	KEY_TO_STRING_CASE(Equal)
	KEY_TO_STRING_CASE(Dash)
	KEY_TO_STRING_CASE(Space)
	KEY_TO_STRING_CASE(Return)
	KEY_TO_STRING_CASE(BackSpace)
	KEY_TO_STRING_CASE(Tab)
	KEY_TO_STRING_CASE(PageUp)
	KEY_TO_STRING_CASE(PageDown)
	KEY_TO_STRING_CASE(End)
	KEY_TO_STRING_CASE(Home)
	KEY_TO_STRING_CASE(Insert)
	KEY_TO_STRING_CASE(Delete)
	KEY_TO_STRING_CASE(Add)
	KEY_TO_STRING_CASE(Subtract)
	KEY_TO_STRING_CASE(Multiply)
	KEY_TO_STRING_CASE(Divide)
	KEY_TO_STRING_CASE(Left)
	KEY_TO_STRING_CASE(Right)
	KEY_TO_STRING_CASE(Up)
	KEY_TO_STRING_CASE(Down)
	KEY_TO_STRING_CASE(Numpad0)
	KEY_TO_STRING_CASE(Numpad1)
	KEY_TO_STRING_CASE(Numpad2)
	KEY_TO_STRING_CASE(Numpad3)
	KEY_TO_STRING_CASE(Numpad4)
	KEY_TO_STRING_CASE(Numpad5)
	KEY_TO_STRING_CASE(Numpad6)
	KEY_TO_STRING_CASE(Numpad7)
	KEY_TO_STRING_CASE(Numpad8)
	KEY_TO_STRING_CASE(Numpad9)
	KEY_TO_STRING_CASE(F1)
	KEY_TO_STRING_CASE(F2)
	KEY_TO_STRING_CASE(F3)
	KEY_TO_STRING_CASE(F4)
	KEY_TO_STRING_CASE(F5)
	KEY_TO_STRING_CASE(F6)
	KEY_TO_STRING_CASE(F7)
	KEY_TO_STRING_CASE(F8)
	KEY_TO_STRING_CASE(F9)
	KEY_TO_STRING_CASE(F10)
	KEY_TO_STRING_CASE(F11)
	KEY_TO_STRING_CASE(F12)
	KEY_TO_STRING_CASE(F13)
	KEY_TO_STRING_CASE(F14)
	KEY_TO_STRING_CASE(F15)
	KEY_TO_STRING_CASE(Pause)
	case sf::Keyboard::KeyCount:
	default: break;
	}

	return "";
}

double Utility::ToRadians(const int degrees)
{
	return degrees * M_PI / 180;
}

sf::Vector2f Utility::UnitVector(const sf::Vector2f vector)
{
	assert(vector != sf::Vector2f(0.f, 0.f));
	return vector / Length(vector);
}

float Utility::Length(const sf::Vector2f vector)
{
	return sqrtf(powf(vector.x, 2) + powf(vector.y, 2));
}

float Utility::ToDegrees(const float angle)
{
	return angle * (180 / M_PI);
}


/*
 *	Dylan Goncalves Martins (D00242562)
 *	Simplified the debug message
 */
void Utility::Debug(const std::string& message)
{
	std::cout << message << std::endl;
}

void Utility::CreateButton(Context& context, std::shared_ptr<GUI::Button>& button, const int x,
                           const int y,
                           const std::string& label, const GUI::Button::Callback& callback,
                           const std::function<bool()>& predicate)
{
	CreateButton(context, button, x, y, label, callback, false, predicate);
}

void Utility::CreateButton(Context& context, std::shared_ptr<GUI::Button>& button, const int x,
                           const int y,
                           const std::string& label, const bool toggle)
{
	CreateButton(context, button, x, y, label, nullptr, toggle, nullptr);
}

void Utility::CreateLabel(const Context& context, std::shared_ptr<GUI::Label>& label, const int x,
                          const int y, const std::string& label_text, const int text_size)
{
	label = std::make_shared<GUI::Label>(label_text, *context.m_fonts, text_size);
	label->setPosition(static_cast<float>(x), static_cast<float>(y));
}
