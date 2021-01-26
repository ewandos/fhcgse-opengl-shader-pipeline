#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <shader.h>
#include <camera.h>
#include <model.h>

#include <iostream>

// SOURCE: https://learnopengl.com/

// callback function when viewport gets resized
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
// input handler
void processInput(GLFWwindow* window);
// mouse input handlers
void mouse_callback(GLFWwindow* window, double xPos, double yPos);
void scroll_callback(GLFWwindow* window, double xOffset, double yOffset);

// settings
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 1200;

// button press detection
bool buttonPressed = false;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_WIDTH / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
glm::vec3 ambientIntensity(0.2f, 0.2f, 0.2f);
glm::vec3 diffuseIntensity(0.7f, 0.7f, 0.7f);
glm::vec3 specularIntensity(1.0f, 1.0f, 1.0f);

// material
glm::vec3 ambientColor = glm::vec3(1.0f, 0.5f, 0.31f);
glm::vec3 diffuseColor = glm::vec3(1.0f, 0.5f, 0.31f);
glm::vec3 specularColor = glm::vec3(0.5f, 0.5f, 0.5f);
// transparency
float alpha = 1.0f;

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "openGl Ewert", NULL, NULL);
    if(window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // correct mismatching window and viewport size
    glfwSetWindowSize(window, SCR_WIDTH / 2, SCR_HEIGHT / 2);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // enable depth test (z-buffer)
    glEnable(GL_DEPTH_TEST);

    // enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // building the shader from the vertex and fragment shader paths
    Shader shader("resources/shader.vs", "resources/shader.fs");

    // model loading
    std::vector<Model> models;
    models.push_back(Model("resources/pizza.obj"));
    models.push_back(Model("resources/pizza_medium.obj"));
    models.push_back(Model("resources/pizza_low.obj"));

    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    /*
    relevant transformation matrices
    1) model = local to world space
    2) view = world to view space
    3) projection = view to clip space
    */
    // RENDER LOOP
    while(!glfwWindowShouldClose(window)) {

        // per-frame time logic
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        processInput(window);

        // rendering commands here:

        // clear colors:
        glClearColor(0.85f, 0.85f, 0.85f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // shader needs to be activated before accessing its uniforms
        shader.use();
        // lighting
        shader.setVec3("light.ambient", ambientIntensity);
        shader.setVec3("light.diffuse", diffuseIntensity);
        shader.setVec3("light.specular", specularIntensity);
        shader.setVec3("light.position", lightPos);
        shader.setVec3("viewPos", camera.Position);

        // set alpha val
        shader.setFloat("alpha", alpha);
        // pass cam pos

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);

        glm::mat4 model = glm::mat4(1.0f);
        shader.setMat4("model", model);

        float length = glm::length(camera.Position);
        if(length < 1.5f) {
            models[0].Draw(shader);
        }
        else if(length > 1.5f && length < 3.0f) {
            models[1].Draw(shader);
        }
        else if(length > 3.0f) {
            models[2].Draw(shader);
        }

        // swap buffer and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

    // toggle transparency
    if(glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS && !buttonPressed)
        buttonPressed = true;
    if(glfwGetKey(window, GLFW_KEY_T) == GLFW_RELEASE && buttonPressed) {
        alpha = alpha == 1.0f ? 0.3f : 1.0f;
        buttonPressed = false;
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xOffset = xpos - lastX;
    float yOffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xOffset, yOffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}