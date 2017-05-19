#ifndef rmi_ap_hpp_
#define rmi_ap_hpp_

#include "client.hpp"
#include <string>
#include "switch_collect_info.hpp"
#include "server_collect_info.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace rmi{
///////////////////////////////////////////////////////////////////////////////////////////////////
class RMI_CLIENT_API ap : public client
{
public:
    ap();
    virtual ~ap();

private:
    ap( const ap &other ) = delete;
    ap& operator=( const ap &other ) = delete;

public:
    void get_server(std::list< std::string >& p1);
    void get_switch(std::list< std::string >& p1);
    void get(const std::string& p1,switch_collect_info& p2);
    void get(const std::string& p1,server_collect_info& p2);
};
///////////////////////////////////////////////////////////////////////////////////////////////////
} // end namespace rmi
///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // rmi_ap_hpp_
