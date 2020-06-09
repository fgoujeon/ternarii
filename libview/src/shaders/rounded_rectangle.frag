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

#ifdef GL_ES
precision lowp float;
#endif

uniform lowp vec2 u_dimension; //normalized dimension of rectangle
uniform lowp vec4 u_color;
uniform lowp float u_radius;
uniform lowp float u_smoothness;
uniform lowp float u_outline_thickness;
uniform lowp vec4 u_outline_color;

varying highp vec2 v_position;

void main()
{
    vec2 abs_pos = abs(v_position);

    //Are we inside the rectangle?
    if(abs_pos.x > u_dimension.x || abs_pos.y > u_dimension.y)
    {
        discard;
    }

    bool on_x_edge = abs_pos.x > u_dimension.x - u_radius;
    bool on_y_edge = abs_pos.y > u_dimension.y - u_radius;

    //Compute the distance to the inner rectangle whose vertices are the centers
    //of the circles that make the corners.
    float dist;
    if(on_x_edge && on_y_edge) //corner
    {
        vec2 circle_center = vec2
        (
            u_dimension.x - u_radius,
            u_dimension.y - u_radius
        );
        dist = distance(circle_center, abs_pos);
    }
    else if(on_x_edge) //left or right edge
    {
        dist = abs_pos.x - (u_dimension.x - u_radius);
    }
    else if(on_y_edge) //top or bottom edge
    {
        dist = abs_pos.y - (u_dimension.y - u_radius);
    }
    else
    {
        dist = 0.0;
    }

    //Fill
    float fill_alpha = 1.0 - smoothstep
    (
        u_radius - u_outline_thickness - u_smoothness,
        u_radius - u_outline_thickness,
        dist
    );
    gl_FragColor = u_color * fill_alpha;

    //Outline
    if(u_outline_thickness > 0.0)
    {
        float outline_alpha_0 = 1.0 - fill_alpha;
        float outline_alpha_1 = 1.0 - smoothstep
        (
            u_radius - u_smoothness,
            u_radius,
            dist
        );
        gl_FragColor += u_outline_color * outline_alpha_0 * outline_alpha_1;
    }
}
)^"
