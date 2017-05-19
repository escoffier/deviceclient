#ifndef rmi_capability_hpp_
#define rmi_capability_hpp_

#include <string>
#include <list>
#include "rmi.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace rmi{
///////////////////////////////////////////////////////////////////////////////////////////////////
class capability : public rmi::rmitype< capability >
{
public:
    capability()
    {
    }

    capability( const capability &other )
    {
        m_cpu = other.m_cpu;
        m_memory = other.m_memory;
        m_disk = other.m_disk;
    }

    inline capability& operator=( const capability &other )
    {
        if( this != &other )
        {
            m_cpu = other.m_cpu;
            m_memory = other.m_memory;
            m_disk = other.m_disk;
        }

        return( *this );
    }

public:
    inline void cpu( int value )
    {
        m_cpu = value;
    }

    inline void memory( int value )
    {
        m_memory = value;
    }

    inline void disk( int value )
    {
        m_disk = value;
    }

public:
    inline int cpu() const
    {
        return m_cpu;
    }

    inline int memory() const
    {
        return m_memory;
    }

    inline int disk() const
    {
        return m_disk;
    }

public:
    virtual void read( rmi::message &stream )
    {
        stream >> m_cpu;
        stream >> m_memory;
        stream >> m_disk;
    }

    virtual void write( rmi::message &stream ) const
    {
        stream << m_cpu;
        stream << m_memory;
        stream << m_disk;
    }

private:
    int m_cpu;
    int m_memory;
    int m_disk;
};
///////////////////////////////////////////////////////////////////////////////////////////////////
} // end namespace rmi
///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // rmi_capability_hpp_

