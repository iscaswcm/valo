// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "Math/Vector3.h"
#include "Math/Color.h"

namespace Raycer
{
	struct Light
	{
		Vector3 position = Vector3(0.0, 0.0, 0.0);
		Color color = Color::WHITE;
		double intensity = 1.0;

		template<class Archive>
		void serialize(Archive& ar)
		{
			ar(CEREAL_NVP(position),
				CEREAL_NVP(color),
				CEREAL_NVP(intensity));
		}
	};
}