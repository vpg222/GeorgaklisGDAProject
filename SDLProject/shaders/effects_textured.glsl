uniform sampler2D diffuse;
varying vec2 texCoordVar;

void main() {
    vec4 color = texture2D(diffuse, texCoordVar);
    float c = (color.r + color.g + color.b) / 3.0;
    vec3 luminance = vec3(dot(vec3(0.2126,0.7152,0.0722), color.rgb));
    vec3 m = mix(luminance, color.rgb, 0.5);
    gl_FragColor = vec4(m, color.a);
}
