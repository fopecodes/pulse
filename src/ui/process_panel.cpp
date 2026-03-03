#include "process_panel.hpp"

#include <format>
#include <ftxui/dom/elements.hpp>

using namespace ftxui;

namespace pulse::ui {

    auto process_panel::render(const std::vector<proc::process_info>& procs) -> Element {
        Elements rows;

        rows.push_back(
            hbox({
                text(std::format("{:>7}", "PID")) | bold,
                text(std::format("  {:<10}", "USER")) | bold,
                text(std::format("  {:>6}", "CPU%")) | bold,
                text(std::format("  {:>10}", "MEM")) | bold,
                text(std::format("  {}", "STATE")) | bold,
                text(std::format("  {}", "NAME")) | bold,
            }) | color(Color::Cyan)
        );
        rows.push_back(separator());

        auto count = std::min(static_cast<int>(procs.size()), m_max_visible);

        for (int i = 0; i < count; ++i) {
            const auto& p = procs[i];
            auto mem_str = p.memory_kb >= 1024 ? std::format("{:.1f} MiB", static_cast<double>(p.memory_kb) / 1024.0) : std::format("{} KiB", p.memory_kb);

            auto line_color = p.cpu_percent > 50.0 ? Color::Red : p.cpu_percent > 10.0 ? Color::Yellow : Color::White;

            rows.push_back(
                hbox({
                    text(std::format("{:>7}", p.pid)),
                    text(std::format("  {:<10}", p.user.substr(0, 10))),
                    text(std::format("  {:>5.1f}%", p.cpu_percent)),
                    text(std::format("  {:>10}", mem_str)),
                    text(std::format("      {}", p.state)),
                    text(std::format("  {}", p.name)),
                }) | color(line_color)
            );
        }

        return vbox({
            text(std::format(" Processes ({})", procs.size())) | bold,
            separator(),
            vbox(rows) | flex,
        }) | border;
    }
}
