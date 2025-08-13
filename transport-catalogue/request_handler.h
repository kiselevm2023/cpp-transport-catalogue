 #pragma once

#include <optional>

#include "transport_catalogue.h"
#include "map_renderer.h"
#include "transport_router.h"

namespace transport_catalogue {

class RequestHandler {
public:
    RequestHandler(
        const TransportCatalogue& db,
        const renderer::MapRenderer& renderer,
        const transport_router::TransportRouter& router
    ) :
        db_(db), renderer_(renderer), router_(router)
    {}

    const std::optional<BusInfo> GetBusStat(const std::string_view& bus_name) const;

    const std::optional<std::unordered_set<std::string_view>> GetBusesByStop(const std::string_view& stop_name) const;

    svg::Document RenderMap() const;

    std::optional<transport_router::RouteItems> GetRouteInfo(const std::string_view from, const std::string_view to) const;

private:
    const TransportCatalogue& db_;
    const renderer::MapRenderer& renderer_;
    const transport_router::TransportRouter& router_;
};

}  // namespace transport_catalogue