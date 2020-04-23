#version 330 core
out vec4 FragColor;


in vec3 gNormals ;
in vec3 gFragPos ;
in float heightFactorG;


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
    vec3 reflectDir = reflect(-dirLight.direction, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), mat.shininess);
    // combine results
   
    vec3 diffuse  = dirLight.diffuse  * (diff * mat.diffuse);
    vec3 specular = dirLight.specular * (spec * mat.specular);

	float height = heightG / heightFactorG;

	vec3 lightGray = vec3(0.82f, 0.82f, 0.82f);
	vec3 gray = vec3(0.5f, 0.4f, 0.5f);
    vec3 darkGray = vec3(0.26f, 0.26f, 0.26f);

    vec3 blue = vec3(0.1f, 0.1f, 0.8f);
    vec3 green = vec3(0.1f, 0.8f, 0.1f);
    vec3 darkGreen = vec3(0.1f, 0.3f, 0.1f);

	vec3 colour;

	if(height < 0.5f)
	{
		colour = vec3(mix(blue, darkGreen, smoothstep(0.01f, 0.5f, height)).rgb);
		//colour = gray;
	}
	else if(height < 0.8)
	{
		colour = vec3(mix(darkGreen, gray, smoothstep(0.55f, 0.8f, height)).rgb);
		//colour = green;
	}
	else
	{
		colour = gray;
	}
	

	//else
	//{
	//	colour = gray;
	//}
	

    FragColor = vec4((ambient + diffuse + specular) * colour, 1.0f);

	
}

