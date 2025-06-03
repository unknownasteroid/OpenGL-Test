#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <ctime>
#include <chrono>
#include <random>
#include <climits>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define Call(x) \
    clearErrors();\
    x;\
    logCall(#x, __FILE__, __LINE__);\

std::mt19937 rnd(time(NULL));

float rnd_float(float l, float r) {
    return l + ((float)rnd() / (float)UINT32_MAX) * (r - l);
}

static void clearErrors() {
    while (glGetError() != GL_NO_ERROR);
}

static bool logCall(const char* function, const char* file, int line) {
    while (GLenum error = glGetError()) {
        std::cout << "[OpenGL Error] (" << error << "):\n\tline: " << line << "\n\tfunction: " << function << "\n\tfile: " << file << std::endl;
        return false;
    }
    return true;
}

static std::string loadSource(const std::string& filepath) {
    std::ifstream cin(filepath);
    std::string source;
    std::string line;
    while (getline(cin, line))
        source += line + '\n';
    return source;
}

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

    int result;
    glGetProgramiv(programId, GL_VALIDATE_STATUS, &result);
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    if (result == GL_FALSE) {
        int len;
        glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &len);

        char log[len];
        glGetProgramInfoLog(programId, len, &len, log);

        std::cout << "Failed to create shader program:\n" << log << std::endl;
        glDeleteProgram(programId);
        return 0;
    }
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
    glfwSwapInterval(0);
    if (glewInit() != GLEW_OK) {
        std::cout << "GLEW error!" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwSetFramebufferSizeCallback(window, (GLFWframebuffersizefun)framebuffer_size_callback);

    std::cout << "Video driver:\n" << glGetString(GL_VERSION) << std::endl;



    unsigned int shaderProgram = createShader(loadSource("res/shaders/basic_vertex.shader"), loadSource("res/shaders/basic_fragment.shader"));
    glUseProgram(shaderProgram);

    int u_Color_id = glGetUniformLocation(shaderProgram, "u_Color");
    glUniform4f(u_Color_id, 1.0f, 1.0f, 1.0f, 1.0f);



    int n; std::cin >> n;

    int nV = n * 6;
    float* vertices = (float*)malloc(sizeof(float) * nV);
    for (int i = 0; i < nV; ++i) {
        vertices[i] = rnd_float(-1.0f, 1.0f);
        // std::cout << vertices[i] << std::endl;
    }

    int nCol = n * 4;
    float* colors = (float*)malloc(sizeof(float) * nCol);
    for (int i = 0; i < nCol; i += 4) {
        colors[i] = rnd_float(0.0f, 1.0f);
        colors[i + 1] = rnd_float(0.0f, 1.0f);
        colors[i + 2] = rnd_float(0.0f, 1.0f);
        colors[i + 3] = 1.0f;
    }


    unsigned int VBO[2];
    glGenBuffers(2, VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * nV, vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * nCol, colors, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4, 0);
    glEnableVertexAttribArray(1);

    free(vertices);
    free(colors);



    /* Loop until the user closes the window */
    float prevt = glfwGetTime();
    float frames = 0.0f;
    while (!glfwWindowShouldClose(window)) {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, n / 2);        

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
        
        ++frames;
        float curt = glfwGetTime();
        if (curt - prevt >= 1.0) {
            std::cout << "FPS: " << frames / (curt - prevt) << std::endl;
            frames = 0.0;
            prevt = curt;
        }
    }



    glDeleteBuffers(2, VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}
