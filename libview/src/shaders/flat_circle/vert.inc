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

#define POSITION_ATTRIBUTE_LOCATION 0

uniform highp mat3 transformationProjectionMatrix;
attribute highp vec2 position;

varying highp vec2 var_position;

void main()
{
    var_position = position;
    gl_Position.xywz = vec4(transformationProjectionMatrix * vec3(position, 1.0), 0.0);
}

)^"
