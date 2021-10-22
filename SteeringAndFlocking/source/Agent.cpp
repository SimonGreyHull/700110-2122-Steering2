#include "Agent.h"
#include "Behaviour.h"
#include "World.h"
#include "glm/gtx/norm.hpp"

Agent::Agent(const glm::vec2& pPosition, const glm::vec2& pVelocity, World* pWorld) : m_Position(pPosition), m_Velocity(pVelocity), m_World(pWorld)
{
    m_Behaviour = 0;
}

Agent::~Agent()
{
    if (m_Behaviour)
    {
        delete m_Behaviour;
    }
}

void Agent::SetSeekBehaviour(glm::vec2 pTarget)
{
    if (m_Behaviour != 0)
    {
        delete m_Behaviour;
    }

    m_Behaviour = new SeekBehaviour(pTarget, this, m_World);
}

void Agent::SetPosition(const glm::vec2& pPosition)
{
    m_Position = pPosition;
}

glm::vec2 Agent::Position() { return m_Position; }
glm::vec2 Agent::Velocity() { return m_Velocity; }

void Agent::Update(float pSeconds)
{
    if (m_Behaviour)
    {
        glm::vec2 acceleration = m_Behaviour->GetSteeringForce(); // calculate desired acceleration
        // note that this is not proper simulation - we are kind of assuming mass to be 1 
        // integrate velocity
        m_Velocity = m_Velocity + acceleration * pSeconds;
    }

    if (glm::length2(m_Velocity) > MAX_SPEED * MAX_SPEED)
    {
        m_Velocity = glm::normalize(m_Velocity);
        m_Velocity *= MAX_SPEED;
    }

    // integrate position
    m_Position = m_Position + m_Velocity * pSeconds;

    if (m_Position.y > m_World->Height() / 2)
    {
        m_Position.y = -m_World->Height() / 2;
    }
    else if (m_Position.y < -m_World->Height() / 2)
    {
        m_Position.y = m_World->Height() / 2;
    }

    if (m_Position.x > m_World->Width() / 2)
    {
        m_Position.x = -m_World->Width() / 2;
    }
    else if (m_Position.x < -m_World->Width() / 2)
    {
        m_Position.x = m_World->Width() / 2;
    }
}