#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include "headers/cube_data.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include <iostream>


#define STB_IMAGE_IMPLEMENTATION
#include "include/stb_image.h"



// Размер окна
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Обработка изменения размера окна
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// Обработка клавиши ESC
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

unsigned int createShaderProgram()
{
const char* vertexShaderSource =
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aNormal;\n"

    "out vec3 FragPos;\n"
    "out vec3 Normal;\n"

    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"

    "void main()\n"
    "{\n"
    "   FragPos = vec3(model * vec4(aPos, 1.0));\n"
    "   Normal = mat3(transpose(inverse(model))) * aNormal;\n"
    "   gl_Position = projection * view * vec4(FragPos, 1.0);\n"
    "}\0";

const char* fragmentShaderSource =
    "#version 330 core\n"

    "in vec3 FragPos;\n"
    "in vec3 Normal;\n"

    "out vec4 FragColor;\n"

    "uniform vec3 lightPos;\n"
    "uniform vec3 lightColor;\n"
    "uniform vec3 objectColor;\n"
    "uniform vec3 viewPos;\n"
    "void main()\n"
    "{\n"

    "   float ambientStrength = 0.3;\n"
    "   vec3 ambient = ambientStrength * lightColor;\n"

    "   vec3 norm = normalize(Normal);\n"
    "   vec3 lightDir = normalize(lightPos - FragPos);\n"

"   float diff = max(dot(norm, lightDir), 0.0);\n"
"   vec3 diffuse = diff * lightColor;\n"

"   vec3 viewDir = normalize(viewPos - FragPos);\n"
"   vec3 reflectDir = reflect(-lightDir, norm);\n"

"   float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);\n"

"   vec3 specular = 0.85 * spec * lightColor;\n"

"   vec3 result = (ambient + diffuse + specular) * objectColor;\n"

    "   FragColor = vec4(result, 1.0);\n"
    "}\0";

    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

int main()
{

    // Инициализация GLFW
    glfwInit();

    // Версия OpenGL
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    // Core profile
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Создание окна
    GLFWwindow* window = glfwCreateWindow(
        SCR_WIDTH,
        SCR_HEIGHT,
        "Graph4 - Variant 15",
        NULL,
        NULL
    );

    // Проверка окна
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;

        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);


// GLAD 2
int version = gladLoadGL(glfwGetProcAddress);

if (version == 0)
{
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
}

std::cout << "Loaded OpenGL "
          << GLAD_VERSION_MAJOR(version)
          << "."
          << GLAD_VERSION_MINOR(version)
          << std::endl;


          // Viewport
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glEnable(GL_DEPTH_TEST);

    // Callback resize
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    

unsigned int VBO;
unsigned int VAO;

glGenVertexArrays(1, &VAO);
glGenBuffers(1, &VBO);

glBindVertexArray(VAO);

glBindBuffer(GL_ARRAY_BUFFER, VBO);
glBufferData(GL_ARRAY_BUFFER,
             sizeof(vertices),
             vertices,
             GL_STATIC_DRAW);

glVertexAttribPointer(
    0,
    3,
    GL_FLOAT,
    GL_FALSE,
    6 * sizeof(float),
    (void*)0
);

glEnableVertexAttribArray(0);

glVertexAttribPointer(
    1,
    3,
    GL_FLOAT,
    GL_FALSE,
    6 * sizeof(float),
    (void*)(3 * sizeof(float))
);

glEnableVertexAttribArray(1);



unsigned int shaderProgram = createShaderProgram();

unsigned int texture;

glGenTextures(1, &texture);
glBindTexture(GL_TEXTURE_2D, texture);

glTexParameteri(GL_TEXTURE_2D,
                GL_TEXTURE_WRAP_S,
                GL_REPEAT);

glTexParameteri(GL_TEXTURE_2D,
                GL_TEXTURE_WRAP_T,
                GL_REPEAT);

glTexParameteri(GL_TEXTURE_2D,
                GL_TEXTURE_MIN_FILTER,
                GL_LINEAR);

glTexParameteri(GL_TEXTURE_2D,
                GL_TEXTURE_MAG_FILTER,
                GL_LINEAR);

int width, height, nrChannels;

unsigned char* data =
    stbi_load(
        "textures/platinum.jpg",
        &width,
        &height,
        &nrChannels,
        0
    );

if (data)
{
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB,
        width,
        height,
        0,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        data
    );

    glGenerateMipmap(GL_TEXTURE_2D);
}
else
{
    std::cout << "Texture load failed: "
              << stbi_failure_reason()
              << std::endl;
}
stbi_image_free(data);

unsigned int modelLoc =
    glGetUniformLocation(shaderProgram, "model");

unsigned int viewLoc =
    glGetUniformLocation(shaderProgram, "view");

unsigned int projectionLoc =
    glGetUniformLocation(shaderProgram, "projection");

unsigned int lightPosLoc =
    glGetUniformLocation(shaderProgram, "lightPos");

unsigned int lightColorLoc =
    glGetUniformLocation(shaderProgram, "lightColor");

unsigned int objectColorLoc =
    glGetUniformLocation(shaderProgram, "objectColor");
unsigned int viewPosLoc =
    glGetUniformLocation(shaderProgram, "viewPos");
    // Главный цикл
    while (!glfwWindowShouldClose(window))
    
{


    // Ввод
    processInput(window);

    // Цвет очистки
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);

    // Очистка экрана
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

glUseProgram(shaderProgram);

glUniform3f(lightPosLoc, 1.5f, 1.0f, 2.0f);

glUniform3f(viewPosLoc,
            0.0f,
            0.0f,
            3.0f);

glUniform3f(lightColorLoc,
            1.0f,
            1.0f,
            1.0f);

glUniform3f(objectColorLoc,
            0.88f,
            0.88f,
            0.92f);
glm::mat4 model = glm::mat4(1.0f);

model = glm::rotate(
    model,
    (float)glfwGetTime(),
    glm::vec3(0.5f, 1.0f, 0.0f)
);

glm::mat4 view = glm::translate(
    glm::mat4(1.0f),
    glm::vec3(0.0f, 0.0f, -3.0f)
);

glm::mat4 projection = glm::perspective(
    glm::radians(45.0f),
    (float)SCR_WIDTH / (float)SCR_HEIGHT,
    0.1f,
    100.0f
);



glUseProgram(shaderProgram);

glUniformMatrix4fv(
    modelLoc,
    1,
    GL_FALSE,
    glm::value_ptr(model)
);

glUniformMatrix4fv(
    viewLoc,
    1,
    GL_FALSE,
    glm::value_ptr(view)
);

glUniformMatrix4fv(
    projectionLoc,
    1,
    GL_FALSE,
    glm::value_ptr(projection)
);

glBindVertexArray(VAO);

glDrawArrays(GL_TRIANGLES, 0, 36);

glfwSwapBuffers(window);

    // События
    glfwPollEvents();
}
glDeleteVertexArrays(1, &VAO);
glDeleteBuffers(1, &VBO);
glDeleteProgram(shaderProgram);
    // Завершение GLFW
    glfwTerminate();

    return 0;
}