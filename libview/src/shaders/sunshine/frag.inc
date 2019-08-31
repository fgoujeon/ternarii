/*
Copyright 2018 - 2019 Florian Goujeon

This file is part of Ternarii.

Ternarii is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Ternarii is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Ternarii.  If not, see <https://www.gnu.org/licenses/>.
*/

R"^(

#define PI2 6.28318530718
#define RAY_COUNT 16.0

#ifdef GL_ES
precision mediump float;
#endif

uniform lowp vec4 color;

varying highp vec2 var_position;

void main()
{
    float angle = atan(var_position.y, var_position.x);
    float normalized_angle = angle / PI2 + 0.5 /*+ mod(time / 24.0, 1.0)*/;
    float normalized_angle_mod = mod(normalized_angle, 1.0 / RAY_COUNT) * RAY_COUNT;
    float normalized_angle_saw_wave = abs(normalized_angle_mod - 0.5) * 2.0;

    if(normalized_angle_saw_wave > 0.5)
    {
        discard;
    }

    float radius = length(var_position);
    float radius_alpha = (1.0 - radius);

    float angle_alpha = smoothstep(0.5, 0.45, normalized_angle_saw_wave);

    float alpha = color.w * radius_alpha * angle_alpha;

    gl_FragColor = vec4(color.xyz, alpha);
}

)^"
