#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

// Dimensiones de la ventana
const GLuint WIDTH = 1280, HEIGHT = 720;


// Ruta de la carpeta de texturas del skybox
const std::string skyboxPath = "/home/enigma/repos/skybox-test/images/";

// Coordenadas de los vértices del skybox
GLfloat skyboxVertices[] = {
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
    1.0f,  1.0f, -1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
    1.0f, -1.0f,  1.0f
};

// Compilar y enlazar los shaders
void compileShaders(GLuint& shaderProgram)
{
    // Vertex Shader
    const char* vertexShaderSource =
        "#version 330 core\n"
        "layout (location = 0) in vec3 position;\n"
        "out vec3 texCoord;\n"
        "uniform mat4 projection;\n"
        "uniform mat4 view;\n"
        "void main()\n"
        "{\n"
        "    gl_Position = projection * view * vec4(position, 1.0f);\n"
        "    texCoord = position;\n"
        "}\0";

    // Fragment Shader
    const char* fragmentShaderSource =
        "#version 330 core\n"
        "in vec3 texCoord;\n"
        "out vec4 fragColor;\n"
        "uniform samplerCube skybox;\n"
        "void main()\n"
        "{\n"
        "    fragColor = texture(skybox, texCoord);\n"
        "}\n\0";

    // Compilar shaders
    GLuint vertexShader, fragmentShader;
    GLint success;
    GLchar infoLog[512];

    // Vertex Shader
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Comprobar errores de compilación del Vertex Shader
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "Error al compilar el Vertex Shader:\n" << infoLog << std::endl;
    }

    // Fragment Shader
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Comprobar errores de compilación del Fragment Shader
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "Error al compilar el Fragment Shader:\n" << infoLog << std::endl;
    }

    // Programa de shaders
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Comprobar errores de enlace del programa de shaders
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "Error al enlazar el programa de shaders:\n" << infoLog << std::endl;
    }

    // Eliminar shaders (ya no son necesarios después del enlace)
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

// Mover la cámara según las entradas del teclado
    void moveCamera(GLFWwindow* window, glm::mat4& view)
        {
        float cameraSpeed = 0.05f;
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            view = glm::translate(view, cameraSpeed * glm::vec3(0.0f, 0.0f, -1.0f));
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            view = glm::translate(view, cameraSpeed * glm::vec3(0.0f, 0.0f, 1.0f));
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            view = glm::translate(view, cameraSpeed * glm::vec3(-1.0f, 0.0f, 0.0f));
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            view = glm::translate(view, cameraSpeed * glm::vec3(1.0f, 0.0f, 0.0f));
        }



int main()
{
    // Inicializar GLFW
    if (!glfwInit())
    {
        std::cout << "Error al inicializar GLFW" << std::endl;
        return -1;
    }

    // Configurar opciones de GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Crear una ventana GLFW
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Skybox", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Error al crear la ventana GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Inicializar GLEW
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Error al inicializar GLEW" << std::endl;
        return -1;
    }

    // Configurar viewport
    glViewport(0, 0, WIDTH, HEIGHT);

    // Compilar y enlazar los shaders
    GLuint shaderProgram;
    compileShaders(shaderProgram);

    // Cargar texturas del skybox
    std::string skyboxTextures[] = {
    "right.jpg", "left.jpg", "top.jpg", "bottom.jpg", "front.jpg", "back.jpg"
    };

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, numChannels;
    stbi_set_flip_vertically_on_load(false);

    for (unsigned int i = 0; i < 6; i++)
    {
        std::string imagePath = skyboxPath + skyboxTextures[i];
        unsigned char* data = stbi_load(imagePath.c_str(), &width, &height, &numChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Error al cargar la textura: " << imagePath << std::endl;
            stbi_image_free(data);
            return -1;
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // Crear y configurar los VAO y VBO
    GLuint skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glBindVertexArray(0);

    // Habilitar prueba de profundidad
    glEnable(GL_DEPTH_TEST);

    // Bucle principal de renderizado
    while (!glfwWindowShouldClose(window))
    {
        // Comprobar si se han activado eventos de teclado/mouse
        glfwPollEvents();

        // Limpiar el buffer de color y profundidad
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Obtener la matriz de proyección
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);

        // Obtener la matriz de vista
        glm::mat4 view;
        view = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        // Usar el programa de shaders
        glUseProgram(shaderProgram);

        // Enviar matrices de proyección y vista al shader
        GLint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        // Dibujar el skybox
        glBindVertexArray(skyboxVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        
        // Intercambiar los buffers de la ventana
        glfwSwapBuffers(window);
    }

    // Limpiar recursos
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &skyboxVBO);
    glDeleteTextures(1, &textureID);
    glDeleteProgram(shaderProgram);

    // Terminar GLFW
    glfwTerminate();

    return 0;
}
