#ifndef rmi_user_hpp_
#define rmi_user_hpp_

#include <string>
#include <list>
#include "rmi.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace rmi{
///////////////////////////////////////////////////////////////////////////////////////////////////
class user : public rmi::rmitype< user >
{
public:
    user()
    {
    }

    user( const user &other )
    {
        m_name = other.m_name;
        m_time = other.m_time;
    }

    inline user& operator=( const user &other )
    {
        if( this != &other )
        {
            m_name = other.m_name;
            m_time = other.m_time;
        }

        return( *this );
    }

public:
    inline void name( const std::string& value )
    {
        m_name = value;
    }

    inline void time( const std::string& value )
    {
        m_time = value;
    }

public:
    inline const std::string& name() const
    {
        return m_name;
    }

    inline const std::string& time() const
    {
        return m_time;
    }

public:
    virtual void read( rmi::message &stream )
    {
        stream >> m_name;
        stream >> m_time;
    }

    virtual void write( rmi::message &stream ) const
    {
        stream << m_name;
        stream << m_time;
    }

private:
    std::string m_name;
    std::string m_time;
};
///////////////////////////////////////////////////////////////////////////////////////////////////
} // end namespace rmi
///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // rmi_user_hpp_

