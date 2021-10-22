#pragma once
#include "glm/glm.hpp"

class Behaviour;
class World;

class Agent
{
private:
    glm::vec2 m_Position;
    glm::vec2 m_Velocity;

    World* m_World;
    Behaviour* m_Behaviour;

public:
    const float MAX_SPEED = 25.f;

    Agent(const glm::vec2& pPosition, const glm::vec2& pVelocity, World* pWorld);
    ~Agent();
    void SetPosition(const glm::vec2& pPosition);

    glm::vec2 Position();
    glm::vec2 Velocity();

    void SetSeekBehaviour(glm::vec2 pTarget);

    void Update(float pSeconds);
};


