#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <math.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <iostream>

float x_mod = 0;
float y_mod = 0;
float theta_mod = 0;

glm::mat4 identity_matrix = glm::mat4(1.0f);

void Key_Callback(
    GLFWwindow* window,
    int key,        // Key Code
    int scancode,
    int action,
    int mod
) {
    if (key == GLFW_KEY_D || key == GLFW_KEY_RIGHT &&
        action == GLFW_PRESS) {
        x_mod += 0.1f;
    }

    if (key == GLFW_KEY_A || key == GLFW_KEY_LEFT &&
        action == GLFW_PRESS) {
        x_mod -= 0.1f;
    }

    if (key == GLFW_KEY_W || key == GLFW_KEY_UP &&
        action == GLFW_PRESS) {
        y_mod += 0.1f;
    }

    if (key == GLFW_KEY_S || key == GLFW_KEY_DOWN &&
        action == GLFW_PRESS) {
        y_mod -= 0.1f;
    }

    if (key == GLFW_KEY_Q &&
        action == GLFW_PRESS) {
        theta_mod += 5.0f;
    }

    if (key == GLFW_KEY_E &&
        action == GLFW_PRESS) {
        theta_mod += 5.0f;
    }
}

int main(void) 
{
    GLFWwindow* window;

    float x = 0, y = 0, z = 0;
    float scale_x = 2, scale_y = 2, scale_z = 1;
    float axis_x = 1, axis_y = 1, axis_z = 2;
    float theta = 90;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(600, 600, "Sydrenz Cao", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    gladLoadGL();

    glfwSetKeyCallback(window, Key_Callback);

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

    std::string path = "3D/bunny.obj";
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
    glGenVertexArrays(1, &VAO);     // Generates 1 VAO and outputs GLuint
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // Tells OpenGL we're working on this VAO
    glBindVertexArray(VAO);

    // Assigns VBO to this VAO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(
        GL_ARRAY_BUFFER,    // Type of Buffer
        sizeof(GL_FLOAT) * attributes.vertices.size(),   // Size in bytes
        &attributes.vertices[0],           // Array itself
        GL_STATIC_DRAW      // Static (for now, GL_DYNAMIC_ARRAY if moving)
    );

    glVertexAttribPointer(
        0,          // 0 == Position
        3,          // XYZ
        GL_FLOAT,    // what array it is
        GL_FALSE,
        3 * sizeof(GL_FLOAT),
        (void*)0
    );

    // Enables Index 0
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        sizeof(GLuint) * mesh_indices.size(),
        mesh_indices.data(),
        GL_STATIC_DRAW
    );

    // Tells OpenGL we're done w/ VBO and VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glm::mat4 projection = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, -1.f, 1.0f);
                                    //left, right, bottom, up,  near, far

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        glm::mat4 transformation_matrix = glm::translate(identity_matrix, glm::vec3(x + x_mod, y + y_mod, z));
        transformation_matrix = glm::scale(transformation_matrix, glm::vec3(scale_x, scale_y, scale_z));
        transformation_matrix = glm::rotate(transformation_matrix, glm::radians(theta += theta_mod), glm::normalize(glm::vec3(axis_x, axis_y, axis_z)));

        unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform");
        glUniformMatrix4fv(transformLoc,    // Address of the transform variable
            1,                              // How many matrices to assign
            GL_FALSE,                       // Transpose?
            glm::value_ptr(transformation_matrix));     // Pointer to the matrix



        unsigned int projLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(projLoc,    // Address of the transform variable
            1,                              // How many matrices to assign
            GL_FALSE,                       // Transpose?
            glm::value_ptr(projection));     // Pointer to the matrix

        glUseProgram(shaderProgram);

        glBindVertexArray(VAO);
        //glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawElements(
            GL_TRIANGLES,
            mesh_indices.size(),
            GL_UNSIGNED_INT,
            0
        );

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}