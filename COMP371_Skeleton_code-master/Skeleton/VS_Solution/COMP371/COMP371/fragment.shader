#version 330 core

out vec4 color;
uniform int object_type;

void main()
{
	if(object_type == 0){
    color = vec4(1.0f, 0.5f, 0.2f, 1.0f);
}
if(object_type == 1){
    color = vec4(1.0f, 0.5f, 0.5f, 1.0f);
}
if(object_type == 2){
    color = vec4(0.5f, 0.5f, 0.5f, 1.0f);
}
if(object_type == 3){
color = vec4(0.2f, 0.5f, 0.5f, 1.0f);
}
} 