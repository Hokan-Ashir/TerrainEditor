uniform sampler2D baseMap;
uniform sampler2D layer1;
uniform sampler2D layer2;
uniform sampler2D layer3;
uniform sampler2D layer4;
uniform sampler2D layer5;
uniform sampler2D mask123;
uniform sampler2D mask45;

varying vec2 Texcoord;

void main( void )
{
   vec4 base = texture2D( baseMap, Texcoord );
   vec4 layer1C = texture2D( layer1, Texcoord );
   vec4 layer2C = texture2D( layer2, Texcoord );
   vec4 layer3C = texture2D( layer3, Texcoord );
   vec4 layer4C = texture2D( layer4, Texcoord );
   vec4 layer5C = texture2D( layer5, Texcoord );
   vec4 mask1 = texture2D (mask123, Texcoord );
   vec4 mask2 = texture2D (mask45, Texcoord );
   
   vec4 col;
   
   col = (layer1C-base)* mask1.r + (layer2C-base)* mask1.g + (layer3C-base)* mask1.b; //first mask textures
   col += (layer4C-base)* mask2.r + (layer5C-base)* mask2.g; //second mask textures

   base = base + col;
   
   gl_FragColor = base;
    
}