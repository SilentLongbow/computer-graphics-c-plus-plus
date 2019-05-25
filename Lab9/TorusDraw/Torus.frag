#version 330
in vec4 theColour;

void main()
{
   //gl_FragColor = theColour;   // Mapping the colour to the fragment

   gl_FragColor = vec4(vec3(gl_FragCoord.z), 1.0);    // Mapping the depth component of the fragment to its colour to give a depth-map
}
