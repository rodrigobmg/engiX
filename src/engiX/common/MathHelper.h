//***************************************************************************************
// MathHelper.h by Frank Luna (C) 2011 All Rights Reserved.
//
// Helper math class.
//***************************************************************************************

#pragma  once

#include <Windows.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include "Precision.h"
#include <crtdbg.h>

namespace engiX
{
    class MathHelper
    {
    public:
        // Returns random real in [0, 1).
        static real RandF()
        {
            return (real)(rand()) / (real)RAND_MAX;
        }

        // Returns random real in [a, b).
        static real RandF(real a, real b)
        {
            return a + RandF()*(b-a);
        }

        template<typename T>
        static T Min(const T& a, const T& b)
        {
            return a < b ? a : b;
        }

        template<typename T>
        static T Max(const T& a, const T& b)
        {
            return a > b ? a : b;
        }

        template<typename T>
        static T Lerp(const T& a, const T& b, real t)
        {
            return a + (b-a)*t;
        }

        template<typename T>
        static T Clamp(const T& x, const T& low, const T& high)
        {
            return x < low ? low : (x > high ? high : x); 
        }

        // Returns the polar angle of the point (x,y) in [0, 2*PI).
        static real AngleFromXY(real x, real y);

        static DirectX::XMMATRIX InverseTranspose(DirectX::CXMMATRIX M)
        {
            // Inverse-transpose is just applied to normals.  So zero out 
            // translation row so that it doesn't get into our inverse-transpose
            // calculation--we don't want the inverse-transpose of the translation.
            DirectX::XMMATRIX A = M;
            A.r[3] = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

            DirectX::XMVECTOR det = XMMatrixDeterminant(A);
            return XMMatrixTranspose(XMMatrixInverse(&det, A));
        }

        // Spherical Coordinates (radius r, inclination Theta, azimuth Phi)
        // Radius r: The radius of the spherical coordinate system
        // Inclination Theta: Rotation angle in radians around Y axis in the XZ plane, Theta = [0, 2Pi]
        // Azimuth Phi: Rotation angle in radians around the axis between the sphere center and the rotate point around the Y axis, Phi = [0, Pi]
        // The method assumes the provided Theta and Phi are within the correct range
        // Conversion formulas: http://en.wikipedia.org/wiki/Spherical_coordinate_system#Cartesian_coordinates
        //
        // In a LH coordinate system like DX, the rotation angle is counter clock wise and the reference angle for
        // rotating around the Y axis in the XZ plane is the counter clock wise angle with the vector (1.0, 0.0, 0.0)
        //
        //
        static void ConvertSphericalToCartesian(_In_ const real& sphericalRadius, _In_ const real& sphericalTheta, _In_ const real& sphericalPhi, _Out_ DirectX::XMFLOAT3& cartesianXyz)
        {
            _ASSERTE(sphericalRadius >= 0.0f);
            _ASSERTE(sphericalTheta >= 0.0f);
            _ASSERTE(sphericalTheta <= DirectX::g_XMTwoPi.f[0]);
            _ASSERTE(sphericalPhi >= 0.0f);
            _ASSERTE(sphericalPhi <= DirectX::g_XMPi.f[0]);

            cartesianXyz.x = sphericalRadius * real_sin(sphericalPhi) * real_cos(sphericalTheta);
            cartesianXyz.z = sphericalRadius * real_sin(sphericalPhi) * real_sin(sphericalTheta);
	        cartesianXyz.y = sphericalRadius * real_cos(sphericalPhi);
        }

        static DirectX::XMVECTOR RandUnitVec3();
        static DirectX::XMVECTOR RandHemisphereUnitVec3(DirectX::XMVECTOR n);

        static const real Infinity;
        static const real Pi;


    };
}