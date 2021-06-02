#version 330

#define N 2

//Zmienne jednorodne
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform vec4 lp[N];



//Atrybuty
in vec4 vertex; //wspolrzedne wierzcholka w przestrzeni modelu
in vec4 normal;
in vec2 texCoord0;

//Zmienne interpolowane
out vec4 l[N];
out vec4 n;
out vec4 v;
out vec2 iTexCoord0;

void main(void) {
//Glowny i najprostszy model w projekcie: Model phonga z odbiciem i sparametryzowanymi zrodlami swiatla

  
  //Dla kazdego zrodla swiatla:
  for (int i=0;i<N;i++) {
        l[i] = normalize(V*lp[i] - V*M*vertex);
    }
  
 

  n = normalize(V*M*normal);
  v = normalize(vec4(0,0,0,1) - V*M*vertex);
  iTexCoord0=texCoord0;
 
  gl_Position=P*V*M*vertex;
}
