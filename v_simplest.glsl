#version 330

///Zmienne jednorodne
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

//Atrybuty
in vec4 vertex; //wspolrzedne wierzcholka w przestrzeni modelu

in vec4 normal; //wektor normalny w przestrzeni modelu

in vec2 texCoord0;
in vec4 lp2; // pozycja swiatla ruchomego

//Zmienne interpolowane

//1 zrodlo
out vec4 l;
out vec4 n;
out vec4 v;

//2 zrdolo
out vec4 l2;
out vec4 n2;
out vec4 v2;

out vec2 iTexCoord0;
out vec2 iTexCoord1;

void main(void) {
    vec4 lp = vec4(0, 50, 50, 1); //pozcyja światła, przestrzeń świata
    
    l = normalize(V * lp - V*M*vertex); //wektor do światła w przestrzeni oka
    v = normalize(vec4(0, 0, 0, 1) - V * M * vertex); //wektor do obserwatora w przestrzeni oka
    n = normalize(V * M * normal); //wektor normalny w przestrzeni oka
    
    //2 zrodlo
    l2 = normalize(V * lp2 - V*M*vertex); //wektor do światła w przestrzeni oka
    v2 = normalize(vec4(0, 0, 0, 1) - V * M * vertex); //wektor do obserwatora w przestrzeni oka
    n2 = normalize(V * M * normal); //wektor normalny w przestrzeni oka


    iTexCoord0 = texCoord0;
    iTexCoord1 = (n.xy+1)/2;//cw10

   
    
    gl_Position=P*V*M*vertex;
    
}
