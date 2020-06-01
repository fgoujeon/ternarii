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
precision mediump float;
#endif

uniform highp mat3 u_transformation_matrix;
uniform lowp vec4 u_color;
uniform mediump float u_radius;
uniform mediump float u_smoothness;

varying highp vec2 v_position;

void main()
{
    float xscaling = distance(u_transformation_matrix[0].x, u_transformation_matrix[0].y);
    float yscaling = distance(u_transformation_matrix[1].x, u_transformation_matrix[1].y);

    float xradius = u_radius / xscaling;
    float yradius = u_radius / yscaling;

    vec2 v_position_abs = abs(v_position);

    float alpha;

    bool on_x_edge = v_position_abs.x > 1.0 - xradius;
    bool on_y_edge = v_position_abs.y > 1.0 - yradius;

    if(on_x_edge && on_y_edge) //corner
    {
        float x = v_position_abs.x;
        float y = v_position_abs.y;
        float x0 = 1.0 - xradius;
        float y0 = 1.0 - yradius;

        float ellipse =
            (x - x0) * (x - x0) / (xradius * xradius) +
            (y - y0) * (y - y0) / (yradius * yradius)
        ;

        float s = u_smoothness / u_radius / 0.5;
        alpha = 1.0 - smoothstep(1.0 - s, 1.0, ellipse);
    }
    else if(on_x_edge) //left or right edge
    {
        float s = u_smoothness / xscaling;
        alpha = 1.0 - smoothstep(1.0 - s, 1.0, v_position_abs.x);
    }
    else if(on_y_edge) //top or bottom edge
    {
        float s = u_smoothness / yscaling;
        alpha = 1.0 - smoothstep(1.0 - s, 1.0, v_position_abs.y);
    }
    else
    {
        alpha = 1.0;
    }

    gl_FragColor = vec4(u_color.xyz, alpha * u_color.w);
}
)^"
