#include "Behaviour.h"
#include "Agent.h"
#include "World.h"

    SeekBehaviour::SeekBehaviour(glm::vec2 pTarget, Agent* pAgent, World* pWorld) : Behaviour(pAgent, pWorld), m_Target(pTarget)
    {}

    glm::vec2 SeekBehaviour::GetSteeringForce()
    {
        glm::vec2 steeringForce = m_Target - m_Agent->Position();
        
        return steeringForce;
    }
