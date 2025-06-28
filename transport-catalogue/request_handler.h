#pragma once

#include <optional>

#include "transport_catalogue.h"
#include "map_renderer.h"

namespace transport_catalogue {

    class RequestHandler {
    public:
        RequestHandler(const TransportCatalogue& db, const renderer::MapRenderer& renderer) :
            db_(db), renderer_(renderer) {}

        const std::optional<BusInfo> GetBusStat(const std::string_view& bus_name) const;

        const std::optional<std::unordered_set<std::string_view>> GetBusesByStop(const std::string_view& stop_name) const;

        svg::Document RenderMap() const;

    private:
        const TransportCatalogue& db_;
        const renderer::MapRenderer& renderer_;
    };

}  // namespace transport_catalogue