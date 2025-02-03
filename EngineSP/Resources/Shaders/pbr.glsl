
#version 410 core
layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_uv;
layout (location = 3) in vec3 a_tangent;
layout (location = 4) in vec3 a_bitangent;
//-----------------Structs-----------------//
out Vertex
{
	vec3 Position;
	vec3 Normal;
    vec2 uv;
    mat3 TBN;
} vertex;
//-----------------Uniforms-----------------//
uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_proj;
//-----------------Main-----------------//
void main(){

	vertex.Normal = mat3(u_model) * a_normal;
	vertex.uv = a_uv;
	vertex.Position = vec3(u_model * vec4(a_position, 1.0)); 
    vertex.TBN = mat3(u_model) * mat3(a_tangent, a_bitangent, a_normal);
	gl_Position = u_proj * u_view * u_model * vec4(a_position,1.0);

};
//VERTEX//

#version 410 core
layout (location = 0) out vec4 out_fragment;
layout (location = 1) out vec4 out_brightness;

#define PI 3.14159265358979323846
#define MAX_LIGHTS 10
const vec3 BLOOM_THRESHOD = vec3(0.2126, 0.7152, 0.0722);
//-----------------IN-----------------//
in Vertex
{
	vec3 Position;
	vec3 Normal;
    vec2 uv;
    mat3 TBN;

} vertex;
//-----------------Structs-----------------//
struct Material 
{

 vec3 albedo;
 vec3 emissive;

 float roughness;
 float metalness;
 float occlusion;

 sampler2D OcclusionMap;
 bool UseOcclusionMap;
 sampler2D RoughnessMap;
 bool UseRoughnessMap;
 sampler2D MetalnessMap;
 bool UseMetalnessMap;
 sampler2D EmissiveMap;
 bool UseEmissiveMap;
 sampler2D AlbedoMap;
 bool UseAlbedoMap;
 sampler2D NormalMap;
 bool UseNormalMap;

};

// point light type
struct PointLight
{
  float Intensity;
  vec3 Position;
  vec3 Radiance;
};
// direct light type
struct DirectLight
{
  float Intensity;
  vec3 Direction;
  vec3 Radiance;
};
// spot light type
struct SpotLight
{
  float Intensity;
  vec3 Direction;
  vec3 Position;
  vec3 Radiance;
  float FallOff;
  float CutOff;
};

//-----------------Uniforms-----------------//

uniform Material u_material;
uniform vec3 u_viewPos;

//light uniforms
uniform PointLight u_pointLights[MAX_LIGHTS]; 
uniform int u_nbrPointLight; 

uniform DirectLight u_directLights[MAX_LIGHTS]; 
uniform int u_nbrDirectLight; 

uniform SpotLight u_spotLights[MAX_LIGHTS]; 
uniform int u_nbrSpotLight; 

// environment 
uniform samplerCube u_irradMap;
uniform samplerCube u_prefilMap; 
uniform sampler2D u_brdfMap; 

//shadow
uniform sampler2D u_depthMap; 
uniform mat4 u_lightSpace;
uniform float u_shadowBias;

//-----------------Functions-----------------//

// computes fresnel reflectivity
vec3 FresnelSchlick(float cosTheta, vec3 F0) 
{
//cosTheta is V dot H
  return F0 + (vec3(1.0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// computes geometry sclick ggx 
float GeometrySchlickGGX(float NdotV, float roughness) 
{
  float k = pow(roughness + 1.0, 2.0) / 8.0;
  return NdotV / (NdotV * (1.0 - k) + k);
}
// computes geometry smith ggx
float GeometrySmithGGX(float NdotV, float NdotL, float roughness) 
{
  return GeometrySchlickGGX(NdotV, roughness) * GeometrySchlickGGX(NdotL, roughness);
}

// computes distribution ggx
float DistributionGGX(vec3 N, vec3 H, float roughness) 
{
  float a = roughness * roughness; //linear roughness
  float a2 = a * a;
  float NdotH = max(dot(N, H), 0.0);
  float denom = (NdotH * NdotH) * (a2 - 1.0) + 1.0;
  return a2 / (PI * denom * denom);
}
// compute point lights
vec3 ComputePointLights(vec3 N, vec3 V, vec3 F0, vec3 albedo, float roughness, float metalness) 
{
  vec3 result = vec3(0.0);

  for (int i = 0; i < u_nbrPointLight; ++i) 
  {
    // compute parameters
    vec3 L = normalize(u_pointLights[i].Position - vertex.Position);// light direction
    vec3 H = normalize(L + V);
    float NdotL = clamp(dot(N, L), 0.0, 1.0 ); //light cannot be negative (but the dot product can)      
    float NdotV = clamp(dot(N, V), 0.0, 1.0);    

    // Cook-Torrance (BRDF)
    float NDF = DistributionGGX(N, H, roughness);   
    vec3 FS = FresnelSchlick(max(dot(H, V), 0.0), F0);
    float GS = GeometrySmithGGX(NdotV, NdotL, roughness);      

    // diffuse light
    vec3 diffuse = (vec3(1.0) - FS) * (1.0 - u_material.metalness);

    // specular light
    vec3 specular = (NDF * GS * FS) / max(4.0 * NdotV * NdotL, 0.0001); 

    // light attenuation
    float distance = length(u_pointLights[i].Position - vertex.Position);
    float attenuation = u_pointLights[i].Intensity / (distance * distance); 

    //linear attenuation factor

//    float attenuation = u_pointLights[i].Intensity - distance;
//    attenuation = attenuation / u_pointLights[i].Intensity;
//    attenuation = max(attenuation,0.0);

    // combine components
    result += (diffuse * albedo / PI + specular) * u_pointLights[i].Radiance * attenuation * NdotL; 

    // break if max light
    if (i >= MAX_LIGHTS - 1) { break; }
  }

  return result;
}
// compute direct lights
vec3 ComputeDirectLights(vec3 N, vec3 V, vec3 F0, vec3 albedo, float roughness, float metalness) 
{
  vec3 result = vec3(0.0);

  for (int i = 0; i < u_nbrDirectLight; ++i) 
  {
    // compute parameters
    vec3 L = normalize(-u_directLights[i].Direction);
    vec3 H = normalize(L + V);
    float NdotL = max(dot(N, L), 0.0);       
    float NdotV = max(dot(N, V), 0.0);    

    // Cook-Torrance (BRDF)
    float NDF = DistributionGGX(N, H, roughness);   
    vec3 FS = FresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);
    float GS = GeometrySmithGGX(NdotV, NdotL, roughness);      

    // diffuse light
    vec3 diffuse = (vec3(1.0) - FS) * (1.0 - metalness);

    // specular light
    vec3 specular = (NDF * GS * FS) / max(4.0 * NdotV * NdotL, 0.0001); 
    
    // combine components
    result += (diffuse * albedo / PI + specular) * u_directLights[i].Radiance * NdotL * u_directLights[i].Intensity; 

    // break if max light
    if (i >= MAX_LIGHTS - 1) { break; }
  }

  return result;
}
// compute spot lights
vec3 ComputeSpotLights(vec3 N, vec3 V, vec3 F0, vec3 albedo, float roughness, float metalness) 
{
  vec3 result = vec3(0.0);

  for (int i = 0; i < u_nbrSpotLight; ++i) 
  {
    // compute parameters
    vec3 L = normalize(u_spotLights[i].Position - vertex.Position);
    float NdotL = max(dot(N, L), 0.0);       
    float NdotV = max(dot(N, V), 0.0);    
    vec3 H = normalize(L + V);

    // Cook-Torrance (BRDF)
    float NDF = DistributionGGX(N, H, roughness);   
    vec3 FS = FresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);
    float GS = GeometrySmithGGX(NdotV, NdotL, roughness);      

    // diffuse light
    vec3 diffuse = (vec3(1.0) - FS) * (1.0 - metalness);

    // specular light
    vec3 specular = (NDF * GS * FS) / max(4.0 * NdotV * NdotL, 0.0001); 

    // compute spot
    float theta = dot(L, normalize(-u_spotLights[i].Direction));
    float epsilon = (u_spotLights[i].FallOff - u_spotLights[i].CutOff);
    float spotFactor = clamp((theta - u_spotLights[i].CutOff)/epsilon, 0.0, 1.0);
    
    // light attenuation
    float distance = length(u_spotLights[i].Position - vertex.Position);
    float attenuation = u_spotLights[i].Intensity / (distance * distance); 

    // combine components
    result += (diffuse * albedo / PI + specular) * u_spotLights[i].Radiance * 
    u_spotLights[i].Intensity * attenuation * NdotL * spotFactor; 

    // break if max light
    if (i >= MAX_LIGHTS - 1) { break; }
  }

  return result;
}
// compute ambient lights
vec3 ComputeAmbientLight(vec3 N, vec3 V, vec3 F0, vec3 albedo, float roughness, float metalness) 
{
  // angle between surface normal and light direction.
	float cosTheta = max(0.0, dot(N, V));
	
	// get diffuse contribution factor 
	vec3 F = FresnelSchlick(cosTheta, F0);
	vec3 Kd = mix(vec3(1.0) - F, vec3(0.0), metalness);

	// irradiance map contains exitant radiance 
	vec3 diffuseIBL = Kd * albedo * texture(u_irradMap, N).rgb;

	// sample pre-filtered map at correct mipmap level.
	int mipLevels = 5;
	vec3 Lr = 2.0 * cosTheta * N - V;
	vec3 Ks = textureLod(u_prefilMap, Lr, roughness * mipLevels).rgb;

	// split-sum approx.factors for Cook-Torrance specular BRDF.
	vec2 brdf = texture(u_brdfMap, vec2(cosTheta, roughness)).rg;
	vec3 specularIBL = (F0 * brdf.x + brdf.y) * Ks;

	return (diffuseIBL + specularIBL);	
}
// compute shadow
float ComputeShadow()
{
  vec4 position = u_lightSpace * vec4(vertex.Position, 1.0); 
  vec3 coords = (position.xyz / position.w) * 0.5 + 0.5; //perform perspective divide, transform to [0,1] range

  // pixel size (1024 -> map size)
  float pixelSize = 1.0/1024;
  float shadow = 0.0;
  // compute average pcf
  for(int x = -1; x <= 1; ++x)
  {
    for(int y = -1; y <= 1; ++y)
    {
      float depth = texture(u_depthMap, coords.xy + vec2(x, y) * pixelSize).r; 
      shadow += (position.z - u_shadowBias) > depth ? 0.7: 0.0;        
    }    
  }
  shadow /= 9.0;

  if(coords.z > 1.0)
    shadow = 0.0;

  return shadow;
}
//-----------------Main-----------------//
void main(){
	
	//Lambertian diffuse
	vec3 N = normalize(vertex.Normal);

    vec3 V = normalize( u_viewPos - vertex.Position);

    if(u_material.UseNormalMap) 
    { 
        // convert from [0,1] range to [-1, 1] range
        N = 2.0 * texture(u_material.NormalMap, vertex.uv).rgb - 1.0;
        N = normalize(vertex.TBN * N); 
    }

    // material roughness
    float roughness = u_material.roughness;
    if(u_material.UseRoughnessMap)
    {
    roughness = texture(u_material.RoughnessMap, vertex.uv).r;
    }

    // material occlusion
    float occlusion = u_material.occlusion;
    if(u_material.UseOcclusionMap)
    {
    occlusion = texture(u_material.OcclusionMap, vertex.uv).r;
    }

    // material metallic
    float metalness = u_material.metalness;
    if(u_material.UseMetalnessMap)
    {
    metalness = texture(u_material.MetalnessMap, vertex.uv).r;
    }

    // material emissivness
    vec3 emissive = u_material.emissive;
    if(u_material.UseEmissiveMap)
    {
    emissive = texture(u_material.EmissiveMap, vertex.uv).rgb;
    }

    // material albedo 
    vec3 albedo = u_material.albedo;
    if(u_material.UseAlbedoMap)
    {
    albedo = texture(u_material.AlbedoMap, vertex.uv).rgb;
    }

    vec3 F0 = mix(vec3(0.04), albedo, metalness);

    vec3 result = ComputePointLights(N, V, F0, albedo, roughness, metalness) + 
                  ComputeDirectLights(N, V, F0, albedo, roughness, metalness) + 
                  ComputeSpotLights(N, V, F0, albedo, roughness, metalness) +
                  ComputeAmbientLight(N, V, F0, albedo, roughness, metalness);

    result = (result * occlusion) + emissive;

    result *= (1.0 - ComputeShadow());
     // output brightness 
     //dot product = 1 means parallel vectors
     //dot product = 0 means perpendicular vectors
     //dot product >1 means upper quadrants (0-180deg)
    if(dot(result, BLOOM_THRESHOD) > 1.0) 
    {
        out_brightness = vec4(result, 1.0);
    }
    else
    {
        out_brightness = vec4(0.0, 0.0, 0.0, 1.0); 
    }

	out_fragment = vec4(result, 1.0);
}
//FRAGMENT//
