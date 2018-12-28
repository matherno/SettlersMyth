#version 330

layout(location = 0) in vec4 inVoxel;

flat out int colourIdx;

void main(){
    colourIdx = int(inVoxel.w);

    gl_Position = vec4(inVoxel.xyz, 1);
}
