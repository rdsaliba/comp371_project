 #version 330 core
 out vec4 FragColor; 
 in vec3 fragPos; 
 in vec3 vertexColor; 
 in vec2 vertexUV; 
 in vec3 norm; 
 in vec4 fragPosLightSpace; 
 uniform int isOn;
 uniform vec3 lightPos; 
 uniform vec3 viewPos; 
 uniform sampler2D textureSampler; 
 uniform sampler2D shadowMap; 

 float ShadowCalculation(vec4 fragPosLSpace) 
 { 
    // perform perspective divide
     vec3 projCoords = fragPosLSpace.xyz / fragPosLSpace.w; 
    // transform to [0,1] range
     projCoords = projCoords * 0.5 + 0.5; 
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
     float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
     float currentDepth = projCoords.z; 
    // calculate bias (based on depth map resolution and slope)
     vec3 normal = normalize(norm); 
     vec3 lightDir = normalize(lightPos - fragPos); 
     float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005); 
    // check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
     float shadow = 0.0; 
     vec2 texelSize = 1.0 / textureSize(shadowMap, 0); 
     for (int x = -1; x <= 1; ++x) 
     { 
     for (int y = -1; y <= 1; ++y) 
     { 
     float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
     shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0; 
     } 
     }   
     shadow /= 9.0;   

    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
     if (projCoords.z > 1.0) 
     shadow = 0.0; 

     return shadow; 
 } 

        
 void main() 
 { 
    vec3 color = vertexColor; 
    vec3 lightColor = vec3(1);  //Bright White
//ambiant
    vec3 lightPos = vec3(0.0f, 30.0f, 0.0f);  //TODO: Remove
    vec3 ambient = 0.15 * color;   //0.15
//diffuse
    vec3 lightDir = normalize(lightPos - fragPos); 
    vec3 normal = normalize(norm); 
    float diff = max(dot(lightDir, normal), 0.0); 
    vec3 diffuse = diff * color; 
//specular
    vec3 viewDir = normalize(viewPos - fragPos); 
    float spec = 0.0; 
    vec3 reflectDir = reflect(-lightDir, normal); 
    spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);  
    vec3 specular = spec * lightColor;  

// calculate shadow
    float shadow = ShadowCalculation(fragPosLightSpace); 
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;  
    vec4 textureColor = texture( textureSampler, vertexUV ); 
    FragColor = textureColor * vec4(lighting, 1.0); 
// FragColor = textureColor; 
   if(isOn == 0) {FragColor = textureColor * vec4(ambient, 1.0);}
   if(isOn == 1) {FragColor = textureColor * vec4(lighting, 1.0);}
 } ;