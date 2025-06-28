#include "map_renderer.h"

#include <memory>
#include <cstdlib>
#include <iostream>
#include <optional>

namespace renderer {

namespace {

    class SvgCatalogue : public svg::Drawable {
    public:
        SvgCatalogue(const RenderSettings& settings, const SphereProjector& proj) :
            settings_(settings), proj_(proj) {}

        const RenderSettings& GetSettings() const {
            return settings_;
        }

        const SphereProjector& GetProj() const {
            return proj_;
        }

    private:
        const RenderSettings& settings_;
        const SphereProjector& proj_;
    };
    
    class Route : public SvgCatalogue {
    public:
        Route(const domain::Bus& bus, const RenderSettings& settings, const SphereProjector& proj, std::size_t color) :
            SvgCatalogue(settings, proj), bus_(bus), color_(color) {}

        void Draw(svg::ObjectContainer& container) const override {
            svg::Polyline polyline;
            for (const auto& stop : bus_.stops) {
                polyline.AddPoint(GetProj()(stop->coordinates));
            }
            polyline.SetStrokeColor(GetSettings().color_palette[color_])
                    .SetFillColor("none")
                    .SetStrokeWidth(GetSettings().line_width)
                    .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                    .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
            container.Add(polyline);
        }
    
    private:
        const domain::Bus& bus_;
        std::size_t color_;
    };

    class RouteNames : public SvgCatalogue {
    public:
        RouteNames(const domain::Bus& bus, const RenderSettings& settings, const SphereProjector& proj, std::size_t color) :
            SvgCatalogue(settings, proj), bus_(bus), color_(color) {}

        void Draw(svg::ObjectContainer& container) const override {
            std::vector<svg::Text> route_names;

            route_names.push_back(svg::Text().SetData(bus_.name)
                                .SetPosition(GetProj()(bus_.stops[0]->coordinates))
                                .SetOffset({GetSettings().bus_label_offset[0], GetSettings().bus_label_offset[1]})
                                .SetFontSize(GetSettings().bus_label_font_size)
                                .SetFontFamily("Verdana")
                                .SetFontWeight("bold")
                                .SetStrokeColor(GetSettings().underlayer_color)
                                .SetFillColor(GetSettings().underlayer_color)
                                .SetStrokeWidth(GetSettings().underlayer_width)
                                .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                                .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND));

            route_names.push_back(svg::Text().SetData(bus_.name)
                                .SetPosition(GetProj()(bus_.stops[0]->coordinates))
                                .SetOffset({GetSettings().bus_label_offset[0], GetSettings().bus_label_offset[1]})
                                .SetFontSize(GetSettings().bus_label_font_size)
                                .SetFontFamily("Verdana")
                                .SetFontWeight("bold")
                                .SetFillColor(GetSettings().color_palette[color_]));
            if (!bus_.is_roundtrip) {
                std::size_t stop_it = bus_.stops.size() / 2;
                if (bus_.stops[0]->name != bus_.stops[stop_it]->name) {
                    route_names.push_back(svg::Text().SetData(bus_.name)
                                        .SetPosition(GetProj()(bus_.stops[stop_it]->coordinates))
                                        .SetOffset({GetSettings().bus_label_offset[0], GetSettings().bus_label_offset[1]})
                                        .SetFontSize(GetSettings().bus_label_font_size)
                                        .SetFontFamily("Verdana")
                                        .SetFontWeight("bold")
                                        .SetStrokeColor(GetSettings().underlayer_color)
                                        .SetFillColor(GetSettings().underlayer_color)
                                        .SetStrokeWidth(GetSettings().underlayer_width)
                                        .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                                        .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND));

                    route_names.push_back(svg::Text().SetData(bus_.name)
                                        .SetPosition(GetProj()(bus_.stops[stop_it]->coordinates))
                                        .SetOffset({GetSettings().bus_label_offset[0], GetSettings().bus_label_offset[1]})
                                        .SetFontSize(GetSettings().bus_label_font_size)
                                        .SetFontFamily("Verdana")
                                        .SetFontWeight("bold")
                                        .SetFillColor(GetSettings().color_palette[color_]));
                }
            }

            for (const auto& route : route_names) {
                container.Add(route);
            }
        }
    private:
        const domain::Bus& bus_;
        std::size_t color_;
    };

    class StopSymbols : public SvgCatalogue {
    public:
        StopSymbols(const std::set<domain::Stop>& stops, const RenderSettings& settings, const SphereProjector& proj) :
            SvgCatalogue(settings, proj), stops_(stops) {}

        void Draw(svg::ObjectContainer& container) const override {
            std::vector<svg::Circle> stop_symbols;

            for (const auto& stop : stops_) {
                stop_symbols.push_back(svg::Circle().SetCenter(GetProj()(stop.coordinates))
                                                    .SetRadius(GetSettings().stop_radius)
                                                    .SetFillColor("white"));
            }

            for (const auto& symbol : stop_symbols) {
                container.Add(symbol);
            }
        }
    
    private:
        const std::set<domain::Stop>& stops_;
    };

    class StopNames : public SvgCatalogue {
    public:
        StopNames(const std::set<domain::Stop>& stops, const RenderSettings& settings, const SphereProjector& proj) :
            SvgCatalogue(settings, proj), stops_(stops) {}

        void Draw(svg::ObjectContainer& container) const override {
            std::vector<svg::Text> stop_names;

            for (const auto& stop : stops_) {
                stop_names.push_back(svg::Text().SetData(stop.name)
                                    .SetPosition(GetProj()(stop.coordinates))
                                    .SetOffset({GetSettings().stop_label_offset[0], GetSettings().stop_label_offset[1]})
                                    .SetFontSize(GetSettings().stop_label_font_size)
                                    .SetFontFamily("Verdana")
                                    .SetStrokeColor(GetSettings().underlayer_color)
                                    .SetFillColor(GetSettings().underlayer_color)
                                    .SetStrokeWidth(GetSettings().underlayer_width)
                                    .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                                    .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND));

                stop_names.push_back(svg::Text().SetData(stop.name)
                                    .SetPosition(GetProj()(stop.coordinates))
                                    .SetOffset({GetSettings().stop_label_offset[0], GetSettings().stop_label_offset[1]})
                                    .SetFontSize(GetSettings().stop_label_font_size)
                                    .SetFontFamily("Verdana")
                                    .SetFillColor("black"));
            }

            for (const auto& stop : stop_names) {
                container.Add(stop);
            }
        }

    private:
        const std::set<domain::Stop>& stops_;
    };

    template <typename DrawableIterator>
    void DrawPicture(DrawableIterator begin, DrawableIterator end, svg::ObjectContainer& target) {
        for (auto it = begin; it != end; ++it) {
            (*it)->Draw(target);
        }
    }

    template <typename Container>
    void DrawPicture(const Container& container, svg::ObjectContainer& target) {
        using namespace std;
        DrawPicture(begin(container), end(container), target);
    }

}  // namespace

    svg::Document MapRenderer::Render(const std::deque<domain::Bus>& buses) const {
        std::vector<geo::Coordinates> all_cords;
        for(const auto& bus : buses) {
            for (const auto& stop : bus.stops) {
                all_cords.push_back(stop->coordinates);
            }
        }
        const SphereProjector proj {
            all_cords.begin(), all_cords.end(),
            settings_.width, settings_.height, settings_.padding
        };

        std::set<domain::Stop> stops;
        for (const auto& bus : buses) {
            for (const auto& stop : bus.stops) {
                stops.insert(*stop);
            }
        }

        std::vector<std::unique_ptr<svg::Drawable>> routes;
        AddRouteSvg<Route>(routes, buses, proj);
        AddRouteSvg<RouteNames>(routes, buses, proj);
        AddStopSvg<StopSymbols>(routes, stops, proj);
        AddStopSvg<StopNames>(routes, stops, proj);

        svg::Document doc;
        DrawPicture(routes, doc);
        return doc;
    }

    void MapRenderer::SetSettings(const RenderSettings& settings) {
        settings_ = settings;
    }

}  // namespace renderer