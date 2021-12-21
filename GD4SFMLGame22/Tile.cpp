#include "Tile.hpp"

#include "ResourceHolder.hpp"
#include "ResourceIdentifiers.hpp"

Tile::Tile(const TextureHolder& textures, const sf::IntRect sub_rect) : SpriteNode(textures.Get(Textures::kLevelTileSet), sub_rect)
{
}
