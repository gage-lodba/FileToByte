#pragma once
#include <sstream>
#include <filesystem>
#include <future>
// What we technically only need is:
#include <fstream>  // For file input/output operations
#include <iostream> // For standard input/output operations
#include <iomanip>  // For setting the output format
#include <vector>   // For using the vector container
#include <string>   // For using the string class


namespace FileToByte {
	extern std::string Convert(std::string Path, std::string VarName);
}