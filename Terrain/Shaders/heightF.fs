#version 330 core
out vec4 FragColor;


in vec3 gNormals ;
in vec3 gFragPos ;
in float heightFactorG;
in float gVisibility;


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

uniform sampler2D texture1;
uniform DirLight dirLight;
uniform Material mat ;
uniform vec3 viewPos ;


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

	vec3 lighting = vec3(ambient + diffuse + specular);
	lighting = pow(lighting, vec3(1.0/2/2));
	
    FragColor = vec4((lighting) * colour, 1.0f); // Final result.
	FragColor = mix(vec4(1.0f, 1.0f, 1.0f, 1.0f), FragColor, gVisibility);
}

