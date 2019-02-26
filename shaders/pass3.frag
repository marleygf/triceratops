// Pass 3 fragment shader
//
// Output fragment colour based using
//    (a) Cel shaded diffuse surface
//    (b) wide silhouette in black

#version 300 es

uniform mediump vec3 lightDir;     // direction toward the light in the VCS
uniform mediump vec2 texCoordInc;  // texture coord difference between adjacent texels

in mediump vec2 texCoords;              // texture coordinates at this fragment

// The following four textures are now available and can be sampled
// using 'texCoords'

uniform sampler2D colourSampler;
uniform sampler2D normalSampler;
uniform sampler2D depthSampler;
uniform sampler2D laplacianSampler;

out mediump vec4 outputColour;          // the output fragment colour as RGBA with A=1

void main()

{
  // [0 marks] Look up values for the depth and Laplacian.  Use only
  // the R component of the texture as texture2D( ... ).r

  mediump float depth = texture2D(depthSampler, texCoords).r;
  mediump float laplacian = texture2D(laplacianSampler, texCoords).r;

  // [1 mark] Discard the fragment if it is a background pixel not
  // near the silhouette of the object.

  if(depth == 1.0 && laplacian > -0.1){
    outputColour = vec4(0.5,0.5,0.5,0.5);
    return;
  }

  // [0 marks] Look up value for the colour and normal.  Use the RGB
  // components of the texture as texture2D( ... ).rgb or texture2D( ... ).xyz.

  mediump vec3 colour = texture2D(colourSampler, texCoords).rgb;
  mediump vec3 normal = texture2D(normalSampler, texCoords).xyz;

  // [2 marks] Compute Cel shading, in which the diffusely shaded
  // colour is quantized into four possible values.  Do not allow the
  // diffuse component, N dot L, to be below 0.2.  That will provide
  // some ambient shading.  Your code should use the 'numQuata' below
  // to have that many divisions of quanta of colour.  Your code
  // should be very efficient.

  const int numQuanta = 3;

  mediump float NdotL = dot(normalize(normal), lightDir);
  int bin = int(round(NdotL * float(numQuanta)));
  NdotL = float(bin) * (1.0/float(numQuanta));
  if(NdotL < 0.2){
    NdotL = 0.2;
  }

  // [2 marks] Count number of fragments in the 3x3 neighbourhood of
  // this fragment with a Laplacian that is less than -0.1.  These are
  // the edge fragments.  Your code should use the 'kernelRadius'
  // below and check all fragments in the range
  //
  //    [-kernelRadius,+kernelRadius] x [-kernelRadius,+kernelRadius]
  //
  // around this fragment.

  const int kernelRadius = 1;
  int numEdges = 0;
  for(int x = -kernelRadius; x <= kernelRadius; x++){
    for(int y = -kernelRadius; y <= kernelRadius; y++){
      if(!(x == 0 && y == 0)){
        mediump vec2 neighbourCoords = vec2(texCoords.x + (float(x) * texCoordInc.x), texCoords.y + (float(y) * texCoordInc.x));
        mediump float neighbour = texture2D(laplacianSampler, neighbourCoords).r;
        if(neighbour < -0.1){
          numEdges++;
        }
      }
    }    
  }

  // [0 marks] Output the fragment colour.  If there is an edge
  // fragment in the 3x3 neighbourhood of this fragment, output a
  // black colour.  Otherwise, output the cel-shaded colour.
  //
  // Since we're making this black if there is any edge in the 3x3
  // neighbourhood of the fragment, the silhouette will be wider
  // than if we test only the fragment.

  mediump vec3 diffuseColour = NdotL * colour;
  diffuseColour = diffuseColour;
  if(numEdges > 0){
    diffuseColour = vec3(0,0,0);
  }
  outputColour = vec4(diffuseColour, 1.0 );
}
