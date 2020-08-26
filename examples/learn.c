#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

static const char* vertex_shader_text =
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"out vec4 vertexColor;\n"
"\n"
"void main()\n"
"{"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"   vertexColor = vec4(aPos.x + 0.5, aPos.y + 0.5, aPos.z + 0.5, 1.0);\n"
"}";
static const char* frag_sharder_text =
"#version 330 core\n"
"out vec4 FragColor;\n"
"in vec4 vertexColor;\n"
"\n"
"void main()\n"
"{"
"    FragColor = vertexColor;\n"
//"    FragColor = vec4(0.3f, 0.0f, 0.0f, 1.0f);"
"}";
static const char* frag_sharder_green_text =
"#version 330 core\n"
"out vec4 FragColor;\n"
"uniform vec4 destColor;\n"
"\n"
"void main()\n"
"{"
//"    FragColor = vec4(0.0f, 3.0f, 0.0f, 1.0f);"
"    FragColor = destColor;"
"}";

static GLuint vertex_shader;
static GLuint frag_sharder;
static GLuint frag_sharder_green;
static GLuint sharder_program;
static GLuint sharder_program_green;

static float vertices[] = {
    -0.5f, -0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
    0.0f, 0.5f, 0.0f,
    1.f, 0.5f, 0.0f,
};
static float vertices2[] = {
    // 位置              // 颜色
     0.0f, 0.f, 0.0f,  1.0f, 0.0f, 0.0f,   // 右下
    -1.f, 0.f, 0.0f,  0.0f, 1.0f, 0.0f,   // 左下
     -0.5f,  1.f, 0.0f,  0.0f, 0.0f, 1.0f    // 顶部
};
static GLuint VBO;
static GLuint VBO2;
static GLuint VAO;
static GLuint VAO2;
static GLuint VAO3;
//static GLuint EBO;

static void error_callback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    printf("frame buffer size:%d, %d\n", width, height);
    glViewport(0, 0, width, height);
}

static void process_input(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

static int compile_shader(const char *source, GLenum sharder_type, unsigned int *dest_shader) {
    GLuint shader = glCreateShader(sharder_type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetShaderInfoLog(shader, sizeof(info_log), NULL, info_log);
        fprintf(stderr, "ERROR::SHADER::VERTEX::COMPILATION_FAILED:%s\n", info_log);
        return 1;
    }
    *dest_shader = shader;
    return 0;
}

static int frame = 0;
static void render(GLFWwindow *window) {
//    glBindBuffer(GL_ARRAY_BUFFER, VBO);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
//    glEnableVertexAttribArray(0);
//    glUseProgram(sharder_program);
//    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//    glBindVertexArray(VAO);
//    glDrawArrays(GL_TRIANGLES, 0, 3);
//    glDrawArrays(GL_TRIANGLES, 1, 3);
//    glBindVertexArray(VAO);
//    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
//    glBindVertexArray(0);
    
    float timeValue = glfwGetTime();
    float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
    int colorLocation = glGetUniformLocation(sharder_program_green, "destColor");
//    int val = frame % 512;
//    if (val >= 256) val = 512 - val;
    
    glUseProgram(sharder_program);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    
    glUseProgram(sharder_program_green);
    glBindVertexArray(VAO2);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(VAO3);
    glDrawArrays(GL_TRIANGLES, 0, 3);
//    float vec[] = {0, greenValue, 0, 1.0f};
//    glUniform1fv(colorLocation, 1, vec);
    glUniform4f(colorLocation, 0.f, greenValue, 0.f, 1.0f);
    glUseProgram(0);
}

int main() {
    glfwSetErrorCallback(error_callback);

    if (!glfwInit()) exit(EXIT_FAILURE);
//    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "Failed to initialize GLAD\n");
        return -2;
    }
    
    if (compile_shader(vertex_shader_text, GL_VERTEX_SHADER, &vertex_shader)) {
        return -3;
    }
    
    if (compile_shader(frag_sharder_text, GL_FRAGMENT_SHADER, &frag_sharder)) {
        return -4;
    }
    
    if (compile_shader(frag_sharder_green_text, GL_FRAGMENT_SHADER, &frag_sharder_green)) {
        return -5;
    }
    
//    int n_attrs;
//    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &n_attrs);
    
    sharder_program = glCreateProgram();
    glAttachShader(sharder_program, vertex_shader);
    glAttachShader(sharder_program, frag_sharder);
    glLinkProgram(sharder_program);
    int success;
    glGetProgramiv(sharder_program, GL_LINK_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetShaderInfoLog(sharder_program, sizeof(info_log), NULL, info_log);
        fprintf(stderr, "link error:%s\n", info_log);
        return -3;
    }
    
    sharder_program_green = glCreateProgram();
    glAttachShader(sharder_program_green, vertex_shader);
    glAttachShader(sharder_program_green, frag_sharder_green);
    glLinkProgram(sharder_program_green);
    glGetProgramiv(sharder_program, GL_LINK_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetShaderInfoLog(sharder_program, sizeof(info_log), NULL, info_log);
        fprintf(stderr, "link error:%s\n", info_log);
        return -3;
    }
    
    glDeleteShader(vertex_shader);
    glDeleteShader(frag_sharder);
    glDeleteShader(frag_sharder_green);
    
    
    unsigned int indices[] = { // 注意索引从0开始!
        0, 1, 2, // 第一个三角形
        1, 2, 3  // 第二个三角形
    };
    
    glGenVertexArrays(1, &VAO);
    glGenVertexArrays(1, &VAO2);
    glGenVertexArrays(1, &VAO3);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &VBO2);
//    glGenBuffers(1, &EBO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    glBindVertexArray(VAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    
//    glBindBuffer(GL_ARRAY_BUFFER, VBO);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glBindVertexArray(VAO2);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(0);
    
    //
    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);
    
    glBindVertexArray(VAO3);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    while(!glfwWindowShouldClose(window)) {
        process_input(window);
        
        glClearColor(0.5f, 0.5f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        render(window);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
        ++frame;
    }

    glfwTerminate();
    return 0;
}
