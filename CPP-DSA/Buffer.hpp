#pragma once

#include <memory>

#if defined(_WIN32) || defined(_WIN64)
#define PLATFORM_WINDOWS 1
#include <windows.h>
#elif defined(__linux__) || defined(__APPLE__)
#define PLATFORM_POSIX 1
#include <sys/mman.h>
#include <unistd.h>
#else
#error "Unsupported Operating System"
#endif

template <typename T = std::byte>
struct Buffer {
protected:

    void* data = nullptr;

private:

    size_t physical_size = 0; // Current size backed by physical RAM
    const size_t maximum_capacity; // Upper bound of virtual reservation

#pragma region Methods

    static size_t get_system_page_size() {
#if PLATFORM_WINDOWS
        SYSTEM_INFO si;
        GetSystemInfo(&si);
        return static_cast<size_t>(si.dwPageSize);
#elif PLATFORM_POSIX
        return static_cast<size_t>(sysconf(_SC_PAGESIZE));
#endif
    }

    static size_t system_page_size() {
        static const size_t page_size = get_system_page_size();
        return page_size;
    }

    std::byte* map() {
        std::byte* address;
#if PLATFORM_WINDOWS
        // Reserve virtual address space; no physical memory or page file is used
        address = VirtualAlloc(NULL, maximum_capacity, MEM_RESERVE, PAGE_NOACCESS);
        return address;
#elif PLATFORM_POSIX
        // Reserve via PROT_NONE mapping
        address = mmap(NULL, maximum_capacity, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if (address == MAP_FAILED)
            address = NULL;
        return address;
#endif
    }

    void unmap() {
#if PLATFORM_WINDOWS
        // MEM_RELEASE requires size parameter to be 0 to release the entire region
        VirtualFree(data, 0, MEM_RELEASE);
#elif PLATFORM_POSIX
        munmap(data, maximum_capacity);
#endif
    }

    static size_t align_to_page(size_t size, size_t page_size) {
        if (page_size == 0) return size;
        const size_t remainder = size % page_size;
        if (remainder == 0) return size;
        if (size > SIZE_MAX - (page_size - remainder)) return 0;
        return size + (page_size - remainder);
    }

    /*static size_t align_to_page(size_t size, size_t page_size) {
        if (page_size == 0) return size;
        return (size + page_size - 1) & ~(page_size - 1);
    }*/

#pragma endregion

public:

    Buffer() = delete;

    Buffer(size_t maximumCapacity) : maximum_capacity(align_to_page(maximumCapacity * sizeof(T), system_page_size())), data(map()) {}

    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;

    Buffer(Buffer&& other) noexcept :
        data(other.data),
        physical_size(other.physical_size),
        maximum_capacity(other.maximum_capacity)
    {
        other.data = nullptr;
    }

    ~Buffer() {
        unmap();
    }

#pragma region Methods

    constexpr bool is_mapped() const noexcept {
        return data != nullptr;
    }

#pragma region Getters

    constexpr T* get_data() noexcept {
        return reinterpret_cast<T*>(data);
    }

    constexpr const T* get_data() const noexcept {
        return reinterpret_cast<const T*>(data);
    }

    constexpr operator T* () noexcept {
        return get_data();
    }

    constexpr operator const T* () const noexcept {
        return get_data();
    }

    constexpr std::byte* get_raw_data() noexcept {
        return reinterpret_cast<std::byte*>(data);
    }

    constexpr const std::byte* get_raw_data() const noexcept {
        return reinterpret_cast<const std::byte*>(data);
    }

    constexpr operator std::byte* () noexcept {
        return get_raw_data();
    }

    constexpr operator const std::byte* () const noexcept {
        return get_raw_data();
    }

    constexpr T& operator[](size_t index) noexcept {
        return get_data()[index];
    }

    constexpr const T& operator[](size_t index) const noexcept {
        return get_data()[index];
    }

#pragma endregion

#pragma region Memory Management

    bool grow(size_t newSize) {
        size_t alignedNewSize = align_to_page(newSize * sizeof(T), system_page_size());

        // Cannot grow beyond what we initially claimed virtual space for
        if (alignedNewSize > maximum_capacity) return false;
        if (alignedNewSize <= physical_size) return true; // Already large enough

        // Calculate how many more bytes need to be committed
        size_t delta = alignedNewSize - physical_size;
        void* targetAddress = get_raw_data() + physical_size;

#if PLATFORM_WINDOWS
        // Commit physical pages starting exactly where the previous allocation left off.
        void* result = VirtualAlloc(targetAddress, delta, MEM_COMMIT, PAGE_READWRITE);
        if (!result) return false;
#elif PLATFORM_POSIX
        // Elevate the permissions from PROT_NONE to READ | WRITE to eventually commit physical RAM.
        if (mprotect(targetAddress, delta, PROT_READ | PROT_WRITE) != 0)
            return false;
#endif

        physical_size = alignedNewSize;

        return true;
    }

    bool resize(size_t newSize) {
        return (newSize > physical_size) ? grow(newSize) : shrink(newSize);
    }

    bool shrink(size_t newSize) {
        // Round new_size up to the nearest page boundary. 
        // This protects active data if the user requests an unaligned shrink size.
        size_t alignedNewSize = align_to_page(newSize * sizeof(T), system_page_size());

        // Cannot shrink to a size larger than what is currently committed
        if (alignedNewSize >= physical_size) return false;

        // Calculate how many bytes we are giving back to the OS.
        size_t delta = physical_size - alignedNewSize;
        void* targetAddress = get_raw_data() + alignedNewSize;

#if PLATFORM_WINDOWS
        // MEM_DECOMMIT frees the RAM back to the OS but keeps the underlying address space reserved.
        if (!VirtualFree(targetAddress, delta, MEM_DECOMMIT))
            return false;

#elif PLATFORM_POSIX
        // Strip read & write permissions from the discarded range.
        if (mprotect(targetAddress, delta, PROT_NONE) != 0)
            return false;

#if defined(__linux__)
        // Tell Linux that the anonymous pages are no longer needed.
        // They may be discarded and recreated as zero-filled pages.
        if (madvise(targetAddress, shrinkDelta, MADV_DONTNEED) != 0)
            return false;
#elif defined(__APPLE__)
        // Tell macOS that the pages can be reclaimed.
        // Unlike MADV_DONTNEED, this is a weaker reclamation guarantee.
        if (madvise(targetAddress, shrinkDelta, MADV_FREE) != 0)
            return false;
#endif

#endif

        physical_size = alignedNewSize;

        return true;
    }

#pragma endregion

#pragma endregion

};