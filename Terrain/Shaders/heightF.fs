#version 330 core
out vec4 FragColor;


in vec3 gNormals ;
in vec3 gWorldPos_FS_in ;


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
    float height = gNormals.y / 20.f;
	vec4 green = vec4(0.0, 1.0, 0.0, 0.0);
	vec4 grey = vec4(0.25, 0.5, 0.25, 0.0);
	vec3 colour;

	if(height > 0.1)
	{
		colour = vec3(mix(green, grey, smoothstep(0.1, 1.0, height)).rgb);
	}
	else if(height > 0.4)
	{
		colour = vec3(mix(green, grey, smoothstep(0.4, 1.0, height)).rgb);
	}
	else if(height > 0.6)
	{
		colour = vec3(mix(green, grey, smoothstep(0.6, 1.0, height)).rgb);
	}
	else if(height > 0.8)
	{
		colour = vec3(mix(green, grey, smoothstep(0.8, 1.0, height)).rgb);
	}
	else
	{
		colour = vec3(mix(green, grey, smoothstep(1.0, 1.0, height)).rgb);
	}

     vec3 viewDir = normalize(viewPos - gWorldPos_FS_in);
	 vec3 norm = normalize(gNormals);
	 vec3 ambient = dirLight.ambient * (0.05 * colour);     
     vec3 lightDir = normalize(-dirLight.direction);
    // diffuse shading
    float diff = max(dot(norm, dirLight.direction), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-dirLight.direction, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), mat.shininess);
    // combine results
   
    vec3 diffuse  = dirLight.diffuse  * (diff * colour);
    vec3 specular = dirLight.specular * (spec * mat.specular);
	

    FragColor = vec4((ambient + diffuse + specular), 1.0f);

	
}

