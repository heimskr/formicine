#include "performance.h"
#include "ansi.h"

namespace formicine {
	performance perf = {};

	watcher::watcher(const std::string &name_, performance *parent_): name(name_), parent(parent_) {
#ifndef DISABLE_PERFORMANCE
		restart();
#endif
	}

	watcher::~watcher() {
#ifndef DISABLE_PERFORMANCE
		if (!canceled)
			parent->stop(name);
#endif
	}

	void watcher::restart() {
#ifndef DISABLE_PERFORMANCE
		parent->start(name);
#endif
	}

	performance::~performance() {
#ifndef DISABLE_PERFORMANCE
		results();
#endif
	}

#ifndef DISABLE_PERFORMANCE
	void performance::start(const std::string &timer_name) {
		timers[timer_name] = std::chrono::system_clock::now().time_since_epoch();
#else
	void performance::start(const std::string &) {
#endif
	}

#ifndef DISABLE_PERFORMANCE
	performance::timetype performance::stop(const std::string &timer_name) {
		std::chrono::system_clock::duration now = std::chrono::system_clock::now().time_since_epoch();
		const auto diff = now - timers[timer_name];
		totals[timer_name] += diff;
		runs[timer_name].push_back(diff);
		return diff;
#else
	performance::timetype performance::stop(const std::string &) {
		return {};
#endif
	}

#ifndef DISABLE_PERFORMANCE
	bool performance::reset(const std::string &timer_name) {
		const bool did_exist = 0 < timers.count(timer_name);
		timers.erase(timer_name);
		totals.erase(timer_name);
		return did_exist;
#else
	bool performance::reset(const std::string &) {
		return true;
#endif
	}

	void performance::results() {
#ifndef DISABLE_PERFORMANCE
		for (const auto &pair: totals)
			DBG(ansi::bold(pair.first) << ": "_d << ansi::cyan(std::to_string(runs[pair.first].size())) << " -> "_d << ansi::orange(std::to_string(pair.second.count())) << " μs (average: " << ansi::magenta(std::to_string(pair.second.count() / runs[pair.first].size())) << " μs)");
		DBG("Finished list.");
#endif
	}

	watcher performance::watch(const std::string &timer_name) {
		return watcher(timer_name, this);
	}
}
