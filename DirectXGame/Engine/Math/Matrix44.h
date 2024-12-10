#pragma once
#include <cmath>
#include <assert.h>
#include "Vec3.h"

class Matrix4x4 {
public:

	static const int kMatrixNum = 4;
	float m[kMatrixNum][kMatrixNum];
public:

	inline Matrix4x4() {
		m[0][0] = 1.0f, m[0][1] = 0.0f, m[0][2] = 0.0f, m[0][3] = 0.0f;  //1行目
		m[1][0] = 0.0f, m[1][1] = 1.0f, m[1][2] = 0.0f, m[1][3] = 0.0f;  //2行目
		m[2][0] = 0.0f, m[2][1] = 0.0f, m[2][2] = 1.0f, m[2][3] = 0.0f;  //3行目
		m[3][0] = 0.0f, m[3][1] = 0.0f, m[3][2] = 0.0f, m[3][3] = 1.0f;  //4行目
	}

	inline Matrix4x4(
		float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33) {
		m[0][0] = m00, m[0][1] = m01, m[0][2] = m02, m[0][3] = m03;  //1行目
		m[1][0] = m10, m[1][1] = m11, m[1][2] = m12, m[1][3] = m13;  //2行目
		m[2][0] = m20, m[2][1] = m21, m[2][2] = m22, m[2][3] = m23;  //3行目
		m[3][0] = m30, m[3][1] = m31, m[3][2] = m32, m[3][3] = m33;  //4行目
	}

	friend inline Matrix4x4 operator+(const Matrix4x4& m1, const Matrix4x4& m2) {
		Matrix4x4 result{};
		for (size_t row = 0; row < kMatrixNum; row++) {
			for (size_t column = 0; column < kMatrixNum; column++) {
				result.m[row][column] = m1.m[row][column] + m2.m[row][column];
			}
		}
		return result;
	}

	friend inline Matrix4x4 operator-(const Matrix4x4& m1, const Matrix4x4& m2) {
		Matrix4x4 result{};
		for (size_t row = 0; row < kMatrixNum; row++) {
			for (size_t column = 0; column < kMatrixNum; column++) {
				result.m[row][column] = m1.m[row][column] - m2.m[row][column];
			}
		}
		return result;
	}

	friend inline Matrix4x4 operator*(const Matrix4x4& m1, const Matrix4x4& m2) {
		Matrix4x4 result{};
		for (size_t row = 0; row < kMatrixNum; row++) {
			for (size_t column = 0; column < kMatrixNum; column++) {
				result.m[row][column] = (m1.m[row][0] * m2.m[0][column]) + (m1.m[row][1] * m2.m[1][column]) + (m1.m[row][2] * m2.m[2][column]) + (m1.m[row][3] * m2.m[3][column]);
			}
		}
		return result;
	}

	friend inline Matrix4x4 operator*(const float scalar, const Matrix4x4& matrix) {
		Matrix4x4 result{};
		for (size_t row = 0; row < kMatrixNum; row++) {
			for (size_t column = 0; column < kMatrixNum; column++) {
				result.m[row][column] = matrix.m[row][column] * scalar;
			}
		}
	}

	friend inline Matrix4x4 operator*(const Matrix4x4& matrix, const float scalar) {
		return scalar * matrix;
	}

	inline Matrix4x4 Inverse() {

		Matrix4x4 result{};
		float sweep[kMatrixNum][kMatrixNum * 2]{};

		// 拡大係数行列を作成
		for (size_t row = 0; row < kMatrixNum; row++) {
			for (size_t column = 0; column < kMatrixNum; column++) {
				sweep[row][column] = m[row][column];
				sweep[row][kMatrixNum + column] = (row == column) ? 1.0f : 0.0f;
			}
		}

		// 掃き出し法の適用
		for (size_t k = 0; k < kMatrixNum; k++) {
			// 主成分が0に近い場合、行のスワップを試みる
			if (std::abs(sweep[k][k]) < 1e-6f) {
				for (size_t i = k + 1; i < kMatrixNum; i++) {
					if (std::abs(sweep[i][k]) > 1e-6f) {
						std::swap_ranges(sweep[k], sweep[k] + (kMatrixNum * 2), sweep[i]);
						break;
					}
				}
				
			}

			// ピボットのスケーリング
			float pivot = 1.0f / sweep[k][k];
			for (size_t column = 0; column < (kMatrixNum * 2); column++) {
				sweep[k][column] *= pivot;
			}

			// ピボット行を使用して他の行を掃き出す
			for (size_t row = 0; row < kMatrixNum; row++) {
				if (row == k) continue;
				pivot = -sweep[row][k];
				for (size_t column = 0; column < (kMatrixNum * 2); column++) {
					sweep[row][column] += sweep[k][column] * pivot;
				}
			}
		}

		// 逆行列の取得
		for (size_t row = 0; row < kMatrixNum; row++) {
			for (size_t column = 0; column < kMatrixNum; column++) {
				result.m[row][column] = sweep[row][column + kMatrixNum];
			}
		}

		return result;

	}

	inline Matrix4x4 Transpose() const {
		Matrix4x4 result{};
		for (size_t row = 0; row < kMatrixNum; row++) {
			for (size_t column = 0; column < kMatrixNum; column++) {
				result.m[row][column] = m[column][row];
			}
		}
		return result;
	}
};

    inline Matrix4x4 MakeIdentity44() {
		return{
			1.0f,0.0f,0.0f,0.0f,
			0.0f,1.0f,0.0f,0.0f,
			0.0f,0.0f,1.0f,0.0f,
			0.0f,0.0f,0.0f,1.0f
		};
	}

	inline Matrix4x4 MakeTranslateMatrix(const Vector3& translate) {
		return {
			1.0f,0.0f,0.0f,0.0f,
			0.0f,1.0f,0.0f,0.0f,
			0.0f,0.0f,1.0f,0.0f,
			translate.x,translate.y,translate.z,1.0f
		};
	}

	inline Matrix4x4 MakeScaleMatrix(const Vector3& scale) {
		return {
			scale.x,0.0f,0.0f,0.0f,
			0.0f,scale.y,0.0f,0.0f,
			0.0f,0.0f,scale.z,0.0f,
			0.0f,0.0f,0.0f,1.0f
		};
	}

	inline Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix) {
		Vector3 result{};
		result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + 1.0f * matrix.m[3][0];
		result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + 1.0f * matrix.m[3][1];
		result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + 1.0f * matrix.m[3][2];
		float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + 1.0f * matrix.m[3][3];
		assert(w != 0.0f);
		result.x /= w;
		result.y /= w;
		result.z /= w;

		return result;
	}

	inline Matrix4x4 MakeRotateXMatrix(float radian) {
		return {
			1.0f,0.0f,0.0f,0.0f,
			0.0f,std::cosf(radian),std::sinf(radian),0.0f,
			0.0f,-std::sinf(radian),std::cosf(radian),0.0f,
			0.0f,0.0f,0.0f,1.0f
		};
	}

	inline Matrix4x4 MakeRotateYMatrix(float radian) {
		return {
			std::cosf(radian),0.0f,-std::sinf(radian),0.0f,
			0.0f,1.0f,0.0f,0.0f,
			std::sinf(radian),0.0f,std::cosf(radian),0.0f,
			0.0f,0.0f,0.0f,1.0f
		};
	}

	inline Matrix4x4 MakeRotateZMatrix(float radian) {
		return {
			std::cosf(radian),std::sinf(radian),0.0f,0.0f,
			-std::sinf(radian),std::cosf(radian),0.0f,0.0f,
			0.0f,0.0f,1.0f,0.0f,
			0.0f,0.0f,0.0f,1.0f
		};
	}

	inline Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translation) {
		
		Matrix4x4 rotateMatrix = MakeRotateXMatrix(rotate.x) * MakeRotateYMatrix(rotate.y) * MakeRotateZMatrix(rotate.z);
		Matrix4x4 scaleMatrix = MakeScaleMatrix(scale);
		Matrix4x4 translateMatrix = MakeTranslateMatrix(translation);

		return scaleMatrix * rotateMatrix * translateMatrix;
	}

	inline Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip) {
		Matrix4x4 result{};
		result.m[0][0] = (1.0f / aspectRatio) * (1.0f / std::tanf(fovY / 2.0f));
		result.m[1][1] = (1.0f / std::tanf(fovY / 2.0f));
		result.m[2][2] = farClip / (farClip - nearClip);
		result.m[2][3] = 1.0f;
		result.m[3][2] = (-nearClip * farClip) / (farClip - nearClip);

		return result;
	}

	inline Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip) {
		Matrix4x4 result{};
		result.m[0][0] = 2.0f / (right - left);
		result.m[1][1] = 2.0f / (top - bottom);
		result.m[2][2] = 1.0f / (farClip - nearClip);
		result.m[3][0] = (left + right) / (left - right);
		result.m[3][1] = (top + bottom) / (bottom - top);
		result.m[3][2] = nearClip / (nearClip - farClip);
		result.m[3][3] = 1.0f;

		return result;
	}

	inline Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth = 0.0f, float maxDepth = 1.0f) {
		Matrix4x4 result{};
		float w = width / 2.0f;
		float h = height / 2.0f;

		result.m[0][0] = w;
		result.m[1][1] = -h;
		result.m[2][2] = maxDepth - minDepth;
		result.m[3][0] = left + w;
		result.m[3][1] = top + h;
		result.m[3][2] = minDepth;
		result.m[3][3] = 1.0f;

		return result;
	}

	inline Vector3 TransformNormal(const Vector3& vector, const Matrix4x4& matrix) {
	    return {
	        vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0],
	        vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1],
	        vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2]};
    }

	inline Matrix4x4 MakeRotateAxisAngle(const Vector3& axis, float angle) {
		Matrix4x4 result = MakeIdentity44();

		result.m[0][0] = std::powf(axis.x, 2.0f) * (1.0f - std::cosf(angle)) + cos(angle);
		result.m[0][1] = axis.x * axis.y * (1.0f - std::cosf(angle)) + axis.z * std::sinf(angle);
		result.m[0][2] = axis.x * axis.z * (1.0f - std::cosf(angle)) - axis.y * std::sinf(angle);

		result.m[1][0] = axis.x * axis.y * (1.0f - std::cosf(angle)) - axis.z * std::sinf(angle);
		result.m[1][1] = std::powf(axis.y, 2.0f) * (1.0f - std::cosf(angle)) + cos(angle);
		result.m[1][2] = axis.y * axis.z * (1.0f - std::cosf(angle)) + axis.x * std::sinf(angle);

		result.m[2][0] = axis.x * axis.z * (1.0f - std::cosf(angle)) + axis.y * std::sinf(angle);
		result.m[2][1] = axis.y * axis.z * (1.0f - std::cosf(angle)) - axis.x * std::sinf(angle);
		result.m[2][2] = std::powf(axis.z, 2.0f) * (1.0f - std::cosf(angle)) + cos(angle);

		return result;
	}

	inline Matrix4x4 DirectionToDirection(const Vector3& from, const Vector3& to) {
		Matrix4x4 result = MakeIdentity44();
		Vector3 axis{};

		Vector3 fn = from.Normalize();
		Vector3 tn = to.Normalize();
		
		if (fn == -tn) {
			if (fn.x != 0 || fn.z != 0) {
				axis = { 0.0f,fn.z,-fn.x };
			}
			else {
				axis = Cross(fn, tn).Normalize();
			}
		}else if (fn == tn) {
			return MakeIdentity44();
		}
		else {
			axis = Cross(fn, tn).Normalize();
		}

		
		float cos = Dot(fn, tn);
		float sin = Cross(fn, tn).Length();


		result.m[0][0] = std::powf(axis.x, 2) * (1.0f - cos) + cos;
		result.m[0][1] = axis.x * axis.y * (1.0f - cos) + axis.z * sin;
		result.m[0][2] = axis.x * axis.z * (1.0f - cos) - axis.y * sin;

		result.m[1][0] = axis.x * axis.y * (1.0f - cos) - axis.z * sin;
		result.m[1][1] = std::powf(axis.y, 2) * (1.0f - cos) + cos;
		result.m[1][2] = axis.y * axis.z * (1.0f - cos) + axis.x * sin;

		result.m[2][0] = axis.x * axis.z * (1.0f - cos) + axis.y * sin;
		result.m[2][1] = axis.y * axis.z * (1.0f - cos) - axis.x * sin;
		result.m[2][2] = std::powf(axis.z, 2) * (1.0f - cos) + cos;

		return result;
	}

	inline Vector3 MakeScale(const Matrix4x4& matrix) {

		Vector3 scaleX = { matrix.m[0][0],matrix.m[0][1] ,matrix.m[0][2] };
		Vector3 scaleY = { matrix.m[1][0],matrix.m[1][1] ,matrix.m[1][2] };
		Vector3 scaleZ = { matrix.m[2][0],matrix.m[2][1] ,matrix.m[2][2] };
		Vector3 result;

		result.x = scaleX.Length();
		result.y = scaleY.Length();
		result.z = scaleZ.Length();

		return result;
	}
