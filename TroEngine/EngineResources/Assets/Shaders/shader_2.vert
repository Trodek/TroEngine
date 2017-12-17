#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 texCoord;
layout(location = 2) in vec3 normals;
layout(location = 3) in vec4 color;

out vec4 ourColor;
out vec3 Normal;
out vec2 TexCoord;
out vec3 currPos;
out float maxHeight;

uniform mat4 Model;
uniform mat4 view;
uniform mat4 projection;

void main()
{ 
    float height = 0.5f;
    float factor = 0.5f;
    float mult = sin(position.x/factor);    
    vec3 pos = position;
    pos.y=mult*height;
    gl_Position = projection * view * Model * vec4(pos, 1.0f);
    ourColor = color;
    TexCoord = texCoord.xy;
    currPos = pos;
    maxHeight = height; 
}