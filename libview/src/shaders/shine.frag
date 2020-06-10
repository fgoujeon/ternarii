R"^(/*
Copyright 2018 - 2020 Florian Goujeon

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

#define PI2 6.28318530718

#ifdef GL_ES
precision lowp float;
#endif

uniform lowp vec4 u_color;
uniform lowp float u_angle; //[0,1]
uniform lowp float u_ray_count;
uniform lowp float u_ray_width; //[0,1]
uniform lowp float u_smoothness;

varying highp vec2 v_position;

void main()
{
    //compute polar coordinate of current position
    float angle = atan(v_position.y, v_position.x); //[-pi,pi], in radians
    float radius = length(v_position);

    //normalize angle
    angle = angle / PI2 + 0.5; //[0,1]

    //shift angle (typically for animation)
    angle = mod(angle + u_angle, 1.0); //[0,1]

    //make saw wave function with angle
    angle = mod(angle, 1.0 / u_ray_count) * u_ray_count; //[0,1]
    angle = abs(angle - 0.5) / 0.5; //[0,1]

    //we want rays to be opaque in center and transparent at end of radius
    float alpha = (1.0 - radius);

    //smooth ray borders
    alpha = alpha * smoothstep(u_ray_width + u_smoothness / radius, u_ray_width, angle);

    gl_FragColor = u_color * alpha;
}
)^"
