#version 330 core
 		in vec4 ourColor;
 		in vec3 Normal;
 		in vec2 TexCoord;

 		out vec4 color;

 		uniform sampler2D ourTexture;

 		void main()
 		{
 			color = vec4(0.0,0.0,1.0,1.0);
 		}