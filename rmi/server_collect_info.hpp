#ifndef rmi_server_collect_info_hpp_
#define rmi_server_collect_info_hpp_

#include <string>
#include <list>
#include "rmi.hpp"
#include "process.hpp"
#include "user.hpp"
#include "capability.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace rmi{
///////////////////////////////////////////////////////////////////////////////////////////////////
class server_collect_info : public rmi::rmitype< server_collect_info >
{
public:
    server_collect_info()
    {
    }

    server_collect_info( const server_collect_info &other )
    {
        m_top = other.m_top;
        m_who = other.m_who;
        m_ps = other.m_ps;
    }

    inline server_collect_info& operator=( const server_collect_info &other )
    {
        if( this != &other )
        {
            m_top = other.m_top;
            m_who = other.m_who;
            m_ps = other.m_ps;
        }

        return( *this );
    }

public:
    inline void top( const capability& value )
    {
        m_top = value;
    }

    inline void who( const std::list< user >& value )
    {
        m_who = value;
    }

    inline void ps( const std::list< process >& value )
    {
        m_ps = value;
    }

public:
    inline const capability& top() const
    {
        return m_top;
    }

    inline const std::list< user >& who() const
    {
        return m_who;
    }

    inline const std::list< process >& ps() const
    {
        return m_ps;
    }

public:
    virtual void read( rmi::message &stream )
    {
        stream >> m_top;
        stream >> m_who;
        stream >> m_ps;
    }

    virtual void write( rmi::message &stream ) const
    {
        stream << m_top;
        stream << m_who;
        stream << m_ps;
    }

private:
    capability m_top;
    std::list< user > m_who;
    std::list< process > m_ps;
};
///////////////////////////////////////////////////////////////////////////////////////////////////
} // end namespace rmi
///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // rmi_server_collect_info_hpp_

