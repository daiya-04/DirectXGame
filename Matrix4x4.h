#pragma once
#include"Vector3.h"

const int kMatrixNum = 4;

typedef struct {
	float m[kMatrixNum][kMatrixNum];
}Matrix4x4;

//加算
Matrix4x4 Add(const Matrix4x4& m1, const Matrix4x4& m2);
//減算
Matrix4x4 Sub(const Matrix4x4& m1, const Matrix4x4& m2);
//乗算
Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);
//逆行列
Matrix4x4 Inverse(const Matrix4x4& m);
//転置行列
Matrix4x4 Transpose(const Matrix4x4& m);
//単位行列
Matrix4x4 MakeIdentity44();
//平行移動行列
Matrix4x4 MakeTranslateMatrix(const Vector3& translate);
//拡大縮小行列
Matrix4x4 MakeScaleMatrix(const Vector3& scale);
//座標変換
//Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix);
//X軸回転行列
Matrix4x4 MakerotateXMatrix(float radian);
//Y軸回転行列
Matrix4x4 MakerotateYMatrix(float radian);
//Z軸回転行列
Matrix4x4 MakerotateZMatrix(float radian);
//3次元アフィン変換行列
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);
//透視投影行列
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);
//正射影行列
Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);
//ビューポート変換行列
Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth = 0.0f, float maxDepth = 1.0f);

//void MatrixScreenPrintf(int x, int y, const Matrix44& matrix,const char* label);