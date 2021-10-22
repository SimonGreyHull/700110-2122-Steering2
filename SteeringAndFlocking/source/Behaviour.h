#pragma once
#include "glm/glm.hpp"

class Agent;
class World;

class Behaviour
{
protected:
    World* m_World;
    Agent* m_Agent;
public:
    Behaviour(Agent* pAgent, World* pWorld) :m_Agent(pAgent), m_World(pWorld) {}

    virtual glm::vec2 GetSteeringForce() = 0;
};

class SeekBehaviour : public Behaviour
{
private:

    glm::vec2 m_Target;

public:
    SeekBehaviour(glm::vec2 pTarget, Agent* pAgent, World* pWorld);

    glm::vec2 GetSteeringForce();
};