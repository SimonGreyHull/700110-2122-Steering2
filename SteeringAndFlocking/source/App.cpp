#pragma region includes
/*
GLEW is the OpenGL Extension Wrangler Library which is a simple tool that helps C/C++ developers
initialize extensions and write portable applications.
http://glew.sourceforge.net/
*/
#include <GL/glew.h>

/*
Dear ImGui is a bloat-free graphical user interface library for C++.
https://github.com/ocornut/imgui
*/
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

/*
GLFW is an Open Source, multi-platform library for OpenGL, OpenGL ES and Vulkan development on the desktop. 
It provides a simple API for creating windows, contexts and surfaces, receiving input and events.
https://www.glfw.org/
*/
#include <GLFW/glfw3.h>

/*
OpenGL Mathematics (GLM) is a header only C++ mathematics library for graphics software based on the 
OpenGL Shading Language (GLSL) specifications.
https://github.com/g-truc/glm
*/
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <iostream>
#include <string>
#include <time.h>

#include "World.h"
#include "Agent.h"

#pragma endregion includes

#pragma region data

World world;
Agent* selectedAgent;

#pragma endregion data

#pragma region simulation logic


#pragma endregion simulation logic

#pragma region window setup

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse)
    {
        return;
    }

    if (!(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS))
    {
        return;
    }

    double xPosition, yPosition;
    int windowWidth, windowHeight;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);
    glfwGetCursorPos(window, &xPosition, &yPosition);
    
    yPosition = -yPosition + windowHeight;

    glm::vec2 clickWorldPosition((-world.Width() / 2) + world.Width() * xPosition / windowWidth ,
        (-world.Height() / 2) + world.Height() * yPosition / windowHeight );
}

GLFWwindow* setupWindow()
{
    GLFWwindow* window;

    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    /* Initialize the library */
    if (!glfwInit())
        return 0;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello, GLFW!", NULL, NULL);

    if (!window)
    {
        glfwTerminate();
        return 0;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    glfwSetMouseButtonCallback(window, mouseButtonCallback);

    return window;
}

void cleanUp(GLFWwindow * window)
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}

#pragma region log

const int LOG_LENGTH = 8;
static std::string guiLog[] = { "", "", "", "", "", "", "", "" };


static void AddToLog(std::string pLogEntry)
{
    for (int i = 1; i < LOG_LENGTH; i++)
    {
        guiLog[i - 1] = guiLog[i];
    }
    guiLog[LOG_LENGTH - 1] = pLogEntry;
}

static void DisplayLog()
{
    ImGui::Text("Simulation Log");
    for (int i = 0; i < LOG_LENGTH; i++)
    {
        ImGui::Text(guiLog[i].c_str());
    }
}

#pragma endregion

static void AgentIMGui(Agent& pAgent)
{
    glm::vec2 position = pAgent.Position();
    std::string positionStr = "Pos[" + std::to_string(position.x) + "," + std::to_string(position.y) + "]";
    glm::vec2 velocity = pAgent.Velocity();
    std::string velocityStr = "Vel[" + std::to_string(velocity.x) + "," + std::to_string(velocity.y) + "]";
    ImGui::Text(positionStr.c_str());
    ImGui::Text(velocityStr.c_str());
}

void renderImGui()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Hello, IMGUI!");

    ImGuiStyle& style = ImGui::GetStyle();
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.16f, 0.29f, 0.48f, 0.54f);

    DisplayLog();
    if (selectedAgent)
    {
        AgentIMGui(*selectedAgent);
    }
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.16f, 0.29f, 0.48f, 0.54f);
    
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

#pragma endregion window setup

#pragma region openGL code

GLuint bufferIDs[3];

static GLuint setupOpenGL()
{
    // load vertex buffer data for 10 x 10 square into graphics card
    // origin of the square is the centre point

    glGenBuffers(3, bufferIDs);
    glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[0]);
    float squareVertices[8] = {
        -5.0f, -5.0f,
        5.0f, -5.0f,
        5.0f, 5.0f,
        -5.0f, 5.0f
    };
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), squareVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[1]);
    float triangleVertices[6] = {
        0.0f, 0.5f,
        0.5f, -0.5f,
        -0.5f, -0.5f
    };
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), triangleVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, false, 2 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    // load vertex buffer data to graphics card
    glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[2]);

    float vertices[2 * 36];

    float angleChange = 6.28f / 36;

    for (int i = 0; i < 36; i ++)
    {
        vertices[2 * i] = sin(i * angleChange);
        vertices[2 * i + 1] = cos(i * angleChange);
    }

    glBufferData(GL_ARRAY_BUFFER, 74 * sizeof(float), vertices, GL_STATIC_DRAW);

    // write, compile and link some shaders
    std::string vertexShader =
        "#version 330 core\n"
        "layout(location = 0) in vec2 iPosition;\n"
        "uniform vec2 uTranslation = vec2(0.0, 0.0);\n"
        "uniform mat4 uProjection;\n"
        "void main(){\n"
        "   gl_Position = uProjection * vec4((iPosition + uTranslation).xy, 0.0, 1.0);\n"
        "}\n";

    std::string fragmentShader =
        "#version 330 core\n"
        "layout(location = 0) out vec4 oColour;\n"
        "uniform vec4 uColour = vec4(1.0, 0.0, 1.0, 1.0);\n"
        "void main(){\n"
        "   oColour = uColour;\n"
        "}\n";
    
    GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    const char* shader = vertexShader.c_str();
    glShaderSource(vertexShaderID, 1, &shader, nullptr);
    glCompileShader(vertexShaderID);

    int result;
    glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &result);

    GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    shader = fragmentShader.c_str();
    glShaderSource(fragmentShaderID, 1, &shader, nullptr);
    glCompileShader(fragmentShaderID);

    GLuint shaderProgramID = glCreateProgram();

    glAttachShader(shaderProgramID, vertexShaderID);
    glAttachShader(shaderProgramID, fragmentShaderID);
    glLinkProgram(shaderProgramID);
    glValidateProgram(shaderProgramID);
    glUseProgram(shaderProgramID);

    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);

    return shaderProgramID;
}

static void AgentRender(Agent& pAgent, GLint pColourLocation, GLint pTranslationLocation)
{
    float green[4] = { 0.f, 1.f, 0.f, 1.f };
    float red[4] = { 1.f, 0.f, 0.f, 1.f };
    float blue[4] = { 0.f, 0.f, 1.f, 1.f };

    glUniform4fv(pColourLocation, 1, green);
    glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[2]);
    glVertexAttribPointer(0, 2, GL_FLOAT, false, 2 * sizeof(float), 0);
    glUniform2fv(pTranslationLocation, 1, glm::value_ptr(pAgent.Position()));
    glDrawArrays(GL_LINE_LOOP, 0, 36);

    glUniform4fv(pColourLocation, 1, red);
    glVertexAttribPointer(0, 2, GL_FLOAT, false, 2 * sizeof(float), 0);
    glUniform2fv(pTranslationLocation, 1, glm::value_ptr(pAgent.Position() + pAgent.Velocity()));
    glDrawArrays(GL_LINE_LOOP, 0, 36);
}

static void renderOpenGL(GLFWwindow* window, int shaderProgramID)
{
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    /* Render here */
    glClear(GL_COLOR_BUFFER_BIT);

    GLint colourLocation = glGetUniformLocation(shaderProgramID, "uColour");
    GLint translationLocation = glGetUniformLocation(shaderProgramID, "uTranslation");
    GLint projectionLocation = glGetUniformLocation(shaderProgramID, "uProjection");

    glm::mat4 projection = glm::ortho(-world.Width() / 2 - 1.0f, world.Width() / 2 + 1.0f, -world.Height() / 2 - 1.0f, world.Height() / 2 + 1.0f, -1.f, 1.f);

    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));

    float green[4] = { 0.f, 1.f, 0.f, 1.f };
    float red[4] = { 1.f, 0.f, 0.f, 1.f };
    float blue[4] = { 0.f, 0.f, 1.f, 1.f };

    double xPosition, yPosition;
    int windowWidth, windowHeight;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);
    glfwGetCursorPos(window, &xPosition, &yPosition);

    glm::vec2 currentTranslation(-45.0f, -45.0f);
    glm::vec2 xTranslation(10.0f, 0.0f);
    glm::vec2 yTranslation(0.0f, 10.0f);
    glm::vec2 xReset(-100.0f, 0.f);

    glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[0]);
    glVertexAttribPointer(0, 2, GL_FLOAT, false, 2 * sizeof(float), 0);
    glUniform4fv(colourLocation, 1, green);
    for (int rowIndex = 0; rowIndex < 10; rowIndex++)
    {
        for (int colIndex = 0; colIndex < 10; colIndex++)
        {
            glUniform2fv(translationLocation, 1, glm::value_ptr(currentTranslation));
            glDrawArrays(GL_LINE_LOOP, 0, 4);
            currentTranslation += xTranslation;
        }
        currentTranslation += yTranslation;
        currentTranslation += xReset;
    }
    
    for (auto agent : world.Agents())
    {
        AgentRender(*agent, colourLocation, translationLocation);
    }
}

#pragma endregion openGL code

int main(void)
{
    GLFWwindow * window = setupWindow(); 

    world.AddAgent(glm::vec2(-30, 10), glm::vec2(0, 10));
    world.AddAgent(glm::vec2(30, 0), glm::vec2(0, 0));

    if (glewInit() != GLEW_OK)
    {
        std::cout << "Error initialising GLEW" << std::endl;
    }

    int shaderProgramID = setupOpenGL();

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);

    double lastTime = glfwGetTime();
    double timeNow = lastTime;
    double renderTime = 1.0 / 60.0;
    double updateTime = 1.0 / 100.0f;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        timeNow = glfwGetTime();
        double elaspedTime = timeNow - lastTime;
        timeNow = lastTime;

        renderTime -= elaspedTime;
        updateTime -= elaspedTime;

        // render 60 times a second
        if (renderTime < 0)
        {
            renderTime += 1.0 / 60.0;
            renderOpenGL(window, shaderProgramID);
            renderImGui();
            glfwSwapBuffers(window);
        }

        // update physics 100 times a second
        if (updateTime < 0)
        {
            world.UpdatePhysics(0.01f);

            updateTime += 1.0 / 100.0f;
        }

        /* Poll for and process events */
        glfwPollEvents();

    }

    cleanUp(window);

    return 0;
}