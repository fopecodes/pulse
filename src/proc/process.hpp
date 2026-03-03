#pragma once

#include <charconv>
#include <cstdint>
#include <expected>
#include <string>
#include <unordered_map>
#include <vector>

namespace pulse::proc {

    struct process_info {
        int pid{};
        std::string name;
        char state{};
        double cpu_percent{};
        uint64_t memory_kb{};
        std::string user;
    };

    class process_reader {
        public:
            [[nodiscard]] auto read() -> std::expected<std::vector<process_info>, std::string>;
        private:
            struct prev_cpu {
                uint64_t utime{};
                uint64_t stime{};
            };

            std::unordered_map<int, prev_cpu> m_previous;
            uint64_t m_prev_total_cpu{};
            bool m_has_previous{false};

            [[nodiscard]] auto get_total_cpu_time() -> uint64_t;
    };
}
