#version 330 core
out vec4 FragColor;

float calculateShadows(vec4 fragPosSpace);

in vec3 gNormals ;
in vec3 gFragPos ;
in float heightFactorG;
in float gVisibility;
in vec4 gFragPosLightSpace;


in float heightG;


struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;    
    float shininess;
};                                                                        


struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
}; 

uniform DirLight dirLight;
uniform sampler2D shadowMap;
uniform Material mat ;
uniform vec3 viewPos ;
uniform bool gammaCorrection; // Turn on if gamma correction should be on.


void main()
{   
     vec3 viewDir = normalize(viewPos -  gFragPos);
	 vec3 norm = normalize(gNormals);
	 vec3 ambient = dirLight.ambient * mat.ambient;     
     vec3 lightDir = normalize(-dirLight.direction);
    // diffuse shading
    float diff = max(dot(norm, dirLight.direction), 0.0);
    // specular shading
	vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), mat.shininess);
    // combine results
   
    vec3 diffuse  = dirLight.diffuse  * (diff * mat.diffuse);
    vec3 specular = dirLight.specular * (spec * mat.specular);

	float height = heightG / heightFactorG; // Value used to check what the height is.

	vec3 lightGray = vec3(0.82f, 0.82f, 0.82f);
	vec3 gray = vec3(0.5f, 0.4f, 0.5f);
    vec3 darkGray = vec3(0.26f, 0.26f, 0.26f);

    vec3 blue = vec3(0.1f, 0.1f, 0.7f);
    vec3 green = vec3(0.1f, 0.7f, 0.1f);
    vec3 darkGreen = vec3(0.1f, 0.2f, 0.1f);

	vec3 colour;

	if(height < 0.4f) // If height is less than 0.4 then do this colour.
	{
		colour = vec3(mix(blue, darkGreen, smoothstep(0.01f, 0.5f, height)).rgb);
	}
	else if(height < 0.7f) // If height is less than 0.8 then do this colour.
	{
		colour = vec3(mix(darkGreen, gray, smoothstep(0.40f, 0.8f, height)).rgb);
	}
	else // If the height value does not fit in either if statement then set it to gray.
	{
		colour = gray;
	}

	float shadow = calculateShadows(gFragPosLightSpace);

	//vec3 lighting = vec3(ambient + diffuse + specular, 1.0f);

	//if(gammaCorrection) // If gamma correction is true
	//{
	//	lighting = pow(lighting, vec3(1.0/2.2)); // Apply the gamma correction
	////	FragColor = vec4((lighting) * colour, 1.0f); // Apply the lighting.	
	//	FragColor = mix(vec4(1.0f, 1.0f, 1.0f, 1.0f), FragColor, gVisibility); // Apply the fog.
	//}
	//else
	//{
		FragColor = vec4((ambient + (1 - shadow)*(diffuse + specular)) * colour, 1.0f); // Apply the lighting.
		FragColor = mix(vec4(1.0f, 1.0f, 1.0f, 1.0f), FragColor, gVisibility); // Apply the fog.
	//}
}

float calculateShadows(vec4 fragPosSpace)
{
	vec3 projCoords = gFragPosLightSpace.xyz / gFragPosLightSpace.w;
	projCoords = projCoords * 0.5 + 0.5;
	float closestDepth = texture(shadowMap, projCoords.xy).r;
	float currentDepth = projCoords.z;
	float shadow = 0;
	if(currentDepth > closestDepth)
	{
		shadow = 1;
	}
	return shadow;
}