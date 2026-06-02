#ifndef SC_CONVERT_H
#define SC_CONVERT_H

/**
 * Reads a missing stickers text file, inverts the logic, 
 * and outputs the commands for the owned stickers.
 * * @param filename The path to the text file containing the missing list.
 * @return 0 on success, 1 on error (e.g., file not found).
 */
int convert_stickers(const char *filename, int duplicate_flag);

#endif // SC_CONVERT_H
