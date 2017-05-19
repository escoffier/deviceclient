#ifndef WHL_RMI_ERROR_HPP_
#define WHL_RMI_ERROR_HPP
#include <exception>
#include <string>

//////////////////////////////////////////////////////////////////////////
namespace whl{
//////////////////////////////////////////////////////////////////////////
class rmierror : public std::exception
{
public:
    rmierror( const std::string &info ) : m_info( info )
    {
    }

public:
#ifdef _MSC_VER
    virtual const char* what() const
#else
    virtual const char* what() const _GLIBCXX_USE_NOEXCEPT
#endif
    {
        return m_info.c_str();
    }

private:
    std::string m_info;
};
//////////////////////////////////////////////////////////////////////////
}// end namespace whl
//////////////////////////////////////////////////////////////////////////
#endif // WHL_RMI_ERROR_HPP_

