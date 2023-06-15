#include "glfwsetup.h"

float height = 600.f;
float width = 600.f;
GLuint VAO, VBO, EBO, VBO_UV;

glm::mat4 identity_matrix = glm::mat4(1.0f);

model3D::model3D(std::string filePath) {
    this->x = x;
    this->y = y;
    this->z = z;

    this->scale_x = this->scale_y = this->scale_z = 1.0f;
    this->axis_x = this->axis_y = 1.f;
    this->axis_z = 0.f;

    loadModel(filePath);
}

model3D::~model3D() {
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
}

void model3D::loadModel(std::string& filePath) {
    std::ifstream fObj(filePath);
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
        filePath.c_str()
    );

    if (!fObj.is_open()) {
        std::cout << "Failed to open file: " << filePath << std::endl;
        return;
    }

    std::string line;

    while (std::getline(fObj, line)) {
        std::istringstream sStream(line);
        std::string sDataType;
        sStream >> sDataType;

        if (sDataType == "v") {
            // Processes vertex position data
            float x, y, z;
            sStream >> x >> y >> z;
            this->vertices.push_back(x);
            this->vertices.push_back(y);
            this->vertices.push_back(z);
        }
        else if (sDataType == "vn") {
            // Processes vertex normal data
            float nx, ny, nz;
            sStream >> nx >> ny >> nz;
            this->normals.push_back(nx);
            this->normals.push_back(ny);
            this->normals.push_back(nz);
        }
        else if (sDataType == "vt") {
            // Processes texture coordinate data
            float u, v;
            sStream >> u >> v;
            this->texCoords.push_back(u);
            this->texCoords.push_back(v);
        }
        else if (sDataType == "f") {
            // Processes face indices data
            unsigned int v1, v2, v3;
            unsigned int vn1, vn2, vn3;
            unsigned int vt1, vt2, vt3;
            char slash;
            sStream >> v1 >> slash >> vt1 >> slash >> vn1
                    >> v2 >> slash >> vt2 >> slash >> vn2
                    >> v3 >> slash >> vt3 >> slash >> vn3;
            indices.push_back(v1 - 1);
            indices.push_back(v2 - 1);
            indices.push_back(v3 - 1);
        }
    }

    fObj.close();

    std::vector<GLuint> mesh_indices;
    for (int i = 0; i < shapes[0].mesh.indices.size(); i++) {
        mesh_indices.push_back(
            shapes[0].mesh.indices[i].vertex_index
        );
    }

    glGenVertexArrays(1, &this->VAO);     // Generates 1 VAO and outputs GLuint
    glGenBuffers(1, &this->VBO);
    glGenBuffers(1, &this->VBO_UV);
    glGenBuffers(1, &this->EBO);

    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

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

    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        sizeof(GLuint) * mesh_indices.size(),
        mesh_indices.data(),
        GL_STATIC_DRAW
    );

    glBindBuffer(GL_ARRAY_BUFFER, VBO_UV);      // Bind the UV buffer
    glBufferData(GL_ARRAY_BUFFER,
        sizeof(GLfloat) * (sizeof(UV) / sizeof(UV[0])),     // Float * size of the UV array
        &UV[0],             // The UV array earlier
        GL_DYNAMIC_DRAW);

    // Add in how to interpret the array
    glVertexAttribPointer(
        2,                      // 2 for UV or tex coords
        2,                      // UV
        GL_FLOAT,               // Type of Array
        GL_FALSE,
        2 * sizeof(float),      // Every 2 index
        (void*)0
    );

    // Enable 2 for our UV / Tex coords
    glEnableVertexAttribArray(2);

    // Tells OpenGL we're done w/ VBO and VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


}

void createShaderProgram() {

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
}

/*
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
*/

glm::mat4 getModelMatrix() {

}



int main(void) 
{
    model3D myModel("3D/myCube.obj");
    GLFWwindow* window;
    std::vector<glm::mat4> modelMatrices;

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
    // Set the current texture were working on to Texture 0
    glActiveTexture(GL_TEXTURE0);
    // Bind our next tasks to Tex0 to our current reference similar to what we're doing to VBOs
    glBindTexture(GL_TEXTURE_2D, texture);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

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

    //glfwSetKeyCallback(window, Key_Callback);

    createShaderProgram();
    

    // ------------------------------------------- //

    std::string path = "3D/myCube.obj";
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
        0.f, 1.f,
        0.f, 0.f,
        1.f, 1.f,
        1.f, 0.f,
        1.f, 1.f,
        1.f, 0.f,
        0.f, 1.f,
        0.f, 0.f
    };

    glViewport(0, 0, width, height);
    glm::mat4 projection = glm::perspective(
        glm::radians(60.f), // FOV
        height / width,   // Aspect Ratio
        0.1f,   // Near
        50.0f   // Far
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

        glm::mat4 transformation_matrix = glm::translate(identity_matrix, glm::vec3(x , y , z));
        transformation_matrix = glm::scale(transformation_matrix, glm::vec3(scale_x, scale_y, scale_z));
        // transformation_matrix = glm::rotate(transformation_matrix, glm::radians(theta_xmod += y_mod), glm::normalize(glm::vec3(axis_x, 0, axis_z)));     // Rotation w/ Normalized X-Axis
        // transformation_matrix = glm::rotate(transformation_matrix, glm::radians(theta_ymod += x_mod), glm::normalize(glm::vec3(0, axis_y, axis_z)));     // Rotation w/ Normalized Y-Axis
       
        glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, -5.0f);    // Top View

        glm::vec3 WorldUp = glm::vec3(0, 1.0f, 0);
        glm::vec3 center = glm::vec3(0, 1.0f, 0);

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
        glDrawElements(
            GL_TRIANGLES,
            mesh_indices.size(),
            GL_UNSIGNED_INT,
            0
        );
        // ---------------------------- //
        
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }
    myModel.~model3D();

    glfwTerminate();
    return 0;
}