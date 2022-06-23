#ifndef RENDERWINDOW_H
#define RENDERWINDOW_H

#include <QWindow>
#include <QOpenGLFunctions_4_5_Core>
#include <QTimer>
#include <QElapsedTimer>
#include "shader.h"
#include <unordered_map>
#include "Core/Camera.h"
#include "VisualObject/VisualObject.h"
#include <QCursor>
//#include "Scene/Scene.h"
#include "Core/PointLight.h"
#include "Core/SSBO.h"

class QOpenGLContext;
class Shader;
class Scene;
class MainWindow;
class Mesh;

/// This inherits from QWindow to get access to the Qt functionality and
// OpenGL surface.
// We also inherit from QOpenGLFunctions, to get access to the OpenGL functions
// This is the same as using "glad" and "glw" from general OpenGL tutorials
class RenderWindow : public QWindow, public QOpenGLFunctions_4_5_Core
{
    Q_OBJECT
public:
    RenderWindow(const QSurfaceFormat &format, MainWindow *mainWindow);
    ~RenderWindow() override;
    QOpenGLContext *context() { return mContext; }
    void exposeEvent(QExposeEvent *) override;  //gets called when app is shown and resized
    int Rotation{};
private slots:
    void render();          //the actual render - function
private:
    void init();            //initialize things we need before rendering
    QOpenGLContext *mContext{nullptr};  //Our OpenGL context
    bool mInitialized{false};
    QTimer *mRenderTimer{nullptr};           //timer that drives the gameloop
    MainWindow *mMainWindow{nullptr};        //points back to MainWindow to be able to put info in StatusBar
    class QOpenGLDebugLogger *mOpenGLDebugLogger{nullptr};  //helper class to get some clean debug info from OpenGL
    ///Helper function that uses QOpenGLDebugLogger or plain glGetError()
    void checkForGLerrors();
    double calculateFramerate();
    ///Starts QOpenGLDebugLogger if possible
    void startOpenGLDebugger();
public:
    static RenderWindow* Get();
private:
    inline static RenderWindow* s_renderWindow = nullptr;
public:
    uint32_t LoadTexture(const std::string& texture, bool flip = true);
    Shader& LoadShader(const std::string& shader);
    // should probably have a texture class so i can see if it has transparency.
    // for now just supporting opaque geo properly with instanced drawing.
    struct InstancedSubmitData
    {
        uint32_t elementCount;
        uint32_t IBO;
    };
    void InstancedSubmit(uint32_t vao, uint32_t texture, InstancedSubmitData submitData, const glm::mat4& transform);
    void InstancedPresent();
    std::unordered_map<uint32_t, std::pair<std::vector<glm::mat4>, InstancedSubmitData>> m_instancedData;
    void DrawRect(const glm::mat4& transform, const glm::vec4& color);
    void DrawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color);
    void DrawAABB(const glm::vec3& pos, const glm::vec3& extent = glm::vec3(0.5f), const glm::vec4& color = glm::vec4(0.f, 1.f, 0.f, 1.f));
    void DrawSphere(const glm::vec3& pos, float r, const glm::vec4& color = glm::vec4(0.f, 1.f, 0.f, 1.f));
    void SubdivideSphere(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, int n, std::vector<glm::vec3>& v);
    std::shared_ptr<Camera> GetCamera();
    std::shared_ptr<Camera> GetPlayCamera();
    template<typename T>
    void LoadScene();

    struct PointLightUniform
    {
        uint32_t position;
        uint32_t constant;
        uint32_t linear;
        uint32_t quadratic;
        uint32_t lightRange;
        uint32_t ambient;
        uint32_t diffuse;
        uint32_t specular;
    };
    std::unordered_map<uint32_t, std::vector<PointLightUniform>> m_PointLightUniforms;
    static constexpr uint32_t MAX_LIGHTS = 4096;
    void InitPointLightUniforms();
    void UpdatePointLights(const std::vector<PointLight>& lights);
    std::shared_ptr<SSBOManager> m_ssboManager;
    enum class EditorMode
    {
        Play = 0,
        Debug
    };
    EditorMode GetEditorMode() const { return m_EditorMode; }
private:
    void DebugUpdate(float deltaTime);
    float m_DebugCameraSpeed = 80.f;
    EditorMode m_EditorMode = EditorMode::Play;
    std::unordered_map<std::string, Shader> m_shaders;
    std::unordered_map<std::string, uint32_t> m_textureMap;
    std::unique_ptr<Scene> m_scene;
    std::unique_ptr<Scene> m_nextScene;
    std::shared_ptr<Camera> m_camera;
    std::shared_ptr<Camera> m_debugCamera;
    QCursor m_cursor{};
    class ALCcontext* alContext;
protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
public:
    void GUI_LandscapeUVSlider(int value);
    void GUI_WireframeToggele();
    void GUI_HouseScene();
    void GUI_WoodsScene();
    void GUI_WorldScene();
    void GUI_HeightmapScene();
    void GUI_EksamenScene();
    void GUI_DrawMode();
    void GUI_DebugLines();
    void GUI_PlayDebug();
private:
    uint32_t drawMode{ 0 };
};


template<typename T>
inline void RenderWindow::LoadScene()
{
    m_nextScene = std::make_unique<T>();
}

#endif // RENDERWINDOW_H
