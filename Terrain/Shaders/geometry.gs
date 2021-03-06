#version 330 core
layout(triangles) in ;
layout(triangle_strip, max_vertices = 3) out ;
vec3 getNormal() ;

in vec3 posES[] ;
in vec3 normES[] ;
in float heightFactorTE[];
in vec2 texCoordsTE[];
in float heightTE[];
in float visibility[];
in vec4 fragPosLightSpaceTE[];

out vec3 gNormals ;
out vec3 gFragPos ;
out vec2 gTexCoords;
out float heightFactorG;
out float heightG;
out float gVisibility;
out vec4 gFragPosLightSpace;


void main()
{
  
   for(int i = 0 ; i < 3; i++)
   {
      gl_Position = gl_in[i].gl_Position ;
      gFragPos = posES[i] ;
	  gTexCoords = texCoordsTE[i];
      gNormals = normES[i];    
	  heightFactorG = heightFactorTE[i];
	  heightG = heightTE[i];
	  gVisibility = visibility[i];
	  gFragPosLightSpace = fragPosLightSpaceTE[i];
      EmitVertex() ;
  }
     EndPrimitive() ;

}


vec3 getNormal()
{
    vec3 a = vec3(gl_in[1].gl_Position) - vec3(gl_in[0].gl_Position);
    vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[0].gl_Position);
    return normalize(cross(a, b));
}