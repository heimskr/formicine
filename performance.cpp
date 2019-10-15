#include "performance.h"
#include "ansi.h"

namespace formicine {
	performance perf = {};

	watcher::watcher(const std::string &name_, performance *parent_): name(name_), parent(parent_) {
		parent->start(name);
	}

	watcher::~watcher() {
		if (!canceled)
			parent->stop(name);
	}

	performance::~performance() {
		results();
	}

	void performance::start(const std::string &timer_name) {
		timers[timer_name] = std::chrono::system_clock::now().time_since_epoch();
	}

	performance::timetype performance::stop(const std::string &timer_name) {
		std::chrono::system_clock::duration now = std::chrono::system_clock::now().time_since_epoch();
		const auto diff = now - timers[timer_name];
		totals[timer_name] += diff;
		runs[timer_name].push_back(diff);
		return diff;
	}

	bool performance::reset(const std::string &timer_name) {
		const bool did_exist = 0 < timers.count(timer_name);
		timers.erase(timer_name);
		totals.erase(timer_name);
		return did_exist;
	}

	void performance::results() {
		for (const auto &pair: totals) {
			DBG(ansi::bold(pair.first) << ": "_d << ansi::cyan(std::to_string(runs[pair.first].size())) << " -> "_d << ansi::orange(std::to_string(pair.second.count())) << " μs (average: " << ansi::magenta(std::to_string(pair.second.count() / runs[pair.first].size())) << " μs)");
		}
		DBG("Finished list.");
	}

	watcher performance::watch(const std::string &timer_name) {
		return watcher(timer_name, this);
	}
}
