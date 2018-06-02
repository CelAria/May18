#version 330 core

out vec4 color;

in VS_OUT{
vec3 FragPos;
vec4 FragPosLightSpace;
vec3 Fragcolor;
} fs_in;


uniform sampler2D shadowMap;
uniform vec3 lightPos;
uniform vec3 viewPos;


float ShadowCalculation(vec4 fragPosLightSpace)
{
	
	// perform perspective divide
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	// transform to [0,1] range
	projCoords = projCoords * 0.5 + 0.5;
	
	// get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
	float closestDepth = texture(shadowMap, projCoords.xy).r;
	// get depth of current fragment from light's perspective
	float currentDepth = projCoords.z;
	// check whether current frag pos is in shadow
	float shadow = currentDepth > closestDepth ? 1.0 : 0.0;

	return shadow;
}

void main()
{
	
	// calculate shadow
	float shadow = ShadowCalculation(fs_in.FragPosLightSpace);
	color = vec4(((1.0f - shadow) * fs_in.Fragcolor), 1.0);

	//color = texture(shadowMap, fs_in.FragPos.xy); // debug 
} 