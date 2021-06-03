#version 330

uniform sampler2D textureMap0; //zmienna reprezentująca jednostkę teksturującą
uniform sampler2D textureMap1; //zmienna reprezentująca DRUGA jednostkę teksturującą

out vec4 pixelColor; //Zmienna wyjsciowa fragment shadera. Zapisuje sie do niej ostateczny (prawie) kolor piksela

in vec4 n;
in vec4 l;
in vec4 v;
//2 zrodlo
in vec4 n2;
in vec4 l2;
in vec4 v2;

in vec2 iTexCoord0; //Zmienna do interpolacji współrzędnych teksturowania
in vec2 iTexCoord1;
void main(void) {

	///Znormalizowane interpolowane wektory
	vec4 ml = normalize(l);
	vec4 mn = normalize(n);
	vec4 mv = normalize(v);
	//Wektor odbity
	vec4 mr = reflect(-ml, mn);

	//Parametry powierzchni
	vec4 metal = texture(textureMap0,iTexCoord0); //Pobranie koloru
	vec4 sky = texture(textureMap1,iTexCoord1); //Pobranie koloru z drugiej jedenostki teksturujacej
	vec4 kd = mix(metal, sky, 0.3);
	vec4 ks = vec4(1.0, 0.3f, 0.0f,0.2f);//czerwone
	
	//Obliczenie modelu oświetlenia
	float nl = clamp(dot(mn, ml), 0, 1);
	float rv = pow(clamp(dot(mr, mv), 0, 1), 50);


	//2 zrodlo
	///Znormalizowane interpolowane wektory
	vec4 ml2 = normalize(l2);
	vec4 mn2 = normalize(n2);
	vec4 mv2 = normalize(v2);
	//Wektor odbity
	vec4 mr2 = reflect(-ml2, mn2);

	//Parametry powierzchni
	vec4 ks2 = vec4(1.0,0.85f,0.f,0.75f);//zolte
	
	//Obliczenie modelu oświetlenia
	float nl2 = clamp(dot(mn2, ml2), 0, 1);
	float rv2 = pow(clamp(dot(mr2, mv2), 0, 1), 50);


	
	
	pixelColor= vec4(kd.rgb * nl, kd.a) + vec4(ks.rgb*rv, 0)+ vec4(kd.rgb * nl2, kd.a) + vec4(ks2.rgb*rv2, 0);//  1 i 2 wlaczone swiatlo
	
	//pixelColor= vec4(kd.rgb * nl, kd.a) + vec4(ks.rgb*rv, 0);//tylko pierwsze swiatlo czerwone
	
	//pixelColor= vec4(kd.rgb * nl2, kd.a) + vec4(ks2.rgb*rv2, 0);//tylko drugie swiatlo zolte
	
	//pixelColor = metal;
}
