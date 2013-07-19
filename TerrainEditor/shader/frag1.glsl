uniform sampler2D baseMap;
uniform sampler2D layer1;
uniform sampler2D layer2;
uniform sampler2D layer3;
uniform sampler2D mask123;varying vec2 Texcoord;

void main( void )
{
vec4 base = texture2D( baseMap, Texcoord ); //основная текстура
vec4 layer1C = texture2D( layer1, Texcoord ); //первый слой
vec4 layer2C = texture2D( layer2, Texcoord ); //второй слой
vec4 layer3C = texture2D( layer3, Texcoord ); //третий слой
vec4 mask1 = texture2D (mask123, Texcoord ); //слой-маска //В качестве слоя-маски 1ой текстуры будем использовать R-канал текстуры-маски
//2ой текстуры - G-канал
//3ей текстуры - B-канал
//и высчитаем цвет пикселя
vec4 col = (layer1C-base)* mask1.r + (layer2C-base)* mask1.g + (layer3C-base)* mask1.b;
base = base + col; //накладываем на основную текстуру 
gl_FragColor = base;
}