#pragma once

#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include "engiXDefs.h"
#include "ViewInterfaces.h"
#include "SceneNode.h"

namespace engiX
{
    class GameScene;

    class SceneCameraNode : public SceneNode
    {
    public:
        const static real DefaultNearPlane;
        const static real DefaultFarPlane;
        const static real DefaultFovAngle;

        SceneCameraNode(GameScene* pScene);
        virtual ~SceneCameraNode() {}

        Mat4x4 SceneWorldViewProjMatrix() const;

        // Place the camera in its own space using spherical coordinates (radius r, inclination Theta, azimuth Phi)
        // Radius r: The radius of the spherical coordinate system
        // Inclination Theta: Rotation angle in radians around Y axis in the XZ plane, Theta = [0, 2Pi]
        // Azimuth Phi: Rotation angle in radians around the axis between the sphere center and the rotate point around the Y axis, Phi = [0, Pi]
        // The method assumes the provided Theta and Phi are within the correct range
        // More on the spherical coordinates here: http://en.wikipedia.org/wiki/Spherical_coordinate_system#Cartesian_coordinates
        void PlaceOnSphere(_In_ real radius, _In_ real theta, _In_ real phi, _In_ Vec3 lookat = Vec3(DirectX::g_XMZero));
        HRESULT OnConstruct();
        void OnRender() {}
        HRESULT OnPreRender() { return S_OK; }
        void OnPostRender() {}
        void SetAsThirdPerson(WeakActorPtr target);
        void OnUpdate(_In_ const Timer& time);
        void SetAsThirdPerson(ActorID targetId) { m_targetId = targetId; }

    protected:
        Mat4x4 m_projMat;
        real m_nearPlane;
        real m_farPlane;
        real m_fovAngle;
        Vec3 m_pos;
        Vec3 m_lookat;
        ActorID m_targetId;
    };
}