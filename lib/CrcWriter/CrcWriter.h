//
// Created by user on 14/10/2024.
//

#ifndef CRCWRITER_H
#define CRCWRITER_H



#include <FastCRC.h>  // https://github.com/FrankBoesing/FastCRC
#include <Arduino.h>

class CrcWriter {
public:
    CrcWriter() {
        _hash = _hasher.crc32(nullptr, 0);
    }

    size_t write(uint8_t c) {
        _hash = _hasher.crc32_upd(&c, 1);
        return 1;
    }

    size_t write(const uint8_t *buffer, size_t length) {
        _hash = _hasher.crc32_upd(buffer, length);
        return length;
    }

    uint32_t hash() const {
        return _hash;
    }

private:
    FastCRC32 _hasher;
    uint32_t _hash;
};




#endif //CRCWRITER_H
