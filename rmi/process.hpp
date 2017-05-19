#ifndef rmi_process_hpp_
#define rmi_process_hpp_

#include <string>
#include <list>
#include "rmi.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace rmi{
///////////////////////////////////////////////////////////////////////////////////////////////////
class process : public rmi::rmitype< process >
{
public:
    process()
    {
    }

    process( const process &other )
    {
        m_id = other.m_id;
        m_name = other.m_name;
        m_path = other.m_path;
    }

    inline process& operator=( const process &other )
    {
        if( this != &other )
        {
            m_id = other.m_id;
            m_name = other.m_name;
            m_path = other.m_path;
        }

        return( *this );
    }

public:
    inline void id( int value )
    {
        m_id = value;
    }

    inline void name( const std::string& value )
    {
        m_name = value;
    }

    inline void path( const std::string& value )
    {
        m_path = value;
    }

public:
    inline int id() const
    {
        return m_id;
    }

    inline const std::string& name() const
    {
        return m_name;
    }

    inline const std::string& path() const
    {
        return m_path;
    }

public:
    virtual void read( rmi::message &stream )
    {
        stream >> m_id;
        stream >> m_name;
        stream >> m_path;
    }

    virtual void write( rmi::message &stream ) const
    {
        stream << m_id;
        stream << m_name;
        stream << m_path;
    }

private:
    int m_id;
    std::string m_name;
    std::string m_path;
};
///////////////////////////////////////////////////////////////////////////////////////////////////
} // end namespace rmi
///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // rmi_process_hpp_

