#pragma once

#include "glm/glm.hpp"
#include <vector>

class Agent;

class World
{
private:
    float m_Width;
    float m_Height;

    std::vector<Agent*> m_Agents;

public:
    World() { m_Height = 100; m_Width = 100; }

    float Width() { return m_Width; }
    float Height() { return m_Height; }

    void AddAgent(glm::vec2 pPosition, glm::vec2 pVelocity);
    std::vector<Agent*> Agents() { return m_Agents; }
    void UpdatePhysics(float pSeconds);
};