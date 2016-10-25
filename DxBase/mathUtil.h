#pragma once
#include <DirectXMath.h>
#include "math3d.h"

#define M_PI 3.14159265358979323846


namespace MathUtil
{


	inline float parBezFunc(float t, float p1, float p2) //parameterized Bezier Curve Function
	{
		return (3 * ((1 - t)*(1 - t))*t*p1) + (3 * (1 - t)*(t*t)*p2) + (t*t*t);
	}

	



	inline DirectX::XMMATRIX compose(const float rotate[4], const float position[3])
	{
		float w = rotate[3], x = rotate[0], y = rotate[1], z = rotate[2];
		float n = rotate[3] * rotate[3] + rotate[0] * rotate[0] + rotate[1] * rotate[1] + rotate[2] * rotate[2];
		float s = equal(n, 0.f) ? 0.f : 2.f / n;
		float wx = s * w * x, wy = s * w * y, wz = s * w * z;
		float xx = s * x * x, xy = s * x * y, xz = s * x * z;
		float yy = s * y * y, yz = s * y * z, zz = s * z * z;
		DirectX::XMFLOAT4X4 mw(1 - (yy + zz), xy - wz, xz + wy, 0,
		                       xy + wz, 1 - (xx + zz), yz - wx, 0,
		                       xz - wy, yz + wx, 1 - (xx + yy), 0,
		                       position[0], position[1], position[2], 1);

		return DirectX::XMMatrixTranspose(XMLoadFloat4x4(&mw));
	}

	inline DirectX::XMMATRIX compose(vec4 rotate, vec3 position)
	{
		float w = rotate.w, x = rotate.x, y = rotate.y, z = rotate.z;
		float n = rotate.w * rotate.w + rotate.x* rotate.x + rotate.y * rotate.y + rotate.z * rotate.z;
		float s = equal(n, 0.f) ? 0.f : 2.f / n;
		float wx = s * w * x, wy = s * w * y, wz = s * w * z;
		float xx = s * x * x, xy = s * x * y, xz = s * x * z;
		float yy = s * y * y, yz = s * y * z, zz = s * z * z;
		DirectX::XMFLOAT4X4 mw(1 - (yy + zz), xy - wz, xz + wy, 0,
			xy + wz, 1 - (xx + zz), yz - wx, 0,
			xz - wy, yz + wx, 1 - (xx + yy), 0,
			position.x, position.y, position.z, 1);

		return DirectX::XMMatrixTranspose(XMLoadFloat4x4(&mw));
	}
	inline DirectX::XMMATRIX compose(vec4 rotate)
	{
		float w = rotate.w, x = rotate.x, y = rotate.y, z = rotate.z;
		float n = rotate.w * rotate.w + rotate.x* rotate.x + rotate.y * rotate.y + rotate.z * rotate.z;
		float s = equal(n, 0.f) ? 0.f : 2.f / n;
		float wx = s * w * x, wy = s * w * y, wz = s * w * z;
		float xx = s * x * x, xy = s * x * y, xz = s * x * z;
		float yy = s * y * y, yz = s * y * z, zz = s * z * z;
		DirectX::XMFLOAT4X4 mw(1 - (yy + zz), xy - wz, xz + wy, 0,
			xy + wz, 1 - (xx + zz), yz - wx, 0,
			xz - wy, yz + wx, 1 - (xx + yy), 0,
			0,0,0, 1);

		return DirectX::XMMatrixTranspose(XMLoadFloat4x4(&mw));
	}
	inline DirectX::XMMATRIX compose(vec3 position)
	{
		DirectX::XMFLOAT4X4 mw(1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			position.x, position.y, position.z, 1);

		return /*DirectX::XMMatrixTranspose*/(XMLoadFloat4x4(&mw));
	}
	float bezier(float X, float x1, float y1, float x2, float y2);

	inline vec4 Lerp(const vec4 &start, const vec4 &end, float alpha)
	{
		vec4 interp = start * (1-alpha) +  end * alpha;
		interp = normalize(interp);
		return interp;
	}

	inline vec4 Slerp(vec4 &v0, vec4 &v1, float alpha)
	{
		float dot = v0.dot(v1);

		if (dot<0.0f)
		{
			dot = -dot;
			v0 = -v0;
		}

		const float DOT_THRESHOLD = 0.9995f;
		if (dot > DOT_THRESHOLD)
			return Lerp(v0, v1, alpha);

		dot = clamp(dot, -1.0f, 1.0f);
		float theta_0 = acosf(dot);
		float theta = theta_0*alpha;

		vec4 v2 = v1 + -v0*dot;
		v2 = normalize(v2);

		return v0*cos(theta) + v2*sin(theta);
	}

	inline DirectX::XMVECTOR fromMatrixToEuler(const DirectX::XMMATRIX &m) {// Setup the Euler angles, given a rotation matrix.
		float pitch, heading, bank;
		float	sp = -m.r[1].m128_f32[2];// Extract sin(pitch) from _23.
		if (fabs(sp) > 9.99999f) {// Check for Gimbel lock
			pitch = M_PI/2 * sp;// Looking straight up or down
			heading = atan2(-m.r[2].m128_f32[0], m.r[0].m128_f32[0]);// Compute heading, slam bank to zero
			bank = 0.0f;
		}
		else {
			heading = atan2(m.r[0].m128_f32[2], m.r[2].m128_f32[2]);
			pitch = asin(sp);
			bank = atan2(m.r[1].m128_f32[0], m.r[1].m128_f32[1]);
		}
		DirectX::XMVECTOR result;
		result.m128_f32[0] = pitch;
		result.m128_f32[1] = heading;
		result.m128_f32[2] = bank;
		return result;
	}
}
