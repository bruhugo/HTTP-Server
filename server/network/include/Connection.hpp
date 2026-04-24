namespace server
{
namespace network 
{

enum class ConnectionState {
    OPEN, CLOSED, CLOSING
};

class Connection {
public:
    Connection(int fd);
    ~Connection();

    void close();
private:
    int fd;
};

} // basic
    
} // server
