#include "hex.h"
#include <iostream>
#include <sstream>
#include <iomanip>

/**
 * @brief Format an 8-bit value as a 2-digit hex string
 * @param i 8-bit value to format
 * @return String containing exactly 2 hex digits
 * @details Uses stringstream to format number with leading zeros.
 * Casts to uint16_t to prevent printing as char.
 * 
 */
std::string hex::to_hex8 (uint8_t i)
{
    std::ostringstream os;
    
    os << std::hex                      // format as hexadecimal
       << std::setfill('0')             // pad with zeros
       << std::setw(2)                  // use only 2 digits
       << static_cast<uint16_t>(i);     // cast to prevent char interpretation
    return os.str();
}

/**
 * @brief Format a 32-bit value as an 8-digit hex string
 * @param i 32-bit value to format
 * @return String containing exactly 8 hex digits
 * @details Uses stringstream to format number with leading zeros
 */
std::string hex::to_hex32 (uint32_t i)
{
    std::ostringstream os;

    os << std::hex                     // format as hexadecimal
       << std::setfill('0')            // pad with zeros
       << std::setw(8)                 // use only 8 digits
       << i;
    return os.str();
}

/**
 * @brief Formats a 12-bit value as a hexadecimal string
 * @param i The value to format
 * @return String in format "0x" followed by 3 hex digits
 * @details Formats the 12 least significant bits of the input
 *          as a hex string with leading zeros, used for CSR
 *          instruction formatting. Example: 0xfff
 */
std::string hex::to_hex0x12(uint32_t i)
{
    std::ostringstream os;
    
    os << "0x"
       << std::hex                      // format as hexadecimal
       << std::setfill('0')             // pad with zeros
       << std::setw(3)                  // use only 3 digits
       << (i & 0x00000fff);             // bit masking to avoid entire number being printed
    return os.str();
}

/**
 * @brief Formats a 20-bit value as a hexadecimal string
 * @param i The value to format
 * @return String in format "0x" followed by 5 hex digits
 * @details Formats the 20 least significant bits of the input
 *          as a hex string with leading zeros, used for LUI
 *          and AUIPC instruction formatting. Example: 0x12345
 */
std::string hex::to_hex0x20(uint32_t i)
{
    std::ostringstream os;
    
    os << "0x"
       << std::hex                      // format as hexadecimal
       << std::setfill('0')             // pad with zeros
       << std::setw(5)                  // use only 5 digits
       << i;                            
    return os.str();
}

/**
 * @brief Format a 32-bit value as a hex string with "0x" prefix
 * @param i 32-bit value to format
 * @return String containing "0x" followed by 8 hex digits
 * @details Uses to_hex32() to format the number and prepends "0x"
 */
std::string hex::to_hex0x32 (uint32_t i)
{
    return std::string("0x") + to_hex32(i);
}