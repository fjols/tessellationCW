#version 330 core
layout(triangles) in ;
layout(triangle_strip, max_vertices = 3) out ;
vec3 getNormal() ;

in vec3 WorldPos_FS_in[] ;
in vec3 Normals[] ;

out vec3 gNormals ;
out vec3 gWorldPos_FS_in ;
out vec3 gFragPos ;

uniform float normLength ;


void main()
{
  
   for(int i = 0 ; i < 3; i++)
   {
      gFragPos = vec3(0.0) ;
	  gl_Position = gl_in[i].gl_Position ;
      gWorldPos_FS_in = WorldPos_FS_in[i] ;
	  gNormals = getNormal() ;    
      EmitVertex() ;
	

  }
     EndPrimitive() ;
  
 
 /*     vec4 cen = (gl_in[0].gl_Position +  gl_in[1].gl_Position +  gl_in[2].gl_Position)/3 ;
  //// vec3 norm = getNormal() ;
 //  for(int i = 0 ; i < 3; i++)
   //{
     
   ////   gl_Position = cen ; //gl_in[i].gl_Position;
   ////   EmitVertex() ;
   //   gl_Position = cen + vec4(norm*normLength,0.0);
   //   EmitVertex() ;
	//  EndPrimitive() ;*/
     
   //}

}


vec3 getNormal()
{
    vec3 a = vec3(gl_in[1].gl_Position) - vec3(gl_in[0].gl_Position);
    vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[0].gl_Position);
    return normalize(cross(a, b));
}