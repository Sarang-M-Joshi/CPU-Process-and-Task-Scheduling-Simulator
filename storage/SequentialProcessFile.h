#pragma once

#include "../models/Process.h"
#include <cstdio>
#include <cerrno>
#include <fstream>
#include <string>
#include <utility>
#include <vector>

namespace storage {

class SequentialProcessFile {
    std::string path_;
    static bool parse(const std::string& line, scheduler::Process& p) {
        std::size_t a = line.find(','), b = a == std::string::npos ? a : line.find(',', a + 1), c = b == std::string::npos ? b : line.find(',', b + 1);
        if (a == std::string::npos || b == std::string::npos || c == std::string::npos || line.find(',', c + 1) != std::string::npos) return false;
        try {
            std::size_t n1, n2, n3, n4;
            p.pid = std::stoi(line.substr(0, a), &n1); p.arrivalTime = std::stoi(line.substr(a + 1, b - a - 1), &n2);
            p.burstTime = std::stoi(line.substr(b + 1, c - b - 1), &n3); p.priority = std::stoi(line.substr(c + 1), &n4);
            return n1 == a && n2 == b - a - 1 && n3 == c - b - 1 && n4 == line.size() - c - 1 && p.arrivalTime >= 0 && p.burstTime > 0;
        } catch (...) { return false; }
    }
    static void write(std::ostream& out, const scheduler::Process& p) { out << p.pid << ',' << p.arrivalTime << ',' << p.burstTime << ',' << p.priority << '\n'; }
    bool readAll(std::vector<scheduler::Process>& records) const {
        std::ifstream in(path_);
        if (!in) {
            errno = 0;
            std::FILE* probe = std::fopen(path_.c_str(), "r");
            if (probe) { std::fclose(probe); return false; }
            return errno == ENOENT; // A missing file is an empty sequential file.
        }
        std::string line;
        while (std::getline(in, line)) { scheduler::Process p; if (!parse(line, p)) return false; records.push_back(p); }
        return in.eof();
    }
public:
    explicit SequentialProcessFile(std::string path = "process_records.csv") : path_(std::move(path)) {}
    const std::string& path() const { return path_; }
    bool display(std::vector<scheduler::Process>& records) const { records.clear(); return readAll(records); }
    bool append(const scheduler::Process& p) {
        std::vector<scheduler::Process> records; if (!readAll(records)) return false;
        for (const auto& old : records) if (old.pid == p.pid) return false;
        std::ofstream out(path_, std::ios::app); if (!out) return false; write(out, p); out.flush(); return static_cast<bool>(out);
    }
    bool find(int pid, scheduler::Process& result) const {
        std::vector<scheduler::Process> records; if (!readAll(records)) return false;
        for (const auto& p : records) if (p.pid == pid) { result = p; return true; }
        return false;
    }
    bool erase(int pid, bool& found) {
        found = false; std::vector<scheduler::Process> records; if (!readAll(records)) return false;
        std::string temp = path_ + ".tmp", backup = path_ + ".bak";
        std::ofstream out(temp, std::ios::trunc); if (!out) return false;
        for (const auto& p : records) { if (p.pid == pid) found = true; else write(out, p); }
        out.flush(); if (!out) { out.close(); std::remove(temp.c_str()); return false; } out.close();
        if (!found) { std::remove(temp.c_str()); return true; }
        // Rename the original to a backup first; restore it if installing the completed temp file fails.
        std::remove(backup.c_str());
        const bool originalExists = static_cast<bool>(std::ifstream(path_));
        if (originalExists && std::rename(path_.c_str(), backup.c_str()) != 0) { std::remove(temp.c_str()); return false; }
        if (std::rename(temp.c_str(), path_.c_str()) != 0) {
            if (originalExists) std::rename(backup.c_str(), path_.c_str());
            std::remove(temp.c_str()); return false;
        }
        if (originalExists) std::remove(backup.c_str());
        return true;
    }
};
}  // namespace storage
