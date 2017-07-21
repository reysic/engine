#pragma once

#include <vector>

#include "Engine/Renderer/Particles/Emitter.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Math/AABB2.hpp"


//-----------------------------------------------------------------------------------------------
enum EmitterType
{
	EMITTER_TYPE_EXPLOSION,
	EMITTER_TYPE_FIREWORK,
	EMITTER_TYPE_SMOKE,
	EMITTER_TYPE_FOUNTAIN
};


//-----------------------------------------------------------------------------------------------
struct Force
{
	Vector2 direction;
	float secLeft;
	float magnitude;
	bool isConstant;
};


//-----------------------------------------------------------------------------------------------
struct Particle
{
	void AddForce( const Vector2& direction, const float magnitude, const float secLeft, 
				   const bool isConstant )
	{
		Force f = Force();
		f.direction = direction;
		f.magnitude = magnitude;
		f.secLeft = secLeft;
		f.isConstant = isConstant;

		forces.push_back( f );
	}

	Vector2 position = Vector2::ZERO;
	Vector2 velocity = Vector2::ZERO;
	float mass = 1.0f;
	float lifetime = 5.0f;
	float secondsLeft = lifetime;
	Rgba color = Rgba::WHITE;

	std::vector< Force > forces;
};


//-----------------------------------------------------------------------------------------------
class Emitter
{
public:
	Emitter();
	Emitter( const Vector2& position, const Vector2& direction, const float emissionsPerSecond, 
		     const int maxParticles, const EmitterType type, const Vector2& minDegrees = Vector2::ZERO, 
		     const Vector2& maxDegrees = Vector2( 360.0f, 360.0f ), const Rgba& particleColor = Rgba::WHITE );

	void Update( const float deltaSeconds );
	void Render();
	void FireEmitter();
	void RemoveDeadParticles();
	void HandleTypeConstruction();

	// Unique constructor functions
	void MakeExplosion();
	void MakeFirework();
	void MakeSmoke();
	void MakeFountain();

	Vector2 GetRandomParticleDirection();

public:
	Vector2 m_position;
	Vector2 m_direction;
	float m_emissionsPerSecond;

	Vector2 m_minDegrees;
	Vector2 m_maxDegrees;

	Rgba m_particleColor;

	std::vector< Particle > m_particles;
	int m_maxParticles;
	float m_secondsSinceLastEmission;

	EmitterType m_type;
	bool m_usesCone;
	bool m_isLooping;

	Texture* m_texture;
	AABB2 m_bounds;
	float m_ageInSeconds;
	float m_lifetimeInSeconds;
	bool m_isDead;
};