/**
 * @file RingBuffer.h
 * @brief Lock-free single-producer/single-consumer ring buffer.
 *
 * Provides a fixed-capacity SPSC ring buffer with no wasted slots. The API and
 * identifiers follow Unreal Engine C++ naming conventions while retaining the
 * original implementation's standard-library atomics and memory-ordering
 * semantics.
 *
 * @note BufferSize must be a non-zero power of two.
 * @note T must be a trivial type.
 * @note This container is intended for exactly one producer and one consumer.
 *
 * @author Jan Oleksiewicz <jnk0le@hotmail.com>
 * @version 2.0.5
 * @date 22 Jun 2017
 * @copyright SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstdint>
#include <iostream>
#include <cstddef>
#include <limits>
#include <atomic>
#include <type_traits>

namespace Ark
{
	/**
     * @brief Lock-free single-producer/single-consumer ring buffer.
     * The ring buffer uses monotonically increasing producer and consumer
     *
     * indices and masks them into the fixed-size backing array. BufferSize must
     * be a power of two.
     *
     * @tparam T Element type stored by the buffer. Must be trivial.
     * @tparam BufferSize Number of elements in the buffer. Must be a power of two.
     * @tparam bFakeTSO When true, explicit acquire/release ordering on indices is
     *         relaxed for environments where the required ordering is guaranteed.
     * @tparam CacheLineSize Cache-line alignment used to separate indices and storage.
     * @tparam IndexType Unsigned integral type used for producer/consumer indices.
     */
    template<typename T, size_t BufferSize = 16, bool bFakeTSO = false, size_t CacheLineSize = 0, typename IndexType = size_t>
	class TRingBuffer
	{
	public:
		/**
		 * @brief Default constructor, will initialize Head and Tail indexes
		 */
		TRingBuffer() : Head(0), Tail(0) {}

		/**
		 * @brief Special case constructor to premature out unnecessary initialization code when object is
		 * instantiated in .bss section
		 * @warning If object is instantiated on stack, heap or inside noinit section then the contents have to be
		 * explicitly cleared before use
		 * @param Dummy Ignored
		 */
		TRingBuffer(int Dummy) { (void)(Dummy); }

		/**
		 * @brief Clear buffer from producer side
		 * @warning function may return without performing any action if consumer tries to read Data At the same time
		 */
		void ProducerClear(void) {
			// Head modification will lead to underflow if cleared during consumer read
			// doing this properly with CAS is not possible without modifying the consumer code
			ConsumerClear();
		}

		/**
		 * @brief Clear buffer from consumer side
		 */
		void ConsumerClear(void) {
			Tail.store(Head.load(std::memory_order_relaxed), std::memory_order_relaxed);
		}

		/**
		 * @brief Check if buffer is empty
		 * @return True if buffer is empty
		 */
		bool IsEmpty(void) const {
			return ReadAvailable() == 0;
		}

		/**
		 * @brief Check if buffer is full
		 * @return True if buffer is full
		 */
		bool IsFull(void) const {
			return WriteAvailable() == 0;
		}

		/**
		 * @brief Check how many elements can be read from the buffer
		 * @return Number of elements that can be read
		 */
		IndexType ReadAvailable(void) const {
			return Head.load(IndexAcquireBarrier) - Tail.load(std::memory_order_relaxed);
		}

		/**
		 * @brief Check how many elements can be Written into the buffer
		 * @return Number of free slots that can be be Written
		 */
		IndexType WriteAvailable(void) const {
			return BufferSize - (Head.load(std::memory_order_relaxed) - Tail.load(IndexAcquireBarrier));
		}

		/**
		 * @brief Inserts Data into internal buffer, without blocking
		 * @param Data element to be inserted into internal buffer
		 * @return True if Data was inserted
		 */
		bool Insert(T Data)
		{
			IndexType TemporaryHead = Head.load(std::memory_order_relaxed);

			if((TemporaryHead - Tail.load(IndexAcquireBarrier)) == BufferSize)
				return false;
			else
			{
				DataBuffer[TemporaryHead++ & BufferMask] = Data;
				std::atomic_signal_fence(std::memory_order_release);
				Head.store(TemporaryHead, IndexReleaseBarrier);
			}
			return true;
		}

		/**
		 * @brief Inserts Data into internal buffer, without blocking
		 * @param[in] Data Pointer to memory location where element, to be inserted into internal buffer, is located
		 * @return True if Data was inserted
		 */
		bool Insert(const T* Data)
		{
			IndexType TemporaryHead = Head.load(std::memory_order_relaxed);

			if((TemporaryHead - Tail.load(IndexAcquireBarrier)) == BufferSize)
				return false;
			else
			{
				DataBuffer[TemporaryHead++ & BufferMask] = *Data;
				std::atomic_signal_fence(std::memory_order_release);
				Head.store(TemporaryHead, IndexReleaseBarrier);
			}
			return true;
		}

		/**
		 * @brief Inserts Data returned by callback function, into internal buffer, without blocking
		 *
		 * This is a special purpose function that can be used to avoid redundant availability checks in case when
		 * acquiring Data have a side effects (like clearing status flags by reading a peripheral Data register)
		 *
		 * @param GetDataCallback Pointer to callback function that returns element to be inserted into buffer
		 * @return True if Data was inserted and callback called
		 */
		bool InsertFromCallbackWhenAvailable(T (*GetDataCallback)(void))
		{
			IndexType TemporaryHead = Head.load(std::memory_order_relaxed);

			if((TemporaryHead - Tail.load(IndexAcquireBarrier)) == BufferSize)
				return false;
			else
			{
				//execute callback only when there is space in buffer
				DataBuffer[TemporaryHead++ & BufferMask] = GetDataCallback();
				std::atomic_signal_fence(std::memory_order_release);
				Head.store(TemporaryHead, IndexReleaseBarrier);
			}
			return true;
		}

		/**
		 * @brief Removes single element without reading
		 * @return True if one element was removed
		 */
		bool Remove()
		{
			IndexType TemporaryTail = Tail.load(std::memory_order_relaxed);

			if (TemporaryTail == Head.load(std::memory_order_relaxed))
				{
					return false;
				}
			else
				Tail.store(++TemporaryTail, IndexReleaseBarrier); // release in case Data was loaded/used before

			return true;
		}

		/**
		 * @brief Removes multiple elements without reading and storing it elsewhere
		 * @param Count Maximum number of elements to Remove
		 * @return Number of removed elements
		 */
		size_t Remove(size_t Count) {
			IndexType TemporaryTail = Tail.load(std::memory_order_relaxed);
			IndexType avail = Head.load(std::memory_order_relaxed) - TemporaryTail;

			Count = (Count > avail) ? avail : Count;

			Tail.store(TemporaryTail + Count, IndexReleaseBarrier);
			return Count;
		}

		/**
		 * @brief Reads one element from internal buffer without blocking
		 * @param[out] Data Reference to memory location where removed element will be stored
		 * @return True if Data was fetched from the internal buffer
		 */
		bool Remove(T& Data) {
			return Remove(&Data); // references are anyway implemented as pointers
		}

		/**
		 * @brief Reads one element from internal buffer without blocking
		 * @param[out] Data Pointer to memory location where removed element will be stored
		 * @return True if Data was fetched from the internal buffer
		 */
		bool Remove(T* Data) {
			IndexType TemporaryTail = Tail.load(std::memory_order_relaxed);

			if(TemporaryTail == Head.load(IndexAcquireBarrier))
				return false;
			else
			{
				*Data = DataBuffer[TemporaryTail++ & BufferMask];
				std::atomic_signal_fence(std::memory_order_release);
				Tail.store(TemporaryTail, IndexReleaseBarrier);
			}
			return true;
		}

		/**
		 * @brief Gets the first element in the buffer on consumed side
		 *
		 * It is safe to use and modify item contents only on consumer side
		 *
		 * @return Pointer to first element, nullptr if buffer was empty
		 */
		T* Peek() {
			IndexType TemporaryTail = Tail.load(std::memory_order_relaxed);

			if(TemporaryTail == Head.load(IndexAcquireBarrier))
				return nullptr;
			else
				return &DataBuffer[TemporaryTail & BufferMask];
		}

		/**
		 * @brief Gets the n'th element on consumed side
		 *
		 * It is safe to use and modify item contents only on consumer side
		 *
		 * @param Index Item offset starting on the consumed side
		 * @return Pointer to requested element, nullptr if Index exceeds storage Count
		 */
		T* At(size_t Index) {
			IndexType TemporaryTail = Tail.load(std::memory_order_relaxed);

			if((Head.load(IndexAcquireBarrier) - TemporaryTail) <= Index)
				return nullptr;
			else
				return &DataBuffer[(TemporaryTail + Index) & BufferMask];
		}

		/**
		 * @brief Gets the n'th element on consumed side
		 *
		 * Unchecked operation, assumes that software already knows if the element can be used, if
		 * requested Index is out of bounds then reference will point to somewhere inside the buffer
		 * The IsEmpty() and ReadAvailable() will place appropriate memory barriers if used as loop limiter
		 * It is safe to use and modify T contents only on consumer side
		 *
		 * @param Index Item offset starting on the consumed side
		 * @return Reference to requested element, undefined if Index exceeds storage Count
		 */
		T& operator[](size_t Index) {
			return DataBuffer[(Tail.load(std::memory_order_relaxed) + Index) & BufferMask];
		}

		/**
		 * @brief Insert multiple elements into internal buffer without blocking
		 *
		 * This function will Insert as much Data as possible from given buffer.
		 *
		 * @param[in] Buffer Pointer to buffer with Data to be inserted from
		 * @param Count Number of elements to write from the given buffer
		 * @return Number of elements Written into internal buffer
		 */
		size_t WriteBuffer(const T* Buffer, size_t Count);

		/**
		 * @brief Insert multiple elements into internal buffer without blocking
		 *
		 * This function will continue writing new entries until all Data is Written or there is no more space.
		 * The callback function can be used to indicate to consumer that it can start fetching Data.
		 *
		 * @warning This function is not deterministic
		 *
		 * @param[in] Buffer Pointer to buffer with Data to be inserted from
		 * @param Count Number of elements to write from the given buffer
		 * @param CountToCallback Number of elements to write before calling a callback function in first loop
		 * @param ExecuteDataCallback Pointer to callback function executed after every loop iteration
		 * @return Number of elements Written into internal  buffer
		 */
		size_t WriteBuffer(const T* Buffer, size_t Count, size_t CountToCallback, void (*ExecuteDataCallback)(void));

		/**
		 * @brief Load multiple elements from internal buffer without blocking
		 *
		 * This function will read up to specified amount of Data.
		 *
		 * @param[out] Buffer Pointer to buffer where Data will be loaded into
		 * @param Count Number of elements to load into the given buffer
		 * @return Number of elements that were read from internal buffer
		 */
		size_t ReadBuffer(T* Buffer, size_t Count);

		/**
		 * @brief Load multiple elements from internal buffer without blocking
		 *
		 * This function will continue reading new entries until all requested Data is read or there is nothing
		 * more to read.
		 * The callback function can be used to indicate to producer that it can start writing new Data.
		 *
		 * @warning This function is not deterministic
		 *
		 * @param[out] Buffer Pointer to buffer where Data will be loaded into
		 * @param Count Number of elements to load into the given buffer
		 * @param CountToCallback Number of elements to load before calling a callback function in first iteration
		 * @param ExecuteDataCallback Pointer to callback function executed after every loop iteration
		 * @return Number of elements that were read from internal buffer
		 */
		size_t ReadBuffer(T* Buffer, size_t Count, size_t CountToCallback, void (*ExecuteDataCallback)(void));

	private:
		constexpr static IndexType BufferMask = BufferSize - 1; //!< bitwise mask for a given buffer size
		constexpr static std::memory_order IndexAcquireBarrier = bFakeTSO ?
				  std::memory_order_relaxed
				: std::memory_order_acquire; // do not load from, or store to buffer before confirmed by the opposite side
		constexpr static std::memory_order IndexReleaseBarrier = bFakeTSO ?
				  std::memory_order_relaxed
				: std::memory_order_release; // do not update own side before all operations on DataBuffer committed

		alignas(CacheLineSize) std::atomic<IndexType> Head; //!< Head Index
		alignas(CacheLineSize) std::atomic<IndexType> Tail; //!< Tail Index

		// put buffer after variables so everything can be reached with short offsets
		alignas(CacheLineSize) T DataBuffer[BufferSize]{}; //!< actual buffer

		// let's assert that no UB will be compiled in
		static_assert((BufferSize != 0), "BufferSize must be greater than zero");
		static_assert((BufferSize & BufferMask) == 0, "BufferSize must be a power of two");
		static_assert(sizeof(IndexType) <= sizeof(size_t),
			"indexing type size is larger than size_t, operation is not lock free and doesn't make sense");

		static_assert(std::numeric_limits<IndexType>::is_integer, "IndexType must be an integral type");
		static_assert(!(std::numeric_limits<IndexType>::is_signed), "IndexType must be unsigned");
		static_assert(BufferMask <= ((std::numeric_limits<IndexType>::max)() >> 1),
			"buffer size is too large for a given indexing type (maximum size for n-bit type is 2^(n-1))");

		static_assert(std::is_trivial<T>::value, "T must be a trivial type");
	};

	template<typename T, size_t BufferSize, bool bFakeTSO, size_t CacheLineSize, typename IndexType>
	size_t TRingBuffer<T, BufferSize, bFakeTSO, CacheLineSize, IndexType>::WriteBuffer(const T* Buffer, size_t Count)
	{
		IndexType Available = 0;
		IndexType TemporaryHead = Head.load(std::memory_order_relaxed);
		size_t ToWrite = Count;

		Available = BufferSize - (TemporaryHead - Tail.load(IndexAcquireBarrier));

		if(Available < Count) // do not write more than we can
			ToWrite = Available;

		// maybe divide it into 2 separate writes
		for (size_t i = 0; i < ToWrite; i++)
			DataBuffer[TemporaryHead++ & BufferMask] = Buffer[i];

		std::atomic_signal_fence(std::memory_order_release);
		Head.store(TemporaryHead, IndexReleaseBarrier);

		return ToWrite;
	}

	template<typename T, size_t BufferSize, bool bFakeTSO, size_t CacheLineSize, typename IndexType>
	size_t TRingBuffer<T, BufferSize, bFakeTSO, CacheLineSize, IndexType>::WriteBuffer(const T* Buffer, size_t Count,
			size_t CountToCallback, void(*ExecuteDataCallback)())
	{
		size_t Written = 0;
		IndexType Available = 0;
		IndexType TemporaryHead = Head.load(std::memory_order_relaxed);
		size_t ToWrite = Count;

		if(CountToCallback != 0 && CountToCallback < Count)
			ToWrite = CountToCallback;

		while(Written < Count)
		{
			Available = BufferSize - (TemporaryHead - Tail.load(IndexAcquireBarrier));

			if (Available == 0) // less than ??
				break;

			if (ToWrite > Available) // do not write more than we can
				ToWrite = Available;

			while (ToWrite--)
				DataBuffer[TemporaryHead++ & BufferMask] = Buffer[Written++];

			std::atomic_signal_fence(std::memory_order_release);
			Head.store(TemporaryHead, IndexReleaseBarrier);

			if(ExecuteDataCallback != nullptr)
				ExecuteDataCallback();

			ToWrite = Count - Written;
		}

		return Written;
	}

	template<typename T, size_t BufferSize, bool bFakeTSO, size_t CacheLineSize, typename IndexType>
	size_t TRingBuffer<T, BufferSize, bFakeTSO, CacheLineSize, IndexType>::ReadBuffer(T* Buffer, size_t Count)
	{
		IndexType Available = 0;
		IndexType TemporaryTail = Tail.load(std::memory_order_relaxed);
		size_t ToRead = Count;

		Available = Head.load(IndexAcquireBarrier) - TemporaryTail;

		if(Available < Count) // do not read more than we can
			ToRead = Available;

		// maybe divide it into 2 separate reads
		for (size_t i = 0; i < ToRead; i++)
			Buffer[i] = DataBuffer[TemporaryTail++ & BufferMask];

		std::atomic_signal_fence(std::memory_order_release);
		Tail.store(TemporaryTail, IndexReleaseBarrier);

		return ToRead;
	}

	template<typename T, size_t BufferSize, bool bFakeTSO, size_t CacheLineSize, typename IndexType>
	size_t TRingBuffer<T, BufferSize, bFakeTSO, CacheLineSize, IndexType>::ReadBuffer(T* Buffer, size_t Count,
			size_t CountToCallback, void(*ExecuteDataCallback)())
	{
		size_t read = 0;
		IndexType Available = 0;
		IndexType TemporaryTail = Tail.load(std::memory_order_relaxed);
		size_t ToRead = Count;

		if(CountToCallback != 0 && CountToCallback < Count)
			ToRead = CountToCallback;

		while(read < Count)
		{
			Available = Head.load(IndexAcquireBarrier) - TemporaryTail;

			if (Available == 0) // less than ??
				break;

			if (ToRead > Available) // do not write more than we can
				ToRead = Available;

			while (ToRead--)
				Buffer[read++] = DataBuffer[TemporaryTail++ & BufferMask];

			std::atomic_signal_fence(std::memory_order_release);
			Tail.store(TemporaryTail, IndexReleaseBarrier);

			if(ExecuteDataCallback != nullptr)
				ExecuteDataCallback();

			ToRead = Count - read;
		}

		return read;
	}

} // namespace Jnk0le

