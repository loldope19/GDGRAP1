/* * * * * * * * * * * * * * * * * * * * * * * * * * * 
 * Quiz 2 - #7 (Sydrenz Cao | GDGRAP1)               * 
 * Completed Items:                                  * 
 * - Center it with the full model visible           * 
 * - Constantly rotate it along the Y axis           * 
 * - Use perspective projection with FOV 90          * 
 * - Use a Direction light with direction (-1,-1,0)  * 
 *   and the color green for all components          * 
 *                                                   * 
 * Incomplete Items:                                 *
 * - Apply any texture / picture on the model        *
 * * * * * * * * * * * * * * * * * * * * * * * * * * */

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

#include <string>
#include <iostream>

float height = 600.f;
float width = 600.f;

float x = 0, y = 0, z = 0;      // Centered Model (?) [1]
float scale_x = 0.05f, scale_y = 0.05f, scale_z = 0.05f;
float axis_x = 1.0f, axis_y = 1.0f, axis_z = 0.0f;

float x_mod = 0;
float y_mod = 0;
float scale_mod = 0;
float theta_xmod = 0, theta_ymod = 0;
float zoom_mod = 0;

float ambientStr = 0.1f;
float specStr = 0.5f;
float specPhong = 0.5f;

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, -6.0f);
glm::vec3 WorldUp = glm::vec3(0, 1.0f, 0);
glm::vec3 center = glm::vec3(0, 0.0f, 0);

glm::mat4 identity_matrix = glm::mat4(1.0f);
glm::vec3 lightPos = glm::vec3(-1, -1, 0);
glm::vec3 lightColor = glm::vec3(0, 1, 0);      // Color Green [5]

glm::vec3 ambientColor = lightColor;

void Key_Callback(
    GLFWwindow* window,
    int key,        // Key Code
    int scancode,
    int action,
    int mod
) {
    if (key == GLFW_KEY_D &&
        action == GLFW_PRESS) {
        x_mod += 1.0f;              // Movement (X-Axis)
    }

    if (key == GLFW_KEY_A &&
        action == GLFW_PRESS) {
        x_mod -= 1.0f;              // Movement (X-Axis)
    }

    if (key == GLFW_KEY_W &&
        action == GLFW_PRESS) {
        y_mod += 1.0f;              // Movement (Y-Axis)
    }

    if (key == GLFW_KEY_S &&
        action == GLFW_PRESS) {
        y_mod -= 1.0f;              // Movement (Y-Axis)
    }

    if (key == GLFW_KEY_UP &&
        action == GLFW_PRESS) {
        theta_xmod += 2.0f;         // Rotation (X-Axis)
    }

    if (key == GLFW_KEY_DOWN &&
        action == GLFW_PRESS) {
        theta_xmod -= 2.0f;         // Rotation (X-Axis)
    }

    if (key == GLFW_KEY_LEFT &&
        action == GLFW_PRESS) {
        theta_ymod += 2.0f;         // Rotation (Y-Axis)
    }

    if (key == GLFW_KEY_RIGHT &&
        action == GLFW_PRESS) {
        theta_ymod -= 2.0f;         // Rotation (Y-Axis)
    }

    if (key == GLFW_KEY_Q &&
        action == GLFW_PRESS) {
        scale_mod += 1.0f;          // Increase Scale
    }

    if (key == GLFW_KEY_E &&
        action == GLFW_PRESS) {
        scale_mod -= 1.0f;          // Decrease Scale
    }

    if (key == GLFW_KEY_Z &&
        action == GLFW_PRESS) {
        zoom_mod += 0.5f;          // Zoom In
    }

    if (key == GLFW_KEY_X &&
        action == GLFW_PRESS) {
        zoom_mod -= 0.5f;          // Zoom Out
    }
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(width, height, "Sydrenz Cao", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    gladLoadGL();

    int img_width,      // Texture Width
        img_height,     // Texture Height
        colorChannels;  // No. of Color Channels

    // Fix the flipped texture
    stbi_set_flip_vertically_on_load(true);

    // Loads the texture and fills out the variables
    unsigned char* tex_bytes =
        stbi_load("3d/ayaya.png",   // Texture Path
            &img_width,             // Fills out the width
            &img_height,            // Fills out the height
            &colorChannels,         // Fills out the color channel
            0);

    // OpenGL reference to the texture
    GLuint texture;

    // Generate a reference
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Assign the loaded texture to the OpenGL reference
    glTexImage2D(GL_TEXTURE_2D,
        0,                          // Texture 0
        GL_RGBA,                    // Target color format of the texture
        img_width,                  // Texture width
        img_height,                 // Texture height
        0,
        GL_RGBA,                    // Color format of the texture
        GL_UNSIGNED_BYTE,
        tex_bytes);                 // Loaded texture in bytes

    // Generate the mipmaps to the current textures
    glGenerateMipmap(GL_TEXTURE_2D);
    // Free up the loaded bytes
    stbi_image_free(tex_bytes);

    // Enable Depth Testing
    glEnable(GL_DEPTH_TEST);

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

    std::string path = "3D/djSword.obj";
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

    GLfloat UV[]{       // UV Data
        0.f, 10.f,
        0.f, 0.f,
        10.f, 10.f,
        10.f, 0.f,
        10.f, 10.f,
        10.f, 0.f,
        0.f, 10.f,
        0.f, 0.f
    };

    GLint objColorLoc = glGetUniformLocation(shaderProgram, "objColor");

    std::vector<GLuint> mesh_indices;
    for (int i = 0; i < shapes[0].mesh.indices.size(); i++) {
        mesh_indices.push_back(
            shapes[0].mesh.indices[i].vertex_index
        );
    }

    std::vector<GLfloat> fullVertexData;
    for (int i = 0; i < shapes[0].mesh.indices.size(); i++) {
        tinyobj::index_t vData = shapes[0].mesh.indices[i];

        fullVertexData.push_back(
            attributes.vertices[(vData.vertex_index * 3)]
        );

        fullVertexData.push_back(
            attributes.vertices[(vData.vertex_index * 3) + 1]
        );

        fullVertexData.push_back(
            attributes.vertices[(vData.vertex_index * 3) + 2]
        );

        fullVertexData.push_back(
            attributes.normals[(vData.normal_index * 3)]
        );

        fullVertexData.push_back(
            attributes.normals[(vData.normal_index * 3) + 1]
        );

        fullVertexData.push_back(
            attributes.normals[(vData.normal_index * 3) + 2]
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

    GLuint VAO, VBO, VBO_UV;
    glGenVertexArrays(1, &VAO);     // Generates 1 VAO and outputs GLuint
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &VBO_UV);

    // Tells OpenGL we're working on this VAO
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,
        sizeof(GLfloat) * fullVertexData.size(),
        fullVertexData.data(),
        GL_DYNAMIC_DRAW
    );

    glVertexAttribPointer(
        0,          // 0 == Position
        3,          // XYZ
        GL_FLOAT,    // what array it is
        GL_FALSE,
        6 * sizeof(GL_FLOAT),
        (void*)0
    );

    GLintptr normalPtr = 3 * sizeof(float);
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        6 * sizeof(float),
        (void*)normalPtr
    );

    // Enables Index 0
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_UV);
    glBufferData(GL_ARRAY_BUFFER,
        (sizeof(UV) / sizeof(UV[0])),
        &UV[0],
        GL_STATIC_DRAW);

    glVertexAttribPointer(
        2, 
        2, 
        GL_FLOAT,
        GL_FALSE,
        8 * sizeof(float), 
        (void*)0
    );

    // Enable 2 for our UV / Tex coords
    glEnableVertexAttribArray(2);

    // Tells OpenGL we're done w/ VBO and VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glViewport(0, 0, width, height);
    glm::mat4 projection = glm::perspective(
        glm::radians(90.f), // FOV 90 [4]
        height / width, 
        0.1f, 
        50.0f 
    );

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Get the location of tex 0 in the fragment shader
        GLuint tex0Address = glGetUniformLocation(shaderProgram, "tex0");
        // Tell OpenGL to use the texture
        glBindTexture(GL_TEXTURE_2D, texture);
        // Use the texture at 0
        glUniform1i(tex0Address, 0);

        GLuint lightAddress = glGetUniformLocation(shaderProgram, "lightPos");
        glUniform3fv(lightAddress, 1, glm::value_ptr(lightPos));

        GLuint lightColorAddress = glGetUniformLocation(shaderProgram, "lightColor");
        glUniform3fv(lightColorAddress, 1, glm::value_ptr(lightColor));

        GLuint ambientStrAddress = glGetUniformLocation(shaderProgram, "ambientStr");
        glUniform1f(ambientStrAddress, ambientStr);

        GLuint ambientColorAddress = glGetUniformLocation(shaderProgram, "ambientColor");
        glUniform3fv(ambientColorAddress, 1, glm::value_ptr(ambientColor));

        GLuint cameraPosAddress = glGetUniformLocation(shaderProgram, "cameraPos");
        glUniform3fv(cameraPosAddress, 1, glm::value_ptr(cameraPos));

        GLuint specStrAddress = glGetUniformLocation(shaderProgram, "specStr");
        glUniform1f(specStrAddress, specStr);

        GLuint specPhongAddress = glGetUniformLocation(shaderProgram, "specPhong");
        glUniform1f(specStrAddress, specPhong);

        glm::mat4 transformation_matrix = glm::translate(identity_matrix, glm::vec3(x, y, z + zoom_mod));
        transformation_matrix = glm::scale(transformation_matrix, glm::vec3(scale_x + scale_mod, scale_y + scale_mod, scale_z + scale_mod));
        transformation_matrix = glm::rotate(transformation_matrix, glm::radians(theta_xmod += y_mod), glm::normalize(glm::vec3(axis_x, 0, axis_z)));     // Rotation w/ Normalized X-Axis
        // Y-Axis Rotation [3]
        transformation_matrix = glm::rotate(transformation_matrix, glm::radians(theta_ymod += 0.2f), glm::normalize(glm::vec3(0, axis_y, axis_z)));     // Rotation w/ Normalized Y-Axis

        glm::mat4 viewMatrix = glm::lookAt(cameraPos, center, WorldUp);

        unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform");
        glUniformMatrix4fv(transformLoc,                // Address of the transform variable
            1,                                          // How many matrices to assign
            GL_FALSE,                                   // Transpose?
            glm::value_ptr(transformation_matrix));     // Pointer to the matrix

        unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(viewLoc,
            1,
            GL_FALSE,
            glm::value_ptr(viewMatrix));

        unsigned int projLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(projLoc,    // Address of the transform variable
            1,                              // How many matrices to assign
            GL_FALSE,                       // Transpose?
            glm::value_ptr(projection));     // Pointer to the matrix

        // ---------------------------- //
        glUseProgram(shaderProgram);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, fullVertexData.size() / 6);
        // ---------------------------- //

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
}
