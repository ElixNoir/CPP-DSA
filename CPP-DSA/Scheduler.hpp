#pragma once

#include "BitmappedPriorityDeque.hpp"
#include "SparseDeque.hpp"

namespace DSA {

	template <typename ArgsType = EMPTY_STRUCT, size_t FRAMES = 60>
	class StaticScheduler {
	public:

		using Callback = void (*)(ArgsType);

		struct TaskKey {

			uint32_t index;
			uint16_t frame;

		};

		struct Task {

			Callback callback;
			[[no_unique_address]] ArgsType args;

		};

	protected:

		uint16_t frame_counter = 0;

		SparseDeque<StaticContainerConfiguration<4>, Task> deques[FRAMES];

	public:

#pragma region Methods

#pragma region Getters

		[[nodiscard]] constexpr Task& get(TaskKey taskKey) noexcept {
			return deques[taskKey.frame].get(taskKey.index);
		}

		[[nodiscard]] constexpr const Task& get(TaskKey taskKey) const noexcept {
			return deques[taskKey.frame].get(taskKey.index);
		}

#pragma endregion

#pragma region Cancel, Execute, & Schedule

		constexpr void cancel(TaskKey taskKey) noexcept {
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

		[[nodiscard]] constexpr bool can_schedule(uint16_t frame_offset, uint32_t count = 1) const noexcept {
			return deques[frame_offset + frame_counter].can_enqueue_back(count);
		}

		constexpr TaskKey schedule(Callback function, uint16_t frame_offset, ArgsType args = ArgsType{}) noexcept {
			const uint16_t frame = frame_offset + frame_counter;
			auto& deque = deques[frame];
			const uint32_t index = deque.enqueue_back(Task{ function, args });
			deque.track(index);
			return TaskKey{ index, frame };
		}

		constexpr void schedule_at(Callback function, uint16_t frame, uint32_t index, ArgsType args = ArgsType{}) noexcept {
			auto& deque = deques[frame];
			deque.get(index) = Task{ function, args };
			deque.track(index);
		}

#pragma endregion

#pragma endregion

	};

}