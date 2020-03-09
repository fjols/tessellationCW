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
uniform float heightFactor;


void main()
{   
    float height = gWorldPos_FS_in.y / heightFactor;
	vec3 green = vec3(0.3, 0.35, 0.15);
	vec4 grey = vec4(0.5, 0.4, 0.5, 0.0);
    
    vec3 brown = vec3(0.61, 0.36, 0.0);
    vec4 white = vec4(0.7, 0.7, 0.7, 0.0);

	vec3 colour;

	if(height < 0.2)
	{
		//colour = vec3(mix(green, grey, smoothstep(0.3, 0.6, height)).rgb);
        colour = green;
	}
   else if(height < 0.5)
	{
       //colour = vec3(mix(brown, grey, smoothstep(0.6, 0.8, height)).rgb);
       colour = brown;
	}
	//else if(height > 0.8)
	///{
	//	//colour = vec3(mix(white, grey, smoothstep(0.8, 1.0, height)).rgb);
   //colour = white;
   //}
	else
	{
		//colour = vec3(mix(green, grey, smoothstep(0.25f, 1.0, height)).rgb);
		colour = vec3(1.0f, 1.0f, 1.0f);
	}

     vec3 viewDir = normalize(viewPos - gWorldPos_FS_in);
	 vec3 norm = normalize(gNormals);
	 vec3 ambient = dirLight.ambient * colour;     
     vec3 lightDir = normalize(-dirLight.direction);
    // diffuse shading
    float diff = max(dot(norm, dirLight.direction), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-dirLight.direction, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), mat.shininess);
    // combine results
   
    vec3 diffuse  = dirLight.diffuse  * (diff * colour);
    vec3 specular = dirLight.specular * (spec * colour);
	

    FragColor = vec4((ambient + diffuse + specular) * colour, 1.0f);

	
}

