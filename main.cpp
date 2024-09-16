    #define TINYOBJLOADER_IMPLEMENTATION
    #include "tiny_obj_loader.h"
    #include <glad/glad.h>
    #include <GLFW/glfw3.h>
    #include <stb/stb_image.h>
    #include <iostream>
    #include <glm/glm.hpp>
    #include <glm/gtc/matrix_transform.hpp>
    #include <glm/gtc/type_ptr.hpp>
    #include <vector>

    // Rotation angles
    float leftArmRotationAngle = 0.0f;
    float rightArmRotationAngle = 0.0f;
    float lowerLeftArmRotationAngle = 0.0f;
    float lowerRightArmRotationAngle = 0.0f;

    float leftLegRotationAngle = 0.0f;
    float lowerLeftLegRotationAngle = 0.0f;
    float rightLegRotationAngle = 0.0f;
    float lowerRightLegRotationAngle = 0.0f;

    float torsoRotationAngle = 0.0f;
    float headRotationAngle = 0.0f;

    int animationPlay = 0;
    int resetPlay = 0;

    int playAnimationFlag = 0;
    int resetAnimationFlag = 0;

    // Shader source code
    const char* vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec2 aTexCoord;
        out vec2 TexCoord;
        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;
        void main() {
            gl_Position = projection * view * model * vec4(aPos, 1.0);
            TexCoord = aTexCoord;
        }
    )";

    const char* fragmentShaderSource = R"(
        #version 330 core
        out vec4 FragColor;
        in vec2 TexCoord;
        uniform sampler2D texture1;
        void main() {
            FragColor = texture(texture1, TexCoord);
        }
    )";

    const char* vertexShaderSource2 = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;
        void main() {
            gl_Position = projection * view * model * vec4(aPos, 1.0);
        }
    )";

    const char* fragmentShaderSource2 = R"(
        #version 330 core
        out vec4 FragColor;
        void main() {
            FragColor = vec4(1.0, 0.5, 0.5, 1.0); // Different color (light red)
        }
    )";

    // Camera parameters
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    float cameraSpeed = 0.05f; // Movement speed

    // Mouse parameters
    bool firstMouse = true;
    float lastX = 400, lastY = 300;
    float yaw = -90.0f, pitch = 0.0f;

    void updateAnimations() {
        if (glfwGetTime() > 0.05f && animationPlay <= 20) {
            std::cout << animationPlay << std::endl;
            leftLegRotationAngle += 1.0f;
            lowerLeftLegRotationAngle += 0.15f;
            rightLegRotationAngle -= 1.0f;
            lowerRightLegRotationAngle -= 0.15f;

            leftArmRotationAngle += 1.0f;
            rightArmRotationAngle -= 1.0f;

            lowerLeftArmRotationAngle += 1.0f;
            lowerRightArmRotationAngle -= 1.0f;

            torsoRotationAngle += 0.02f;
            headRotationAngle += 0.4f;

            glfwSetTime(0.0f);
            animationPlay += 1;
        }
        else {
            resetPlay = 0;
        }
    }

    void resetAnimations() {
        if (glfwGetTime() > 0.05f && resetPlay <= 20) {
            std::cout << resetPlay << std::endl;

            // Reverse the direction of all rotations
            leftLegRotationAngle -= 1.0f;
            lowerLeftLegRotationAngle -= 0.15f;
            rightLegRotationAngle += 1.0f;
            lowerRightLegRotationAngle += 0.15f;

            leftArmRotationAngle -= 1.0f;
            rightArmRotationAngle += 1.0f;

            lowerLeftArmRotationAngle -= 1.0f;
            lowerRightArmRotationAngle += 1.0f;

            torsoRotationAngle -= 0.02f;
            headRotationAngle -= 0.4f;

            glfwSetTime(0.0f);
            resetPlay += 1;
        }
        else {
            animationPlay = 0;
        }
    }

    // Callback function for mouse movement
    void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
        if (firstMouse) {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos;
        lastX = xpos;
        lastY = ypos;

        float sensitivity = 0.1f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw += xoffset;
        pitch += yoffset;

        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        glm::vec3 direction;
        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFront = glm::normalize(direction);
    }

    // Callback function for window resize
    void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
    }

    void processInput(GLFWwindow* window) {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            cameraPos += cameraSpeed * cameraFront;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            cameraPos -= cameraSpeed * cameraFront;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
            resetAnimationFlag = 0;
            playAnimationFlag = 1;
        }

        if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
            resetAnimationFlag = 1;
            playAnimationFlag = 0;
        }
    }

    // Vertex structure including texture coordinates
    struct Vertex {
        glm::vec3 position;
        glm::vec2 texCoord;
    };

    // Function to create shaders
    unsigned int createShader(const char* vertexSource, const char* fragmentSource) {
        unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexSource, NULL);
        glCompileShader(vertexShader);

        unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
        glCompileShader(fragmentShader);

        unsigned int shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        return shaderProgram;
    }

    // Function to create the new shaders
    unsigned int createShader2(const char* vertexSource, const char* fragmentSource) {
        unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexSource, NULL);
        glCompileShader(vertexShader);

        unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
        glCompileShader(fragmentShader);

        unsigned int shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        return shaderProgram;
    }

    // Function to load OBJ files
    void loadOBJ(const char* filename, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices) {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string err;

        bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename);
        if (!err.empty()) {
            std::cerr << err << std::endl;
        }
        if (!ret) {
            std::cerr << "Failed to load .obj file" << std::endl;
            exit(-1);
        }

        for (const auto& shape : shapes) {
            for (const auto& index : shape.mesh.indices) {
                Vertex vertex;
                vertex.position = glm::vec3(
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]
                );

                if (!attrib.texcoords.empty()) {
                    vertex.texCoord = glm::vec2(
                        attrib.texcoords[2 * index.texcoord_index + 0],
                        1.0f - attrib.texcoords[2 * index.texcoord_index + 1]  // Flip V coordinate
                    );
                }
                else {
                    vertex.texCoord = glm::vec2(0.0f, 0.0f);
                }

                vertices.push_back(vertex);
                indices.push_back(indices.size());
            }
        }
    }

    // Function to set up mesh data
    void setupMesh(unsigned int& VAO, unsigned int& VBO, unsigned int& EBO, const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    int main() {
        // Initialize GLFW
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        // Create window
        GLFWwindow* window = glfwCreateWindow(800, 600, "OBJ Loader", NULL, NULL);
        if (window == NULL) {
            std::cerr << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return -1;
        }
        glfwMakeContextCurrent(window);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        glfwSetCursorPosCallback(window, mouse_callback);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Hide cursor

        // Load OpenGL functions with GLAD
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cerr << "Failed to initialize GLAD" << std::endl;
            return -1;
        }

        glEnable(GL_DEPTH_TEST);

        // Load the texture using stb_image
        int width, height, nrChannels;
        unsigned char* data = stbi_load("textures/block.jpeg", &width, &height, &nrChannels, 0);
        if (!data) {
            std::cerr << "Failed to load texture" << std::endl;
            return -1;
        }
        else {
            std::cout << "Loaded" << std::endl;
        }

        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Load OBJ files
        std::vector<Vertex> headVertices, torsoVertices, l1ArmUpperVertices, l1ArmLowerVertices;
        std::vector<Vertex> r1ArmUpperVertices, r1ArmLowerVertices, l1LegUpperVertices, l1LegLowerVertices;
        std::vector<Vertex> r1LegUpperVertices, r1LegLowerVertices;
        std::vector<unsigned int> headIndices, torsoIndices, l1ArmUpperIndices, l1ArmLowerIndices;
        std::vector<unsigned int> r1ArmUpperIndices, r1ArmLowerIndices, l1LegUpperIndices, l1LegLowerIndices;
        std::vector<unsigned int> r1LegUpperIndices, r1LegLowerIndices;

        //Arms
        std::vector<Vertex> extraSphereVertices;
        std::vector<unsigned int> extraSphereIndices;

        std::vector<Vertex> extraSphereVertices2;
        std::vector<unsigned int> extraSphereIndices2;

        //Lower legs
        std::vector<Vertex> extraSphereVertices3;
        std::vector<unsigned int> extraSphereIndices3;

        std::vector<Vertex> extraSphereVertices4;
        std::vector<unsigned int> extraSphereIndices4;

        //Upper legs
        std::vector<Vertex> extraSphereVertices5;
        std::vector<unsigned int> extraSphereIndices5;

        std::vector<Vertex> extraSphereVertices6;
        std::vector<unsigned int> extraSphereIndices6;

        //head
        std::vector<Vertex> extraSphereVertices7;
        std::vector<unsigned int> extraSphereIndices7;

        loadOBJ("appendix/Sphere.obj", headVertices, headIndices);
        loadOBJ("appendix/Cube.obj", torsoVertices, torsoIndices);
        loadOBJ("appendix/Cylinder.obj", l1ArmUpperVertices, l1ArmUpperIndices);
        loadOBJ("appendix/Cylinder.obj", l1ArmLowerVertices, l1ArmLowerIndices);
        loadOBJ("appendix/Cylinder.obj", r1ArmUpperVertices, r1ArmUpperIndices);
        loadOBJ("appendix/Cylinder.obj", r1ArmLowerVertices, r1ArmLowerIndices);
        loadOBJ("appendix/Cylinder.obj", l1LegUpperVertices, l1LegUpperIndices);
        loadOBJ("appendix/Cylinder.obj", l1LegLowerVertices, l1LegLowerIndices);
        loadOBJ("appendix/Cylinder.obj", r1LegUpperVertices, r1LegUpperIndices);
        loadOBJ("appendix/Cylinder.obj", r1LegLowerVertices, r1LegLowerIndices);
        loadOBJ("appendix/Sphere.obj", extraSphereVertices, extraSphereIndices);
        loadOBJ("appendix/Sphere.obj", extraSphereVertices2, extraSphereIndices2);
        loadOBJ("appendix/Sphere.obj", extraSphereVertices3, extraSphereIndices3);
        loadOBJ("appendix/Sphere.obj", extraSphereVertices4, extraSphereIndices4);
        loadOBJ("appendix/Sphere.obj", extraSphereVertices5, extraSphereIndices5);
        loadOBJ("appendix/Sphere.obj", extraSphereVertices6, extraSphereIndices6);
        loadOBJ("appendix/Sphere.obj", extraSphereVertices7, extraSphereIndices7);

        unsigned int headVAO, headVBO, headEBO, torsoVAO, torsoVBO, torsoEBO;
        unsigned int l1ArmUpperVAO, l1ArmUpperVBO, l1ArmUpperEBO, l1ArmLowerVAO, l1ArmLowerVBO, l1ArmLowerEBO;
        unsigned int r1ArmUpperVAO, r1ArmUpperVBO, r1ArmUpperEBO, r1ArmLowerVAO, r1ArmLowerVBO, r1ArmLowerEBO;
        unsigned int l1LegUpperVAO, l1LegUpperVBO, l1LegUpperEBO, l1LegLowerVAO, l1LegLowerVBO, l1LegLowerEBO;
        unsigned int r1LegUpperVAO, r1LegUpperVBO, r1LegUpperEBO, r1LegLowerVAO, r1LegLowerVBO, r1LegLowerEBO;

        setupMesh(headVAO, headVBO, headEBO, headVertices, headIndices);
        setupMesh(torsoVAO, torsoVBO, torsoEBO, torsoVertices, torsoIndices);
        setupMesh(l1ArmUpperVAO, l1ArmUpperVBO, l1ArmUpperEBO, l1ArmUpperVertices, l1ArmUpperIndices);
        setupMesh(l1ArmLowerVAO, l1ArmLowerVBO, l1ArmLowerEBO, l1ArmLowerVertices, l1ArmLowerIndices);
        setupMesh(r1ArmUpperVAO, r1ArmUpperVBO, r1ArmUpperEBO, r1ArmUpperVertices, r1ArmUpperIndices);
        setupMesh(r1ArmLowerVAO, r1ArmLowerVBO, r1ArmLowerEBO, r1ArmLowerVertices, r1ArmLowerIndices);
        setupMesh(l1LegUpperVAO, l1LegUpperVBO, l1LegUpperEBO, l1LegUpperVertices, l1LegUpperIndices);
        setupMesh(l1LegLowerVAO, l1LegLowerVBO, l1LegLowerEBO, l1LegLowerVertices, l1LegLowerIndices);
        setupMesh(r1LegUpperVAO, r1LegUpperVBO, r1LegUpperEBO, r1LegUpperVertices, r1LegUpperIndices);
        setupMesh(r1LegLowerVAO, r1LegLowerVBO, r1LegLowerEBO, r1LegLowerVertices, r1LegLowerIndices);
        unsigned int extraSphereVAO, extraSphereVBO, extraSphereEBO;
        setupMesh(extraSphereVAO, extraSphereVBO, extraSphereEBO, extraSphereVertices, extraSphereIndices);

        unsigned int extraSphereVAO2, extraSphereVBO2, extraSphereEBO2;
        setupMesh(extraSphereVAO2, extraSphereVBO2, extraSphereEBO2, extraSphereVertices2, extraSphereIndices2);

        unsigned int extraSphereVAO3, extraSphereVBO3, extraSphereEBO3;
        setupMesh(extraSphereVAO3, extraSphereVBO3, extraSphereEBO3, extraSphereVertices3, extraSphereIndices3);

        unsigned int extraSphereVAO4, extraSphereVBO4, extraSphereEBO4;
        setupMesh(extraSphereVAO4, extraSphereVBO4, extraSphereEBO4, extraSphereVertices4, extraSphereIndices4);

        unsigned int extraSphereVAO5, extraSphereVBO5, extraSphereEBO5;
        setupMesh(extraSphereVAO5, extraSphereVBO5, extraSphereEBO5, extraSphereVertices5, extraSphereIndices5);

        unsigned int extraSphereVAO6, extraSphereVBO6, extraSphereEBO6;
        setupMesh(extraSphereVAO6, extraSphereVBO6, extraSphereEBO6, extraSphereVertices6, extraSphereIndices6);

        unsigned int extraSphereVAO7, extraSphereVBO7, extraSphereEBO7;
        setupMesh(extraSphereVAO7, extraSphereVBO6, extraSphereEBO6, extraSphereVertices7, extraSphereIndices7);

        unsigned int shaderProgram = createShader(vertexShaderSource, fragmentShaderSource);
        unsigned int shaderProgram2 = createShader2(vertexShaderSource2, fragmentShaderSource2);

        // Inside your rendering loop
        while (!glfwWindowShouldClose(window)) {
            processInput(window);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Draw torso with the first shader program
            glUseProgram(shaderProgram);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture);

            glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
            glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

            unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
            unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");

            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

            // Draw torso
            glBindVertexArray(torsoVAO);
            glm::mat4 torsoModel = glm::mat4(1.0f);
            torsoModel = glm::translate(torsoModel, glm::vec3(0.0f, 0.0f, 0.0f));
            torsoModel = glm::rotate(torsoModel, torsoRotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
            torsoModel = glm::scale(torsoModel, glm::vec3(1.0f, 2.0f, 1.0f));
            unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(torsoModel));
            glDrawElements(GL_TRIANGLES, torsoIndices.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);

            // Continue drawing other parts of the model as before
            // ...

            // Draw the upper left arm with the second shader program
            glUseProgram(shaderProgram2);

            // Make sure to update the uniform variables with the new shader program
            viewLoc = glGetUniformLocation(shaderProgram2, "view");
            projectionLoc = glGetUniformLocation(shaderProgram2, "projection");
            modelLoc = glGetUniformLocation(shaderProgram2, "model");

            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));


            // Upper left arm pivot
            glUseProgram(shaderProgram2);
            glBindVertexArray(l1ArmUpperVAO);
            glm::mat4 leftArmUpper = glm::mat4(1.0f);
            leftArmUpper = glm::translate(torsoModel, glm::vec3(-0.75f, 0.3f, 0.0f));
            leftArmUpper = glm::rotate(leftArmUpper, glm::radians(leftArmRotationAngle), glm::vec3(1.0f, 0.0f, 0.0f));
            leftArmUpper = glm::scale(leftArmUpper, glm::vec3(0.5f, 0.25f, 0.5f));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(leftArmUpper));
            glDrawElements(GL_TRIANGLES, l1ArmUpperIndices.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);



            //Lower left arm pivot
            glUseProgram(shaderProgram); // Or use any shader program as needed

            glBindVertexArray(extraSphereVAO);
            glm::mat4 extraSphereModel = glm::mat4(1.0f);
            extraSphereModel = glm::translate(leftArmUpper, glm::vec3(0.0f, -1.0f, 0.0f)); // Position the sphere
            extraSphereModel = glm::rotate(extraSphereModel, glm::radians(lowerLeftArmRotationAngle), glm::vec3(1.0f, 0.0f, 0.0f));
            extraSphereModel = glm::scale(extraSphereModel, glm::vec3(0.5f)); // Scale the sphere
            unsigned int sphere1 = glGetUniformLocation(shaderProgram2, "model");
            glUniformMatrix4fv(sphere1, 1, GL_FALSE, glm::value_ptr(extraSphereModel));
            glDrawElements(GL_TRIANGLES, extraSphereIndices.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);



            // Draw the lower left arm with the first shader program
            glUseProgram(shaderProgram2);
            glBindVertexArray(l1ArmLowerVAO);
            glm::mat4 leftArmLower = glm::mat4(1.0f);
            leftArmLower = glm::translate(extraSphereModel, glm::vec3(0.0f, -2.5f, 0.0f)); // Pivot relative to upper arm
            leftArmLower = glm::scale(leftArmLower, glm::vec3(0.6f, 2.0f, 0.4f));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(leftArmLower));
            glDrawElements(GL_TRIANGLES, l1ArmLowerIndices.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);




            // Draw the upper right arm with the second shader program
            glUseProgram(shaderProgram2);

            glBindVertexArray(r1ArmUpperVAO);
            glm::mat4 rightArmUpper = glm::mat4(1.0f);
            rightArmUpper = glm::translate(torsoModel, glm::vec3(0.75f, 0.3f, 0.0f));
            rightArmUpper = glm::rotate(rightArmUpper, glm::radians(rightArmRotationAngle), glm::vec3(1.0f, 0.0f, 0.0f));
            rightArmUpper = glm::scale(rightArmUpper, glm::vec3(0.5f, 0.25f, 0.5f));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(rightArmUpper));
            glDrawElements(GL_TRIANGLES, r1ArmUpperIndices.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);

            //Create the pivot for right arm
            glUseProgram(shaderProgram); // Or use any shader program as needed

            glBindVertexArray(extraSphereVAO2);
            glm::mat4 extraSphereModel2 = glm::mat4(1.0f);
            extraSphereModel2 = glm::translate(rightArmUpper, glm::vec3(0.0f, -1.0f, 0.0f)); // Position the sphere
            extraSphereModel2 = glm::rotate(extraSphereModel2, glm::radians(lowerRightArmRotationAngle), glm::vec3(1.0f, 0.0f, 0.0f));
            extraSphereModel2 = glm::scale(extraSphereModel2, glm::vec3(0.5f)); // Scale the sphere
            unsigned int sphere2 = glGetUniformLocation(shaderProgram2, "model");
            glUniformMatrix4fv(sphere2, 1, GL_FALSE, glm::value_ptr(extraSphereModel2));
            glDrawElements(GL_TRIANGLES, extraSphereIndices2.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);


            // Draw the lower right arm with the first shader program
            glUseProgram(shaderProgram2);

            glBindVertexArray(r1ArmLowerVAO);
            glm::mat4 rightArmLower = glm::mat4(1.0f);
            rightArmLower = glm::translate(extraSphereModel2, glm::vec3(0.0f, -2.5f, 0.0f)); // Adjusted to pivot relative to upper arm
            rightArmLower = glm::scale(rightArmLower, glm::vec3(0.6f, 2.0f, 0.4f));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(rightArmLower));
            glDrawElements(GL_TRIANGLES, r1ArmLowerIndices.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);




            //Draw pivot for the head
            glUseProgram(shaderProgram); // Or use any shader program as needed

            glBindVertexArray(extraSphereVAO7);
            glm::mat4 extraSphereModel7 = glm::mat4(1.0f);
            extraSphereModel7 = glm::translate(torsoModel, glm::vec3(0.0f, 0.5f, 0.0f)); // Position the sphere
            extraSphereModel7 = glm::rotate(extraSphereModel7, glm::radians(headRotationAngle), glm::vec3(1.0f, 0.0f, 0.0f));
            extraSphereModel7 = glm::scale(extraSphereModel7, glm::vec3(0.25f)); // Scale the sphere
            unsigned int sphere7 = glGetUniformLocation(shaderProgram2, "model");
            glUniformMatrix4fv(sphere7, 1, GL_FALSE, glm::value_ptr(extraSphereModel7));
            glDrawElements(GL_TRIANGLES, extraSphereIndices7.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);


            // Draw the head
            glUseProgram(shaderProgram2);
            glBindVertexArray(headVAO);
            glm::mat4 headModel = glm::mat4(1.0f);
            headModel = glm::translate(extraSphereModel7, glm::vec3(0.0f, 1.25f, 0.0f)); // Position head above torso
            headModel = glm::scale(headModel, glm::vec3(2.0f, 2.0f, 2.0f)); // Scale head to the desired size
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(headModel));
            glDrawElements(GL_TRIANGLES, headIndices.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);


            //Pivot for upper left leg
            glUseProgram(shaderProgram2); // Or use any shader program as needed

            glBindVertexArray(extraSphereVAO5);
            glm::mat4 extraSphereModel5 = glm::mat4(1.0f);
            extraSphereModel5 = glm::translate(torsoModel, glm::vec3(-0.25f, -0.5f, 0.0f)); // Position the sphere
            extraSphereModel5 = glm::rotate(extraSphereModel5, glm::radians(leftLegRotationAngle), glm::vec3(1.0f, 0.0f, 0.0f));
            extraSphereModel5 = glm::scale(extraSphereModel5, glm::vec3(0.25f)); // Scale the sphere
            unsigned int sphere5 = glGetUniformLocation(shaderProgram2, "model");
            glUniformMatrix4fv(sphere5, 1, GL_FALSE, glm::value_ptr(extraSphereModel5));
            glDrawElements(GL_TRIANGLES, extraSphereIndices5.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);




            // Draw the upper left leg with the first shader program
            glUseProgram(shaderProgram2);

            glBindVertexArray(l1LegUpperVAO);
            glm::mat4 leftLegUpperModel = glm::mat4(1.0f);
            leftLegUpperModel = glm::translate(extraSphereModel5, glm::vec3(0.0f, -1.5f, 0.0f)); // Position the leg below the torso
            leftLegUpperModel = glm::scale(leftLegUpperModel, glm::vec3(1.0f, 1.0f, 1.0f)); // Make the upper leg shorter (height reduced)
            unsigned int x = glGetUniformLocation(shaderProgram, "model");
            glUniformMatrix4fv(x, 1, GL_FALSE, glm::value_ptr(leftLegUpperModel));
            glDrawElements(GL_TRIANGLES, l1LegUpperIndices.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);

            //Pivot for lower left leg
            glUseProgram(shaderProgram);

            glBindVertexArray(extraSphereVAO3);
            glm::mat4 extraSphereModel3 = glm::mat4(1.0f);
            extraSphereModel3 = glm::translate(leftLegUpperModel, glm::vec3(0.0f, -1.0f, 0.0f)); // Position the sphere
            extraSphereModel3 = glm::rotate(extraSphereModel3, glm::radians(lowerLeftLegRotationAngle), glm::vec3(1.0f, 0.0f, 0.0f));
            extraSphereModel3 = glm::scale(extraSphereModel3, glm::vec3(0.5f)); // Scale the sphere
            unsigned int sphere3 = glGetUniformLocation(shaderProgram2, "model");
            glUniformMatrix4fv(sphere3, 1, GL_FALSE, glm::value_ptr(extraSphereModel3));
            glDrawElements(GL_TRIANGLES, extraSphereIndices3.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);



            // Draw the lower left leg with the first shader program

            glUseProgram(shaderProgram2);

            glBindVertexArray(l1LegLowerVAO);
            glm::mat4 leftLegLowerModel = glm::mat4(1.0f);
            leftLegLowerModel = glm::translate(extraSphereModel3, glm::vec3(0.0f, -1.0f, 0.0f)); // Position the lower leg below the upper leg
            leftLegLowerModel = glm::scale(leftLegLowerModel, glm::vec3(0.5f, 1.5f, 0.5f)); // Make the lower leg longer (height increased)
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(leftLegLowerModel));
            glDrawElements(GL_TRIANGLES, l1LegLowerIndices.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);


            //Pivot for upper right leg
            glBindVertexArray(extraSphereVAO6);
            glm::mat4 extraSphereModel6 = glm::mat4(1.0f);
            extraSphereModel6 = glm::translate(torsoModel, glm::vec3(0.25f, -0.5f, 0.0f)); // Position the sphere
            extraSphereModel6 = glm::rotate(extraSphereModel6, glm::radians(rightLegRotationAngle), glm::vec3(1.0f, 0.0f, 0.0f));
            extraSphereModel6 = glm::scale(extraSphereModel6, glm::vec3(0.25f)); // Scale the sphere
            unsigned int sphere6 = glGetUniformLocation(shaderProgram2, "model");
            glUniformMatrix4fv(sphere6, 1, GL_FALSE, glm::value_ptr(extraSphereModel6));
            glDrawElements(GL_TRIANGLES, extraSphereIndices6.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);

            // Draw the upper right leg with the second shader program
            glUseProgram(shaderProgram2);
            glBindVertexArray(r1LegUpperVAO);
            glm::mat4 rightLegUpperModel = glm::mat4(1.0f);
            rightLegUpperModel = glm::translate(extraSphereModel6, glm::vec3(0.0f, -1.5f, 0.0f)); // Position the leg below the torso
            rightLegUpperModel = glm::scale(rightLegUpperModel, glm::vec3(1.0f, 1.0f, 1.0f)); // Make the upper leg shorter (height reduced)
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(rightLegUpperModel));
            glDrawElements(GL_TRIANGLES, r1LegUpperIndices.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);

            //Pivot lower right leg
            glUseProgram(shaderProgram);

            glBindVertexArray(extraSphereVAO4);
            glm::mat4 extraSphereModel4 = glm::mat4(1.0f);
            extraSphereModel4 = glm::translate(rightLegUpperModel, glm::vec3(0.0f, -1.0f, 0.0f)); // Position the sphere
            extraSphereModel4 = glm::rotate(extraSphereModel4, glm::radians(lowerRightLegRotationAngle), glm::vec3(1.0f, 0.0f, 0.0f));
            extraSphereModel4 = glm::scale(extraSphereModel4, glm::vec3(0.5f)); // Scale the sphere
            unsigned int sphere4 = glGetUniformLocation(shaderProgram2, "model");
            glUniformMatrix4fv(sphere1, 1, GL_FALSE, glm::value_ptr(extraSphereModel4));
            glDrawElements(GL_TRIANGLES, extraSphereIndices4.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);


            // Draw the lower right leg with the first shader program
            glUseProgram(shaderProgram2);

            glBindVertexArray(r1LegLowerVAO);
            glm::mat4 rightLegLowerModel = glm::mat4(1.0f);
            rightLegLowerModel = glm::translate(extraSphereModel4, glm::vec3(0.0f, -1.0f, 0.0f)); // Position the lower leg below the upper leg
            rightLegLowerModel = glm::scale(rightLegLowerModel, glm::vec3(0.5f, 1.5f, 0.5f)); // Make the lower leg longer (height increased)
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(rightLegLowerModel));
            glDrawElements(GL_TRIANGLES, r1LegLowerIndices.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);


            if (playAnimationFlag) updateAnimations();
            if (resetAnimationFlag) resetAnimations();

            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        glfwTerminate();
        return 0;
    }
