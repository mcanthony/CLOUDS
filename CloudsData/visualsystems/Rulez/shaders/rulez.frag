#version 120
#extension GL_ARB_texture_rectangle : enable

#define TWO_PI 6.28318530717958647693
#define PI 3.14159265358979323846
#define HALF_PI  1.57079632679489661923

uniform vec3 lightPos;
uniform float growth;
uniform float maxDepth;

varying vec3 nEye;
varying vec3 vEye;

void main()
{
    if (gl_TexCoord[0].t > growth) discard;
    
    vec3 l = normalize(lightPos - vEye);
	vec3 lightN = normalize(cross(dFdx(vEye), dFdy(vEye)));
    //vec3 lightN = normalize(nEye);
    
	float diffuse = max(dot(lightN, l), 0.0);
    
    //gl_FragColor = vec4(gl_TexCoord[0].t / maxDepth, gl_TexCoord[0].t / maxDepth, gl_TexCoord[0].t / maxDepth, 1.0);//gl_Color;//vec4(gl_TexCoord[0].s, gl_TexCoord[0].s, gl_TexCoord[0].s, 1.0) * diffuse;//
    gl_FragColor = vec4(gl_Color.rgb * (diffuse/* + specular*/), 1.0);//gl_Color;//vec4(gl_TexCoord[0].s, gl_TexCoord[0].s, gl_TexCoord[0].s, 1.0) * diffuse;//
}