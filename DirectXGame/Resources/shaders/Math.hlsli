
float32_t4x4 MakeRotateXMat(float32_t angleRad){
    return float32_t4x4(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, cos(angleRad), sin(angleRad), 0.0f,
        0.0f,-sin(angleRad), cos(angleRad), 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );
}

float32_t4x4 MakeRotateYMat(float32_t angleRad){
    return float32_t4x4(
        cos(angleRad), 0.0f,-sin(angleRad), 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        sin(angleRad), 0.0f, cos(angleRad), 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );
}

float32_t4x4 MakeRotateZMat(float32_t angleRad){
    return float32_t4x4(
        cos(angleRad), sin(angleRad), 0.0f, 0.0f,
        -sin(angleRad), cos(angleRad), 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );
}

//angleはradian
float32_t4x4 MakeRotateAxisAngle(float32_t3 axis, float32_t angle) {
    float32_t4x4 rotateMat;
    float32_t cosA = cos(angle);
    float32_t sinA = sin(angle);

    rotateMat[0][0] = pow(axis.x, 2.0f) * (1.0f - cosA) + cosA;
    rotateMat[0][1] = axis.x * axis.y * (1.0f - cosA) + axis.z * sinA;
    rotateMat[0][2] = axis.x * axis.z * (1.0f - cosA) - axis.y * sinA;
    rotateMat[0][3] = 0.0f;

    rotateMat[1][0] = axis.x * axis.y * (1.0f - cosA) - axis.z * sinA;
    rotateMat[1][1] = pow(axis.y, 2.0f) * (1.0f - cosA) + cosA;
    rotateMat[1][2] = axis.y * axis.z * (1.0f - cosA) + axis.x * sinA;
    rotateMat[1][3] = 0.0f;

    rotateMat[2][0] = axis.x * axis.z * (1.0f - cosA) + axis.y * sinA;
    rotateMat[2][1] = axis.y * axis.z * (1.0f - cosA) - axis.x * sinA;
    rotateMat[2][2] = pow(axis.z, 2.0f) * (1.0f - cosA) + cosA;
    rotateMat[2][3] = 0.0f;

    rotateMat[3][0] = 0.0f;
    rotateMat[3][1] = 0.0f;
    rotateMat[3][2] = 0.0f;
    rotateMat[3][3] = 1.0f;

    return rotateMat;
}