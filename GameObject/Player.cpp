#include "Player.h"

#include "Core/ShaderManager.h"
#include "Core/Input.h"
#include "Core/TextureManager.h"
#include "VisualObject/Kube.h"
#include "Core/Utils.h"
#include "VisualObject/Mesh.h"
#include "Core/Globals.h"

Player::Player(Scene& scene, std::shared_ptr<class Camera> camera)
    : GameObject(scene, glm::mat4(1.f))
    , m_camera(camera)
{
    m_vo = std::make_unique<Mesh>(*this,
        Globals::AssetPath + std::string("Ball.obj"),
        TextureManager::GetTexture("testtexture2.png"));
    objectType = ObjectType::Simulated;
    m_name = "Player";
}

void Player::Update(float deltaTime)
{
    // Check for input
    float forwardInput{};
    float rightInput{};
    if (Input::Keyboard[Qt::Key_Up] || Input::Keyboard[Qt::Key_W])
        forwardInput += 1.f;
    if (Input::Keyboard[Qt::Key_Down] || Input::Keyboard[Qt::Key_S])
        forwardInput -= 1.f;
    if (Input::Keyboard[Qt::Key_Right] || Input::Keyboard[Qt::Key_D])
        rightInput += 1.f;
    if (Input::Keyboard[Qt::Key_Left] || Input::Keyboard[Qt::Key_A])
        rightInput -= 1.f;

    if (Input::Keyboard[Qt::Key_Space] && m_Grounded)
    {
        auto vel = GetVelocity();
        vel.y = m_JumpForce;
        SetVelocity(vel);
        LOG("Jump!");
    }
    else if (!Input::Keyboard[Qt::Key_Space])
    {
        auto vel = GetVelocity();
        vel.y -= 20.f * deltaTime;
        SetVelocity(vel);
    }

    if (Input::Keyboard[Qt::Key_P])
    {
        LOG_HIGHLIGHT("Pos: " + std::to_string(GetPosition().x) + ", " + std::to_string(GetPosition().y)
            + ", " + std::to_string(GetPosition().z));
    }

    auto [mouseX, mouseY] = Input::MousePos;
    static auto oldMouseX = mouseX;
    static auto oldMouseY = mouseY;
    /*float mouseDx = oldMouseX - mouseX;
    float mouseDy = oldMouseY - mouseY;*/
    auto [mouseDx, mouseDy] = Input::MousePosDelta;
    if (Input::Mouse[Qt::RightButton])
    {
        //QCursor::setPos(oldMouseX, oldMouseY);
    }
    else
    {
        oldMouseX = mouseX;
        oldMouseY = mouseY;
    }
    if (auto camera = m_camera.get())
    {
        // Getting direction to move from camera vectors.
        auto cameraForward = camera->GetCameraForward();
        auto cameraRight = camera->GetCameraRight();

        auto forward = cameraForward * forwardInput;
        auto right = cameraRight * rightInput;

        auto wishDir = forward; // + right;
        wishDir = { wishDir.x, 0.f, wishDir.z };
        if (glm::length(wishDir) > 0.01f)
        {
            wishDir = glm::normalize(wishDir);
        }
        else
        {
            wishDir = glm::vec3(0.f);
        }

        // Moving in desired direction.
        AddPositionOffset(wishDir * MoveSpeed * deltaTime);

        // Snapping cube to surface function.
        // Doing this in Scene::Simulate now.
        //constexpr float yOffset = 2.f;
        //auto playerPos = GetPosition();
        //float playerY = Scene::TerrainFunc(playerPos.x, playerPos.z);
        //SetPosition(glm::vec3(playerPos.x, playerY + yOffset, playerPos.z));

        // Getting rotation to use on camera from mouse.
        static float currentRotationX{};
        static float currentRotationY{ -0.2f };
        if (Input::Mouse[Qt::RightButton])
        {
            //currentRotationX += mouseDx / 200.f;
            currentRotationY += mouseDy / 200.f;
            currentRotationY = std::clamp<float>(currentRotationY, -glm::half_pi<float>() + 0.2f, glm::half_pi<float>() - 0.2f);
        }
        currentRotationX -= rightInput * TurnSpeed * deltaTime;

        m_cameraRadius -= Input::LastMouseWheelDelta / 8.f;
        m_cameraRadius = std::clamp(m_cameraRadius, 5.f, 5000.f);

        // Use matrix to rotate from mouse input.
        glm::mat4 camPos(1.f);
        camPos = glm::rotate(camPos, currentRotationX, { 0.f, 1.f, 0.f });
        camPos = glm::rotate(camPos, currentRotationY, { 1.f, 0.f, 0.f });
        camPos = glm::translate(camPos, { 0.f, 0.f, m_cameraRadius });
        glm::vec3 camPosV = camPos * glm::vec4{ 0.f, 0.f, 0.f, 1.f };

        // Move the rotated camera to the player.
        camPosV = camPosV + GetPosition();

        float minCameraY = m_scene.GetHeightFromHeightmap(camPosV) + 1.f;
        if (minCameraY > camPosV.y)
            camPosV.y = minCameraY;

        camera->SetPosition(camPosV);
        camera->SetTarget(GetPosition());

        // rotate player like a ball :D
        if (m_vo && wishDir != glm::vec3(0.f))
        {
            auto rotationAxis = glm::cross(wishDir, {0.f, 1.f, 0.f});
            m_currentRotation += deltaTime * m_cameraRadius;
            auto rotationMatrix = glm::rotate(glm::mat4(1.f), -m_currentRotation, rotationAxis);
            m_vo->SetLocalTransform(rotationMatrix);
        }

        //RenderWindow::Get()->DrawAABB(playerPos, m_physicsBox.extent, {0.5f, 0.5f, 1.f, 1.f});
    }
    else { LOG_ERROR("NO CAMERA?!"); }
}

void Player::EndOverlap(GameObject* other)
{
    if (other->GetName() == "ItemPickup" || other->GetName() == "RedTrophy")
    {
        LOG_HIGHLIGHT("SCORE: " + std::to_string(++m_score));
    }
}