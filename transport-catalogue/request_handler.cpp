#include "request_handler.h"

#include <algorithm>

namespace transport_catalogue {

    using namespace domain;

    const std::optional<BusInfo> RequestHandler::GetBusStat(const std::string_view& bus_name) const {
        auto bus = db_.FindBus(bus_name);
        if (bus != nullptr) {
            return db_.GetBusInfo(bus->name);
        }
        return std::nullopt;
    }

    const std::optional<std::unordered_set<std::string_view>> RequestHandler::GetBusesByStop(const std::string_view& stop_name) const {
        auto stop = db_.FindStop(stop_name);
        if (stop != nullptr) {
            return db_.GetStopInfo(stop->name);
        }
        return std::nullopt;
    }

    svg::Document RequestHandler::RenderMap() const {
        auto buses = db_.GetBuses();
        std::sort(buses.begin(), buses.end(), [](const Bus& a, const Bus& b) 
            {
                return a.name < b.name;
            }
        );
        return renderer_.Render(buses);
    }

}  // namespace transport_catalogue