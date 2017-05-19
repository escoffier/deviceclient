#ifndef rmi_switch_collect_info_hpp_
#define rmi_switch_collect_info_hpp_

#include <string>
#include <list>
#include "rmi.hpp"
#include "flow.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace rmi{
///////////////////////////////////////////////////////////////////////////////////////////////////
class switch_collect_info : public rmi::rmitype< switch_collect_info >
{
public:
    switch_collect_info()
    {
    }

    switch_collect_info( const switch_collect_info &other )
    {
        m_ifconfig = other.m_ifconfig;
    }

    inline switch_collect_info& operator=( const switch_collect_info &other )
    {
        if( this != &other )
        {
            m_ifconfig = other.m_ifconfig;
        }

        return( *this );
    }

public:
    inline void ifconfig( const std::list< flow >& value )
    {
        m_ifconfig = value;
    }

public:
    inline const std::list< flow >& ifconfig() const
    {
        return m_ifconfig;
    }

public:
    virtual void read( rmi::message &stream )
    {
        stream >> m_ifconfig;
    }

    virtual void write( rmi::message &stream ) const
    {
        stream << m_ifconfig;
    }

private:
    std::list< flow > m_ifconfig;
};
///////////////////////////////////////////////////////////////////////////////////////////////////
} // end namespace rmi
///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // rmi_switch_collect_info_hpp_

