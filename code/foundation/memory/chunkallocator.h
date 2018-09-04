#pragma once
//------------------------------------------------------------------------------
/**
	This allocator creates memory in user-specified (default 65535) byte chunks.
	Each time an object is requested, the current chunk is checked for storage,
	and if the object fits, an iterator is progressed with the size of the object,
	after which a pointer to the allocated space is returned. If the object does not
	fit, the allocator creates a new chunk, and retires the old, without
	considering the potential space lost in the process.

	This type of allocator allows for several objects of different types to be
	allocated linearly in memory, thus providing a cache-friendly access pattern
	for places where memory allocation is somewhat deterministic. 

	(C) 2017 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/config.h"
#include "util/array.h"
namespace Memory
{

template <int ChunkSize>
class ChunkAllocator
{
public:
	/// constructor
	ChunkAllocator();
	/// destructor
	~ChunkAllocator();

	/// copy constructor
	ChunkAllocator(const ChunkAllocator& rhs);
	/// assignment operator
	void operator=(const ChunkAllocator& rhs);

	/// move constructor
	ChunkAllocator(ChunkAllocator&& rhs);
	/// move operator
	void operator=(ChunkAllocator&& rhs);
	
	/// allocate new object, and calls constructor, but beware because this allocator does not run the destructors
	template <typename T> T* Alloc();
	/// allocate new chunk of size
	void* Alloc(SizeT size);
	/// retires a chunk and creates a new one (might waste memory)
	void NewChunk();
	/// release all memory
	void Release();

private:
	byte* currentChunk;
	byte* iterator;
	Util::Array<byte*> retiredChunks;
};

//------------------------------------------------------------------------------
/**
*/
template <int ChunkSize>
inline
ChunkAllocator<ChunkSize>::ChunkAllocator() :
	currentChunk(nullptr),
	iterator(nullptr)
{
	// constructor
}

//------------------------------------------------------------------------------
/**
*/
template <int ChunkSize>
inline
ChunkAllocator<ChunkSize>::~ChunkAllocator()
{
	this->Release();
}

//------------------------------------------------------------------------------
/**
*/
template<int ChunkSize>
inline
ChunkAllocator<ChunkSize>::ChunkAllocator(ChunkAllocator&& rhs)
{
	this->retiredChunks = rhs.retiredChunks;
	this->currentChunk = rhs.currentChunk;
	this->iterator = rhs.iterator;

	rhs.retiredChunks.Clear();
	rhs.currentChunk = nullptr;
	rhs.iterator = nullptr;
}

//------------------------------------------------------------------------------
/**
*/
template<int ChunkSize>
inline
ChunkAllocator<ChunkSize>::ChunkAllocator(const ChunkAllocator& rhs)
{
	IndexT i;
	for (i = 0; i < rhs.retiredChunks.Size(); i++)
	{
		byte* chunk = (byte*)Memory::Alloc(ObjectArrayHeap, ChunkSize);
		memcpy(chunk, rhs.retiredChunks[i], ChunkSize);
		this->retiredChunks.Append(chunk);
	}
	if (rhs.currentChunk)
	{
		this->currentChunk = (byte*)Memory::Alloc(ObjectArrayHeap, ChunkSize);
		ptrdiff_t size = rhs.iterator - rhs.currentChunk;
		memcpy(this->currentChunk, rhs.currentChunk, size);
		this->iterator = this->currentChunk + size;
	}
	else
	{
		this->currentChunk = nullptr;
		this->iterator = nullptr;
	}
}

//------------------------------------------------------------------------------
/**
*/
template<int ChunkSize>
inline void
ChunkAllocator<ChunkSize>::operator=(ChunkAllocator&& rhs)
{
	this->retiredChunks = rhs.retiredChunks;
	this->currentChunk = rhs.currentChunk;
	this->iterator = rhs.iterator;

	rhs.retiredChunks.Clear();
	rhs.currentChunk = nullptr;
	rhs.iterator = nullptr;
}

//------------------------------------------------------------------------------
/**
*/
template<int ChunkSize>
inline void
ChunkAllocator<ChunkSize>::operator=(const ChunkAllocator& rhs)
{
	IndexT i;
	for (i = 0; i < rhs.retiredChunks.Size(); i++)
	{
		byte* chunk = (byte*)Memory::Alloc(ObjectArrayHeap, ChunkSize);
		memcpy(chunk, rhs.retiredChunks[i], ChunkSize);
		this->retiredChunks.Append(chunk);
	}
	if (rhs.currentChunk)
	{
		this->currentChunk = (byte*)Memory::Alloc(ObjectArrayHeap, ChunkSize);
		ptrdiff_t size = rhs.iterator - rhs.currentChunk;
		memcpy(this->currentChunk, rhs.currentChunk, size);
		this->iterator = this->currentChunk + size;
	}
	else
	{
		this->currentChunk = nullptr;
		this->iterator = nullptr;
	}
}

//------------------------------------------------------------------------------
/**
*/
template <int ChunkSize>
inline void
ChunkAllocator<ChunkSize>::NewChunk()
{
	if (this->currentChunk != nullptr)
		this->retiredChunks.Append(this->currentChunk);
	this->currentChunk = (byte*)Memory::Alloc(ObjectArrayHeap, ChunkSize);
	this->iterator = this->currentChunk;
}

//------------------------------------------------------------------------------
/**
*/
template<int ChunkSize>
inline void ChunkAllocator<ChunkSize>::Release()
{
	IndexT i;
	for (i = 0; i < this->retiredChunks.Size(); i++)
	{
		Memory::Free(ObjectArrayHeap, this->retiredChunks[i]);
	}
	if (this->currentChunk) Memory::Free(ObjectArrayHeap, this->currentChunk);
	this->retiredChunks.Clear();
	this->currentChunk = nullptr;
	this->iterator = nullptr;
}

//------------------------------------------------------------------------------
/**
*/
template <int ChunkSize>
template<typename T>
inline T*
ChunkAllocator<ChunkSize>::Alloc()
{
	static_assert(sizeof(T) <= ChunkSize, "Size of type is bigger than the chunk size!");
	if (this->iterator == nullptr)
	{
		this->NewChunk();
	}
	else
	{
		// we cast the pointer diff but it should be safe since it should never be above ChunkSize
		SizeT remainder = ChunkSize - SizeT(this->iterator - this->currentChunk);
		if (remainder < sizeof(T))
			this->NewChunk();
	}

	T* ret = new (this->iterator) T();
	this->iterator += sizeof(T);
	return ret;
}

//------------------------------------------------------------------------------
/**
*/
template <int ChunkSize>
inline void*
ChunkAllocator<ChunkSize>::Alloc(SizeT size)
{
	n_assert(size <= ChunkSize);
	n_assert(size != 0);
	if (this->iterator == nullptr)
	{
		this->NewChunk();
	}
	else
	{
		PtrDiff remainder = this->currentChunk + ChunkSize - this->iterator;
		if (remainder < size)
			this->NewChunk();
	}
	void* ret = this->iterator;
	this->iterator += size;
	return ret;
}

} // namespace Memory