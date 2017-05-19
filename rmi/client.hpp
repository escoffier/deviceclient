#ifndef RMI_CLIENT_HPP_
#define RMI_CLIENT_HPP_
#include <rmi.hpp>

#ifdef RMI_CLIENT_EXPORT
	#ifdef _MSC_VER
		#define RMI_CLIENT_API __declspec( dllexport )
	#else
		#define RMI_CLIENT_API __attribute__( ( visibility( "default" ) ) )
	#endif
#else
	#ifdef _MSC_VER
		#define RMI_CLIENT_API __declspec( dllimport )
	#endif
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace rmi{
///////////////////////////////////////////////////////////////////////////////////////////////////
class RMI_CLIENT_API network;
class RMI_CLIENT_API client
{
public:
    friend class network_impl;

public:
    client();
    virtual ~client();

private:
    client( const client &other ) = delete;
    client& operator=( const client &other ) = delete;

public:
    void start( const std::string &ip, int port );
	void stop();
	void wait();

protected:
	network*     m_network;
	rmi::packets m_packets;
    rmi::message m_request;
    rmi::message m_response;
};
///////////////////////////////////////////////////////////////////////////////////////////////////
} // end namespace rmi
///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // RMI_CLIENT_HPP_
