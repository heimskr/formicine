#ifndef FORMICINE_PERFORMANCE_H_
#define FORMICINE_PERFORMANCE_H_

#include <chrono>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace spjalla {
	class performance;

	struct watcher {
		std::string name;
		performance *parent;

		watcher(const std::string &, performance *);
		~watcher();
	};

	/**
	 * Contains utilities for measuring performance of specific blocks of code.
	 */
	class performance {
		using timetype = std::chrono::system_clock::duration;

		private:
			/** Maps timer names to the times they were started. */
			std::unordered_map<std::string, timetype> timers = {};

			/** Maps timer names to a list of times they've taken. */
			std::unordered_map<std::string, std::vector<timetype>> runs = {};

			/** Maps timer names to the total amount of time they've taken. */
			std::unordered_map<std::string, timetype> totals = {};

		public:
			performance() = default;
			performance(const performance &) = delete;
			performance(performance &&) = delete;
			performance & operator=(const performance &) = delete;
			performance & operator=(performance &&) = delete;

			~performance();

			/** Starts a timer. */
			void start(const std::string &);

			/** Stops a timer and returns the amount of time that elapsed. */
			timetype stop(const std::string &);

			/** Removes all data about a timer. Returns true if anything was found and removed. */
			bool reset(const std::string &);

			/** Displays all results so far. */
			void results();

			/** Returns a watcher for a given timer name. It starts a timer on creation and stops it on destruction. */
			watcher watch(const std::string &);
		
			template <typename R, typename... Args>
			timetype operator()(const std::string &timer_name, std::function<R(Args...)> fn, Args && ...args) {
				start(timer_name);
				fn(args...);
				return stop(timer_name);
			}
	};

	extern performance perf;
}

#endif
