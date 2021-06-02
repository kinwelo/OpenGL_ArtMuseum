#version 330

//Zmienne jednorodne
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform vec4 lp[2];



//Atrybuty
in vec4 vertex; //wspolrzedne wierzcholka w przestrzeni modelu
in vec4 normal;
in vec2 texCoord0;

//Zmienne interpolowane
out vec4 l[2];
out vec4 n;
out vec4 v;
out vec2 iTexCoord0;

void main(void) {
//Glowny i najprostszy model w projekcie: Model phonga z odbiciem i sparametryzowanymi zrodlami swiatla
//vec4 lp[2];
//lp[0]=lpx;
//lp[1]=lpx2;
  
  //Dla kazdego zrodla swiatla:
  for (int i=0;i<2;i++) {
        l[i] = normalize(V*lp[i] - V*M*vertex);
    }
  
 

  n = normalize(V*M*normal);
  v = normalize(vec4(0,0,0,1) - V*M*vertex);
  iTexCoord0=texCoord0;
 
  gl_Position=P*V*M*vertex;
}
