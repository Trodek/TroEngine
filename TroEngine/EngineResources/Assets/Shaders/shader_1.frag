#version 330 core
in vec4 ourColor;
in vec3 Normal;
in vec2 TexCoord;
in vec3 currPos;
in float maxHeight;

out vec4 color;

void main()
{
    float margin = 1.0f;
    float max_value = maxHeight;
    float min_value = -maxHeight;
    vec4 initial_color = vec4(0.63,0.79,0.94,1.0);
    vec4 end_color = vec4(0.0,0.278,0.67,1.0);
    
    float range = min_value - max_value;
    float c1 = range * (max_value + margin);
    float c2 = range * min_value;
    
    float c = range* currPos.y;
    
    if(c <= c1)
        color = initial_color;
    else if(c >= c2)
        color = end_color;
    else
        color = (initial_color * (c2-c) + end_color * (c - c1))/(c2-c1);
}