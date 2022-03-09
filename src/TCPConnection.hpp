#include <boost/asio.hpp>

class TCPConnection {
  public:
    TCPConnection();
    ~TCPConnection();
    bool write();
    bool read();
    void load_data(std::string &);
    void load_data(std::vector<int> &);
    void load_data(std::vector<unsigned char> &);
    void load_data(boost::asio::mutable_buffer &);
    std::string& unload_data();
    std::vector<int>& unload_data();
    std::vector<unsigned char>& unload_data();
    boost::asio::mutable_buffer& unload_data();
    const boost::asio::ip::tcp::socket& get_socket() const;
  private:
    boost::asio::ip::tcp::socket m_socket;
    boost::asio::mutable_buffer m_internal_buffer;
    void m_handle_write(boost::system::error_code&, size_t);
};
