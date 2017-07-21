#include <cstdlib>

#include "Engine/Renderer/Particles/Emitter.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Math/MathUtils.hpp"


//-----------------------------------------------------------------------------------------------
Emitter::Emitter()
	: m_ageInSeconds( 0.0f )
	, m_lifetimeInSeconds( 1.0f )
	, m_isDead( false )
{
	m_position = Vector2::ZERO;
	m_direction = Vector2::ZERO;
	m_emissionsPerSecond = 1.0f;
	m_secondsSinceLastEmission = 0;

	m_minDegrees = Vector2::ZERO;
	m_maxDegrees = Vector2::ZERO;

	m_particleColor = Rgba::WHITE;

	m_maxParticles = 1;

	m_type = EMITTER_TYPE_EXPLOSION;
	m_usesCone = false;

	HandleTypeConstruction();

	m_texture = Texture::CreateOrGetTexture( "Data/Images/particle.png" );
	m_bounds.mins = Vector2( -0.05f, -0.05f );
	m_bounds.maxs = Vector2( 0.05f, 0.05f );

	m_isLooping = false;
}


//-----------------------------------------------------------------------------------------------
Emitter::Emitter( const Vector2& position, const Vector2& direction, const float emissionsPerSecond, 
	              const int maxParticles = 1, const EmitterType type = EMITTER_TYPE_EXPLOSION, 
	              const Vector2& minDegrees, const Vector2& maxDegrees, const Rgba& particleColor )
	: m_ageInSeconds( 0.0f )
	, m_lifetimeInSeconds( 0.75f )
	, m_isDead( false )
{
	m_position = position;
	m_direction = direction;
	m_emissionsPerSecond = emissionsPerSecond;
	m_secondsSinceLastEmission = 0;

	m_minDegrees = minDegrees;
	m_maxDegrees = maxDegrees;

	m_particleColor = particleColor;

	m_maxParticles = maxParticles;

	m_type = type;
	m_usesCone = false;

	HandleTypeConstruction();

	m_texture = Texture::CreateOrGetTexture( "Data/Images/particle.png" );
	m_bounds.mins = Vector2( -2.5f, -2.5f );
	m_bounds.maxs = Vector2( 2.5f, 2.5f );

	m_isLooping = false;
}


//-----------------------------------------------------------------------------------------------
void Emitter::Update( const float deltaSeconds )
{
	if ( m_isDead )
	{
		return;
	}

	m_ageInSeconds += deltaSeconds;

	if ( m_ageInSeconds > m_lifetimeInSeconds && m_particles.empty() && !m_isLooping )
	{
		m_isDead = true;
		return;
	}

	for ( auto partIter = m_particles.begin(); partIter != m_particles.end(); ++partIter )
	{
		Vector2 totalAcceleration = Vector2::ZERO;
		for ( auto forceIter = ( *partIter ).forces.begin(); forceIter != ( *partIter ).forces.end(); ++forceIter )
		{
			Vector2 forceVec = forceIter->direction * forceIter->magnitude;
			Vector2 acceleration = forceVec * ( 1.0f / ( *partIter ).mass );
			totalAcceleration += acceleration;
			forceIter->secLeft -= deltaSeconds;
		}
		( *partIter ).velocity += totalAcceleration * deltaSeconds;
		( *partIter ).position += ( *partIter ).velocity * deltaSeconds;
		( *partIter ).secondsLeft -= deltaSeconds;

		// Check to see if any forces need to be removed
		std::vector< Force >::iterator it = ( *partIter ).forces.begin();

		while ( it != ( *partIter ).forces.end() )
		{
			if ( it->isConstant ) // Run forever if constant
			{
				++it;
			}
			else
			{
				if ( it->secLeft <= 0.0f )
				{
					it = ( *partIter ).forces.erase( it );
				}
				else
				{
					++it;
				}
			}
		}
	}

	float emissions = m_secondsSinceLastEmission * m_emissionsPerSecond;
	if ( 1.0f < emissions )
	{
		FireEmitter();
	}
	m_secondsSinceLastEmission += deltaSeconds;
	RemoveDeadParticles();
}


//-----------------------------------------------------------------------------------------------
void Emitter::Render()
{
	if ( m_isDead )
	{
		return;
	}

// 	g_theRenderer->DrawFilledPolygon( 20, m_position.x, m_position.y,
// 		0.1f, Rgba::MAGENTA );

	for ( auto partIter = m_particles.begin(); partIter != m_particles.end(); ++partIter )
	{
		g_theRenderer->PushView();
		Vector3 positionAsVector3( ( *partIter ).position.x, ( *partIter ).position.y, 0.0f );
		g_theRenderer->TranslateView( positionAsVector3 );
		//g_theRenderer->RotateView( GetOrientationDegrees(), Vector3( 0.0f, 0.0f, 1.0f ) );
		//float percentLifeLeft = ( *partIter ).secondsLeft / ( *partIter ).lifetime;
		//Rgba scaledAlphaColor( ( *partIter ).color.r, ( *partIter ).color.g, ( *partIter ).color.b, ( unsigned char ) ( percentLifeLeft * 255 ) );
		Rgba scaledAlphaColor( ( *partIter ).color.r, ( *partIter ).color.g, ( *partIter ).color.b, 255 );
		g_theRenderer->DrawTexturedAABB2( m_bounds, *m_texture, Vector2( 0.f, 0.f ), Vector2( 1.f, 1.f ), scaledAlphaColor );
		g_theRenderer->PopView();
	}

}


//-----------------------------------------------------------------------------------------------
void Emitter::FireEmitter()
{
	HandleTypeConstruction();

	m_secondsSinceLastEmission = 0.0f;
}


//-----------------------------------------------------------------------------------------------
void Emitter::RemoveDeadParticles()
{
	std::vector< Particle >::iterator it = m_particles.begin();

	while ( it != m_particles.end() )
	{
		if ( ( *it ).lifetime == -1.0f ) // Run forever if constant
		{
			++it;
		}
		else
		{
			if ( ( *it ).secondsLeft <= 0.0f )
			{
				//delete ( *it );
				it = m_particles.erase( it );
			}
			else
			{
				++it;
			}
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Emitter::HandleTypeConstruction()
{
	switch ( m_type )
	{
		case EMITTER_TYPE_EXPLOSION:
			MakeExplosion();
			break;
		case EMITTER_TYPE_FIREWORK:
			MakeFirework();
			break;
		case EMITTER_TYPE_SMOKE:
			MakeSmoke();
			break;
		case EMITTER_TYPE_FOUNTAIN:
			MakeFountain();
			break;
		default:
			MakeExplosion();
			break;
	}
}


//-----------------------------------------------------------------------------------------------
void Emitter::MakeExplosion()
{
	for ( int i = 0; i < m_maxParticles; i++ )
	{
		Vector2 dir = GetRandomParticleDirection();

		Particle p = Particle();
		p.position = m_position;
		p.velocity = Vector2::ZERO;
		p.lifetime = 0.6f;
		p.secondsLeft = p.lifetime;
		
// 		if ( i % 2 == 0 && i % 3 == 0 )
// 		{
// 			p.color = Rgba::ORANGE;
// 		}
// 		else if ( i % 2 )
// 		{
// 			p.color = Rgba::RED;
// 		}
// 		else
// 		{
// 			p.color = Rgba::YELLOW;
// 		}

		p.color = m_particleColor;

		p.AddForce( dir, GetRandomFloatInRange( 150.0f, 250.0f ), 0.2f, false );

		m_particles.push_back( p );
	};
}


//-----------------------------------------------------------------------------------------------
void Emitter::MakeFirework()
{
	for ( int i = 0; i < m_maxParticles; i++ )
	{
		Vector2 dir = GetRandomParticleDirection();

		Particle p = Particle();
		p.position = m_position;
		p.velocity = Vector2( 0.0f, 10.0f );
		p.lifetime = 1.0f;
		p.secondsLeft = p.lifetime;

		if ( i % 2 == 0 )
		{
			p.color = Rgba::RED;
		}
		else
		{
			p.color = Rgba::YELLOW;
		}

		p.AddForce( dir, 20.0f, 5.0f, false );

		m_particles.push_back( p );
	};
}


//-----------------------------------------------------------------------------------------------
void Emitter::MakeSmoke()
{
	for ( int i = 0; i < m_maxParticles; i++ )
	{
		Vector2 partDir = GetRandomParticleDirection();
		Vector2 windDir = Vector2( 1.0f, 0.0f );

		Particle p = Particle();
		p.position = m_position;
		p.velocity = Vector2( 0.0f, 5.0f );
		p.lifetime = 5.0f;
		p.secondsLeft = p.lifetime;
		p.color = Rgba::GRAY;

		p.AddForce( partDir, 1.0f, 0.2f, true );
		p.AddForce( windDir, 1.0f, 5.0f, true );

		m_particles.push_back( p );
	};
}


//-----------------------------------------------------------------------------------------------
void Emitter::MakeFountain()
{
	for ( int i = 0; i < m_maxParticles; i++ )
	{
		Vector2 dir = GetRandomParticleDirection();

		Particle p = Particle();
		p.position = m_position;
		p.velocity = Vector2( 0.0f, 20.0f );
		p.lifetime = 5.0f;
		p.secondsLeft = p.lifetime;

		if ( i % 2 == 0 )
		{
			p.color = Rgba::BLUE;
		}
		else
		{
			p.color = Rgba::GREEN;
		}

		p.AddForce( dir, 10.0f, 0.2f, true );

		m_particles.push_back( p );
	};
}


//-----------------------------------------------------------------------------------------------
Vector2 Emitter::GetRandomParticleDirection()
{
	float LO = m_minDegrees.x;
	float HI = m_maxDegrees.x;
	float degX = LO + static_cast < float > ( rand() ) / ( static_cast < float > ( RAND_MAX / ( HI - LO ) ) );

	LO = m_minDegrees.y;
	HI = m_maxDegrees.y;
	float degY = LO + static_cast < float > ( rand() ) / ( static_cast < float > ( RAND_MAX / ( HI - LO ) ) );

	degX = RangeMap( degX, m_minDegrees.x, m_maxDegrees.x, -1.0f, 1.0f );
	degY = RangeMap( degY, m_minDegrees.y, m_maxDegrees.y, -1.0f, 1.0f );

	Vector2 dir( degX, degY );
	dir.Normalize();

	return dir;
}
