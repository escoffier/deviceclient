#ifndef rmi_flow_hpp_
#define rmi_flow_hpp_

#include <string>
#include <list>
#include "rmi.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace rmi{
///////////////////////////////////////////////////////////////////////////////////////////////////
class flow : public rmi::rmitype< flow >
{
public:
    flow()
    {
    }

    flow( const flow &other )
    {
        m_in = other.m_in;
        m_out = other.m_out;
        m_loss = other.m_loss;
        m_error = other.m_error;
    }

    inline flow& operator=( const flow &other )
    {
        if( this != &other )
        {
            m_in = other.m_in;
            m_out = other.m_out;
            m_loss = other.m_loss;
            m_error = other.m_error;
        }

        return( *this );
    }

public:
    inline void in( int value )
    {
        m_in = value;
    }

    inline void out( int value )
    {
        m_out = value;
    }

    inline void loss( int value )
    {
        m_loss = value;
    }

    inline void error( int value )
    {
        m_error = value;
    }

public:
    inline int in() const
    {
        return m_in;
    }

    inline int out() const
    {
        return m_out;
    }

    inline int loss() const
    {
        return m_loss;
    }

    inline int error() const
    {
        return m_error;
    }

public:
    virtual void read( rmi::message &stream )
    {
        stream >> m_in;
        stream >> m_out;
        stream >> m_loss;
        stream >> m_error;
    }

    virtual void write( rmi::message &stream ) const
    {
        stream << m_in;
        stream << m_out;
        stream << m_loss;
        stream << m_error;
    }

private:
    int m_in;
    int m_out;
    int m_loss;
    int m_error;
};
///////////////////////////////////////////////////////////////////////////////////////////////////
} // end namespace rmi
///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // rmi_flow_hpp_

