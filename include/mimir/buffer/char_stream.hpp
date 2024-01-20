#ifndef MIMIR_BUFFER_CHAR_STREAM_HPP_
#define MIMIR_BUFFER_CHAR_STREAM_HPP_

#include "../common/mixins.hpp"

#include <cstddef>
#include <vector>
#include <iostream>


namespace mimir {

class CharStream : public UncopyableMixin<CharStream> {
private:
    std::vector<char> m_data;

public:
    void write(const char* data, size_t amount) {
        m_data.insert(m_data.end(), data, data + amount);
    }

    void clear() { m_data.clear(); }

    [[nodiscard]] const char* get_data() const { return m_data.data(); }

    [[nodiscard]] size_t get_size() const { return m_data.size(); }
};

}  // namespace mimir

#endif // MIMIR_BUFFERS_CHAR_STREAM_HPP_
