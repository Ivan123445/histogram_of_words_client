#ifndef FILE_UTILS_H
#define FILE_UTILS_H

void send_file(int client_socket, const char *filename, long start, long end);

#endif //FILE_UTILS_H
