#pragma once

#include "BitmappedPriorityDeque.hpp"
#include "SparseDeque.hpp"

#include <functional>

namespace DSA {

	template <typename ArgsType, size_t FRAMES>
	class StaticScheduler {
	public:

		struct TaskKey {

			uint32_t index;
			uint16_t frame;

		};

		struct Task {

			std::function<void(ArgsType)> callback;
			[[no_unique_address]] ArgsType args;

		};

	protected:

		uint16_t frame_counter = 0;

		SparseDeque<DynamicContainerConfiguration<>, Task> deques[FRAMES];

	public:

		constexpr void cancel(const TaskKey& taskKey) noexcept {
			deques[taskKey.frame].untrack(taskKey.index);
		}

		void execute_current_tasks() {
			auto& deque = deques[frame_counter];
			const auto size = deque.get_size();
			for (size_t index = 0; index < size; ++index) {
				Task task = deque.dequeue_front();
				task.callback(task.args);
			}
			++frame_counter;
		}

		[[nodiscard]] constexpr bool can_schedule(uint16_t frame_offset) const noexcept {
			return deques[frame_offset + frame_counter].can_enqueue_back();
		}

		template <std::invocable<Task> F>
		[[nodiscard]] constexpr TaskKey schedule(F function, uint16_t frame_offset) noexcept {
			const size_t frame = frame_offset + frame_counter;
			auto& deque = deques[frame];
			const size_t index = deque.enqueue_back(F);
			deque.track(index);
			return TaskKey { index, frame };
		}

	};

}