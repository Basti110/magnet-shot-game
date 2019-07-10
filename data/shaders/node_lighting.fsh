out vec4 FragColor;
out vec4 gBloom;

in vec2 TexCoords;

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 Position;
    vec3 Color;
    float Linear;
    float Quadratic;
    float Radius;
};

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAmbient;
uniform sampler2D gDiffuse;
uniform sampler2D gSpecular;
uniform sampler2D ssao;


uniform vec3 viewPos; 
uniform Light light;
uniform PointLight pointLights[35];
uniform sampler2D shadowMap;
uniform mat4 shadowTransform;
uniform float shadowOffset;
uniform float shadowSmoothness;
uniform int sizePointLight;
uniform bool sunShadowOn;


void main()
{
    
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 materialAmbient = texture(gAmbient, TexCoords).rgb;
    vec3 materialDiffuse = texture(gDiffuse, TexCoords).rgb;
    vec3 materialSpecular = texture(gSpecular, TexCoords).rgb;

    float alpha = texture(gAmbient, TexCoords).a;
    float shininess = texture(gSpecular, TexCoords).a;

    // ambient
    float occlusion = max(texture(ssao, TexCoords).r, 1 - alpha);
    vec3 ambient = light.ambient * materialAmbient * occlusion;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * materialDiffuse * occlusion);// * occlusion;
    
    // specular
    vec3 viewDir = normalize(-FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess * 255.);
    vec3 specular = light.specular * (spec * materialSpecular); 
    
    //float constant = 1;
    //float linear = 0.7;
    //float quadratic = 1.8;
    //viewDir = normalize(viewPos - FragPos);
    for(int i = 0; i < sizePointLight; ++i)
    {
        //float lightMax = max(max(pointLights[i].Color.r, pointLights[i].Color.g), pointLights[i].Color.b);
        //float mRadius = (-linear + sqrt(linear * linear - 4 * quadratic * (constant - (256.0 / 5.0) * lightMax))) / (2 * quadratic);
        float distance = length(pointLights[i].Position - FragPos);
        
        //if(distance < 5 * mRadius)
        if(distance < 5 * pointLights[i].Radius)
        {
            // diffuse
            vec3 lightDir = normalize(pointLights[i].Position - FragPos);
            vec3 diffusePoint = max(dot(Normal, lightDir), 0.0) * materialDiffuse * pointLights[i].Color;
            // specular
            
            vec3 halfwayDir = normalize(lightDir + viewDir);  
            float spec = pow(max(dot(Normal, halfwayDir), 0.0), 16.0);
            vec3 specularPoint = pointLights[i].Color * spec * materialSpecular;
            // attenuation
            
            float attenuation = 1.0 / (1.0 + pointLights[i].Linear * distance + pointLights[i].Quadratic * distance * distance);
            //float attenuation = 1.0 / (constant + linear * distance + quadratic * distance * distance);
            diffusePoint *= attenuation;
            specularPoint *= attenuation;
            diffuse += diffusePoint;
            specular += specularPoint; 
        }    
    }

    // shadows
    vec4 shadowPos = shadowTransform * vec4(FragPos, 1.0);
    shadowPos.xyz /= shadowPos.w;
    shadowPos.xyz += 1;
    shadowPos.xyz /= 2;

    vec2 poisson[16] = vec2[](
        vec2( 0.175822635012014, -0.4670681557303145),
        vec2( 0.032875836318256, -0.3716577179299378),
        vec2( 0.435093145434407, -0.3747330435865004),
        vec2(-0.326518591238375, -0.2587878434022583),
        vec2(-0.023106715411561, -0.1627266333671908),
        vec2( 0.183966134538231, -0.2418361537067705),
        vec2( 0.398728555404662, -0.1677592745936137),
        vec2(-0.498842987805658, -0.0868929176075223),
        vec2( 0.156701151874775, -0.0477109776640396),
        vec2(-0.296212435824171, -0.0186820240384275),
        vec2( 0.366565313016260,  0.0638630368358581),
        vec2(-0.356609467634652,  0.1716487079271959),
        vec2(-0.037578938213331,  0.1085839022451354),
        vec2( 0.135498347742039,  0.1266952113027832),
        vec2(-0.202344466881747,  0.3661862546950298),
        vec2( 0.056197787889993,  0.4244375483310844)
    );

    float shadowFactor = 1;
    if(sunShadowOn) {
        shadowFactor = 0;
        for (int i = 0; i < 16; i++) {
            vec2 noise = poisson[i] * shadowSmoothness;
            float shadowDepth = texture(shadowMap, shadowPos.xy + noise).x;
            if (shadowPos.z <= shadowDepth + shadowOffset) {
                shadowFactor += 1/16.0;
            }
        }
    }
    
    vec3 result;
    if (alpha < 1) {
        result = materialAmbient * occlusion + ((0.5 * diff + 0.5) * materialDiffuse) + specular;
    }
    else {
        result = (ambient + (0.1 + shadowFactor * 0.9) * diffuse + shadowFactor * specular);
    }

    float brightness = dot(result, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0 && alpha < 1) 
        gBloom = vec4(result, 1.0);
    else
        gBloom = vec4(0.0, 0.0, 0.0, 1.0);
    //vec3 result = vec3(.95) * texture(ssao, TexCoords).r;
    //result = (ambient + (0.5+0.5*shadowFactor) * diffuse + shadowFactor * specular);
    FragColor = vec4(result, 1);
}
