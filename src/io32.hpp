#include <string>

namespace io32 {

using int_type = std::char_traits<char32_t>::int_type;
int_type getchar();
void putchar(const int_type);

} // namespace iobuf
