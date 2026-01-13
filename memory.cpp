#include "memory.h"
#include <iostream>

/**
 * @brief Dumps entire memory contents in formatted hex with ASCII
 * @details Displays memory in 16-byte lines with format:
 * address: hex bytes *ASCII representation*
 * Non-printable characters are shown as dots in the ASCII portion.
 */
void memory::dump() const 
{
    std::vector<char> dumpy; // temporary storage for ASCII characters

    for (size_t i = 0; i < get_size(); i++) 
    {
        // start a new line with the address at every 16-byte boundary
        if (i % 16 == 0) 
        {    
            // print the starting address of the current 16-byte line
            std::cout << hex::to_hex32(i) << ": ";
        }

        // print an extra space at 8-byte midpoint for readability
        if (i % 8 == 0 && i % 16 != 0) 
        {
            std::cout << " ";
        }

        // print byte value in hex
        std::cout << hex::to_hex8(mem[i]) << " ";

        // get byte and convert to ASCII or '.' if not printable
        uint8_t ch = get8(i);
        dumpy.push_back(isprint(ch) ? ch : '.');

        // at end of line, print ASCII portion and clear 'dumpy'
        if (i % 16 == 15) 
        {
            std::cout << "*";
            for (char c : dumpy) 
            {
                std::cout << c;
            }
            std::cout << "*\n";

            dumpy.clear();
        }
    }
}

/**
 * @brief Loads a binary file into simulated memory
 * @param fname Name of file to load
 * @return true if file loaded successfully, false if an error occured
 * @details Opens file in binary mode and reads bytes sequentially into memory.
 * Checks for file open errors and memory size limitations.
 * Prints error messages for file open failure or if program is too big.
 */
bool memory::load_file(const std::string& fname)
{
    // open file in binary mode
    std::ifstream infile(fname, std::ios::in|std::ios::binary);
    
    // check if file opened successfully
    if (!infile)
    {      
        std::cerr << "Can't open file '" << fname << "' for reading." << std::endl;
        return false;
    }

    uint8_t i;
    infile >> std::noskipws; // don't skip whitespace characters when reading

    // read files byte by byte
    for (uint32_t addr = 0; infile >> i; ++addr)
    {
        // check if we've exceeded memory size
        if (check_illegal(addr))
        {
            std::cerr << "Program too big." << std::endl;
            infile.close();
            return false;
        }
        set8(addr,i); // store byte in memory
    }
    infile.close();
    return true;
}