#include"Object3d.hlsli"

struct Material {
	float32_t4 color;
	int32_t enableLighting;
	float32_t4x4 uvTransform;
	float32_t shininess;
};
ConstantBuffer<Material> gMaterial : register(b0);

struct Camera {
	float32_t4x4 view;
	float32_t4x4 projection;
	float32_t3 cameraPos;
};
ConstantBuffer<Camera> gCamera : register(b2);

struct DirectionalLight {
	float32_t4 color;
	float32_t3 direction;
	float intensity;
};
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b3);

struct PointLight{
	float32_t4 color;
	float32_t3 position;
	float intensity;
	float radius;
	float decay;
};

ConstantBuffer<PointLight> gPointLight : register(b4);

struct SpotLight{
	float32_t4 color;
	float32_t3 position;
	float intensity;
	float32_t3 direction;
	float distance;
	float decay;
	float cosAngle;
	float cosFalloffStart;
};

ConstantBuffer<SpotLight> gSpotLight : register(b5);

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
};

Texture2D<float32_t4> gTexture : register(t0);
TextureCube<float32_t4> gEnvironmentTex : register(t1);
SamplerState gSampler : register(s0);


PixelShaderOutput main(VertexShaderOutput input){
	PixelShaderOutput output;
	//float4 transformedUV = mul(float32_t4(input.texcoord,0.0f,1.0f),gMaterial.uvTransform);
	float32_t4 textureColor = gTexture.Sample(gSampler, input.texcoord);
	

	if(textureColor.a <= 0.5){
		discard;
	}
	if(output.color.a == 0.0){
		discard;
	}

	if(gMaterial.enableLighting != 0) {
		float32_t3 toEye = normalize(gCamera.cameraPos - input.worldPosition);

		float32_t3 halfVector = normalize(-gDirectionalLight.direction + toEye);

		float NdotH = dot(normalize(input.normal), halfVector);
		float specularPow = pow(saturate(NdotH), gMaterial.shininess);

		///Half Lambert
	    float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
	    float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);

		float32_t3 diffuseDL = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
		float32_t3 specularDL = gDirectionalLight.color.rgb * gDirectionalLight.intensity * specularPow * float32_t3(1.0f,1.0f,1.0f);

		//pointLight
		float32_t3 pointLightDirection = normalize(input.worldPosition - gPointLight.position);

		halfVector = normalize(-pointLightDirection + toEye);

		NdotH = dot(normalize(input.normal), halfVector);
		specularPow = pow(saturate(NdotH), gMaterial.shininess);

		///Half Lambert
		NdotL = dot(normalize(input.normal), -pointLightDirection);
		cos = pow(NdotL * 0.5f + 0.5f, 2.0f);

		float distance = length(gPointLight.position - input.worldPosition);
		float factor = pow(saturate(-distance / gPointLight.radius + 1.0f), gPointLight.decay);

		float32_t3 diffusePL = gMaterial.color.rgb * textureColor.rgb * gPointLight.color.rgb * cos * gPointLight.intensity * factor;
		float32_t3 specularPL = gPointLight.color.rgb * gPointLight.intensity * factor * specularPow * float32_t3(1.0f,1.0f,1.0f);

		//SpotLight
		float32_t3 spotLightDirectionOnSurface = normalize(input.worldPosition - gSpotLight.position);

		halfVector = normalize(-spotLightDirectionOnSurface + toEye);

		NdotH = dot(normalize(input.normal), halfVector);
		specularPow = pow(saturate(NdotH), gMaterial.shininess);

		///Half Lambert
		NdotL = dot(normalize(input.normal), -spotLightDirectionOnSurface);
		cos = pow(NdotL * 0.5f + 0.5f, 2.0f);

		float cosAngle = dot(spotLightDirectionOnSurface, gSpotLight.direction);
		float falloffFactor = saturate((cosAngle - gSpotLight.cosAngle) / (gSpotLight.cosFalloffStart - gSpotLight.cosAngle));
		distance = length(gSpotLight.position - input.worldPosition);
		float attenuationFactor = pow(saturate(-distance / gSpotLight.distance + 1.0f), gSpotLight.decay);

		//環境マップ
		float32_t3 cameraToPosition = normalize(input.worldPosition - gCamera.cameraPos);
		float32_t3 reflectedVector = reflect(cameraToPosition,normalize(input.normal));
		float32_t4 environmentColor = gEnvironmentTex.Sample(gSampler,reflectedVector);

		float32_t3 diffuseSL = gMaterial.color.rgb * textureColor.rgb * gSpotLight.color.rgb * cos * gSpotLight.intensity * attenuationFactor * falloffFactor;
		float32_t3 specularSL = gSpotLight.color.rgb * gSpotLight.intensity * attenuationFactor * falloffFactor * specularPow * float32_t3(1.0f,1.0f,1.0f);
		
		//合計
		output.color.rgb = diffuseDL + specularDL + diffusePL + specularPL + diffuseSL + specularSL + (environmentColor.rgb * 0.2f);
        output.color.a = gMaterial.color.a * textureColor.a;
	} else {
	    output.color = gMaterial.color * textureColor;
	}
	return output;
}