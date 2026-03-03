#include "process.hpp"

#include <algorithm>
#include <charconv>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <pwd.h>
#include <ranges>
#include <sstream>
#include <sys/types.h>
#include <unistd.h>

namespace fs = std::filesystem;

namespace pulse::proc {

    auto process_reader::get_total_cpu_time() -> uint64_t {
        std::ifstream file("/proc/stat");
        std::string line;
        std::getline(file, line);
        std::istringstream iss(line);
        std::string label;
        uint64_t total = 0, val;
        iss >> label;
        while (iss >> val) total += val;
        return total;
    }

    auto process_reader::read() -> std::expected<std::vector<process_info>, std::string> {
        auto current_total = get_total_cpu_time();
        std::vector<process_info> procs;

        for (const auto& entry : fs::directory_iterator("/proc")) {
            if (!entry.is_directory()) continue;

            auto dirname = entry.path().filename().string();
            int pid{};
            auto [ptr, ec] = std::from_chars(dirname.data(), dirname.data() + dirname.size(), pid);
            if (ec != std::errc{}) continue;

            std::ifstream stat_file(entry.path() / "stat");
            if (!stat_file.is_open()) continue;

            std::string stat_line;
            std::getline(stat_file, stat_line);

            // name is between parentheses
            auto name_start = stat_line.find('(');
            auto name_end = stat_line.rfind(')');
            if (name_start == std::string::npos || name_end == std::string::npos) continue;

            process_info info;
            info.pid = pid;
            info.name = stat_line.substr(name_start + 1, name_end - name_start - 1);

            std::istringstream iss(stat_line.substr(name_end + 2));
            std::string state_str;
            uint64_t ppid, pgrp, session, tty, tpgid, flags;
            uint64_t minflt, cminflt, majflt, cmajflt;
            uint64_t utime, stime;
            iss >> state_str >> ppid >> pgrp >> session >> tty >> tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt >> utime >> stime;

            info.state = state_str.empty() ? '?' : state_str[0];
            

            // cpu usage
            if (m_has_previous && m_previous.contains(pid)) {
                auto& prev = m_previous[pid];
                auto proc_diff = (utime + stime) - (prev.utime + prev.stime);

                auto total_diff = current_total - m_prev_total_cpu;
                if (total_diff > 0) {
                    info.cpu_percent = (static_cast<double>(proc_diff) / static_cast<double>(total_diff)) * 100.0;
                }
            }

            m_previous[pid] = { utime, stime };

            std::ifstream statm_file(entry.path() / "statm");
            if (statm_file.is_open()) {
                uint64_t size, resident;
                statm_file >> size >> resident;
                auto page_size = static_cast<uint64_t>(sysconf(_SC_PAGESIZE));
                info.memory_kb = (resident * page_size) / 1024;
            }

            std::ifstream status_file(entry.path() / "status");
            if (status_file.is_open()) {
                std::string line;
                while (std::getline(status_file, line)) {
                    if (line.starts_with("Uid:")) {
                        std::istringstream uid_iss(line.substr(4));
                        uid_t uid;
                        uid_iss >> uid;
                        if (auto* pw = getpwuid(uid)) {
                            info.user = pw->pw_name;
                        }
                        break;
                    }
                }
            }
            procs.push_back(info);
        }
        m_prev_total_cpu = current_total;
        m_has_previous = true;

        std::ranges::sort(procs, [](const auto& a, const auto& b) {
            return a.cpu_percent > b.cpu_percent;
        });

        return procs;
    }
}
