#pragma once

#include <cstdint>
#include <span>
#include <numbers>
#include <string_view>
#include <ftxui/dom/node.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <thread>
#include <iostream>

namespace Ark 
{

    class Graph 
    {
    public:
        std::vector<int> operator()(int width, int height) const 
        {
            std::vector<int> output(width);
            for (int i = 0; i < width; ++i) {
            float v = 0;
            v += 0.1f * sin((i + shift) * 0.1f);        // NOLINT
            v += 0.2f * sin((i + shift + 10) * 0.15f);  // NOLINT
            v += 0.1f * sin((i + shift) * 0.03f);       // NOLINT
            v *= height;                                // NOLINT
            v += 0.5f * height;                         // NOLINT
            output[i] = static_cast<int>(v);
            }
            return output;
        }
        int shift = 0;
    };

    inline std::vector<int> triangle(int width, int height) 
    {
        std::vector<int> output(width);

        for (int i = 0; i < width; ++i) {
            double x = static_cast<double>(i);

            double y =
                std::sin(x * 0.13) * 0.45 +
                std::sin(x * 0.37) * 0.25 +
                std::cos(x * 0.071) * 0.20 +
                std::sin(x * x * 0.003) * 0.10;

            output[i] = static_cast<int>(
                (y + 1.0) * 0.5 * (height - 1)
            );
        }

        return output;
    }


    template<typename T>
    struct FAudioVisualizer 
    {
        FAudioVisualizer() = default;
        ~FAudioVisualizer() = default;

        FAudioVisualizer(FAudioVisualizer&) = default;
        FAudioVisualizer(FAudioVisualizer&&) = default;

        FAudioVisualizer& operator=(FAudioVisualizer&) = default;
        FAudioVisualizer& operator=(FAudioVisualizer&&) = default;

        void Draw([[maybe_unused]] std::span<T> buffer)
        {
        }

        void Draw([[maybe_unused]] std::uint8_t character)
        {
        }

        inline void Draw(void) // Stub to remove, only for prototyping ...
                               // 
        {

        using namespace ftxui;
        using namespace std::chrono_literals;

        Graph my_graph;

        std::string reset_position;
        for (int i = 0;; ++i) 
        {
            std::ignore = i;
            auto document = hbox({
                vbox({
                    // graph(std::ref(my_graph)),
                    // separator(),
                    // graph(triangle) /* | inverted */,
                    // separator(),
                    graph(std::ref(my_graph)) /* | inverted */
                }) | flex
            });

            document |= border;

            const int min_width = 40;
            document |= size(HEIGHT, GREATER_THAN, min_width);

            auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
            Render(screen, document);
            std::cout << reset_position;
            screen.Print();
            reset_position = screen.ResetPosition();

            const auto sleep_time = 0.03s;
            std::this_thread::sleep_for(sleep_time);
            my_graph.shift++;
        }
                }
    };
}
