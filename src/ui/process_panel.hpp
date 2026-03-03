#pragma once

#include "proc/process.hpp"

#include <ftxui/dom/elements.hpp>
#include <vector>

namespace pulse::ui {

    class process_panel {
        public:
            [[nodiscard]] auto render(const std::vector<proc::process_info>& procs) -> ftxui::Element;

            int m_max_visible = 20;
    };
}
