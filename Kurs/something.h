#pragma once
#include <EvoNDZ/math/vector2.h>

struct body
{
	evo::Vector2f position;
	evo::Vector2f velocity = evo::Vector2f(0, 0);
	float mass;
	float r;
	body(evo::Vector2f pos, float received_mass) { position = pos; mass = received_mass; r = sqrt(mass) * 0.01; }
};