#pragma once
#include <SFML/System/NonCopyable.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/Drawable.hpp>

#include <vector>
#include <memory>
#include <set>

#include "Command.hpp"
#include "CommandQueue.hpp"

class SceneNode : public sf::Transformable, public sf::Drawable, private sf::NonCopyable
{
public:
	typedef  std::unique_ptr<SceneNode> Ptr;
	typedef std::pair<SceneNode*, SceneNode*> Pair;

public:
	explicit SceneNode(Category::Type category = Category::kNone);
	void AttachChild(Ptr child);
	Ptr DetachChild(const SceneNode& node);

	void Update(sf::Time dt, CommandQueue& commands);

	sf::Vector2f GetWorldPosition() const;
	sf::Transform GetWorldTransform() const;

	void OnCommand(const Command& command, sf::Time dt);
	virtual unsigned int GetCategory() const;
	virtual sf::FloatRect GetBoundingRect() const;

	void CheckSceneCollision(SceneNode& scene_graph, std::set<Pair>& collision_pairs, const std::function<bool(SceneNode&)>& check_predicate);
	void RemoveWrecks();


private:
	virtual void UpdateCurrent(sf::Time dt, CommandQueue& commands);
	void UpdateChildren(sf::Time dt, CommandQueue& commands) const;

	//Note draw is from sf::Drawable hence the name, lower case d
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	virtual void DrawCurrent(sf::RenderTarget& render_target, sf::RenderStates states) const;
	void DrawChildren(sf::RenderTarget& target, sf::RenderStates states) const;

	static void DrawBoundingRect(sf::RenderTarget& target, sf::RenderStates states, const sf::FloatRect& bounding_rect);

	virtual bool IsDestroyed() const;
	bool IsMarkedForRemoval() const;
	
	void CheckNodeCollision(SceneNode& node, std::set<Pair>& collisionPairs, const std::function<bool(SceneNode&)>& check_predicate);
	

private:
	std::vector<Ptr> m_children;
	SceneNode* m_parent;
	Category::Type m_default_category;
};
float distance(const SceneNode& lhs, const SceneNode& rhs);
