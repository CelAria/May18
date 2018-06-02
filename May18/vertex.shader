
#version 330 core
  
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal; 

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;
uniform vec3 camera_position;
uniform vec3 light_position;
uniform mat4 lightview_matrix; 
uniform mat4 light_matrix;


out VS_OUT{
vec3 FragPos;
vec4 FragPosLightSpace;
vec3 Fragcolor;
} vs_out;


void main()
{
	vec4 aPos = vec4(position.x, position.y, position.z, 1.0);

	vs_out.FragPos = vec3(model_matrix * aPos); 
	vs_out.FragPosLightSpace = projection_matrix * lightview_matrix * vec4(vs_out.FragPos, 1.0); // from the view of the light 
	

	mat4 transform = projection_matrix * view_matrix * model_matrix;
	vec4 transformednormal= model_matrix * aPos;
	

	gl_Position = transform * aPos;

	// phong model lighting calculations: https://en.wikipedia.org/wiki/Phong_reflection_model

	vec3 v = normalize(camera_position - gl_Position).xyz;	//get value for direction vector from the point on the surface toward each light source
	vec3 n = normalize(transformednormal).xyz;				// get just xyz from transformed normal (normal at this point on surface) 
	vec3 lm = normalize(light_position - gl_Position).xyz;	//direction vector from the point on the surface toward each light source 
	vec3 rm = ((2*dot(lm, n)*n) - lm);						//direction that a perfectly reflected ray of light would take from this point on the surface

	float diffusevalue = dot(lm, n);						//ratio of reflection of the diffuse term of incoming light
	float specularvalue = dot(rm, v);						//ratio of reflection of the specular term of incoming light,

	vec3 diffusecolor = vec3(0.7f, 0.7f, 0.7f);				//declaring a "random" diffuse color
	vec3 specularcolor = vec3(0.2f, 0.9f, 0.2f);			//declaring a "random specular color



	//scaling by whatever is in model matrix, then whatever is in the view matrix, then by the projection matrix


	//vcolor = vec3(1.0, 0.0, 0.0)*dot(transformednormal, vec4(0.0, 1.0, 0.0, 0.0));

	vs_out.Fragcolor = diffusevalue * diffusecolor+ (pow(specularvalue,1.0f)*specularcolor);
	//vs_out.Fragcolor= vec3(0.5f, 0.5f, 0.5f);
}