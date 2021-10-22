#include "World.h"
#include "Agent.h"
#include "glm/gtx/norm.hpp"

void World::AddAgent(glm::vec2 pPosition, glm::vec2 pVelocity)
{
    m_Agents.push_back(new Agent(pPosition, pVelocity, this));
}

void World::UpdatePhysics(float pSeconds)
{
    for (int i = 0; i < m_Agents.size(); i++)
    {
        m_Agents[i]->Update(pSeconds);
    }
}