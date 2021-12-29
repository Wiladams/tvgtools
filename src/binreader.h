#pragma once

#include <cstdint>

namespace tvg {
	// These couple of routines help facilitate bit banging, getting
	// bit values out of integer values.  Using these means we don't
	// have to create sketchy bitfield structures for everything

    // Get the bit value at the specified position
	static inline uint64_t BITMASK64(const size_t low, const size_t high)
	{
		return ((((uint64_t)1 << (high - low)) << 1) - 1) << low;
	}

    static inline uint32_t BITMASK32(const size_t low, const size_t high)
    {
        return ((((uint32_t)1 << (high - low)) << 1) - 1) << low;
    }

    static inline uint8_t BITMASK8(const size_t low, const size_t high)
    {
        return ((((uint8_t)1 << (high - low)) << 1) - 1) << low;
    }

    // Get a value based on a starting and ending position of bits in a value
    static inline uint64_t GETBITS64(const uint64_t value, const size_t low, const size_t high)
    {
        return (value & BITMASK64(low, high)) >> low;
    }
    
    static inline uint32_t GETBITS32(const uint32_t value, const size_t low, const size_t high)
    {
        return (value & BITMASK32(low, high)) >> low;
    }

    static inline uint32_t GETBITS8(const uint8_t value, const size_t low, const size_t high)
    {
        return (value & BITMASK8(low, high)) >> low;
    }

    static inline uint64_t SETBIT64(const uint64_t value, const size_t low, const size_t high, const uint64_t bit)
    {
        return (value & ~BITMASK64(low, high)) | ((bit << low) & BITMASK64(low, high));
    }

    static inline uint32_t SETBIT32(const uint32_t value, const size_t low, const size_t high, const uint32_t bit)
    {
        return (value & ~BITMASK32(low, high)) | ((bit << low) & BITMASK32(low, high));
    }

// BinReader
// A class for reading binary values from a stream
// This particular class assumes little-endian encoding of values.  That means it's not
// suitable for general purpose use, but it's good enough for reading tvg files
//
struct BinReader {
    uint8_t *data;
    size_t size;
    size_t pos;

    // The user is responsible for getting that memory buffer
    // and size.  This class is just a reader, it will not allocate
    // any memory.  It's just a convenience for reading out integer values
    BinReader(uint8_t *data, size_t size) : data(data), size(size), pos(0) {}

    bool setData(uint8_t *data, size_t size)
    {
        this->data = data;
        this->size = size;
        pos = 0;
        return true;
    }
    
    // Tells us whether we've reached the end of the stream
    bool isEOF() {  
        return pos >= size;
    }

    // How many bytes remain to be read from the stream
    size_t remaining() {
        return size - pos;
    }

    size_t readBytes(uint8_t *buffer, size_t size) {
        size_t bytesRead = 0;
        while (bytesRead < size) {
            if (pos >= this->size) {
                break;
            }
            buffer[bytesRead++] = data[pos++];
        }
        return bytesRead;
    }

    uint8_t readOctet() {
        if (pos >= size) {
            return 0;
        }
        return data[pos++];
    }

    uint16_t readUint16() {
        uint16_t value = 0;
        value |= readOctet();
        value |= readOctet() << 8;
        return value;
    }

    uint32_t readUint32() {
        uint32_t value = 0;
        value |= readOctet();
        value |= readOctet() << 8;
        value |= readOctet() << 16;
        value |= readOctet() << 24;
        return value;
    }

    float readFloat() {
        uint32_t value = readUint32();
        return *reinterpret_cast<float*>(&value);
    }
};

}