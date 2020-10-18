#version 330

layout (location = 0) in vec4 position;
layout (location = 1) in vec3 normal;
uniform mat4 mvMatrix;
uniform mat4 mvpMatrix;
uniform mat4 norMatrix;
uniform vec4 lightPos;

out vec4 theColour;
out float nDotL;
out float nDotV;

void main()
{
	vec4 white = vec4(1.0);
	vec4 grey = vec4(0.2);
	vec4 cyan = vec4(0.0, 1.0, 1.0, 1.0);
	vec4 flameOrange = vec4(226.0/255, 88.0/255, 43.0/255, 1);

	vec4 posnEye = mvMatrix * position;
	vec4 normalEye = norMatrix * vec4(normal, 0);
	vec4 lgtVec = normalize(lightPos - posnEye); 
	vec4 viewVec = normalize(vec4(-posnEye.xyz, 0)); 
	vec4 halfVec = normalize(lgtVec + viewVec); 

	vec4 material = flameOrange;
	vec4 ambOut = grey * material;
	float shininess = 100.0;
	float diffTerm = max(dot(lgtVec, normalEye), 0);
	vec4 diffOut = material * diffTerm;
	float specTerm = max(dot(halfVec, normalEye), 0);
	vec4 specOut = white *  pow(specTerm, shininess);
	
	gl_Position = mvpMatrix * position;  //to clip coordinates

	nDotL = dot(normalEye, lgtVec);
	nDotV = dot(viewVec, normalEye);

	theColour = ambOut + diffOut + specOut;
}

