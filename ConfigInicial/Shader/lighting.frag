#version 330 core

// Estructuras (sin cambios)
struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light
{
    vec3 position;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

// Entradas (sin cambios)
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

// Salida (sin cambios)
out vec4 color;

// Uniforms (MODIFICADOS)
uniform vec3 viewPos;
uniform Material material;
uniform sampler2D texture_diffuse1; // Aseg�rate que el nombre coincida (en tu frag original era texture_diffuse1)

// --- MODIFICACI�N ---
// Dos uniformes de luz en lugar de uno
uniform Light sunLight;
uniform Light moonLight;
// --- FIN MODIFICACI�N ---


// --- NUEVA FUNCI�N ---
// Esta funci�n calcula la contribuci�n de UNA luz
vec3 CalcLight(Light light, vec3 norm, vec3 fragPos, vec3 viewDir)
{
    // Ambient
    vec3 ambient = light.ambient * material.diffuse;
    
    // Diffuse
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * material.diffuse;
    
    // Specular
    vec3 viewDirNorm = normalize(viewDir); // Asegurarse que est� normalizado
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDirNorm, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);
    
    return (ambient + diffuse + specular);
}
// --- FIN NUEVA FUNCI�N ---


void main()
{
    // Normalizar vectores una sola vez
    vec3 norm = normalize(Normal);
    vec3 viewDir = viewPos - FragPos; // No normalizar aqu�, CalcLight lo hace

    // --- MODIFICACI�N ---
    // Calcular la luz del sol
    vec3 result = CalcLight(sunLight, norm, FragPos, viewDir);
    // Sumar la luz de la luna
    result += CalcLight(moonLight, norm, FragPos, viewDir);
    // --- FIN MODIFICACI�N ---
    
    // Aplicar textura
    // (Uso texture_diffuse1 como en tu modelLoading.frag)
    vec4 texColor = texture(texture_diffuse1, TexCoords);
    if(texColor.a < 0.1)
        discard;

    color = vec4(result, 1.0f) * texColor;
}