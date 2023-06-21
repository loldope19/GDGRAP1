/* * * * * * * * * * * * * * * * * * * *
 *  GDGRAP1 - Programming Challenge 1  *
 *  By: Sydrenz Anthony P. Cao         *
 *  GDGRAP1 - X22                      *
 * * * * * * * * * * * * * * * * * * * */

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <math.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"
#include "Model.h"

#include <string>
#include <iostream>

using namespace camera;
using namespace model;

float height = 600.f;           // Application Height
float width = 600.f;            // Application Width
float lastX = width / 2.0f;     // Last known X-Value of the Mouse
float lastY = height / 2.0f;    // Last known Y-Value of the Mouse
bool firstMouse = true;         // Checks whether it is the mouse's first time handling movement within the application
double dSpacePress = 3.0;       // Minimum waiting time before spacebar can properly function (3 secs)
double dLastSpace = 0.0;        // Recorded time from the last time the spacebar has properly functioned

/* * * * * * * * * * * * * * * * * * * * * * * * * 
 *  "Fish (Low Poly)" (https://rb.gy/9uykz) by   * 
 *  kaangvl is licensed for Personal Use Only."  * 
 * * * * * * * * * * * * * * * * * * * * * * * * */

std::string objPath = "3D/fish.obj";        // OBJ file path

Camera cCamera(glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(0.0f, 1.0f, 0.0f));      // The main camera of the application
std::vector<Model3D> modelVectors;          // A vector handling all created models

// addModel - Draws and renders the model (OBJ file)
// @param shaderProgram - The program in which both fragment and vertex shaders are attached to
// @param VAO - Vertex Array Object, stores the configuration of vertex attribute pointers and VBOs needed to render geometry.
// @param mesh_indices - A vector storing and handling all indices of the mesh
void addModel(GLuint shaderProgram, GLuint VAO, std::vector<GLuint> mesh_indices) {
    glUseProgram(shaderProgram);

    glBindVertexArray(VAO);
    glDrawElements(
        GL_TRIANGLES,
        mesh_indices.size(),
        GL_UNSIGNED_INT,
        0
    );
}

// processInput - Handles keyboard input as replacement for the (now obsolete) function keyCallback
// @param window - The main window of the file
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);             // Press ESC to exit!!

    float cameraSpeed = 0.005f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cCamera.processKeyboard(UP);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cCamera.processKeyboard(DOWN);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cCamera.processKeyboard(LEFT);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cCamera.processKeyboard(RIGHT);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        double dCurrentTime = glfwGetTime();
        double dElapsedTime = dCurrentTime - dLastSpace;

        if (dElapsedTime >= dSpacePress) {
            Model3D newModel(objPath, cCamera.cameraPos + cCamera.center);
            modelVectors.push_back(newModel);

            dLastSpace = dCurrentTime;
        }
        
    }
}

// mouse_callback - Handles mouse movement in conjunction with the camera class
// @param window - The main window of the file
// @param xpos - Mouse X-Position
// @param xpos - Mouse Y-Position
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    cCamera.processMouseMovement(xoffset, yoffset, true);
}

int main(void) 
{
    GLFWwindow* window;
    GLint objColorLoc;

    Model3D cModel(objPath, cCamera.cameraPos + cCamera.center);
    modelVectors.push_back(cModel);

    dLastSpace = glfwGetTime();

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    window = glfwCreateWindow(width, height, "GDGRAP1 - Sydrenz Cao", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    gladLoadGL();

    glEnable(GL_DEPTH_TEST);

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // --------- Shader program creation --------- //
    std::fstream vertSrc("Shaders/sample.vert");
    std::stringstream vertBuff;
    vertBuff << vertSrc.rdbuf();
    std::string vertS = vertBuff.str();
    const char* v = vertS.c_str();

    std::fstream fragSrc("Shaders/sample.frag");
    std::stringstream fragBuff;
    fragBuff << fragSrc.rdbuf();
    std::string fragS = fragBuff.str();
    const char* f = fragS.c_str();

    GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertShader, 1, &v, NULL);
    glCompileShader(vertShader);

    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &f, NULL);
    glCompileShader(fragShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertShader);
    glAttachShader(shaderProgram, fragShader);

    glLinkProgram(shaderProgram);

    std::string path = cModel.path;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> material;
    std::string warning, error;

    tinyobj::attrib_t attributes;

    bool success = tinyobj::LoadObj(
        &attributes,
        &shapes,
        &material,
        &warning,
        &error,
        path.c_str()
    );

    std::vector<GLuint> mesh_indices;
    for (int i = 0; i < shapes[0].mesh.indices.size(); i++) {
        mesh_indices.push_back(
            shapes[0].mesh.indices[i].vertex_index
            );
    }

    GLfloat vertices[]{
        //x    y     z
        0.5f, 0.5f, 0.f,        // Point 0
        -0.5f, -0.5f, 0.f,      // Point 1
        0.5f, -0.5f, 0.f        // Point 2
    };

    GLfloat indices[]{
        0, 1, 2
    };

    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * attributes.vertices.size(), &attributes.vertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (void*)0);

    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * mesh_indices.size(), mesh_indices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glViewport(0, 0, width, height);
    glm::mat4 projection = glm::perspective(glm::radians(60.f), height / width, 0.1f, 100.0f);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        unsigned int transformLoc;
        glm::mat4 transformation_matrix;

        glm::mat4 viewMatrix = cCamera.getViewMatrix();
        unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(viewLoc,
            1,
            GL_FALSE,
            glm::value_ptr(viewMatrix));

        unsigned int projLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));    

        for (Model3D modelBuffer : modelVectors) {
            transformation_matrix = modelBuffer.transform();
            transformLoc = glGetUniformLocation(shaderProgram, "transform");
            glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transformation_matrix));

            objColorLoc = glGetUniformLocation(shaderProgram, "objColor");
            glUniform3f(objColorLoc, 0.1f, 0.3f, 0.7f);

            addModel(shaderProgram, VAO, mesh_indices);
        }
        

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}