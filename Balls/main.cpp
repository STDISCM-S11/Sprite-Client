#include <GL/glut.h> // Include the GLUT header file
#include <iostream>
#include <numbers>
#include <random>
#include <typeinfo>
#include <algorithm>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <json/json.h>
#include <thread>
#include <mutex>
#include <string>

#include "imgui.h"

#include "imgui_impl_opengl3.h"

#include "imgui_impl_glut.h"
#include "Ball.h"
#include "BallManager.h"
#include "Point.h"
#include "SpriteManager.h"
#include "Sprite.h"

#pragma comment(lib, "rpcrt4.lib") 
#pragma comment(lib, "Ws2_32.lib")

using namespace std;

static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

const int MAX_WIDTH = 1920;
const int MAX_HEIGHT = 1080;
const int MAX_BATCH = 100;

int n = 0;

int batch = 0;
int ballsSpawned = 0;
int currBatch = 0;

Point startBall = { 0, 0 };
Point endBall = { 0, 0 };

pair<float, float> angle;
pair<float, float> velocity;

Point wallPoint1 = { 0, 0 };
Point wallPoint2 = { 0, 0 };

const int targetFPS = 60;
const float targetFrameTime = 1.0f / targetFPS;
float accumulator = 0.0f;
static int currentForm = 0;

bool spawning = false;

int frameCount = 0;
float lastFrameRateCalculationTime = 0.0f;
float calculatedFrameRate = 0.0f;

float backgroundOffsetX = 0.0f;
float backgroundOffsetY = 0.0f;

float cameraX = 0.0f;
float cameraY = 0.0f;

bool isExplorerMode = true;
const float tileSize = 1000.0f; // Example tile size, adjust based on your game's scale
SpriteManager spriteManager = SpriteManager();
float zoomFactor = 2.0f; // Example zoom factor, adjust based on your desired zoom level
const float peripheryTileSize = 10.0f; // Tile size in pixels
const int peripheryWidthTiles = 33; // Number of horizontal tiles
const int peripheryHeightTiles = 19; // Number of vertical tiles
const float peripheryWidth = peripheryWidthTiles * peripheryTileSize;
const float peripheryHeight = peripheryHeightTiles * peripheryTileSize;

//Sprite* mainSprite = nullptr;

GLsizei ballsViewportWidth = 1280;
GLsizei ballsViewportHeight = 720;

std::mutex ballMutex;
std::mutex spriteMutex;

void toggleExplorerMode() {
    isExplorerMode = !isExplorerMode;
}

// Function to adjust camera movement based on keyboard input
void keyboard(unsigned char key, int x, int y) {
    if (!isExplorerMode) {
        return;
    }

    float cameraSpeed = 5.0f;

    //Sprite& currentSprite = SpriteManager::getSprites().front(); // Assuming the controlled sprite is the first one
    //spriteMutex.lock();
    Sprite* mainSprite = spriteManager.getMainSprite();
    //spriteMutex.unlock();


    float spriteX = (*mainSprite).getX();
    float spriteY = (*mainSprite).getY();
    switch (key) {
    case 'w':

        if (spriteY >= ballsViewportHeight) {
            (*mainSprite).setY(ballsViewportHeight);
            break;
        }
        (*mainSprite).moveUp(cameraSpeed);
        // cout << currentSprite.getX() << ", " << currentSprite.getY() << endl;
        break;
    case 's':

        if (spriteY <= 0) {
            (*mainSprite).setY(0);
            break;
        }

        (*mainSprite).moveDown(cameraSpeed);
        break;
    case 'a':
        if (spriteX <= 0) {
            (*mainSprite).setX(0);
            break;
        }
        if (spriteX >= ballsViewportWidth - 200) {
            (*mainSprite).moveLeft(cameraSpeed * 2);
            break;
        }
        (*mainSprite).moveLeft(cameraSpeed);
        break;
    case 'd':
        if (spriteX >= ballsViewportWidth) {
            (*mainSprite).setX(ballsViewportWidth);
            break;
        }

        if (spriteX >= ballsViewportWidth - 200) {
            (*mainSprite).moveRight(cameraSpeed * 2);
            break;
        }


        (*mainSprite).moveRight(cameraSpeed);
        break;
    }
    glutPostRedisplay();
}

void drawBorderLines(float lineWidth, float borderWidth, int numLines) {

    //spriteMutex.lock();
    Sprite* mainSprite = spriteManager.getMainSprite();
    //spriteMutex.unlock();

    //Sprite& mainSprite = SpriteManager::getSprites().front();
    float spriteX = (*mainSprite).getX();
    float spriteY = (*mainSprite).getY();
    float centerX = spriteX - peripheryWidth / 2.0f;
    float centerY = spriteY - peripheryHeight / 2.0f;

    // cout << spriteX << endl; 

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    // gluOrtho2D(0, ballsViewportWidth, 0, ballsViewportHeight);

    gluOrtho2D(0, peripheryWidth + centerX, 0, peripheryHeight + centerY);
    glLineWidth(lineWidth);

    glColor3f(0.0f, 0.0f, 0.0f);

    float borderHeightOffset = 0;
    float borderWidthOffset = 0;
    // cout << centerY << endl;
    if (spriteY >= ballsViewportHeight - numLines - 60) {
        // cout << ballsViewportHeight + 100 << endl;

        for (int i = 0; i < numLines + spriteY - 450; i++) {
            glBegin(GL_LINES);
            glVertex2f(0.0f, (ballsViewportHeight + 100) - i);
            glVertex2f(ballsViewportWidth + 400, (ballsViewportHeight + 100) - i);
            glEnd();
        }
    }

    if (spriteY <= numLines) {
        for (int i = 0; i < numLines - spriteY - 50; i++) {
            glBegin(GL_LINES);
            glVertex2f(0.0f, i);
            glVertex2f(ballsViewportWidth + 100, i);
            glEnd();
        }
    }
    if (spriteX <= numLines) {
        for (int i = 0; i < numLines - spriteX - 20; i++) {
            glBegin(GL_LINES);
            glVertex2f(i, 0.0f);
            glVertex2f(i, ballsViewportHeight);
            glEnd();
        }
    }

    // cout <<  ballsViewportWidth - numLines << " " << ballsViewportWidth << endl;
    if (spriteX >= ballsViewportWidth - 150) {
        for (int i = 0; i < numLines + spriteX - 650; i++) {
            glBegin(GL_LINES);
            glVertex2f((ballsViewportWidth - 550) + i, 0.0f);
            glVertex2f((ballsViewportWidth - 550) + i, ballsViewportHeight);

            glEnd();
        }
    }
    glPopMatrix();
}

static void sliderFloat(string label, float* var, float maxValue, float minValue = 0.0f)
{

    ImGui::Text("%s", label.c_str());
    ImGui::Separator();

    ImGui::SetNextItemWidth(150.0f);

    string inputId = "##input" + label;
    string sliderId = "##slider" + label;

    ImGui::InputFloat(inputId.c_str(), var, 0.0f, 0.0f, "%.2f");
    ImGui::SameLine();

    ImGui::SetNextItemWidth(400.0f);
    ImGui::SliderFloat(sliderId.c_str(), var, minValue, maxValue, " ");

    *var = clamp(*var, minValue, maxValue);

    ImGui::Spacing();
}

static void sliderInt(string label, int* var, int maxValue)
{
    ImGui::Text("%s", label.c_str());
    ImGui::Separator();

    ImGui::SetNextItemWidth(150.0f);

    string inputId = "##input" + label;
    string sliderId = "##slider" + label;

    ImGui::InputInt(inputId.c_str(), var, 0.0f, 0.0f);
    ImGui::SameLine();

    ImGui::SetNextItemWidth(400.0f);
    ImGui::SliderInt(sliderId.c_str(), var, 0.0f, maxValue, " ");

    *var = clamp(*var, 0, maxValue);

    ImGui::Spacing();
}

void display()
{
    // First, clear the entire window with the main background color
    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);

    // Define the viewport and scissor box for the balls section
    GLint ballsViewportX = 0 - static_cast<int>(backgroundOffsetX);
    GLint ballsViewportY = 220 - static_cast<int>(backgroundOffsetY);


    glEnable(GL_SCISSOR_TEST);
    glScissor(ballsViewportX, ballsViewportY, ballsViewportWidth, ballsViewportHeight);
    glViewport(ballsViewportX, ballsViewportY, ballsViewportWidth, ballsViewportHeight);

    ImVec4 ballsBgColor = ImVec4(0.2f, 0.3f, 0.4f, 1.0f);
    glClearColor(ballsBgColor.x, ballsBgColor.y, ballsBgColor.z, ballsBgColor.w);
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, ballsViewportWidth, 0, ballsViewportHeight); // Set coordinate system with (0,0) at bottom left

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if (isExplorerMode) {
        //spriteMutex.lock();
        Sprite* mainSprite = spriteManager.getMainSprite();
        //spriteMutex.unlock();


        drawBorderLines(20.0f, 20.0f, 100);
        //Sprite& mainSprite = SpriteManager::getSprites().front();
        float centerX = (*mainSprite).getX() - peripheryWidth / 2.5f;
        float centerY = (*mainSprite).getY() - peripheryHeight / 2.5f;
        float cameraX = (*mainSprite).getX();
        float cameraY = (*mainSprite).getY();

        //cout << cameraX << endl;

        float leftBoundary = cameraX - peripheryWidth / zoomFactor;
        float rightBoundary = cameraX + peripheryWidth / zoomFactor;
        float topBoundary = cameraY + peripheryHeight / zoomFactor;
        float bottomBoundary = cameraY - peripheryHeight / zoomFactor;

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        gluOrtho2D(leftBoundary, rightBoundary, bottomBoundary, topBoundary);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }

    ballMutex.lock();
    BallManager::drawBalls();
    ballMutex.unlock();

    BallManager::drawWalls();

    spriteMutex.lock();
    spriteManager.drawSprites(cameraX, cameraY, isExplorerMode);
    spriteMutex.unlock();


    // Disable the scissor test to not affect subsequent rendering
    glDisable(GL_SCISSOR_TEST);

    // Reset the viewport to the full window size for UI rendering
    ImGuiIO& io = ImGui::GetIO();
    glViewport(0, 0, (GLsizei)io.DisplaySize.x, (GLsizei)io.DisplaySize.y);

    // Start the Dear ImGui frame and render UI
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGLUT_NewFrame();
    ImGui::NewFrame();
    {
        float oldSize = ImGui::GetFont()->Scale;
        ImGui::GetFont()->Scale *= 1.25;
        ImGui::PushFont(ImGui::GetFont());

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;

        int panelWidth = 600;
        int panelHeight = 1080;

        ImGui::SetNextWindowPos(ImVec2(MAX_WIDTH - panelWidth, 0)); // Position the window at the top-left corner
        ImGui::SetNextWindowSize(ImVec2(panelWidth, panelHeight));
        ImGui::Begin("Control Panel");

        // Toggle between Explorer and Developer Mode
        if (isExplorerMode) {
            // ImGui::Text("Controls");
            // if (ImGui::Button("Developer Mode")) {
            //     isExplorerMode = false; // Switch to Developer Mode
            // }
        }
        else {

            const char* options[] = { "0", "1", "2", "3" };

            for (int i = 0; i < IM_ARRAYSIZE(options); i++)
            {

                if (spawning)
                {
                    ImGui::BeginDisabled();
                    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
                    ImGui::RadioButton(options[i], &currentForm, i);
                    ImGui::PopStyleVar();
                    ImGui::EndDisabled();
                }
                else
                {
                    if (ImGui::RadioButton(options[i], &currentForm, i))
                    {
                        n = 0;

                        startBall = { 0, 0 };
                        endBall = { 0, 0 };

                        angle = { 0, 0 };
                        velocity = { 0, 0 };
                    }
                }

                if (i < IM_ARRAYSIZE(options) - 1)
                {
                    ImGui::SameLine();
                }
            }

            ImGui::Text("Spawn Ball");

            switch (currentForm)
            {
            case 0:
                sliderFloat("x", &startBall.x, ballsViewportWidth);
                sliderFloat("y", &startBall.y, ballsViewportHeight);
                sliderFloat("angle", &angle.first, 360.0f);
                sliderFloat("velocity", &velocity.first, 2000.0f, 300.0f);
                if (ImGui::Button("Spawn Ball"))
                {
                    BallManager::addBall(Ball(startBall.x, startBall.y, velocity.first, angle.first));
                }
                break;
            case 1:
                sliderInt("n", &n, 10000);

                sliderFloat("start x", &startBall.x, ballsViewportWidth);
                sliderFloat("start y", &startBall.y, ballsViewportHeight);
                sliderFloat("end x", &endBall.x, ballsViewportWidth);
                sliderFloat("end y", &endBall.y, ballsViewportHeight);
                sliderFloat("angle", &angle.first, 360.0f);
                sliderFloat("velocity", &velocity.first, 2000.0f, 300.0f);
                // spawning = ImGui::Button("Spawn Ball");
                if (ImGui::Button("Spawn Ball") && !spawning)
                {
                    spawning = true;
                    ballsSpawned = 0;
                    // BallManager::addBallsDistance(n, startBall, endBall, velocity.first, angle.first);
                }
                break;
            case 2:
                sliderInt("n", &n, 10000);

                sliderFloat("x", &startBall.x, ballsViewportWidth);
                sliderFloat("y", &startBall.y, ballsViewportHeight);
                sliderFloat("start angle", &angle.first, 360.0f);
                sliderFloat("end angle", &angle.second, 360.0f);
                sliderFloat("velocity", &velocity.first, 2000.0f, 300.0f);
                // spawning = ImGui::Button("Spawn Ball");

                if (ImGui::Button("Spawn Ball"))
                {
                    spawning = true;
                    ballsSpawned = 0;
                    // BallManager::addBallsAngle(n, startBall, velocity.first, angle.first, angle.second);
                }
                break;
            case 3:
                sliderInt("n", &n, 10000);

                sliderFloat("x", &startBall.x, ballsViewportWidth);
                sliderFloat("y", &startBall.y, ballsViewportHeight);
                sliderFloat("angle", &angle.first, 360.0f);
                sliderFloat("start velocity", &velocity.first, 2000.0f);
                sliderFloat("end velocity", &velocity.second, 2000.0f, 300.0f);
                // spawning = ImGui::Button("Spawn Ball");

                if (ImGui::Button("Spawn Ball"))
                {
                    spawning = true;
                    ballsSpawned = 0;
                    // BallManager::addBallsVelocity(n, startBall, velocity.first, velocity.second, angle.first);
                }
                break;
            }

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            ImGui::Text("Controls");
            if (ImGui::Button("Explorer Mode")) {
                isExplorerMode = !isExplorerMode; // Toggle the explorer mode state
            }
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / calculatedFrameRate, calculatedFrameRate);
        ballMutex.lock();
        ImGui::Text("Number of balls: %zu", BallManager::getBalls().size());
        ballMutex.unlock();
        ImGui::GetFont()->Scale = oldSize;
        ImGui::PopFont();

        ImGui::End();

        // Render ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glutSwapBuffers();
    }

}

void update(int value) {
    static int lastTime = glutGet(GLUT_ELAPSED_TIME);
    static int frameCount = 0; // Count of frames since the last FPS calculation
    static float lastFpsTime = 0.0f; // Last time the FPS was updated

    int currentTime = glutGet(GLUT_ELAPSED_TIME);
    float deltaTime = (currentTime - lastTime) / 1000.0f; // Time since the last frame in seconds
    lastTime = currentTime;

    accumulator += deltaTime;

    if (spawning && ballsSpawned == n)
    {
        spawning = false;
        ballsSpawned = 0;
        currBatch = 0;
    }

    if (spawning && ballsSpawned < n)
    {
        int toSpawn = std::min(n - ballsSpawned, MAX_BATCH);
        currBatch += toSpawn;
        switch (currentForm)
        {
        case 1:
            BallManager::addBallsDistance(n, startBall, endBall, velocity.first, angle.first, ballsSpawned, currBatch);
            break;
        case 2:

            BallManager::addBallsAngle(n, startBall, velocity.first, angle.first, angle.second, ballsSpawned, currBatch);
            break;
        case 3:

            BallManager::addBallsVelocity(n, startBall, velocity.first, velocity.second, angle.first, ballsSpawned, currBatch);
            break;
        }
        ballsSpawned += toSpawn;
    }

    while (accumulator >= targetFrameTime)
    {
        BallManager::updateBalls(deltaTime); // Update all balls with the time elapsed
        accumulator -= targetFrameTime;
    }

    frameCount++;
    float fpsUpdateTime = currentTime / 1000.0f - lastFpsTime;
    if (fpsUpdateTime >= 0.5f) { // Update FPS every 0.5 seconds
        calculatedFrameRate = frameCount / fpsUpdateTime; // Calculate FPS
        frameCount = 0; // Reset frame count for the next FPS calculation
        lastFpsTime = currentTime / 1000.0f; // Reset the FPS timer
    }

    float frameProcessingTime = (glutGet(GLUT_ELAPSED_TIME) - currentTime) / 1000.0f;
    float timeUntilNextFrame = targetFrameTime - frameProcessingTime;
    int delay = max(0, static_cast<int>(timeUntilNextFrame * 1000.0f));

    glutPostRedisplay();
    glutTimerFunc(delay, update, 0); // Adjust the delay dynamically based on the frame processing time
}


void sendSpriteData(SOCKET sock, const std::string& clientId) {
    Json::Value data;

    while (true) {
        data.clear();
        Sprite* mainSprite = spriteManager.getMainSprite();


        if (!mainSprite) {
            continue;
        }

        data["clientId"] = clientId; // Use the client ID received from the server


        // Construct JSON object representing sprite position
        data["x"] = (*mainSprite).getX();
        data["y"] = (*mainSprite).getY();

        // Convert JSON object to string
        Json::StreamWriterBuilder builder;
        builder["commentStyle"] = "None";
        builder["indentation"] = "";  // Empty string means no indentation, compact JSON
        std::string json_string = Json::writeString(builder, data);

        json_string += "\n";

        // Send JSON data to the server
        int bytesSent = send(sock, json_string.c_str(), json_string.length(), 0);
        if (bytesSent == SOCKET_ERROR) {
            std::cerr << "send failed: " << WSAGetLastError() << std::endl;
            break; // Exit loop on send error
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void receiveBallData(SOCKET sock) {
    std::string recvbuf;
    while (true) {
        // Receive data into the buffer
        const int bufferSize = 4096; // Adjust buffer size as needed
        char buffer[bufferSize];
        int bytesReceived = recv(sock, buffer, bufferSize - 1, 0);
        if (bytesReceived <= 0) {
            if (bytesReceived == 0) {
                std::cout << "Connection closed by server" << std::endl;
            }
            else {
                std::cout << "recv failed: " << WSAGetLastError() << std::endl;
            }
            break;
        }

        buffer[bytesReceived] = '\0';
        recvbuf.append(buffer);
        // Process complete JSON messages separated by newline
        size_t pos = 0;
        while ((pos = recvbuf.find("\n")) != std::string::npos) {
            std::string jsonStr = recvbuf.substr(0, pos);
            //recvbuf.erase(0, pos + 1);
            recvbuf.clear();

            Json::Value root;
            Json::Reader reader;
            if (reader.parse(jsonStr, root)) {
                // Process the parsed JSON object here
                // For example, printing the received JSON string

                if (root.isObject()) {
                    //cout << root << endl;
                    if (root["ballData"].size() > 0) {
                        ballMutex.lock();
                        for (const auto& ballData : root["ballData"]) {
                            Ball ball = Ball(ballData["x"].asFloat(), ballsViewportHeight - ballData["y"].asFloat(), ballData["velocity"].asFloat(), ballData["angle"].asFloat());
                            BallManager::addBall(ball);
                        }
                        ballMutex.unlock();
                    }
                    if (root["spriteData"]) {

                        spriteMutex.lock();
                        spriteManager.clearSprites();
                        for (const auto& spriteData : root["spriteData"]) {
                            Sprite sprite(spriteData["x"].asFloat(), spriteData["y"].asFloat());
                            spriteManager.addSprites(sprite);
                        }
                        spriteMutex.unlock();
                    }
                }

                //std::cout << "Received JSON: " << jsonStr << std::endl;
            }
        }
    }
}

static int connectToServer() {
    WSADATA wsaData;
    SOCKET sock = INVALID_SOCKET;
    struct sockaddr_in server;

    // Initialize Winsock
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0)
    {
        std::cerr << "WSAStartup failed: " << result << std::endl;
        return 1;
    }

    // Create the socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET)
    {
        std::cerr << "Socket creation failed with error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    if (inet_pton(AF_INET, "127.0.0.1", &server.sin_addr) <= 0)
    {
        std::cerr << "Invalid address/ Address not supported \n";
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(4000);

    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0)
    {
        std::cerr << "Connect failed with error.\n";
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    std::cout << "Connected to server.\n";

    /* const int RECV_BUF_SIZE = 4096;

     char recvbuf[RECV_BUF_SIZE];
     int totalReceived = 0;*/

    char idBuffer[256]; // Buffer to store the ID string received from the server
    int bytesReceived = recv(sock, idBuffer, sizeof(idBuffer) - 1, 0); // Leave space for null terminator
    if (bytesReceived <= 0) {
        // Handle errors or connection closure
        std::cerr << "Failed to receive ID from server or connection closed.\n";
        closesocket(sock); // Ensure to close the socket
        WSACleanup(); // Assuming Winsock is used, perform cleanup
        return -1; // Exit or handle as appropriate
    }
    idBuffer[bytesReceived] = '\0'; // Null-terminate the received data
    std::string clientId(idBuffer); // Convert received data to a string
    std::cout << "Received Client ID: " << clientId << std::endl;
    clientId.replace(clientId.end() - 2, clientId.end(), "");

    std::thread receiveBallThread(receiveBallData, sock);
    std::thread sendSpriteThread(sendSpriteData, sock, clientId);

    receiveBallThread.join();
    sendSpriteThread.join();
    closesocket(sock);
    WSACleanup();

}




int main(int argc, char** argv)
{

    std::thread serverThread(connectToServer);

    glutInit(&argc, argv);
#ifdef __FREEGLUT_EXT_H__
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
#endif
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_MULTISAMPLE);
    glutInitWindowSize(MAX_WIDTH, MAX_HEIGHT);
    glutCreateWindow("Bouncing balls");

    glutDisplayFunc(display);
    glutTimerFunc(1, update, 0);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    ImGui_ImplGLUT_InstallFuncs();
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGLUT_Init();

#ifdef GL_VERSION_2_0
    ImGui_ImplOpenGL3_Init("#version 120");
#else
    ImGui_ImplOpenGL3_Init();
#endif

    ImGui_ImplGLUT_InstallFuncs();

    Sprite sprite = Sprite(0, 0);

    spriteManager.setMainSprite(&sprite);

    glutKeyboardFunc(keyboard);

    // Main loop
    glutMainLoop();
    serverThread.join();

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGLUT_Shutdown();
    ImGui::DestroyContext();

    return 0;
}
