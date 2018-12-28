#version 330

uniform vec3 inColours[32];
uniform vec3 inLightDir = vec3(-0.5, -0.2, -0.3);

flat in int colourIdxFS;
flat in vec3 normalFS;

out vec4 outputColour;

vec3 colourAt(int index)
  {
  switch (index)
    {
    case 0: return inColours[0];
    case 1: return inColours[1];
    case 2: return inColours[2];
    case 3: return inColours[3];
    case 4: return inColours[4];
    case 5: return inColours[5];
    case 6: return inColours[6];
    case 7: return inColours[7];
    case 8: return inColours[8];
    case 9: return inColours[9];
    case 10: return inColours[10];
    case 11: return inColours[11];
    case 12: return inColours[12];
    case 13: return inColours[13];
    case 14: return inColours[14];
    case 15: return inColours[15];
    case 16: return inColours[16];
    case 17: return inColours[17];
    case 18: return inColours[18];
    case 19: return inColours[19];
    case 20: return inColours[20];
    case 21: return inColours[21];
    case 22: return inColours[22];
    case 23: return inColours[23];
    case 24: return inColours[24];
    case 25: return inColours[25];
    case 26: return inColours[26];
    case 27: return inColours[27];
    case 28: return inColours[28];
    case 29: return inColours[29];
    case 30: return inColours[30];
    case 31: return inColours[31];
    }
  return vec3(0, 0, 0);
  }


void main()
  {
  vec3 colour = colourAt(colourIdxFS);

  float lightFactor = dot(normalFS, normalize(inLightDir * -1));

  colour.r = pow(colour.r, 2.2);
  colour.g = pow(colour.g, 2.2);
  colour.b = pow(colour.b, 2.2);

  lightFactor *= 0.5;
  lightFactor += 0.5;

  colour *= clamp(lightFactor, 0, 1);

  outputColour = vec4(colour, 1);
  }