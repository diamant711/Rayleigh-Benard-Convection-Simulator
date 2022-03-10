#include <boost/asio.hpp>

class TCPConnection {
  public:
    TCPConnection();
    ~TCPConnection();
    bool write(void);
    bool read(void);
    void load_data(const std::string &);
    void load_data(const std::vector<int> &);
    void load_data(const std::vector<unsigned char> &);
    void load_data(const boost::asio::mutable_buffer &);
    const boost::asio::mutable_buffer& unload_data(void) const;
    const boost::asio::ip::tcp::socket& get_socket() const;
  private:
    boost::asio::ip::tcp::socket m_socket;
    boost::asio::mutable_buffer m_internal_buffer;
    void m_handle_write(boost::system::error_code&, size_t);
};
