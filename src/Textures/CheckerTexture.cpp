// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "Precompiled.h"

#include "Textures/CheckerTexture.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Rendering/Color.h"

using namespace Raycer;

void CheckerTexture::initialize(Scene& scene)
{
	(void)scene;
}

Color CheckerTexture::getColor(const Vector2& texcoord, const Vector3& position) const
{
	(void)position;

	if (stripeMode)
	{
		if (texcoord.x < stripeWidth || texcoord.y < stripeWidth || texcoord.x > (1.0 - stripeWidth) || texcoord.y > (1.0 - stripeWidth))
			return color1 * intensity;
		else
			return color2 * intensity;
	}
	else
	{
		if (texcoord.x < 0.5)
			return ((texcoord.y < 0.5) ? color1 : color2) * intensity;
		else
			return ((texcoord.y < 0.5) ? color2 : color1) * intensity;
	}
}

double CheckerTexture::getValue(const Vector2& texcoord, const Vector3& position) const
{
	(void)texcoord;
	(void)position;

	return 0.0;
}
