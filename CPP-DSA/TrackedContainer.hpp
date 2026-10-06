#pragma once

#include "Bitmask.hpp"
#include "IncrementalContainer.hpp"

namespace DSA {

	template <concepts::Container CONTAINER>
	class TrackedContainer : public IncrementalContainer<CONTAINER> {
	public:

		using Base = IncrementalContainer<CONTAINER>;
		using typename Base::INDEX;
		using typename Base::T;

	protected:

		using Base::data;

		using Bitmap = std::conditional_t<
			concepts::ContainerTraits<CONTAINER>::dynamic,
			Bitmask<uintmax_t>*,
			Bitmask<uintmax_t>[CONTAINER::capacity]
		>;

		Bitmap tracked;

#pragma region Methods

		void helper_destroy() {
			INDEX index = 0;
			INDEX count = Base::get_size();

			for (const Bitmask<uintmax_t>* mask = tracked; count != 0; mask++) {
				int bitIndex = 0;

				while (true) {
					bitIndex = mask->index_of_trailing_one(bitIndex);
					if (bitIndex == Bitmask<uintmax_t>::BitCount)
						break;

					destroy_at(Base::get_data() + index + bitIndex);

					bitIndex++;
					count--;
				}

				index += Bitmask<uintmax_t>::BitCount;
			}
		}

		template <bool MODE>
		void helper_resize(INDEX newCapacity) {
			using ALLOCATOR = CONTAINER::ALLOCATOR;

			T* oldData = Base::get_data();
			data = reinterpret_cast<T*>(ALLOCATOR::allocate(newCapacity * sizeof(T)));

			INDEX index = 0;
			INDEX count = Base::get_size();

			for (const Bitmask<uintmax_t>* mask = tracked; count != 0; mask++) {
				int bitIndex = 0;

				while (true) {
					bitIndex = mask->index_of_trailing_one(bitIndex);
					if (bitIndex == Bitmask<uintmax_t>::BitCount)
						break;

					INDEX jndex = index + bitIndex;

					if constexpr (std::is_nothrow_move_constructible_v<T>) {
						if constexpr (MODE == true)
							move_construct_at(Base::get_data() + jndex, oldData[jndex]);
						else if constexpr (MODE == false) {
							if (jndex < newCapacity)
								move_construct_at(Base::get_data() + jndex, oldData[jndex]);
						}
					}
					else {
						try {
							if constexpr (MODE == true)
								move_construct_at(Base::get_data() + jndex, oldData[jndex]);
							else if constexpr (MODE == false) {
								if (jndex < newCapacity)
									move_construct_at(Base::get_data() + jndex, oldData[jndex]);
							}
						}
						catch (...) {
							destroy_range_backward(Base::get_data(), Base::get_data() + jndex);
							ALLOCATOR::deallocate(data);
							data = oldData;
							throw;
						}
					}
					
					destroy_at(oldData + jndex);

					bitIndex++;
					count--;
				}

				index += Bitmask<uintmax_t>::BitCount;
			}

			ALLOCATOR::deallocate(oldData);

			Base::capacity = newCapacity;
		}

#pragma endregion

	public:

		~TrackedContainer() requires (!std::is_trivially_destructible_v<T>) {
			helper_destroy();
		}

		~TrackedContainer() = default;

#pragma region Methods

#pragma region IncrementalContainer

		constexpr void empty() noexcept {
			helper_destroy();
			Base::size = 0;
		}

#pragma endregion

#pragma region TrackedContainer

		constexpr void track(INDEX index) noexcept {
			tracked[index >> std::bit_width(Bitmask<uintmax_t>::BitCount)].set(index & (Bitmask<uintmax_t>::BitCount - 1));
		}

#pragma region Memory Management

		void double_capacity() requires concepts::DynamicContainer<CONTAINER> {
			if constexpr (concepts::ResizableContainer<CONTAINER>)
				Base::double_capacity();
			else
				grow(capacity << 1);
		}

		void grow(INDEX newCapacity) requires concepts::DynamicContainer<CONTAINER> {
			if constexpr (concepts::ResizableContainer<CONTAINER>)
				Base::grow(newCapacity);
			else
				helper_resize<true>(newCapacity);
		}

		void reserve(INDEX newCapacity) requires concepts::DynamicContainer<CONTAINER> {
			if constexpr (concepts::ResizableContainer<CONTAINER>)
				Base::reserve(newCapacity);
			else if (newCapacity > capacity)
				grow(newCapacity);
		}

		void resize(INDEX newCapacity) requires concepts::DynamicContainer<CONTAINER> {
			if constexpr (concepts::ResizableContainer<CONTAINER>)
				Base::resize(newCapacity);
			else {
				if (newCapacity > capacity)
					grow(newCapacity);
				else
					shrink(newCapacity);
			}
		}

		void shrink(INDEX newCapacity) requires concepts::DynamicContainer<CONTAINER> {
			if constexpr (concepts::ResizableContainer<CONTAINER>)
				Base::shrink(newCapacity);
			else
				helper_resize<false>(newCapacity);
		}

#pragma endregion

#pragma endregion

#pragma endregion

	};

}