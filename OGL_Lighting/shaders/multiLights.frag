
#version 330 core
out vec4 FragColor;

#define NR_POINT_LIGHTS 6

in vec3 Normal;  
in vec3 FragPos;
in vec2 TexCoord;

uniform vec3 viewPos; 
uniform vec3 lightColor;
uniform vec3 lightPositions[NR_POINT_LIGHTS];
uniform vec3 objectColor;

uniform sampler2D Texture1;

void main()
{
	vec3 norm = normalize(Normal);
	vec3 result = vec3(0.0, 0.0, 0.0);
	for(int i = 0; i < NR_POINT_LIGHTS; i++)
    {
		vec3 lightDir = normalize(lightPositions[i] - FragPos);
		 
		// ambient
		float ambientStrength = 0.1;
		vec3 ambient = ambientStrength * lightColor;
  	
		// diffuse
		float diff = max(dot(norm, lightDir), 0.0);
		vec3 diffuse = diff * lightColor;
    
		// specular
		float specularStrength = 0.5;
		vec3 viewDir = normalize(viewPos - FragPos);
		vec3 reflectDir = reflect(-lightDir, norm);  
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
		vec3 specular = specularStrength * spec * lightColor;
		
		// attenuation
		float distance = length(lightPositions[i] - FragPos);
		float attenuation = 1.0 / (1.0 + 0.35 * distance + 0.44 * (distance * distance));
		ambient  *= attenuation;
		diffuse  *= attenuation;
		specular *= attenuation;   
        
		result += (ambient + diffuse + specular);
	}
    FragColor = vec4(result, 1.0) * texture(Texture1, TexCoord);
} 