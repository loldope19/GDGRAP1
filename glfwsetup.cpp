#include <GLFW/glfw3.h>
#include <math.h>

int main(void) 
{
    float intAngle = 72;
    intAngle *= 3.14167 / 180.0f;
    float angle;
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(480, 480, "Sydrenz Cao", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);


        glBegin(GL_TRIANGLE_FAN);
        angle = 0.0f;
        for (int i = 0; i < 5; i++) {
            glVertex3f(0.5f * cos(angle), 0.5 * sin(angle), 0.f);
            angle += intAngle;
        }
        glEnd();


        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}