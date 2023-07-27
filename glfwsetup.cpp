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

// Model Variables
float x = 0, y = 0, z = 0;
float scale_x = 2.f, scale_y = 2.f, scale_z = 2.f;
float axis_x = 1.0f, axis_y = 1.0f, axis_z = 0.0f;
float theta_xmod = 90, theta_ymod = 0;

// Light Variables
float ambientStr = 0.1f;
float specStr = 0.5f;
float specPhong = 0.5f;

// Camera Variables
glm::vec3 cameraPos = glm::vec3(-5.0f, 0.0f, 0.0f);
glm::vec3 WorldUp = glm::vec3(0, 1.0f, 0);
glm::vec3 center = glm::vec3(0, 0.0f, 0);

// OpenGL Light Variables
glm::mat4 identity_matrix = glm::mat4(1.0f);
glm::vec3 lightPos = glm::vec3(-5.f, 0, 0);
glm::vec3 lightColor = glm::vec3(1, 1, 1);

glm::vec3 ambientColor = lightColor;

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

    // Fixes the flipped texture
    stbi_set_flip_vertically_on_load(true);

    // Loads the texture and fills out the variables
    // 1st Texture - Brickwall
    unsigned char* tex_bytes =
        stbi_load("3d/brickwall.jpg",   // Texture Path
            &img_width,             // Fills out the width
            &img_height,            // Fills out the height
            &colorChannels,         // Fills out the color channel
            0);

    // 2nd Texture - Yae
    int img_width2, img_height2, colorChannel2;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* tex_bytes2 = stbi_load("3D/yae.png", &img_width2, &img_height2, &colorChannel2, 0);

    // 3rd Texture - Brickwall (Normal)
    int img_width3, img_height3, colorChannel3;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* normal_bytes = stbi_load("3D/brickwall_normal.jpg", &img_width3, &img_height3, &colorChannel3, 0);

    // -------- 1st Image Generation -------- //
    // OpenGL reference to the texture
    GLuint texture;
    // Generate a reference
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D,
        0,                          // Texture 0
        GL_RGB,                    // Target color format of the texture
        img_width,                  // Texture width
        img_height,                 // Texture height
        0,
        GL_RGB,                    // Color format of the texture
        GL_UNSIGNED_BYTE,
        tex_bytes);                 // Loaded texture in bytes

    // Generate the mipmaps to the current textures
    glGenerateMipmap(GL_TEXTURE_2D);
    // Free up the loaded bytes
    stbi_image_free(tex_bytes);

    // -------- 2nd Image Generation -------- //

    GLuint texture2;
    glGenTextures(1, &texture2);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_width2, img_height2, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_bytes2);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(tex_bytes2);

    // -------- 3rd Image Generation -------- //

    GLuint norm_tex;
    glGenTextures(1, &norm_tex);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, norm_tex); 

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img_width3, img_height3, 0, GL_RGB, GL_UNSIGNED_BYTE, normal_bytes);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(normal_bytes);

    // Enable Depth Testing
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // ------- Shader Initialization ------- //
    std::fstream skyboxVertSrc("Shaders/skybox.vert");
    std::stringstream skyboxVertBuff;
    skyboxVertBuff << skyboxVertSrc.rdbuf();
    std::string skyboxVertS = skyboxVertBuff.str();
    const char* sky_v = skyboxVertS.c_str();

    std::fstream skyboxFragSrc("Shaders/skybox.frag");
    std::stringstream skyboxFragBuff;
    skyboxFragBuff << skyboxFragSrc.rdbuf();
    std::string skyboxFragS = skyboxFragBuff.str();
    const char* sky_f = skyboxFragS.c_str();

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

    GLuint vertexShaderSkybox = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShaderSkybox, 1, &sky_v, NULL);
    glCompileShader(vertexShaderSkybox);

    GLuint fragmentShaderSkybox = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderSkybox, 1, &sky_f, NULL);
    glCompileShader(fragmentShaderSkybox);

    GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertShader, 1, &v, NULL);
    glCompileShader(vertShader);

    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &f, NULL);
    glCompileShader(fragShader);

    GLuint skyboxShaderProg = glCreateProgram();
    glAttachShader(skyboxShaderProg, vertexShaderSkybox);
    glAttachShader(skyboxShaderProg, fragmentShaderSkybox);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertShader);
    glAttachShader(shaderProgram, fragShader);

    glLinkProgram(skyboxShaderProg);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShaderSkybox);
    glDeleteShader(fragmentShaderSkybox);
    glDeleteShader(vertShader);
    glDeleteShader(fragShader);

    // ------- Model Initialization ------- //

    std::string path = "3D/plane.obj";
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

    //Vertices for the cube
    float skyboxVertices[]{
        -1.f, -1.f, 1.f, //0
        1.f, -1.f, 1.f,  //1
        1.f, -1.f, -1.f, //2
        -1.f, -1.f, -1.f,//3
        -1.f, 1.f, 1.f,  //4
        1.f, 1.f, 1.f,   //5
        1.f, 1.f, -1.f,  //6
        -1.f, 1.f, -1.f  //7
    };

    //Skybox Indices
    unsigned int skyboxIndices[]{
        1,2,6,
        6,5,1,

        0,4,7,
        7,3,0,

        4,5,6,
        6,7,4,

        0,3,2,
        2,1,0,

        0,1,5,
        5,4,0,

        3,7,6,
        6,2,3
    };

    std::string facesSkybox[]{
        "Skybox/rainbow_rt.png",
        "Skybox/rainbow_lf.png",
        "Skybox/rainbow_up.png",
        "Skybox/rainbow_dn.png",
        "Skybox/rainbow_ft.png",
        "Skybox/rainbow_bk.png"
    };

    std::vector<GLuint> mesh_indices;
    for (int i = 0; i < shapes[0].mesh.indices.size(); i++) {
        mesh_indices.push_back(
            shapes[0].mesh.indices[i].vertex_index
        );
    }

    std::vector<glm::vec3> tangents;
    std::vector<glm::vec3> bitangents;

    for (int i = 0; i < shapes[0].mesh.indices.size(); i += 3) {
        tinyobj::index_t vData1 = shapes[0].mesh.indices[i];
        tinyobj::index_t vData2 = shapes[0].mesh.indices[i + 1];
        tinyobj::index_t vData3 = shapes[0].mesh.indices[i + 2];

        glm::vec3 v1 = glm::vec3(
            attributes.vertices[vData1.vertex_index * 3],
            attributes.vertices[(vData1.vertex_index * 3) + 1],
            attributes.vertices[(vData1.vertex_index * 3) + 2]
        );

        glm::vec3 v2 = glm::vec3(
            attributes.vertices[vData2.vertex_index * 3],
            attributes.vertices[(vData2.vertex_index * 3) + 1],
            attributes.vertices[(vData2.vertex_index * 3) + 2]
        );

        glm::vec3 v3 = glm::vec3(
            attributes.vertices[vData3.vertex_index * 3],
            attributes.vertices[(vData3.vertex_index * 3) + 1],
            attributes.vertices[(vData3.vertex_index * 3) + 2]
        );

        glm::vec2 uv1 = glm::vec2(
            attributes.texcoords[(vData1.texcoord_index * 2)],
            attributes.texcoords[(vData1.texcoord_index * 2) + 1]
        );

        glm::vec2 uv2 = glm::vec2(
            attributes.texcoords[(vData2.texcoord_index * 2)],
            attributes.texcoords[(vData2.texcoord_index * 2) + 1]
        );

        glm::vec2 uv3 = glm::vec2(
            attributes.texcoords[(vData3.texcoord_index * 2)],
            attributes.texcoords[(vData3.texcoord_index * 2) + 1]
        );

        glm::vec3 deltaPos1 = v2 - v1;
        glm::vec3 deltaPos2 = v3 - v1;

        glm::vec2 deltaUV1 = uv2 - uv1;
        glm::vec2 deltaUV2 = uv3 - uv1;

        float r = 1.0f / ((deltaUV1.x * deltaUV2.y) - (deltaUV1.y * deltaUV2.x));
        glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
        glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

        tangents.push_back(tangent);
        tangents.push_back(tangent);
        tangents.push_back(tangent);

        bitangents.push_back(bitangent);
        bitangents.push_back(bitangent);
        bitangents.push_back(bitangent);
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

        fullVertexData.push_back(
            attributes.texcoords[(vData.texcoord_index * 2)]
        );

        fullVertexData.push_back(
            attributes.texcoords[(vData.texcoord_index * 2) + 1]
        );

        fullVertexData.push_back(
            tangents[i].x
        );

        fullVertexData.push_back(
            tangents[i].y
        );

        fullVertexData.push_back(
            tangents[i].z
        );

        fullVertexData.push_back(
            bitangents[i].x
        );

        fullVertexData.push_back(
            bitangents[i].y
        );

        fullVertexData.push_back(
            bitangents[i].z
        );
    }

    // ------- Skybox Initialization ------- //
    unsigned int skyboxTex;
    glGenTextures(1, &skyboxTex);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    for (unsigned int i = 0; i < 6; i++) {
        int w, h, skyCChannel;
        stbi_set_flip_vertically_on_load(false);
        unsigned char* data = stbi_load(facesSkybox[i].c_str(), &w, &h, &skyCChannel, 0);

        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
    }

    stbi_set_flip_vertically_on_load(true);

    unsigned int skyboxVAO, skyboxVBO, skyboxEBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glGenBuffers(1, &skyboxEBO);

    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 36, &skyboxIndices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);

    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);     // Generates 1 VAO and outputs GLuint
    glGenBuffers(1, &VBO);

    // Tells OpenGL we're working on this VAO
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,
        sizeof(GLfloat) * fullVertexData.size(),
        fullVertexData.data(),
        GL_DYNAMIC_DRAW
    );

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GL_FLOAT), (void*)0);

    GLintptr normalPtr = 3 * sizeof(float);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)normalPtr);

    GLintptr uvPtr = 6 * sizeof(float);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)uvPtr);

    GLintptr tangentPtr = 8 * sizeof(float);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)tangentPtr);

    GLintptr bitangentPtr = 11 * sizeof(float);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)bitangentPtr);

    // Enables Indexes 0 - 4
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);

    // Tells OpenGL we're done w/ VBO and VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glViewport(0, 0, width, height);
    glm::mat4 projection = glm::perspective(
        glm::radians(90.f),
        height / width, 
        0.1f, 
        50.0f 
    );

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Start rendering the Skybox
        glDepthMask(GL_FALSE);
        glDepthFunc(GL_LEQUAL);
        glUseProgram(skyboxShaderProg);

        glm::mat4 viewMatrix = glm::lookAt(cameraPos, center, WorldUp);
        
        // -------- Skybox View -------- //
        glm::mat4 sky_view = glm::mat4(1.f);
        sky_view = glm::mat4(glm::mat3(viewMatrix));

        unsigned int skyboxViewLoc = glGetUniformLocation(skyboxShaderProg, "view");
        glUniformMatrix4fv(skyboxViewLoc, 1, GL_FALSE, glm::value_ptr(sky_view));

        unsigned int skyboxProjLoc = glGetUniformLocation(skyboxShaderProg, "projection");
        glUniformMatrix4fv(skyboxProjLoc, 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);
        
        // ---------------------------- //
        // ------- Model Rendering ------- //
        glUseProgram(shaderProgram);

        glm::mat4 transformation_matrix = glm::translate(identity_matrix, glm::vec3(x, y, z));
        transformation_matrix = glm::scale(transformation_matrix, glm::vec3(scale_x, scale_y, scale_z));
        transformation_matrix = glm::rotate(transformation_matrix, glm::radians(theta_xmod), glm::normalize(glm::vec3(axis_x, 0, axis_z)));     // Rotation w/ Normalized X-Axis
        transformation_matrix = glm::rotate(transformation_matrix, glm::radians(theta_ymod += -0.02f), glm::normalize(glm::vec3(0, axis_y, axis_z)));     // Rotation w/ Normalized Y-Axis
        
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
        glUniform1f(specPhongAddress, specPhong);

        glActiveTexture(GL_TEXTURE0);       // Renders Brickwall
        glBindTexture(GL_TEXTURE_2D, texture);
        GLuint tex0Address = glGetUniformLocation(shaderProgram, "tex0");
        glUniform1i(tex0Address, 0);

        glActiveTexture(GL_TEXTURE1);       // Renders Yae (ultimately not sure as to the order between Yae and Brickwall (Normal))
        glBindTexture(GL_TEXTURE_2D, texture2);
        GLuint tex1Address = glGetUniformLocation(shaderProgram, "tex1");
        glUniform1i(tex1Address, 1);

        glActiveTexture(GL_TEXTURE2);       // Renders Brickwall (Normal)
        glBindTexture(GL_TEXTURE_2D, norm_tex);
        GLuint tex2Loc = glGetUniformLocation(shaderProgram, "norm_tex");
        glUniform1i(tex2Loc, 2);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, fullVertexData.size() / 14);

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
