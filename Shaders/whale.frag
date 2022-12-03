#version 330 core

uniform sampler2D tex0;
uniform sampler2D tex1;

uniform vec3 dirlightPos;
uniform vec3 dirlightColor;
uniform float dirambientStr;
uniform float dirspecStr;
uniform float dirspecPhong;
uniform float dirintensityStr;

uniform vec3 pntlightPos;
uniform vec3 pntlightColor;
uniform float pntambientStr;
uniform float pntspecStr;
uniform float pntspecPhong;
uniform float pntintensityStr;

uniform vec3 cameraPos;

in vec2 texCoord;
in vec3 normCoord;
in vec3 fragPos;
out vec4 FragColor;

void main() {	
	// Normalize Values
	vec3 normal = normalize(normCoord);
	vec3 dirlightDir = normalize(dirlightPos - fragPos);
	vec3 pntlightDir = normalize(pntlightPos - fragPos);

	///// Direct Light /////
	// Diffuse
	float diff = max(dot(normal, dirlightDir), 0.0f);
	vec3 dirdiffuse = diff * dirlightColor;

	// Ambient
	vec3 dirambient = dirlightColor * dirambientStr;
	
	// Intensity
	float dirintensity = max(dirintensityStr, 0.1f);

	vec3 result = (dirdiffuse + dirambient) * dirintensity;


	///// Point Light /////
	// Diffuse
	float diff2 = max(dot(normal, pntlightDir), 0.0f);
	vec3 pntdiffuse = diff2 * pntlightColor;
	
	// Ambient
	vec3 pntambient = pntlightColor * pntambientStr;

	// Spec
	vec3 viewDir = normalize(cameraPos - fragPos);
    vec3 reflectDir = reflect(-pntlightDir, normal);
    float spec2 = pow(max(dot(reflectDir, viewDir), 0.1f), pntspecPhong);
    vec3 pntspec = spec2 * pntspecStr * pntlightColor;

	// Attenuation
	float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;
    float distance = length(pntlightPos - fragPos);
    float attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));
	pntambient *= attenuation;
    pntdiffuse *= attenuation;
    pntspec *= attenuation;

	// Intensity
	float pntintensity = max(pntintensityStr, 0.1f);

	vec3 result2 = (pntdiffuse + pntambient + pntspec) * pntintensity;


	FragColor = vec4(result + result2, 1.0f) * texture(tex0, texCoord);
}