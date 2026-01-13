#ifndef _MEMORY_MANAGEMENT_
#define _MEMORY_MANAGEMENT_

#include <iostream>
#include <fstream>
#include <cstdint>
#include <cstddef>
#include <cctype>
#include <vector>
#include <algorithm>
#include "hex.h"

/**
 * @class memory
 * @brief A class that simulates computer memory with various read/write operations
 * @details Inherits from hex class for hexadecimal formatting capabilities
 */
class memory : public hex
{
    public:
        /** 
         * @brief Constructor that allocates and initializes simulated memory
         * @param siz Size of memory to allocate in bytes
         * @details Rounds up size to nearest multiple of 16 and intializes
         * all bytes to 0xa5
         */
        memory(uint32_t siz)
        {
            siz = (siz+15) & 0xfffffff0;    // round up to multiple of 16
            mem.resize(siz, 0xa5);          // allocate memory, fill with 0xa5
        }
        
        /**
         * @brief Destructor
         */
        ~memory(){};
       
        /**
         * @brief Checks if a memory address is valid
         * @param addr Address to check
         * @return true if address is invalid, false if valid
         * @details Prints warning message to stdout if address is out of range
         */
        bool check_illegal(uint32_t addr) const
        {
            // check if address is beyond memory bounds
            if (addr >= get_size())
            {
                std::cout << "WARNING: Address out of range: " << hex::to_hex0x32(addr) << std::endl;
                return true; // address is illegal
            }
            return false; // address is legal
        }

        // getters
        /**
         * @brief Gets the size of simulated memory
         * @return Number of bytes in simulated memory
         */
        uint32_t get_size() const
        {
            return mem.size(); // return total size of memory vector
        }

        /**
         * @brief Reads a single byte of memory
         * @param addr Address to read from
         * @return Value at address if valid, 0 if invalid
         * @details This is the only function that directly reads from the mem vector.
         */
        uint8_t get8(uint32_t addr) const
        {
            // if address is illegal, return 0
            // otherwise, return the byte at that address
            return check_illegal(addr) ? 0 : mem[addr];
        }

        /**
         * @brief Reads two bytes from memory in little-endian order
         * @param addr Starting address to read from
         * @return 16-bit value composed of bytes at addr and addr+1
         * @details Calls get8() twice to read individual bytes. Address validation
         * is handled by get8();
         */
        uint16_t get16(uint32_t addr) const
        {   
            // combine two bytes in little-endian order:
            // first byte (addr) becomes least significant byte
            // second byte (addr + 1) is shifted left 8 bits to become most significant byte
            // both bytes are then ORed together 
            return get8(addr) | (get8(addr + 1) << 8);
        }

        /**
         * @brief Reads four bytes from memory in little-endian order
         * @param addr Starting address to read from
         * @return 32-bit value composed of bytes at addr through addr+3
         * @details Calls get16() twice to read pairs of bytes. Address validation
         * is handled by get8() through get16().
         */
        uint32_t get32(uint32_t addr) const
        {
            // combine two 16-bit values in little-endian order:
            // first two bytes become least significant 16 bits
            // second two bytes are shifted left 16 bits to become most significant 16 bits
            return get16(addr) | (get16(addr + 2) << 16);
        }
        
        /**
         * @brief Reads a byte and sign-extends it to 32 bits
         * @param addr Address to read from
         * @return Sign-extended 32-bit value
         * @details Converts unsigned 8-bit value to signed 8-bit, then 
         * sign-extends to 32-bits signed integer.
         */
        int32_t get8_sx(uint32_t addr) const
        {
            return static_cast<int32_t>(static_cast<int8_t>(get8(addr)));
        }

        /**
         * @brief Reads two bytes and sign-extends to 32 bits
         * @param addr Starting address to read from
         * @return Sign-extended 32-bit value
         * @details Converts unsigned 16-bit value to signed 16-bit, then 
         * sign-extends to 32-bits signed integer.
         */
        int32_t get16_sx(uint32_t addr) const
        {
            return static_cast<int32_t>(static_cast<int16_t>(get16(addr)));
        }

        /**
         * @brief Reads four bytes as a signed 32-bit value
         * @param addr Starting address to read from
         * @return 32-bit signed value
         * @details Reads 32-bit value in little-endian order.
         */
        int32_t get32_sx(uint32_t addr) const
        {
            return get32(addr);
        }
       
        // setters
        /**
         * @brief Writes a single byte to memory
         * @param addr Address to write to
         * @param val Value to write
         * @details Validates address before writing. Along with the 
         * constructor, this is the only function that writes to mem vector.
         */
        void set8(uint32_t addr, uint8_t val)
        {
            // only write to memory if address is valid
            if (!check_illegal(addr))
            {
                mem[addr] = val; // store byte at specified address
            }
        }

        /**
         * @brief Writes two bytes to memory in little-endian order
         * @param addr Starting address to write to
         * @param val 16-bit value to write
         * @details Splits value into two bytes and writes in little-endian
         * order using set8(). 
         */
        void set16(uint32_t addr, uint16_t val)
        {
            // store in little-endian order:
            // most significant byte goes at higher address (addr + 1)
            set8(addr + 1, val >> 8);
            // least significant byte goes at lower address (addr)
            set8(addr, val);
        }

        /**
         * @brief Writes four bytes to memory in little-endian order
         * @param addr Starting address to write to
         * @param val 32-bit value to write
         * @details Splits value into four bytes and writes in little-endian
         * order using set16();
         */
        void set32(uint32_t addr, uint32_t val)
        {
            // store in little-endian order:
            // most significant 16 bits go at higher addresses (addr + 2, addr + 3)
            set16(addr + 2, val >> 16);
            //least significant 16 bits go at lower addresses (addr, addr + 1)
            set16(addr, val);
        }
        
        void dump() const;
        bool load_file(const std::string& fname);
       
    private:
        /**
         * @brief Vector storing simulated memory bytes
         * @details mem[i] represents byte at address i in simulated memory.
         * Size is determined by constructor and rounded to multiple of 16.
         */
        std::vector<uint8_t> mem;
};

#endif