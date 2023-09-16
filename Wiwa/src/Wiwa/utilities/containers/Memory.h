#pragma once
#include <cstring>

typedef char sbyte;

namespace Wiwa {
	class Memory {
	private:
		sbyte* m_MemoryBlock;
		size_t m_MemorySize;
		size_t m_PointIndex;
	public:
		Memory() : m_MemoryBlock(nullptr), m_MemorySize(0), m_PointIndex(0) {}

		Memory(void* block, size_t size) : m_MemoryBlock((sbyte*)block), m_MemorySize(size), m_PointIndex(0){}

		~Memory() {
			//delete[] m_MemoryBlock;
		}

		sbyte* getBlock() { return m_MemoryBlock; }

		void Alloc(size_t size){
			delete[] m_MemoryBlock;

			m_MemorySize = size;
			m_PointIndex = 0;

			m_MemoryBlock = new sbyte[size];
		}

		void Write(void* data, size_t size) {
			memcpy(m_MemoryBlock + size, data, size);
		}

		void Read(void* out, size_t size) {
			memcpy(out, m_MemoryBlock + m_PointIndex, size);

			m_PointIndex += size;
		}

		void Seek(size_t index) {
			m_PointIndex = index;
		}

		size_t Size() { return m_MemorySize; }
	};
}