/*
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

R"^(

#ifdef GL_ES
precision mediump float;
#endif

uniform lowp vec4 color;

varying highp vec2 var_position;

void main()
{
    float radius = 0.5;
    vec2 var_position_abs = abs(var_position);

    float alpha;

    if(var_position_abs.x > 1.0 - radius && var_position_abs.y > 1.0 - radius) //corner
    {
        float dist = distance(var_position_abs, vec2(1.0 - radius));

        if(dist >= radius)
        {
            discard;
        }

        alpha = smoothstep(radius, radius - 0.05, dist);
    }
    else
    {
        float max_xy = max(var_position_abs.x, var_position_abs.y);

        alpha = smoothstep(1.0, 1.0 - 0.05, max_xy);
    }

    gl_FragColor = vec4(color.xyz, alpha * color.w);
}

)^"
