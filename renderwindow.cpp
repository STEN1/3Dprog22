#include "renderwindow.h"
#include <QTimer>
#include <QMatrix4x4>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLDebugLogger>
#include <QKeyEvent>
#include <QStatusBar>
#include <QDebug>
#include <QSurfaceFormat>


#include <chrono>
#include <string>

#include "mainwindow.h"
#include "Core/Logger.h"
#include "Core/ObjLoader.h"
#include "VisualObject/VisualObject.h"
#include "sMath/sMath.h"
#include "Core/Input.h"
#include "Scene/WoodsScene.h"
#include "Scene/HouseScene.h"
#include "Scene/HeightmapScene.h"
#include "Scene/WorldScene.h"
#include "Scene/EksamenScene.h"
#include "Core/SoundManager.h"

#include "stb_image/stb_image.h"

RenderWindow::RenderWindow(const QSurfaceFormat& format, MainWindow* mainWindow)
    : mContext(nullptr), mInitialized(false), mMainWindow(mainWindow)

{
    //This is sent to QWindow:
    setSurfaceType(QWindow::OpenGLSurface);
    setFormat(format);
    //Make the OpenGL context
    mContext = new QOpenGLContext(this);
    //Give the context the wanted OpenGL format (v4.1 Core)
    mContext->setFormat(requestedFormat());
    if (!mContext->create()) {
        delete mContext;
        mContext = nullptr;
        qDebug() << "Context could not be made - quitting this application";
    }

    //Make the gameloop timer:
    mRenderTimer = new QTimer(this);
}

RenderWindow::~RenderWindow()
{
    s_renderWindow = nullptr;
    SoundManager::Cleanup();
}

void RenderWindow::init()
{
    //Connect the gameloop timer to the render function:
    //This makes our render loop
    connect(mRenderTimer, SIGNAL(timeout()), this, SLOT(render()));
    //********************** General OpenGL stuff **********************

    if (!mContext->makeCurrent(this)) {
        LOG_ERROR("makeCurrent() failed");
        return;
    }

    //just to make sure we don't init several times
    //used in exposeEvent()
    if (!mInitialized)
        mInitialized = true;

    //must call this to use OpenGL functions
    initializeOpenGLFunctions();
    LOG_HIGHLIGHT("The active GPU and API:");
    std::string tempString;
    tempString += std::string("  Vendor: ") + std::string((char*)glGetString(GL_VENDOR)) + "\n" +
        std::string("  Renderer: ") + std::string((char*)glGetString(GL_RENDERER)) + "\n" +
        std::string("  Version: ") + std::string((char*)glGetString(GL_VERSION));
    LOG(tempString);

    s_renderWindow = this;

    //Start the Qt OpenGL debugger
    startOpenGLDebugger();

    //general OpenGL stuff:
    glEnable(GL_CULL_FACE);       //draws only front side of models - usually what you want - test it out!
    glClearColor(135.f/255.f, 206.f/255.f, 235.f/255.f, 1.0f);    //gray color used in glClear GL_COLOR_BUFFER_BIT
    glEnable(GL_DEPTH_TEST);         //enables depth sorting - must then use GL_DEPTH_BUFFER_BIT in glClear
    glEnable(GL_BLEND);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_MULTISAMPLE);
    
    // OpenAL
    SoundManager::Init();

    // init SSBOs
    m_ssboManager = std::make_shared<SSBOManager>();
    m_ssboManager->CreateSSBO("Lights",
                               // CurrentNumberOfLights.
        (sizeof(PointLight) * MAX_LIGHTS),  // Max number of lights in light array.
        1);                                 // Binding.

    // init camera
    const qreal retinaScale = devicePixelRatio();
    m_camera = std::make_shared<Camera>(
        static_cast<GLint>(width() * retinaScale),
        static_cast<GLint>(height() * retinaScale),
        Camera::Projection::Perspective);

    m_debugCamera = std::make_shared<Camera>(
        static_cast<GLint>(width() * retinaScale),
        static_cast<GLint>(height() * retinaScale),
        Camera::Projection::Perspective);
    m_debugCamera->SetPosition({ 150.f, 10.f, 150.f });
  
    LoadScene<EksamenScene>();
    auto& lightShader = LoadShader("light");
    auto& landscapeShader = LoadShader("landscape");
    auto& instancedLightShader = LoadShader("instancedlight");


    lightShader.Use();
    lightShader.SetInt(0, "material.diffuse");
    lightShader.SetInt(1, "material.specular");
    lightShader.SetFloat(0.2f, "material.shininess");
    instancedLightShader.Use();
    instancedLightShader.SetInt(0, "material.diffuse");
    instancedLightShader.SetInt(1, "material.specular");
    instancedLightShader.SetFloat(0.2f, "material.shininess");
    landscapeShader.Use();
    landscapeShader.SetInt(0, "material.diffuse");
    landscapeShader.SetInt(1, "material.specular");
    landscapeShader.SetFloat(0.2f, "material.shininess");
    landscapeShader.SetFloat(1.f, "UVscale");
    landscapeShader.SetFloat(1.f, "Uscale");
    landscapeShader.SetFloat(1.f, "Vscale");
    //InitPointLightUniforms();
}

// Called each frame - doing the rendering!!!
void RenderWindow::render()
{
    // timing used for fps counter and camera rotation
    double deltaTime = calculateFramerate();
    if (deltaTime > 0.2f)
        deltaTime = 0.2f;
    mContext->makeCurrent(this);    //must be called every frame (every time mContext->swapBuffers is called)


    if (m_nextScene)
    {
        m_scene = std::move(m_nextScene);
        m_scene->Init();
    }

    if (m_scene)
    {
        if (m_EditorMode == EditorMode::Play)
        {
            m_scene->Update(deltaTime);
        }
        else if (m_EditorMode == EditorMode::Debug)
        {
            DebugUpdate(deltaTime);

        }
        // give all shaders the view and projection matrix
        if (auto camera = GetCamera())
        {
            for (auto& [path, shader] : m_shaders)
            {
                shader.Use();
                shader.SetMat4f(camera->GetViewMatrix(), "view");
                shader.SetMat4f(camera->GetProjectionMatrix(), "projection");
                if (path == "light" || path == "landscape" || path == "instancedlight")
                {
                    shader.SetVec3f(camera->GetPosition(), "uViewPos");
                }
                if (path == "skybox")
                {
                    glm::mat4 view = glm::mat4(glm::mat3(camera->GetViewMatrix()));
                    shader.SetMat4f(view, "view");
                }
            }
            SoundManager::SetListenerPos(camera->GetPosition());
            SoundManager::SetListenerOri(camera->GetCameraForward(), camera->GetCameraUp());
            // TODO:
            // SoundManager::SetListenerVel
            SoundManager::UpdateSources();
        }
        else { LOG_ERROR("NO CAMERA IN RENDER FUNCTION!"); }
        m_scene->Render();
        if (m_EditorMode == EditorMode::Debug)
        {
            glDisable(GL_DEPTH_TEST);
            static constexpr glm::vec3 origo{ 0.f, 0.f, 0.f };
            static constexpr glm::vec3 x{ 1.f, 0.f, 0.f };
            static constexpr glm::vec3 y{ 0.f, 1.f, 0.f };
            static constexpr glm::vec3 z{ 0.f, 0.f, 1.f };
            static constexpr float size = 10.f;
            DrawLine(origo, x * size, glm::vec4(x, 1.f));
            DrawLine(origo, y * size, glm::vec4(y, 1.f));
            DrawLine(origo, z * size, glm::vec4(z, 1.f));
            glEnable(GL_DEPTH_TEST);
        }
        Input::LastMouseWheelDelta = 0;
        Input::MousePosDelta = { 0.f, 0.f };
    }

    //using our expanded OpenGL debugger to check if everything is OK.
    checkForGLerrors();

    mContext->swapBuffers(this);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Depth sorting enabled so using GL_DEPTH_BUFFER_BIT
}

// This function is called from Qt when window is exposed (shown)
// and when it is resized
void RenderWindow::exposeEvent(QExposeEvent *)
{
    //if not already initialized - run init() function - happens on program start up
    if (!mInitialized)
        init();
    //This is just to support modern screens with "double" pixels (Macs and some 4k Windows laptops)
    const qreal retinaScale = devicePixelRatio();
    //Set viewport width and height to the size of the QWindow we have set up for OpenGL
    glViewport(0, 0, static_cast<GLint>(width() * retinaScale), static_cast<GLint>(height() * retinaScale));
    m_camera->SetViewportSize(static_cast<GLint>(width() * retinaScale), static_cast<GLint>(height() * retinaScale));
    m_debugCamera->SetViewportSize(static_cast<GLint>(width() * retinaScale), static_cast<GLint>(height() * retinaScale));
    //If the window actually is exposed to the screen we start the main loop
    if (isExposed())
    {
        //This timer runs the actual MainLoop == the render()-function
        //16 means 16ms = 60 Frames pr second (should be 16.6666666 to be exact...)
        mRenderTimer->start(0);
    }
}

double RenderWindow::calculateFramerate()
{

    static auto previous = std::chrono::high_resolution_clock::now();
    auto now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> delta = now - previous;
    std::chrono::duration<double> deltaSeconds = now - previous;
    double deltaTime = delta.count();
    previous = now;

    static int frameCount{0};
    if (mMainWindow)            //if no mainWindow, something is really wrong...
    {
        ++frameCount;
        if (frameCount > 30)    //once pr 30 frames = update the message == twice pr second (on a 60Hz monitor)
        {
            mMainWindow->statusBar()->showMessage(" Time: " + QString::number(deltaTime, 'g', 4) + " ms  |  " + "FPS: " + QString::number(1000 / deltaTime));
            frameCount = 0;     //reset to show a new message in 30 frames
        }
    }
    return deltaSeconds.count();
}

//Uses QOpenGLDebugLogger if this is present
//Reverts to glGetError() if not
void RenderWindow::checkForGLerrors()
{
    if(mOpenGLDebugLogger)  //if our machine got this class to work
    {
        const QList<QOpenGLDebugMessage> messages = mOpenGLDebugLogger->loggedMessages();
        for (const QOpenGLDebugMessage &message : messages)
        {
            if (!(message.type() == message.OtherType)) // get rid of uninteresting "object ...
                                                        // will use VIDEO memory as the source for
                                                        // buffer object operations"
                // valid error message:
                LOG_ERROR(message.message().toStdString());
        }
    }
    else
    {
        GLenum err = GL_NO_ERROR;
        while((err = glGetError()) != GL_NO_ERROR)
        {
            LOG_ERROR("glGetError returns " + std::to_string(err));
            switch (err) {
            case 1280:
                LOG("GL_INVALID_ENUM - Given when an enumeration parameter is not a "
                                "legal enumeration for that function.");
                break;
            case 1281:
                LOG("GL_INVALID_VALUE - Given when a value parameter is not a legal "
                                "value for that function.");
                break;
            case 1282:
                LOG("GL_INVALID_OPERATION - Given when the set of state for a command "
                                "is not legal for the parameters given to that command. "
                                "It is also given for commands where combinations of parameters "
                                "define what the legal parameters are.");
                break;
            }
        }
    }
}

//Tries to start the extended OpenGL debugger that comes with Qt
//Usually works on Windows machines, but not on Mac...
void RenderWindow::startOpenGLDebugger()
{
    QOpenGLContext * temp = this->context();
    if (temp)
    {
        QSurfaceFormat format = temp->format();
        if (!format.testOption(QSurfaceFormat::DebugContext))
            LOG_HIGHLIGHT("This system can not use QOpenGLDebugLogger, so we revert to glGetError()");

        if(temp->hasExtension(QByteArrayLiteral("GL_KHR_debug")))
        {
            LOG_HIGHLIGHT("This system can log extended OpenGL errors");
            mOpenGLDebugLogger = new QOpenGLDebugLogger(this);
            if (mOpenGLDebugLogger->initialize()) // initializes in the current context
                LOG("Started Qt OpenGL debug logger");
        }
    }
}

RenderWindow* RenderWindow::Get()
{
    return s_renderWindow;
}

uint32_t RenderWindow::LoadTexture(const std::string &texture, bool flip)
{
    std::string path = "../3Dprog22/assets/" + texture;
    auto iter = m_textureMap.find(texture);
    if (iter != m_textureMap.end())
    {
        //LOG("Found in texture map: ");
        //LOG(texture_path);
        return iter->second;
    }
    else
    {
        uint32_t textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        // set the texture wrapping/filtering options (on the currently bound texture object)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        // load and generate the texture
        int width, height, nrChannels;
        stbi_set_flip_vertically_on_load(flip);
        uint8_t* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
            stbi_image_free(data);
        }
        else
        {
            LOG_WARNING("Failed to load texture");
        }
        m_textureMap.emplace(texture, textureID);
        return textureID;
    }
}

Shader& RenderWindow::LoadShader(const std::string &shader)
{
    auto iter = m_shaders.find(shader);
    if (iter == m_shaders.end())
    {
        std::string frag = "../3Dprog22/shaders/" + shader + ".frag";
        std::string vert = "../3Dprog22/shaders/" + shader + ".vert";
        if (auto [newIter, confirmation] = m_shaders.try_emplace(shader, vert, frag); confirmation)
        {
            return (*newIter).second;
        }
    }
    // This should be fun if there are no shaders :)
    return (*iter).second;
}

void RenderWindow::InstancedSubmit(uint32_t vao, uint32_t texture, InstancedSubmitData submitData, const glm::mat4& transform)
{
    uint32_t key = vao * 10000 + texture;
    auto it = m_instancedData.find(key);
    if (it != m_instancedData.end())
    {
        it->second.first.push_back(transform);
    }
    else
    {
        m_instancedData[key].first.push_back(transform);
        m_instancedData[key].second = submitData;
    }
}

void RenderWindow::InstancedPresent()
{
    auto& instancedShader = LoadShader("instancedlight");
    instancedShader.Use();
    for (auto& [vaoTexturePair, data] : m_instancedData)
    {
        auto& transformData = data.first;
        auto& submitData = data.second;

        uint32_t texture = vaoTexturePair % 10000;
        uint32_t VAO = (vaoTexturePair - texture) / 10000;

        glBindVertexArray(VAO); 
        glBindBuffer(GL_ARRAY_BUFFER, submitData.IBO);
        glBufferData(GL_ARRAY_BUFFER, transformData.size() * sizeof(glm::mat4), transformData.data(), GL_STATIC_DRAW);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glDrawElementsInstanced(GL_TRIANGLES, submitData.elementCount, GL_UNSIGNED_INT, 0, transformData.size());
    }
    glBindVertexArray(0);
    m_instancedData.clear();
}

std::shared_ptr<Camera> RenderWindow::GetCamera()
{
    if (m_EditorMode == EditorMode::Play)
        return m_camera;
    else if (m_EditorMode == EditorMode::Debug)
        return m_debugCamera;
}

std::shared_ptr<Camera> RenderWindow::GetPlayCamera()
{
    return m_camera;
}

void RenderWindow::InitPointLightUniforms()
{
    auto& lightShader = LoadShader("light");
    auto& landscapeShader = LoadShader("landscape");
    auto& instancedLightShader = LoadShader("instancedlight");
    
    auto lightID = lightShader.GetProgram();
    auto landscapeID = landscapeShader.GetProgram();
    auto instancedID = instancedLightShader.GetProgram();

    

    for (uint32_t i = 0; i < MAX_LIGHTS; i++)
    {
        std::string position = "uPointLights[" + std::to_string(i) + "].position";
        std::string constant = "uPointLights[" + std::to_string(i) + "].constant";
        std::string linear = "uPointLights[" + std::to_string(i) + "].linear";
        std::string quadratic = "uPointLights[" + std::to_string(i) + "].quadratic";
        std::string lightRange = "uPointLights[" + std::to_string(i) + "].lightRange";
        std::string ambient = "uPointLights[" + std::to_string(i) + "].ambient";
        std::string diffuse = "uPointLights[" + std::to_string(i) + "].diffuse";
        std::string specular = "uPointLights[" + std::to_string(i) + "].specular";

        PointLightUniform pointLightUniform;
        pointLightUniform.position = glGetUniformLocation(lightID, position.c_str());
        pointLightUniform.constant = glGetUniformLocation(lightID, constant.c_str());
        pointLightUniform.linear = glGetUniformLocation(lightID, linear.c_str());
        pointLightUniform.quadratic = glGetUniformLocation(lightID, quadratic.c_str());
        pointLightUniform.lightRange = glGetUniformLocation(lightID, lightRange.c_str());
        pointLightUniform.ambient = glGetUniformLocation(lightID, ambient.c_str());
        pointLightUniform.diffuse = glGetUniformLocation(lightID, diffuse.c_str());
        pointLightUniform.specular = glGetUniformLocation(lightID, specular.c_str());
        m_PointLightUniforms[lightID].push_back(pointLightUniform);

        pointLightUniform.position = glGetUniformLocation(landscapeID, position.c_str());
        pointLightUniform.constant = glGetUniformLocation(landscapeID, constant.c_str());
        pointLightUniform.linear = glGetUniformLocation(landscapeID, linear.c_str());
        pointLightUniform.quadratic = glGetUniformLocation(landscapeID, quadratic.c_str());
        pointLightUniform.lightRange = glGetUniformLocation(landscapeID, lightRange.c_str());
        pointLightUniform.ambient = glGetUniformLocation(landscapeID, ambient.c_str());
        pointLightUniform.diffuse = glGetUniformLocation(landscapeID, diffuse.c_str());
        pointLightUniform.specular = glGetUniformLocation(landscapeID, specular.c_str());
        m_PointLightUniforms[landscapeID].push_back(pointLightUniform);

        pointLightUniform.position = glGetUniformLocation(instancedID, position.c_str());
        pointLightUniform.constant = glGetUniformLocation(instancedID, constant.c_str());
        pointLightUniform.linear = glGetUniformLocation(instancedID, linear.c_str());
        pointLightUniform.quadratic = glGetUniformLocation(instancedID, quadratic.c_str());
        pointLightUniform.lightRange = glGetUniformLocation(instancedID, lightRange.c_str());
        pointLightUniform.ambient = glGetUniformLocation(instancedID, ambient.c_str());
        pointLightUniform.diffuse = glGetUniformLocation(instancedID, diffuse.c_str());
        pointLightUniform.specular = glGetUniformLocation(instancedID, specular.c_str());
        m_PointLightUniforms[instancedID].push_back(pointLightUniform);
    }
}

void RenderWindow::UpdatePointLights(const std::vector<PointLight>& lights)
{
    if (lights.size() > MAX_LIGHTS)
    {
        LOG_ERROR("MAX LIGHTS EXEDED!");
        return;
    }

    // Give current number of lights to shader storage buffer object.
    //m_ssboManager->UpdateData<int>("Lights", { static_cast<int>(lights.size()) }, 0);
    // Give light data.
    auto& lightShader = LoadShader("light");
    lightShader.Use();
    lightShader.SetInt(lights.size(), "currentAmount");

    auto& landscapeShader = LoadShader("landscape");
    landscapeShader.Use();
    landscapeShader.SetInt(lights.size(), "currentAmount");

    auto& instancedLightShader = LoadShader("instancedlight");
    instancedLightShader.Use();
    instancedLightShader.SetInt(lights.size(), "currentAmount");

    m_ssboManager->UpdateData<PointLight>("Lights", lights, 0);

#if 0 // Old code before using SSBO
    for (auto& [shaderID, lightUniforms] : m_PointLightUniforms)
    {
        glUseProgram(shaderID);
        for (uint32_t i = 0; i < lights.size(); i++)
        {
            glUniform3f(
                lightUniforms[i].position,
                lights[i].position.x,
                lights[i].position.y,
                lights[i].position.z);
            glUniform1f(lightUniforms[i].constant, lights[i].constant);
            glUniform1f(lightUniforms[i].linear, lights[i].linear);
            glUniform1f(lightUniforms[i].quadratic, lights[i].quadratic);
            glUniform1f(lightUniforms[i].lightRange, lights[i].lightRange);
            glUniform3f(
                lightUniforms[i].ambient,
                lights[i].ambient.x,
                lights[i].ambient.y,
                lights[i].ambient.z);
            glUniform3f(
                lightUniforms[i].diffuse,
                lights[i].diffuse.x,
                lights[i].diffuse.y,
                lights[i].diffuse.z);
            glUniform3f(
                lightUniforms[i].specular,
                lights[i].specular.x,
                lights[i].specular.y,
                lights[i].specular.z);
        }
    }
    auto& lightShader = LoadShader("light");
    auto& landscapeShader = LoadShader("landscape");
    auto& instancedLightShader = LoadShader("instancedlight");
    lightShader.Use();
    lightShader.SetInt(lights.size(), "uCurrentNrPointLights");
    landscapeShader.Use();
    landscapeShader.SetInt(lights.size(), "uCurrentNrPointLights");
    instancedLightShader.Use();
    instancedLightShader.SetInt(lights.size(), "uCurrentNrPointLights");
#endif
}


void RenderWindow::DrawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color)
{
    static uint32_t VAO{};
    static uint32_t VBO{};
    glm::vec3 data[] = { start, end };
    if (VAO == 0)
    {
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        glEnableVertexAttribArray(0);
    }
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(data), data);
    auto& shader = LoadShader("plainshader");
    shader.Use();
    static constexpr glm::mat4 identity(1.f);
    shader.SetVec4f(color, "uColor");
    shader.SetMat4f(identity, "model");
    glDrawArrays(GL_LINES, 0, 2);
    glBindVertexArray(0);
}

void RenderWindow::DrawAABB(const glm::vec3& pos, const glm::vec3& extent, const glm::vec4& color)
{
    static uint32_t VAO{};
    if (VAO == 0)
    {
        float vData[] = {
            -1.f, 1.f, -1.f,
            1.f, 1.f, -1.f,
            1.f, 1.f, 1.f,
            -1.f, 1.f, 1.f,
            -1.f, -1.f, -1.f,
            1.f, -1.f, -1.f,
            1.f, -1.f, 1.f,
            -1.f, -1.f, 1.f,
        };
        uint32_t eData[] = {
            0, 1,
            1, 2,
            2, 3,
            3, 0,

            4, 5,
            5, 6,
            6, 7,
            7, 4,

            0, 4,
            1, 5,
            2, 6,
            3, 7,
        };

        uint32_t VBO{};
        uint32_t EBO{};
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vData), vData, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(eData), eData, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
        glEnableVertexAttribArray(0);
    }

    glm::mat4 transform(1.f);
    transform[3].x = pos.x;
    transform[3].y = pos.y;
    transform[3].z = pos.z;

    transform[0].x = extent.x;
    transform[1].y = extent.y;
    transform[2].z = extent.z;

    glBindVertexArray(VAO);
    auto& shader = LoadShader("plainshader");
    shader.Use();
    shader.SetMat4f(transform, "model");
    shader.SetVec4f(color, "uColor");
    glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void RenderWindow::DrawSphere(const glm::vec3& pos, float r, const glm::vec4& color)
{
    static uint32_t VAO{};
    static uint32_t numVerts;
    if (VAO == 0)
    {
        uint32_t subdivisions = 3;
        std::vector<glm::vec3> vData;
        glm::vec3 v0{ 0, 0, 1 };
        glm::vec3 v1{ 1, 0, 0 };
        glm::vec3 v2{ 0, 1, 0 };
        glm::vec3 v3{ -1, 0, 0 };
        glm::vec3 v4{ 0, -1, 0 };
        glm::vec3 v5{ 0, 0, -1 };

        SubdivideSphere(v0, v1, v2, subdivisions, vData);
        SubdivideSphere(v0, v2, v3, subdivisions, vData);
        SubdivideSphere(v0, v3, v4, subdivisions, vData);
        SubdivideSphere(v0, v4, v1, subdivisions, vData);
        SubdivideSphere(v5, v2, v1, subdivisions, vData);
        SubdivideSphere(v5, v3, v2, subdivisions, vData);
        SubdivideSphere(v5, v4, v3, subdivisions, vData);
        SubdivideSphere(v5, v1, v4, subdivisions, vData);

        auto vSize = vData.size() * sizeof(glm::vec3);
        numVerts = vData.size();

        uint32_t VBO{};
        uint32_t EBO{};
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vData.size() * sizeof(glm::vec3), vData.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
        glEnableVertexAttribArray(0);
    }
    glm::mat4 transform(1.f);
    transform[3].x = pos.x;
    transform[3].y = pos.y;
    transform[3].z = pos.z;

    transform[0].x = r;
    transform[1].y = r;
    transform[2].z = r;

    glBindVertexArray(VAO);
    auto& shader = LoadShader("plainshader");
    shader.Use();
    shader.SetMat4f(transform, "model");
    shader.SetVec4f(color, "uColor");
    glDrawArrays(GL_LINES, 0, numVerts);
    glBindVertexArray(0);
}

void RenderWindow::SubdivideSphere(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, int n, std::vector<glm::vec3>& v)
{
    if (n > 0) {
        glm::vec3 v1 = glm::normalize(a + b);
        glm::vec3 v2 = glm::normalize(a + c);
        glm::vec3 v3 = glm::normalize(c + b);
        SubdivideSphere(a, v1, v2, n - 1, v);
        SubdivideSphere(c, v2, v3, n - 1, v);
        SubdivideSphere(b, v3, v1, n - 1, v);
        SubdivideSphere(v3, v2, v1, n - 1, v);
    }
    else {
        v.push_back(a);
        v.push_back(b);
        v.push_back(c);
    }
}


void RenderWindow::DrawRect(const glm::mat4& transform, const glm::vec4& color)
{
    static constexpr float rect[] = {
        0.5f, 0.f, -0.5f,
        0.5f, 0.f, 0.5f,
        -0.5f, 0.f, 0.5f,
        -0.5f, 0.f, -0.5f
    };
    static uint32_t VAO{};
    if (VAO == 0)
    {
        uint32_t VBO{};
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(rect), rect, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
        glEnableVertexAttribArray(0);
    }
    glBindVertexArray(VAO);
    auto& shader = LoadShader("plainshader");
    shader.Use();
    shader.SetVec4f(color, "uColor");
    shader.SetMat4f(transform, "model");
    glDrawArrays(GL_LINE_LOOP, 0, 4);
    glBindVertexArray(0);
}

void RenderWindow::DebugUpdate(float deltaTime)
{
    // Check for input
    float forwardInput{};
    float rightInput{};
    float upInput{};
    if (Input::Keyboard[Qt::Key_Up] || Input::Keyboard[Qt::Key_W])
        forwardInput += 1.f;
    if (Input::Keyboard[Qt::Key_Down] || Input::Keyboard[Qt::Key_S])
        forwardInput -= 1.f;
    if (Input::Keyboard[Qt::Key_Right] || Input::Keyboard[Qt::Key_D])
        rightInput += 1.f;
    if (Input::Keyboard[Qt::Key_Left] || Input::Keyboard[Qt::Key_A])
        rightInput -= 1.f;
    if (Input::Keyboard[Qt::Key_Q])
        upInput -= 1.f;
    if (Input::Keyboard[Qt::Key_E])
        upInput += 1.f;
   

    // Getting direction to move from camera vectors.
    auto cameraForward = m_debugCamera->GetCameraForward();
    auto cameraRight = m_debugCamera->GetCameraRight();
    auto worldUp = glm::vec3{ 0.f, 1.f, 0.f };

    auto forward = cameraForward * forwardInput;
    auto right = cameraRight * rightInput;
    auto up = worldUp * upInput;

    auto wishDir = forward + right + up;

    auto [mouseX, mouseY] = Input::MousePos;
    static auto oldMouseX = mouseX;
    static auto oldMouseY = mouseY;
    auto [mouseDx, mouseDy] = Input::MousePosDelta;
    if (!Input::Mouse[Qt::RightButton])
    {
        oldMouseX = mouseX;
        oldMouseY = mouseY;
    }

    static float currentRotationX{};
    static float currentRotationY{ -0.2f };
    if (Input::Mouse[Qt::RightButton])
    {
        currentRotationX += mouseDx / 200.f;
        currentRotationY += mouseDy / 200.f;
        currentRotationY = std::clamp<float>(currentRotationY, -1.f, 1.f);
    }

    auto rotation = glm::rotate(glm::mat4(1.f), currentRotationY, m_debugCamera->GetCameraRight());
    rotation = glm::rotate(rotation, currentRotationX, glm::vec3(0.f, 1.f, 0.f));

    glm::vec3 target = rotation * glm::vec4{ 0.f, 0.f, -1.f, 1.f };

    if (glm::length(wishDir) > 0.01f)
    {
        wishDir = glm::normalize(wishDir);
        auto scrollY = Input::LastMouseWheelDelta;
        if (scrollY != 0)
        {
            m_DebugCameraSpeed += scrollY / 10.f;
            m_DebugCameraSpeed = std::clamp<float>(m_DebugCameraSpeed, 20.f, 200.f);
            LOG("Speed: " + std::to_string(m_DebugCameraSpeed));
        }
        auto pos = m_debugCamera->GetPosition() + wishDir * m_DebugCameraSpeed * deltaTime;
        m_debugCamera->SetPosition(pos);
    }

    target += m_debugCamera->GetPosition();
    m_debugCamera->SetTarget(target);
}

void RenderWindow::mousePressEvent(QMouseEvent* event)
{
    Input::Mouse[event->button()] = true;
    if (event->button() == Qt::RightButton)
    {
        /*m_cursor.setShape(Qt::BlankCursor);
        setCursor(m_cursor);*/
    }
    if (m_scene)
        m_scene->MouseClicked(event->button());
}

void RenderWindow::mouseReleaseEvent(QMouseEvent* event)
{
    Input::Mouse[event->button()] = false;
    if (event->button() == Qt::RightButton)
    {
       /* m_cursor.setShape(Qt::ArrowCursor);
        setCursor(m_cursor);*/
    }
}

void RenderWindow::mouseMoveEvent(QMouseEvent* event)
{
    auto x = event->globalPos().x();
    auto y = event->globalPos().y();
    Input::MousePos = { x, y };
    static float oldX = event->globalPosition().x();
    static float oldY = event->globalPosition().y();
    float newX = event->globalPosition().x();
    float newY = event->globalPosition().y();
    float deltaX = oldX - newX;
    float deltaY = oldY - newY;
    oldX = newX;
    oldY = newY;
    Input::MousePosDelta = { deltaX, deltaY };
}

//Event sent from Qt when program receives a keyPress
// NB - see renderwindow.h for signatures on keyRelease and mouse input
void RenderWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
        mMainWindow->close();       //Shuts down the whole program
    Input::Keyboard[event->key()] = true;
    if (m_scene)
        m_scene->KeyPressed(event->key());
    if (event->key() == Qt::Key_M)
    {
        SoundManager::CheckForErrors();
    }
    QWindow::keyPressEvent(event);
}

void RenderWindow::keyReleaseEvent(QKeyEvent* event)
{
    Input::Keyboard[event->key()] = false;
    QWindow::keyReleaseEvent(event);
}

void RenderWindow::wheelEvent(QWheelEvent* event)
{
    Input::LastMouseWheelDelta = event->angleDelta().y();
}

void RenderWindow::GUI_LandscapeUVSlider(int value)
{
    auto& landscapeShader = LoadShader("landscape");
    landscapeShader.Use();
    landscapeShader.SetFloat((float)value, "UVscale");
    LOG("LandscapeUV slider: " + std::to_string(value));
}

void RenderWindow::GUI_WireframeToggele()
{
    static bool toggle = true;
    if (toggle)
    {
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
        toggle = !toggle;
    }
    else
    {
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
        toggle = !toggle;
    }
}

void RenderWindow::GUI_HouseScene()
{
    LoadScene<HouseScene>();
}

void RenderWindow::GUI_WoodsScene()
{
    LoadScene<WoodsScene>();
}

void RenderWindow::GUI_WorldScene()
{
    LoadScene<WorldScene>();
}

void RenderWindow::GUI_HeightmapScene()
{
    LoadScene<HeightmapScene>();
}

void RenderWindow::GUI_EksamenScene()
{
    LoadScene<EksamenScene>();
}

void RenderWindow::GUI_DrawMode()
{
    auto& lightShader = LoadShader("light");
    auto& landscapeShader = LoadShader("landscape");
    auto& instancedLightShader = LoadShader("instancedlight");
    drawMode++;
    if (drawMode > 3)
        drawMode = 0;
    lightShader.Use();
    lightShader.SetInt(drawMode, "drawMode");
    instancedLightShader.Use();
    instancedLightShader.SetInt(drawMode, "drawMode");
    landscapeShader.Use();
    landscapeShader.SetInt(drawMode, "drawMode");
    switch (drawMode)
    {
    case 0:
        LOG("drawMode: " + std::to_string(drawMode) + ". Light * texture");
        break;
    case 1:
        LOG("drawMode: " + std::to_string(drawMode) + ". Light only");
        break;
    case 2:
        LOG("drawMode: " + std::to_string(drawMode) + ". Worldspace normals");
        break;
    case 3:
        LOG("drawMode: " + std::to_string(drawMode) + ". Texture only");
        break;
    default:
        break;
    }
}

void RenderWindow::GUI_DebugLines()
{
    if (m_scene)
        m_scene->ToggleDebugLines();
}

void RenderWindow::GUI_PlayDebug()
{
    if (m_EditorMode == EditorMode::Play)
        m_EditorMode = EditorMode::Debug;
    else if (m_EditorMode == EditorMode::Debug)
        m_EditorMode = EditorMode::Play;

    m_debugCamera->SetTarget(m_camera->GetTarget());
    m_debugCamera->SetPosition(m_camera->GetPosition());
}
