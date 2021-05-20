#version 330

//Zmienne jednorodne
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform vec4 lp;//cw lp p 40min przekazywane przez uniform jest zmienna jednorodna

//Atrybuty
in vec4 vertex; //wspolrzedne wierzcholka w przestrzeni modelu
in vec4 normal;
in vec2 texCoord0;

//Zmienne interpolowane
out  vec4 l;
out vec4 n;
out vec4 v;
out vec2 iTexCoord0;

void main(void) {

  l = normalize(V*lp - V*M*vertex);
  n = normalize(V*M*normal);
  v = normalize(vec4(0,0,0,1) - V*M*vertex);
  iTexCoord0=texCoord0;
 
    gl_Position=P*V*M*vertex;
}
