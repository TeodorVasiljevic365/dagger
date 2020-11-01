#pragma once

#include "core/core.h"
#include "core/graphics/shader.h"
#include "core/graphics/texture.h"

namespace dagger
{
	struct Proxy
	{
		Vector3 position{ 0, 0, 0 };				// 3
		Vector2 pivot{ 0, 0 };						// 2
		Color   color{ 1.0f, 1.0f, 1.0f, 1.0f };	// 4
		Float32 ratio{ 1.0f };						// 1
		Vector2 scale{ 1.0f, 1.0f };				// 2
	};

	struct Sprite
	{
		Vector3 position{ 0, 0, 0 };				// 3
		Vector2 pivot{ 0, 0 };						// 2
		Color   color{ 1.0f, 1.0f, 1.0f, 1.0f };	// 4
		Float32 ratio{ 1.0f };						// 1
		Vector2 scale{ 1.0f, 1.0f };				// 2
		Texture* image{ nullptr };					// 2
	};

	void AssignSpriteTexture(Sprite& spriteTarget_, String textureName_);
	void AssignSpriteTexture(Sprite& spriteTarget_, ViewPtr<Texture> texture_);
}