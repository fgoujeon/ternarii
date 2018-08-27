#ifndef LIBVIEW_EVENTS_HPP
#define LIBVIEW_EVENTS_HPP

#include "data_types.hpp"
#include <variant>
#include <functional>

namespace libview
{

namespace events
{
    struct clockwise_rotation_request{};
    struct drop_request{};
    struct left_shift_request{};
    struct right_shift_request{};
}

using event = std::variant
<
    events::clockwise_rotation_request,
    events::drop_request,
    events::left_shift_request,
    events::right_shift_request
>;

using event_handler = std::function<void(const event&)>;

} //namespace libview

#endif
