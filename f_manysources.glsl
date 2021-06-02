#version 330

uniform sampler2D textureMap0;

out vec4 pixelColor; //Zmienna wyjsciowa fragment shadera. Zapisuje sie do niej ostateczny (prawie) kolor piksela

in vec4 l[2];
in vec4 n;
in vec4 v;
in vec2 iTexCoord0;



void main(void) {

vec4 ml[2];
vec4 mr[2];
float nl[2];
float rv[2];
vec4 pixelPartialSum = vec4(0,0,0,0);

//pobieramy l n i v do lokalnych zmiennych i normalizujemy

//Bez zmian
vec4 mn = normalize(n);
vec4 mv = normalize(v);

  //Sumujemy wszystkie Ÿród³a œwiat³a:
  for (int i=0;i<2;i++) {
        ml[i] = normalize(l[i]);
        mr[i] = reflect(-ml[i],mn);
        nl[i] = clamp( dot(mn,ml[i]),0,1);
        rv[i] = pow(clamp(dot(mr[i],mv),0,1), 25); 
  }

   vec4 kd=texture(textureMap0,iTexCoord0);
   vec4 ks= vec4(1,1,1,1);
   //zakladamy ze kolory ls i ks to kolor bialy wiec brak we wzorze

   //Obliczamy sume czesciow¹
     for (int i=0;i<2;i++) {

        pixelPartialSum += vec4(kd.rgb * nl[i],kd.a)+vec4(ks.rgb*rv[i],0);
     }

    //Suma wszystkich to nasz pixel color
	pixelColor = pixelPartialSum;
}
