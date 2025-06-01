#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

static void framebuffer_size_callback(GLFWwindow* window, unsigned int width, unsigned int height) {
    glViewport(0, 0, width, height);
}

static unsigned int compileShader(unsigned int type, const std::string& source) {
    unsigned int shaderId = glCreateShader(type);
    const char* src = source.c_str();

    glShaderSource(shaderId, 1, &src, nullptr);
    glCompileShader(shaderId);

    int result;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int len;
        glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &len);

        char log[len];
        glGetShaderInfoLog(shaderId, len, &len, log);

        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader:\n" << log << std::endl;
        glDeleteShader(shaderId);
        return 0;
    }

    return shaderId;
}

static int createShader(const std::string& vertexShaderSource, const std::string& fragmentShaderSource) {
    unsigned int programId = glCreateProgram();
    unsigned int vertexShader   = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    glAttachShader(programId, vertexShader);
    glAttachShader(programId, fragmentShader);

    glLinkProgram(programId);
    glValidateProgram(programId);
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return programId;
}

int main() {
    /* Initialize the library */
    if (!glfwInit()) {
        return -1;
    }

    /* Create a windowed mode window and its OpenGL context */
    GLFWwindow* window = glfwCreateWindow(800, 600, "Hello World", NULL, NULL);

    if (!window) {
        std::cout << "Window error!" << std::endl;
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    if (glewInit() != GLEW_OK) {
        std::cout << "GLEW error!" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwSetFramebufferSizeCallback(window, (GLFWframebuffersizefun)framebuffer_size_callback);

    std::cout << "Video driver:\n" << glGetString(GL_VERSION) << std::endl;



    std::string vertexShaderSource = "#version 330 core\n"
                                     "layout (location = 0) in vec3 aPos;\n"
                                     "void main() {\n"
                                     "    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                     "}\n";

    std::string fragmentShaderSource = "#version 330 core\n"
                                       "out vec4 FragColor;\n"
                                       "void main() {\n"
                                       "    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
                                       "}\n";

    unsigned int shaderProgram = createShader(vertexShaderSource, fragmentShaderSource);
    glUseProgram(shaderProgram);



    float vertices[9] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
    glEnableVertexAttribArray(0);



    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }



    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}
