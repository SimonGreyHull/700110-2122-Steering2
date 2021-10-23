# 700110-2122-Steering2

## Introduction

In this lab we will be refactoring the previous lab to create a more flexible architecture with more complex behaviours.

## Aim

- Build a reusable architecture
- Add existing seek and flee behaviorus
- Add pursue agent behaviour
- Add evade agent behaviour
- Add cohesion behaviour for flocking
- Add allignement behaviour for flocking
- Add separation behaviour for flocking
- Add compound behaviour

## First look at the code

Ensure the code compiles and runs. Then take a look at the code that has been provided. You will notice an Agent class, a Behaviour class and a World class. 

Let's start at the World class. The World class in a container for the world. It contains a vector of Agents, and data members for the Width and Height of the world.

There is also functionality to add an agent to the world, to get the vector of agents from the world (so they can be rendered), and update the world simulation.

``` cpp
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
```

Next, look at the Agent class. The agent has a position and a velocity, a pointer to the world so that it can find out important things about the world, like the width and height of the world and a pointer to it's current behaviour.

``` cpp
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
```
The Agent update method attempts to get a steering force from its behaviour (if it has one) and create an acceleration to then integrates the velocity with repect to time to update the agent velocity, then it does the same with the position.
Then it checks to see that the position is still in the world, and if not wraps the position around the world.
``` cpp
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
```

Behaviour is a base class that again has pointers to the world, and also to the agent that owns the behaviour. A virtual method GetSteeringForce ensures that all concrete behaviours provide this method.
``` cpp
class Behaviour
{
protected:
    World* m_World;
    Agent* m_Agent;
public:
    Behaviour(Agent* pAgent, World* pWorld) :m_Agent(pAgent), m_World(pWorld) {}

    virtual glm::vec2 GetSteeringForce() = 0;
};
```

There is one example of SeekBehaviour already implemented.

``` cpp
class SeekBehaviour : public Behaviour
{
private:

    glm::vec2 m_Target;

public:
    SeekBehaviour(glm::vec2 pTarget, Agent* pAgent, World* pWorld);

    glm::vec2 GetSteeringForce();
};
```

but the seek behaviour is not yet being used.

### Know your next commit!

The first task is to apply the seek behaviour to an agent. The world contains two agents, but before we can do that we need to be able to select an agent.

In the App.cpp file there is already a pointer for the selectedAgent, but it's currently 0. First we need to respond to the mouse click. At the bottom of the mouseButtonCallback method add the following code.

``` cpp
    // flip the y axis
    yPosition = -yPosition + windowHeight;

    // find the world position from the screen position
    glm::vec2 clickWorldPosition((-world.Width() / 2) + world.Width() * xPosition / windowWidth ,
        (-world.Height() / 2) + world.Height() * yPosition / windowHeight );

    selectedAgent = world.GetClosestAgent(clickWorldPosition);
```

Next, we need to add the GetClosestAgent to the World class.

``` cpp
Agent* World::GetClosestAgent(glm::vec2 pClickWorldPosition)
{
    int closestIndex = 0;
    float closestDistanceSquared = 1000;
    float currectDistanceSquared;
    for (int i = 0; i < m_Agents.size(); i++)
    {
        currectDistanceSquared = glm::distance2(m_Agents[i]->Position(), pClickWorldPosition);
        if (currectDistanceSquared < closestDistanceSquared)
        {
            closestDistanceSquared = currectDistanceSquared;
            closestIndex = i;
        }
    }

    if (closestDistanceSquared < 2)
    {
        return m_Agents[closestIndex];
    }
    return 0;
}
```
### Test and commit your code to source control

Test your code is working as expected. You should be able to click on an agent and see a change in ImGui. If it works as expected commit with a suitable message like "Can select an agent by clicking".

### Know your next commit!

Now you should adapt the application to enable you to add the seek behaviour to the selectedAgent. Add the following code at the end of the AgentIMGui method. This will add some controls to the user interface to select an x and a y value. Then, when the Seek button is pressed the SetSeekBehaviour method will be called on the selected agent, passing the current values in from the GUI.

``` cpp
    std::string targetXStr = "x";
    static int x = 0;
    ImGui::DragInt("target x", &x, 1, -50, 50);
    std::string targetYStr = "y";
    static int y = 0;
    ImGui::DragInt("target y", &y, 1, -50, 50);
    if (ImGui::Button("Seek"))
    {
        pAgent.SetSeekBehaviour(glm::vec2(x, y));
    }
```

### Test and commit your code to source control

Test your code is working as expected. Once you're done testing commit with a suitable message like "Added seek behaviour to agent".

### Know your next commit!

You may notice some odd behaviour with your agent. This is probably because when the SeekBehaviour calculates the direction to the seek target it doesn't take into account that the world wraps around itself.

Add a method to the World to get the shortest distance, taking into account that the distance might cross the world's "edge" to wrap around to the other side. 
This code works by checking the "natural" distance between two points, and the distance from the pFrom point to the pTo point crossing the left side, right side, top and bottom. This could definitely be optimised by calculating which sides the pTo point is closest to.

``` cpp
glm::vec2 World::GetShortestWrappedDirection(const glm::vec2& pFrom, const glm::vec2& pTo)
{
    /*
        There will be more efficent ways to reduce this calculation.
        For expediency of writing we check the length of the current world and
        all 9 adjacent "wrapped worlds".
    */

    glm::vec2 directions[9];

    directions[0] = pTo - pFrom;
    directions[0].x -= Width();
    directions[0].y += Height();
    directions[1] = pTo - pFrom;
    directions[1].y += Height();
    directions[2] = pTo - pFrom;
    directions[2].x += Width();
    directions[2].y += Height();
    directions[3] = pTo - pFrom;
    directions[3].x -= Width();

    directions[4] = pTo - pFrom;

    directions[5] = pTo - pFrom;
    directions[5].x += Width();
    directions[6] = pTo - pFrom;
    directions[6].x -= Width();
    directions[6].y -= Height();
    directions[7] = pTo - pFrom;
    directions[7].y -= Height();
    directions[8] = pTo - pFrom;
    directions[8].x += Width();
    directions[8].y -= Height();

    int closestDirectionIndex = 4;
    float closestLengthSquared = glm::length2(directions[4]);
    float lengthSquared;
    for (int i = 1; i < 8; i++)
    {
        lengthSquared = glm::length2(directions[i]);
        if (lengthSquared < closestLengthSquared)
        {
            closestLengthSquared = lengthSquared;
            closestDirectionIndex = i;
        }
    }

    return directions[closestDirectionIndex];
}
```
This is a very brute force method, so if you like a challenge you may want to optimise this.

Next modify the SeekBehaviour to use this method to calculate the shortest direction to the target. Replace the line that calculates the direction from the agent to the target;

``` cpp
glm::vec2 steeringForce = m_Target - m_Agent->Position();
```

With this line, that calls the method you just wrote;

``` cpp
glm::vec2 steeringForce = m_World->GetShortestWrappedDirection(m_Agent->Position(), m_Target);
```
### Test and commit your code to source control

Test your code and once you are convinced that it is working. You should experiment by placing the target close to the edges and corners of the world. When you are convinced your code works commit to source control with a suitable message line "added wrap around behaviour and applied in SeekBehaviour"

### Know your next commit!

Next add the FleeBehaviour. Remember, the Flee Behaviour is very similar to the seek behaviour, but the Flee Behaviour moves away from the target, and not towards it.

To do this you will need to take the following steps:
   - Create a new FleeBehaviour class that inherits the Behaviour class. This will be very similar to the SeekBehaviour class.
   - Write the GetSteeringForce method to calculate a steering force to move the agent away from the target.
   - Write a SetFleeBehaviour method in the Agent class to set the behaviour of the agent to flee.
   - In the App.cpp file modify the AgentIMGui by adding another button that says "Flee" instead of "Seek". When that button is pressed call the SetFleeBehaviour method on the selectedAgent. You can reuse the existing UI implementation to select a target.

### Test and commit your code to source control

Test your code and commit it with an appropriate message like "Implemented FleeBehaviour"

### Know your next commit!

Next add PursueAgentBehaviour. The pursue agent behaviour will need a pointer to an agent to pursue. Then each update it calculates the position that the agent will be in a short time in the future (say 1/4 second) and uses that are the target.
After that, the behaviour is the same as the seek behaviour.

To do this take the following steps:
    - Create a new PursueAgentBehaviour class that inherits the Behaviour class
    - Write the GetSteeringForce method to calculate a steering force
    - Write a SetPursueAgentBehaviour method in the Agent class
    - Modigy the GUI in the App.cpp file
    
To create the PursueAgentBehaviour class add some code like this:

``` cpp
class PursueAgentBehaviour : public Behaviour
{
private:

    Agent* m_Target;

public:
    PursueAgentBehaviour(Agent* pTarget, Agent* pAgent, World* pWorld);

    glm::vec2 GetSteeringForce();
};
```
Note that the agent doesn't have a vec2 as a target, but instead has a pointer to the agent they are pursuing.

Next write the GetSteeringForce method. This should calculate a target that is where the m_Target agent will be in 0.25 seconds.

``` cpp
    glm::vec2 PursueAgentBehaviour::GetSteeringForce()
    {
        glm::vec2 target = m_Target->Position() + (m_Target->Velocity() * 0.25f);
        glm::vec2 steeringForce = m_World->GetShortestWrappedDirection(m_Agent->Position(), target);

        return steeringForce;
    }
```

Write some code to set the behaviour in the Agent class. This will be very similar to existing code, and is a sure sign that this could be refactored. 

The final step is to add the ability to add the behaviour to the selected agent. The code below will select the first agent in the world's list of agent (that is not the currently selected agent) and use that agent as the target.

``` cpp
    if (ImGui::Button("Pursue Agent"))
    {
        Agent* target;
        for (int i = 0; i < world.Agents().size(); i++)
        {
            if (selectedAgent != world.Agents()[i])
            {
                selectedAgent->SetPursueAgentBehaviour(world.Agents()[i]);
            }
        }
    }
```
### Test and commit your code to source control

Test your code and commit it with an appropriate message like "Added pursue agent behaviour"

### Know your next commit!

Next add EvadeAgentBehaviour. This is similar to the pursue agent behaviour, but it flees from the point that the agent will be in a short time in the future!

To do this take the following steps:
    - Create a new EvadeAgentBehaviour class that inherits the Behaviour class
    - Write the GetSteeringForce method to calculate a steering force
    - Write a SetEvadeAgentBehaviour method in the Agent class
    - Modigy the GUI in the App.cpp file

### Test and commit your code to source control

Test your code and commit it with an appropriate message like "Added evade agent behaviour"


