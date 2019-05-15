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

#ifndef LIBVIEW_SCORE_DISPLAY_HPP
#define LIBVIEW_SCORE_DISPLAY_HPP

#include "label.hpp"
#include <libsdl.hpp>

namespace libview
{

class score_display
{
    public:
        score_display(SDL_Renderer& renderer, const SDL_Rect& area);

        void set_score(const unsigned int value);

        void draw(const geometry::system& sys);

    private:
        label label_;
};

} //namespace libview

#endif
