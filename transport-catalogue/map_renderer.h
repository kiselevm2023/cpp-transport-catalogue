#pragma once

#include "svg.h"
#include "geo.h"
#include "domain.h"

#include <vector>
#include <string>
#include <deque>
#include <algorithm>
#include <set>

namespace renderer {

    using namespace std::literals;

namespace {

    inline const double EPSILON = 1e-6;

    bool IsZero(double value) {
        return std::abs(value) < EPSILON;
    }

    class SphereProjector {
    public:
        // points_begin и points_end задают начало и конец интервала элементов geo::Coordinates
        template <typename PointInputIt>
        SphereProjector(PointInputIt points_begin, PointInputIt points_end,
                        double max_width, double max_height, double padding)
            : padding_(padding) //
        {
            // Если точки поверхности сферы не заданы, вычислять нечего
            if (points_begin == points_end) {
                return;
            }

            // Находим точки с минимальной и максимальной долготой
            const auto [left_it, right_it] = std::minmax_element(
                points_begin, points_end,
                [](auto lhs, auto rhs) { return lhs.lng < rhs.lng; });
            min_lon_ = left_it->lng;
            const double max_lon = right_it->lng;

            // Находим точки с минимальной и максимальной широтой
            const auto [bottom_it, top_it] = std::minmax_element(
                points_begin, points_end,
                [](auto lhs, auto rhs) { return lhs.lat < rhs.lat; });
            const double min_lat = bottom_it->lat;
            max_lat_ = top_it->lat;

            // Вычисляем коэффициент масштабирования вдоль координаты x
            std::optional<double> width_zoom;
            if (!IsZero(max_lon - min_lon_)) {
                width_zoom = (max_width - 2 * padding) / (max_lon - min_lon_);
            }

            // Вычисляем коэффициент масштабирования вдоль координаты y
            std::optional<double> height_zoom;
            if (!IsZero(max_lat_ - min_lat)) {
                height_zoom = (max_height - 2 * padding) / (max_lat_ - min_lat);
            }

            if (width_zoom && height_zoom) {
                // Коэффициенты масштабирования по ширине и высоте ненулевые,
                // берём минимальный из них
                zoom_coeff_ = std::min(*width_zoom, *height_zoom);
            } else if (width_zoom) {
                // Коэффициент масштабирования по ширине ненулевой, используем его
                zoom_coeff_ = *width_zoom;
            } else if (height_zoom) {
                // Коэффициент масштабирования по высоте ненулевой, используем его
                zoom_coeff_ = *height_zoom;
            }
        }

        // Проецирует широту и долготу в координаты внутри SVG-изображения
        svg::Point operator()(geo::Coordinates coords) const {
            return {
                (coords.lng - min_lon_) * zoom_coeff_ + padding_,
                (max_lat_ - coords.lat) * zoom_coeff_ + padding_
            };
        }

    private:
        double padding_;
        double min_lon_ = 0;
        double max_lat_ = 0;
        double zoom_coeff_ = 0;
    };

} // namespace

    struct RenderSettings {
        double width;
        double height;
        double padding;
        double line_width;
        double stop_radius;
        int bus_label_font_size;
        double bus_label_offset[2];
        int stop_label_font_size;
        double stop_label_offset[2];
        std::string underlayer_color;
        double underlayer_width = 3.0;
        std::vector<std::string> color_palette;
    };

    class MapRenderer {
    public:
        svg::Document Render(const std::deque<domain::Bus>& buses) const;
        void SetSettings(const RenderSettings& settings);
            
    private:
        RenderSettings settings_;

        template <typename Container>
        void AddRouteSvg(std::vector<std::unique_ptr<svg::Drawable>>& routes, const std::deque<domain::Bus>& buses,
                    const SphereProjector& proj) const {
            std::size_t color_it = 0;
            for (const auto& bus : buses) {
                if (!bus.stops.empty()) {
                    routes.emplace_back(std::make_unique<Container>(bus, settings_, proj, 
                                                                    color_it % settings_.color_palette.size()));
                    ++color_it;
                }
            }
        }

        template <typename Container>
        void AddStopSvg(std::vector<std::unique_ptr<svg::Drawable>>& routes, const std::set<domain::Stop>& stops,
                    const SphereProjector& proj) const {
            routes.emplace_back(std::make_unique<Container>(stops, settings_, proj));
        }
    };

}  // namespace renderer