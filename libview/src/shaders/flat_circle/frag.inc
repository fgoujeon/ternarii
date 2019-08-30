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

uniform lowp vec4 color;

varying highp vec2 var_position;

void main()
{
    highp float radius = 1.0;
    highp float dist = sqrt(dot(var_position, var_position));

    if(dist >= radius)
    {
        discard;
    }

    highp float alpha = smoothstep(radius, radius - 0.05, dist);
    gl_FragColor = vec4(color.xyz, alpha * color.w);
}

)^"