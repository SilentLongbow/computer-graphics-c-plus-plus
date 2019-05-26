#version 330
in vec4 theColour;
in float nDotL;
in float nDotV;

void main()
{

   // 2-tone shading
   if (abs(nDotV) < 0.2) {
      gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);
   } else if (nDotL < 0.1) {
      gl_FragColor = vec4(0.0, 0.6, 0.6, 0.6);
   } else {
      gl_FragColor = vec4(0.0, 1.0, 1.0, 1.0);
   }


   //gl_FragColor = theColour;   // Mapping the colour to the fragment

   //gl_FragColor = vec4(vec3(gl_FragCoord.z), 1.0);    // Mapping the depth component of the fragment to its colour to give a depth-map
}
