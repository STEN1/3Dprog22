QT          += core gui widgets opengl multimedia

TEMPLATE    = app
CONFIG      += c++17

TARGET      = 3D-programmering

mac {
    LIBS += -framework OpenAL
}

win32 {
    LIBS += $$PWD/AL/OpenAL32.lib

# Copy required DLL to output directory
        CONFIG(debug, debug|release) {
            OpenAL32.commands = copy /Y \"$$PWD\\AL\\OpenAL32.dll\" debug
            OpenAL32.target = debug/OpenAL32.dll

            QMAKE_EXTRA_TARGETS += OpenAL32
            PRE_TARGETDEPS += debug/OpenAL32.dll
        } else:CONFIG(release, debug|release) {
            OpenAL32.commands = copy /Y \"$$PWD\\AL\\OpenAL32.dll\" release
            OpenAL32.target = release/OpenAL32.dll

            QMAKE_EXTRA_TARGETS += OpenAL32
            PRE_TARGETDEPS += release/OpenAL32.dll release/OpenAL32.dll
        } else {
            error(Unknown set of dependencies.)
        }
}


SOURCES += main.cpp \
    Core/Logger.cpp \
    Core/ObjLoader.cpp \
    Core/Octree.cpp \
    Core/PhysicsShapes.cpp \
    Core/SSBO.cpp \
    Core/ShaderManager.cpp \
    Core/SoundManager.cpp \
    Core/SoundSource.cpp \
    Core/TextureManager.cpp \
    Core/Utils.cpp \
    GameObject/AnimatedBezier.cpp \
    GameObject/CameraMesh.cpp \
    GameObject/Door.cpp \
    GameObject/GameObject.cpp \
    GameObject/GraphNPC.cpp \
    GameObject/ItemPickup.cpp \
    GameObject/Player.cpp \
    GameObject/Sky.cpp \
    GameObject/Sun.cpp \
    GameObject/Terrain.cpp \
    GameObject/TextBillboard.cpp \
    Scene/EksamenScene.cpp \
    Scene/HeightmapScene.cpp \
    Scene/HouseScene.cpp \
    Scene/Scene.cpp \
    Scene/WoodsScene.cpp \
    Scene/WorldScene.cpp \
    VisualObject/BezierCurve.cpp \
    VisualObject/Graph.cpp \
    VisualObject/Heightmap.cpp \
    VisualObject/Kube.cpp \
    VisualObject/Mesh.cpp \
    VisualObject/Quad.cpp \
    VisualObject/Skybox.cpp \
    VisualObject/StaticMesh.cpp \
    VisualObject/TriangleSurface.cpp \
    VisualObject/VisualObject.cpp \
    mainwindow.cpp \
    renderwindow.cpp \
    shader.cpp \
    stb_image/stb_image.cpp

HEADERS += \
    AL/al.h \
    AL/alc.h \
    AL/alext.h \
    AL/efx-creative.h \
    AL/efx-presets.h \
    AL/efx.h \
    Core/Camera.h \
    Core/Globals.h \
    Core/Input.h \
    Core/Logger.h \
    Core/ObjLoader.h \
    Core/Octree.h \
    Core/PhysicsShapes.h \
    Core/PointLight.h \
    Core/SSBO.h \
    Core/ShaderManager.h \
    Core/SoundManager.h \
    Core/SoundSource.h \
    Core/TextureManager.h \
    Core/Threadpool/thread_pool.hpp \
    Core/Utils.h \
    GameObject/AnimatedBezier.h \
    GameObject/CameraMesh.h \
    GameObject/Door.h \
    GameObject/GameObject.h \
    GameObject/GraphNPC.h \
    GameObject/ItemPickup.h \
    GameObject/Player.h \
    GameObject/SceneWarp.h \
    GameObject/Sky.h \
    GameObject/Sun.h \
    GameObject/Terrain.h \
    GameObject/TextBillboard.h \
    Scene/EksamenScene.h \
    Scene/HeightmapScene.h \
    Scene/HouseScene.h \
    Scene/Scene.h \
    Scene/WoodsScene.h \
    Scene/WorldScene.h \
    VisualObject/BezierCurve.h \
    VisualObject/Graph.h \
    VisualObject/Heightmap.h \
    VisualObject/Kube.h \
    VisualObject/Mesh.h \
    VisualObject/Quad.h \
    VisualObject/Skybox.h \
    VisualObject/StaticMesh.h \
    VisualObject/TriangleSurface.h \
    VisualObject/VisualObject.h \
    glm/common.hpp \
    glm/exponential.hpp \
    glm/ext.hpp \
    glm/fwd.hpp \
    glm/geometric.hpp \
    glm/glm.hpp \
    glm/gtc/bitfield.hpp \
    glm/gtc/bitfield.inl \
    glm/gtc/color_space.hpp \
    glm/gtc/color_space.inl \
    glm/gtc/constants.hpp \
    glm/gtc/constants.inl \
    glm/gtc/epsilon.hpp \
    glm/gtc/epsilon.inl \
    glm/gtc/integer.hpp \
    glm/gtc/integer.inl \
    glm/gtc/matrix_access.hpp \
    glm/gtc/matrix_access.inl \
    glm/gtc/matrix_integer.hpp \
    glm/gtc/matrix_inverse.hpp \
    glm/gtc/matrix_inverse.inl \
    glm/gtc/matrix_transform.hpp \
    glm/gtc/matrix_transform.inl \
    glm/gtc/noise.hpp \
    glm/gtc/noise.inl \
    glm/gtc/packing.hpp \
    glm/gtc/packing.inl \
    glm/gtc/quaternion.hpp \
    glm/gtc/quaternion.inl \
    glm/gtc/quaternion_simd.inl \
    glm/gtc/random.hpp \
    glm/gtc/random.inl \
    glm/gtc/reciprocal.hpp \
    glm/gtc/round.hpp \
    glm/gtc/round.inl \
    glm/gtc/type_aligned.hpp \
    glm/gtc/type_precision.hpp \
    glm/gtc/type_precision.inl \
    glm/gtc/type_ptr.hpp \
    glm/gtc/type_ptr.inl \
    glm/gtc/ulp.hpp \
    glm/gtc/ulp.inl \
    glm/gtc/vec1.hpp \
    glm/integer.hpp \
    glm/mat2x2.hpp \
    glm/mat2x3.hpp \
    glm/mat2x4.hpp \
    glm/mat3x2.hpp \
    glm/mat3x3.hpp \
    glm/mat3x4.hpp \
    glm/mat4x2.hpp \
    glm/mat4x3.hpp \
    glm/mat4x4.hpp \
    glm/matrix.hpp \
    glm/packing.hpp \
    glm/trigonometric.hpp \
    glm/vec2.hpp \
    glm/vec3.hpp \
    glm/vec4.hpp \
    glm/vector_relational.hpp \
    mainwindow.h \
    renderwindow.h \
    sMath/sMath.h \
    shader.h \
    stb_image/stb_image.h \

FORMS += \
    mainwindow.ui

DISTFILES += \
    shaders/instanced.frag \
    shaders/instanced.vert \
    shaders/landscape.frag \
    shaders/landscape.vert \
    shaders/light.frag \
    shaders/light.vert \
    shaders/normals.frag \
    shaders/normals.vert \
    shaders/plainshader.frag \
    shaders/plainshader.vert \
    shaders/textured.frag \
    shaders/textured.vert
