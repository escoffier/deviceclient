#ifndef WHL_RMI_HPP_
#define WHL_RMI_HPP_

#include <stdio.h>
#include <memory.h>
#include <string>
#include <list>

#ifdef _MSC_VER
	#if defined(_WIN64)
		typedef unsigned __int64 SOCKET;
	#else
		typedef unsigned int SOCKET;
	#endif
#else
	typedef unsigned int SOCKET;
#endif

#ifdef RMI_EXPORT
	#ifdef _MSC_VER
		#define RMI_API __declspec( dllexport )
	#else
		#define RMI_API __attribute__( ( visibility( "default" ) ) )
	#endif
#else
	#ifdef _MSC_VER
		#define RMI_API __declspec( dllimport )
	#endif
#endif

#ifndef RMI_REG
#define RMI_REG( s, m ) reg_member< decltype(s::m) >( #m )
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace rmi {
///////////////////////////////////////////////////////////////////////////////////////////////////
class command;
class function;
class module;

///////////////////////////////////////////////////////////////////////////////////////////////////
/** 类型特征
 *
 */
template< typename T >
struct type_traits
{
	typedef T        type;
	typedef T&       reference;
	typedef const T& const_reference;
};

template< typename T >
struct type_traits< T& >
{
	typedef typename type_traits< T >::type type;
	typedef typename type&                  reference;
	typedef typename const type&            const_reference;
};

template< typename T >
struct type_traits< const T >
{
	typedef typename type_traits< T >::type type;
	typedef typename type&                  reference;
	typedef typename const type&            const_reference;
};

template< typename T >
struct type_traits< const T& >
{
	typedef typename type_traits< T >::type type;
	typedef typename type&                  reference;
	typedef typename const type&            const_reference;
};

template<>
struct type_traits< void >
{
	typedef void type;
	typedef void reference;
	typedef void const_reference;
};

template<>
struct type_traits< bool >
{
	typedef bool type;
	typedef bool reference;
	typedef bool const_reference;
};

template<>
struct type_traits< char >
{
	typedef char  type;
	typedef char& reference;
	typedef char  const_reference;
};

template<>
struct type_traits< short >
{
	typedef short  type;
	typedef short& reference;
	typedef short  const_reference;
};

template<>
struct type_traits< int >
{
	typedef int  type;
	typedef int& reference;
	typedef int  const_reference;
};

template<>
struct type_traits< float >
{
	typedef float  type;
	typedef float& reference;
	typedef float  const_reference;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
template< typename T >
struct is_in_type
{
	static const bool value = true;
};

template< typename T >
struct is_in_type< T& >
{
	static const bool value = false;
};

template< typename T >
struct is_in_type< const T >
{
	static const bool value = true;
};

template< typename T >
struct is_in_type< const T& >
{
	static const bool value = true;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
template< typename T >
struct is_out_type
{
	static const bool value = false;
};

template< typename T >
struct is_out_type< T& >
{
	static const bool value = true;
};

template< typename T >
struct is_out_type< const T >
{
	static const bool value = false;
};

template< typename T >
struct is_out_type< const T& >
{
	static const bool value = false;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
template< typename T >
struct is_array_type
{
	static const bool value = false;
};

template< typename T >
struct is_array_type< T& >
{
	static const bool value = is_array_type< T >::value;
};

template< typename T >
struct is_array_type< const T >
{
	static const bool value = is_array_type< T >::value;
};

template< typename T >
struct is_array_type< std::list< T > >
{
	static const bool value = true;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class out_of_packet : public std::exception
{
public:
	out_of_packet()
	{
	}

public:
#ifdef _MSC_VER
	virtual const char* what() const
#else
	virtual const char* what() const _GLIBCXX_USE_NOEXCEPT
#endif	
	{
		return nullptr;
	}
};

class recv_packet_exception : public std::exception
{
public:
	recv_packet_exception()
	{
	}

public:
#ifdef _MSC_VER
	virtual const char* what() const
#else
	virtual const char* what() const _GLIBCXX_USE_NOEXCEPT
#endif	
	{
		return nullptr;
	}
};

class send_packet_exception : public std::exception
{
public:
	send_packet_exception()
	{
	}

public:
#ifdef _MSC_VER
	virtual const char* what() const
#else
	virtual const char* what() const _GLIBCXX_USE_NOEXCEPT
#endif	
	{
		return nullptr;
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class RMI_API packet
{
private:
	friend class message;
	friend class packets;
	static const unsigned int max_bytes = 1300;

private:
	packet();
	~packet();

private:
	packet( const packet &other ) = delete;
	packet& operator=( const packet &other ) = delete;

private:
	unsigned char  data[max_bytes]; // 包内容缓存
	unsigned short len;             // 内容大小
	unsigned short off;             // 内容偏移量
	unsigned int   index;           // 分包索引
	packet*        next;            // 分包
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class RMI_API packets
{
private:
	friend class message;
	friend class FunctionTestCase;
	friend class MsgTestCase;

private:
	struct s_packet_t
	{
		packet      data;
		s_packet_t* prev;
		s_packet_t* next;
	};

public:
	packets();
	~packets();

private:
	packets( const packets &other ) = delete;
	packets& operator=( const packets &other ) = delete;

private:
	packet* alloc();
	void free( packet *pkt );

private:
	s_packet_t* m_idel;
	s_packet_t* m_used;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class RMI_API message
{
private:
	friend class FunctionTestCase;
	friend class MsgTestCase;

private:
	struct s_header_t
	{
		unsigned char  mark;       // 启始位
		unsigned char  version;    // 消息使用的版本号
		unsigned short code;       // 消息的状态值
		unsigned int   sn;         // 消息流水号
		unsigned int   len;        // 加密后的内容长度(包括消息头)
		unsigned char  check[8];   // 消息校验位
	};

public:
	message( packets &pool );
	~message();

private:
	message( const message &other ) = delete;
	message& operator=( const message &other ) = delete;

public:
	bool recv( SOCKET fd );
	bool send( SOCKET fd );
	bool decrypt( const char *key );
	bool encrypt( const char *key );
	void clear();

public:
	inline unsigned char version() const { return m_hdr.version; }
	inline unsigned short code() const   { return m_hdr.code; }
	inline unsigned int sn() const       { return m_hdr.sn; }
	inline unsigned int length() const   { return m_hdr.len; }

public:
	inline void set( unsigned int sn )
	{
		m_hdr.sn = sn;
	}

	inline void response( unsigned short code )
	{
		m_hdr.code = code;
	}

public:
	inline friend message& operator>>( message &msg, bool &data )
	{
		msg.read( data );
		return msg;
	}

	inline friend message& operator>>( message &msg, char &data )
	{
		msg.read( data );
		return msg;
	}

	inline friend message& operator>>( message &msg, short &data )
	{
		msg.read( data );
		return msg;
	}

	inline friend message& operator>>( message &msg, int &data )
	{
		msg.read( data );
		return msg;
	}

	inline friend message& operator>>( message &msg, std::string &data )
	{
		msg.read( data );
		return msg;
	}

	template< typename T >
	inline friend message& operator>>( message &msg, std::list< T > &data )
	{
		int size = 0;
		msg.read( size );
		for( int i = 0; i < size; ++i )
		{
			T obj;
			msg >> obj;
			data.push_back( obj );
		}

		return msg;
	}

public:
	inline friend message& operator<<( message &msg, bool data )
	{
		msg.write( data );
		return msg;
	}

	inline friend message& operator<<( message &msg, char data )
	{
		msg.write( data );
		return msg;
	}

	inline friend message& operator<<( message &msg, short data )
	{
		msg.write( data );
		return msg;
	}

	inline friend message& operator<<( message &msg, int data )
	{
		msg.write( data );
		return msg;
	}

	inline friend message& operator<<( message &msg, const std::string &data )
	{
		msg.write( data );
		return msg;
	}

	template< typename T >
	inline friend message& operator<<( message &msg, const std::list< T > &data )
	{
		msg.write( static_cast<int>( data.size() ) );
		for( auto it = data.begin(); it != data.end(); ++it )
		{
			msg << (*it);
		}

		return msg;
	}

private:
	void read( bool &data );
	void read( char &data );
	void read( short &data );
	void read( int &data );
	void read( std::string &data );

private:
	void write( bool data );
	void write( char data );
	void write( short data );
	void write( int data );
	void write( const std::string &data );

private:
	s_header_t   m_hdr;  // 消息头
	packet*      m_cur;  // 当前处理的分包
	packet*      m_head; // 第一个分包
	packet*      m_tail; // 最后一个分包
	packets&     m_pool; // 包缓存池
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class RMI_API base_type
{
protected:
	base_type( const char *name );
	base_type( const std::type_info &info );

protected:
	virtual ~base_type() = default;

private:
	base_type( base_type &other ) = delete;
	base_type& operator=( base_type &other ) = delete;

public:
	virtual const base_type* get() const = 0;
	virtual bool is_string_type() const
	{
		return false;
	}

	virtual bool is_class_type() const
	{
		return false;
	}

	virtual bool is_reference() const
	{
		return false;
	}

	virtual bool is_array() const
	{
		return false;
	}

	virtual void cpp( const std::string &path ) const
	{
	}

public:
	inline const char* name() const
	{
		return m_name;
	}

private:
	char m_name[64];
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class RMI_API typeinfo
{
public:
	typeinfo();
	typeinfo( const base_type* other );
	typeinfo( const typeinfo &other );
	typeinfo& operator=( const typeinfo &other );

public:
	inline void set_reference( bool flag = true )
	{
		m_reference = flag;
	}

	inline bool is_reference() const
	{
		return m_reference;
	}

	inline void set_array( bool flag = true )
	{
		m_array = flag;
	}

	inline bool is_array() const
	{
		return m_array;
	}

	inline void set_type( const base_type *type )
	{
		m_type = type;
	}

	inline const base_type* type() const
	{
		return m_type;
	}

public:
	bool operator==( const typeinfo &other ) const;
	bool operator!=( const typeinfo &other ) const;
	bool operator==( const base_type *other ) const;
	bool operator!=( const base_type *other ) const;

private:
	const base_type* m_type;
	bool             m_reference;
	bool             m_array;
};

//////////////////////////////////////////////////////////////////////////
class class_type
{
public:
	struct member
	{
		std::string name;
		typeinfo    type;
	};

public:
	class_type() = default;
	virtual ~class_type() = default;

private:
	class_type( const class_type &other ) = delete;
	class_type& operator=( const class_type &other ) = delete;
};

//////////////////////////////////////////////////////////////////////////
template< typename T >
class rmitype : public class_type
{
public:
	rmitype()
	{
	}

	virtual ~rmitype()
	{
	}

private:
	rmitype( const rmitype &other ) = delete;
	rmitype& operator=( const rmitype &other ) = delete;

public:
	inline static const std::list< member >& members()
	{
		return m_members;
	}

public:
	inline friend message& operator>>( message &msg, T &obj )
	{
		obj.read( msg );
		return msg;
	}

	inline friend message& operator<<( message &msg, const T &obj )
	{
		obj.write( msg );
		return msg;
	}

protected:
	template< typename membertype >
	static void reg_member( const std::string &name )
	{
		static_assert( member_type_check_traits< membertype >::value, "add member failed, some type is not rmi type" );
		type< membertype > ti;
		member filed;
		filed.name = name;
		filed.type.set_type( ti.get() );
		filed.type.set_array( ti.is_array() );

		// 查询是否有重复
		auto it = m_members.begin();
		while( it != m_members.end() && it->name != filed.name )
		{
			++it;
		}

		// 添加新的成员变量
		if( it == m_members.end() )
		{
			m_members.push_back( filed );
		}
	}

private:
	virtual void read( message &msg ) = 0;
	virtual void write( message &msg ) const = 0;

protected:
	static std::list< member > m_members;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
template< typename T > std::list< rmi::class_type::member > rmitype< T >::m_members;
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
template< typename T >
struct is_rmi_type
{
private:
	template< typename Arg >
	struct is_same_type
	{
		static const bool value = false;
	};

	template<>
	struct is_same_type< bool >
	{
		static const bool value = true;
	};

private:
	static T& make();
	static bool check( class_type& );
	static void check( ... );
	using return_type = decltype( check( make() ) );

public:
	static const bool value = is_same_type< return_type >::value;
};

template< typename T >
struct is_rmi_type< const T >
{
	static const bool value = is_rmi_type< T >::value;
};

template< typename T >
struct is_rmi_type< T& >
{
	static const bool value = is_rmi_type< T >::value;
};

template< typename T >
struct is_rmi_type< const T& >
{
	static const bool value = is_rmi_type< T >::value;
};

template<>
struct is_rmi_type< void >
{
	static const bool value = true;
};

template<>
struct is_rmi_type< char >
{
	static const bool value = true;
};

template<>
struct is_rmi_type< bool >
{
	static const bool value = true;
};

template<>
struct is_rmi_type< short >
{
	static const bool value = true;
};

template<>
struct is_rmi_type< int >
{
	static const bool value = true;
};

template<>
struct is_rmi_type< float >
{
	static const bool value = true;
};

template<>
struct is_rmi_type< std::string >
{
	static const bool value = true;
};

template< typename T >
struct is_rmi_type< std::list< T > >
{
	static const bool value = is_rmi_type< T >::value;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
template< typename T > class type;
template< typename T >
class type< T& > : public type< T >
{
public:
	type() = default;
	virtual ~type() = default;

private:
	type( const type &other ) = delete;
	type& operator=( const type &other ) = delete;

public:
	virtual bool is_array() const
	{
		return true;
	}
};

template< typename T >
class type< const T > : public type< T >
{
public:
	type() = default;
	virtual ~type() = default;

private:
	type( const type &other ) = delete;
	type& operator=( const type &other ) = delete;
};

template< typename T >
class type< const T& > : public type< T >
{
public:
	type() = default;
	virtual ~type() = default;

private:
	type( const type &other ) = delete;
	type& operator=( const type &other ) = delete;
};

template< typename T >
class type< std::list< T > > : public type< T >
{
public:
	type() = default;
	virtual ~type() = default;

private:
	type( const type &other ) = delete;
	type& operator=( const type &other ) = delete;

public:
	virtual bool is_array() const
	{
		return true;
	}
};

template<>
class type< void > : public base_type
{
public:
	type() : base_type( "void" )
	{
	}

private:
	type( const type &other ) = delete;
	type& operator=( const type &other ) = delete;

public:
	virtual const base_type* get() const
	{
		static type< void > s_type;
		return &s_type;
	}
};

template<>
class type< bool > : public base_type
{
public:
	type() : base_type( "bool" )
	{
	}

private:
	type( const type &other ) = delete;
	type& operator=( const type &other ) = delete;

public:
	virtual const base_type* get() const
	{
		static type< bool > s_type;
		return &s_type;
	}
};

template<>
class type< char > : public base_type
{
public:
	type() : base_type( "char" )
	{
	}

private:
	type( const type &other ) = delete;
	type& operator=( const type &other ) = delete;

public:
	virtual const base_type* get() const
	{
		static type< char > s_type;
		return &s_type;
	}
};

template<>
class type< short > : public base_type
{
public:
	type() : base_type( "short" )
	{
	}

private:
	type( const type &other ) = delete;
	type& operator=( const type &other ) = delete;

public:
	virtual const base_type* get() const
	{
		static type< short > s_type;
		return &s_type;
	}
};

template<>
class type< int > : public base_type
{
public:
	type() : base_type( "int" )
	{
	}

private:
	type( const type &other ) = delete;
	type& operator=( const type &other ) = delete;

public:
	virtual const base_type* get() const
	{
		static type< int > s_type;
		return &s_type;
	}
};

template<>
class type< float > : public base_type
{
public:
	type() : base_type( "float" )
	{
	}

private:
	type( const type &other ) = delete;
	type& operator=( const type &other ) = delete;

public:
	virtual const base_type* get() const
	{
		static type< float > s_type;
		return &s_type;
	}
};

template<>
class type< std::string > : public base_type
{
public:
	type() : base_type( "string" )
	{
	}

private:
	type( const type &other ) = delete;
	type& operator=( const type &other ) = delete;

public:
	virtual bool is_string_type() const
	{
		return true;
	}

	virtual const base_type* get() const
	{
		static type< std::string > s_type;
		return &s_type;
	}
};

template<>
class RMI_API type< class_type > : public base_type
{
public:
	type( const std::type_info &info ) : base_type( info )
	{
	}

private:
	type( const type &other ) = delete;
	type& operator=( const type &other ) = delete;

public:
	virtual const std::list< rmi::class_type::member >& members() const = 0;
	virtual void cpp( const std::string &path ) const;

public:
	virtual bool is_class_type() const
	{
		return true;
	}
};

template< typename T >
class type< rmitype< T > > : public type< class_type >
{
private:
	template< bool BoolType >
	struct bool2type
	{
	};

	class check_reg_member
	{
	private:
		template<typename U, void (*)()> struct HELPS;
		template<typename U> static char Test( HELPS<U, &U::reg>* );
		template<typename U> static int Test( ... );
		const static bool value = sizeof( Test<T>( 0 ) ) == sizeof( char );

	private:
		static void reg_impl( bool2type< true > )
		{
			type_traits< T >::type::reg();
		}

		static void reg_impl( bool2type< false > )
		{
		}

	public:
		static void reg()
		{
			reg_impl( bool2type< value >() );
		}
	};

public:
	type() : type< class_type >( typeid( type_traits< T >::type ) )
	{
		check_reg_member::reg();
	}

private:
	type( const type &other ) = delete;
	type& operator=( const type &other ) = delete;

public:
	virtual const std::list< rmi::class_type::member >& members() const
	{
		check_reg_member::reg();
		return rmitype< T >::members();
	}

	virtual const base_type* get() const
	{
		static type< type_traits< T >::type > s_type;
		return &s_type;
	}
};

template< typename T >
class type : public type< rmitype< T > >
{
public:
	type()
	{
	}

private:
	type( const type &other ) = delete;
	type& operator=( const type &other ) = delete;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class RMI_API command
{
public:
	command( message &request, message &response ) : m_request( request ), m_response( response )
	{
	}

	virtual ~command()
	{
	}

private:
	command( const command &other ) = delete;
	command& operator=( const command &other ) = delete;

public:
	virtual void decode() = 0;
	virtual void encode() = 0;
	virtual void run() = 0;

protected:
	message& m_request;
	message& m_response;
};

//////////////////////////////////////////////////////////////////////////
class RMI_API module
{
public:
	module();
	virtual ~module();

private:
	module( module& ) = delete;
	module& operator=( module& ) = delete;

public:
	virtual void start();
	virtual void stop();
	virtual void set( const std::string& key, const std::string &value );
};

//////////////////////////////////////////////////////////////////////////
template< typename ... T > struct member_type_check_traits;
template< typename H, typename ... T >
struct member_type_check_traits< H, T... >
{
	static const bool value = is_rmi_type< H >::value && member_type_check_traits< T... >::value;
};

template< typename H >
struct member_type_check_traits< H >
{
	static const bool value = is_rmi_type< H >::value;
};

template<>
struct member_type_check_traits<>
{
	static const bool value = true;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class RMI_API api;
class function
{
private:
	template< typename ... T > class typelist;
	template< typename H, typename ... T >
	class typelist< H, T... >
	{
	public:
		inline void get( std::list< typeinfo > &results ) const
		{
			type< H > rt;
			typeinfo obj( rt.get() );
			if( is_out_type< H >::value )
			{
				obj.set_reference();
			}

			if( is_array_type< H >::value )
			{
				obj.set_array();
			}

			results.push_back( obj );

			typelist< T... > other;
			other.get( results );
		}
	};

	template< typename T >
	class typelist< T >
	{
	public:
		inline void get( std::list< typeinfo > &results ) const
		{
			type< T > rt;
			typeinfo obj( rt.get() );
			if( is_out_type< T >::value )
			{
				obj.set_reference();
			}

			if( is_array_type< T >::value )
			{
				obj.set_array();
			}

			results.push_back( obj );
		}
	};

	template<>
	class typelist<>
	{
	public:
		inline void get( std::list< typeinfo > &results ) const
		{
		}
	};

protected:
	function( const std::string &name )
	{
		strcpy( m_name, name.c_str() );
	}

	virtual ~function()
	{
	}

private:
	function( const function &other ) = delete;
	function& operator=( const function &other ) = delete;

public:
	virtual command* make( module *obj, message &request, message &response ) const = 0;

public:
	inline const char* name() const
	{
		return m_name;
	}

	inline const typeinfo& result() const
	{
		return m_result;
	}

	inline const std::list< typeinfo >& args() const
	{
		return m_args;
	}

protected:
	template< typename R, typename ... Args >
	inline void set_typeinfo_for_function()
	{
		type< R > rt;
		m_result.set_type( rt.get() );

		typelist< Args... > ps;
		ps.get( m_args );
	}

protected:
	char                  m_name[64];
	typeinfo              m_result;
	std::list< typeinfo > m_args;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
template< typename R, typename C, typename ... Args >
class member_function_impl : public function
{
public:
	typedef R( C::*func_type )( Args... );
	typedef C class_type;
	static_assert( member_type_check_traits< Args... >::value, "regist member function failed, some type is not rmi type" );

public:
	member_function_impl( const std::string &name, func_type f ) : m_fun( f ), function( name )
	{
		set_typeinfo_for_function< R, Args... >();
	}

public:
	virtual command* make( module *obj, message &request, message &response ) const
	{
		class_type *p = dynamic_cast<class_type*>( obj );
		if( p != nullptr )
		{
			return new command_impl< R, C, Args... >( request, response, p, m_fun );
		}
		else
		{
			return nullptr;
		}
	}

private:
	func_type m_fun;
};

//////////////////////////////////////////////////////////////////////////
class RMI_API api
{
private:
	struct s_fun_t
	{
		function* fun;
		s_fun_t*  next;
	};

public:
	api();
	virtual ~api();

private:
	api( const api &other ) = delete;
	api& operator=( const api &other ) = delete;

public:
	virtual const char* name() = 0;
	virtual void reg() = 0;

public:
	virtual module* create() = 0;
	virtual void destroy( module* obj ) = 0;

public:
	void add( function *fun );
	void get( std::list< function* > &fs );

protected:
	template< typename R, typename C, typename ...Args >
	inline void reg( const std::string &name, R( C::*func )( Args... ) )
	{
		if( !name.empty() )
		{
			add( new member_function_impl< R, C, Args... >( name, func ) );
		}
	}

private:
	s_fun_t* m_head;
	s_fun_t* m_tail;
};

//////////////////////////////////////////////////////////////////////////
template< typename R, typename ... Args > class command_impl;
template< typename R, typename C >
class command_impl< R, C > : public command
{
public:
	typedef R( C::*func_type )();
	typedef C class_type;
	typedef typename type_traits< R >::type return_type;

public:
	command_impl( message &request, message &response, class_type *obj, func_type func ) 
		: command( request, response ), m_obj( obj ), m_func( func )
	{
	}

private:
	command_impl( const command_impl &rhs ) = delete;
	command_impl& operator=( const command_impl &rhs ) = delete;

public:
	virtual bool decode()
	{
		return true;
	}

	virtual bool encode()
	{
		try
		{
			m_return.encode( m_response );
			return true;
		}
		catch( out_of_packet e )
		{
			return false;
		}
	}

	virtual void run()
	{
		m_return = ( m_obj->*m_func )( );
	}

protected:
	class_type* m_obj;
	func_type   m_func;
	return_type m_return;
};

template< typename R, typename C, typename T1 >
class command_impl< R, C, T1 > : public command
{
public:
	typedef R( C::*func_type )( T1 );
	typedef C class_type;
	typedef typename type_traits< R >::type  return_type;
	typedef typename type_traits< T1 >::type param1_type;

public:
	command_impl( message &request, message &response, class_type *obj, func_type func ) 
		: command( request, response ), m_obj( obj ), m_func( func )
	{
	}

private:
	command_impl( const command_impl &rhs ) = delete;
	command_impl& operator=( const command_impl &rhs ) = delete;

public:
	virtual void decode()
	{
		if( is_in_type< T1 >::value )
		{
			m_request >> m_param1;
		}
	}

	virtual void encode()
	{
		m_response << m_return;
		if( is_out_type< T1 >::value )
		{
			m_response << m_param1;
		}
	}

	virtual void run()
	{
		m_return = ( m_obj->*m_func )( m_param1 );
	}

protected:
	class_type* m_obj;
	func_type   m_func;
	return_type m_return;
	param1_type m_param1;
};

template< typename R, typename C, typename T1, typename T2 >
class command_impl< R, C, T1, T2 > : public command
{
public:
	typedef R( C::*func_type )( T1, T2 );
	typedef C class_type;
	typedef typename type_traits< R >::type  return_type;
	typedef typename type_traits< T1 >::type param1_type;
	typedef typename type_traits< T2 >::type param2_type;

public:
	command_impl( message &request, message &response, class_type *obj, func_type func )
		: command( request, response ), m_obj( obj ), m_func( func )
	{
	}

private:
	command_impl( const command_impl &rhs ) = delete;
	command_impl& operator=( const command_impl &rhs ) = delete;

public:
	virtual void decode()
	{
		if( is_in_type< T1 >::value )
		{
			m_request >> m_param1;
		}

		if( is_in_type< T2 >::value )
		{
			m_request >> m_param2;
		}
	}

	virtual void encode()
	{
		m_response << m_return;
		if( is_out_type< T1 >::value )
		{
			m_response << m_param1;
		}

		if( is_out_type< T2 >::value )
		{
			m_response << m_param2;
		}
	}

	virtual void run()
	{
		m_return = ( m_obj->*m_func )( m_param1, m_param2 );
	}

protected:
	class_type* m_obj;
	func_type   m_func;
	return_type m_return;
	param1_type m_param1;
	param2_type m_param2;
};

template< typename R, typename C, typename T1, typename T2, typename T3 >
class command_impl< R, C, T1, T2, T3 > : public command
{
public:
	typedef R( C::*func_type )( T1, T2, T3 );
	typedef C class_type;
	typedef typename type_traits< R >::type  return_type;
	typedef typename type_traits< T1 >::type param1_type;
	typedef typename type_traits< T2 >::type param2_type;
	typedef typename type_traits< T3 >::type param3_type;

public:
	command_impl( message &request, message &response, class_type *obj, func_type func )
		: command( request, response ), m_obj( obj ), m_func( func )
	{
	}

private:
	command_impl( const command_impl &rhs ) = delete;
	command_impl& operator=( const command_impl &rhs ) = delete;

public:
	virtual void decode()
	{
		if( is_in_type< T1 >::value )
		{
			m_request >> m_param1;
		}

		if( is_in_type< T2 >::value )
		{
			m_request >> m_param2;
		}

		if( is_in_type< T3 >::value )
		{
			m_request >> m_param3;
		}
	}

	virtual void encode()
	{
		m_response << m_return;
		if( is_out_type< T1 >::value )
		{
			m_response << m_param1;
		}

		if( is_out_type< T2 >::value )
		{
			m_response << m_param2;
		}

		if( is_out_type< T3 >::value )
		{
			m_response << m_param3;
		}
	}

	virtual void run()
	{
		m_return = ( m_obj->*m_func )( m_param1, m_param2, m_param3 );
	}

protected:
	class_type* m_obj;
	func_type   m_func;
	return_type m_return;
	param1_type m_param1;
	param2_type m_param2;
	param3_type m_param3;
};

template< typename R, typename C, typename T1, typename T2, typename T3, typename T4 >
class command_impl< R, C, T1, T2, T3, T4 > : public command
{
public:
	typedef R( C::*func_type )( T1, T2, T3, T4 );
	typedef C class_type;
	typedef typename type_traits< R >::type  return_type;
	typedef typename type_traits< T1 >::type param1_type;
	typedef typename type_traits< T2 >::type param2_type;
	typedef typename type_traits< T3 >::type param3_type;
	typedef typename type_traits< T4 >::type param4_type;

public:
	command_impl( message &request, message &response, class_type *obj, func_type func )
		: command( request, response ), m_obj( obj ), m_func( func )
	{
	}

private:
	command_impl( const command_impl &rhs ) = delete;
	command_impl& operator=( const command_impl &rhs ) = delete;

public:
	virtual void decode()
	{
		if( is_in_type< T1 >::value )
		{
			m_request >> m_param1;
		}

		if( is_in_type< T2 >::value )
		{
			m_request >> m_param2;
		}

		if( is_in_type< T3 >::value )
		{
			m_request >> m_param3;
		}

		if( is_in_type< T4 >::value )
		{
			m_request >> m_param4;
		}
	}

	virtual void encode()
	{
		m_response << m_return;
		if( is_out_type< T1 >::value )
		{
			m_response << m_param1;
		}

		if( is_out_type< T2 >::value )
		{
			m_response << m_param2;
		}

		if( is_out_type< T3 >::value )
		{
			m_response << m_param3;
		}

		if( is_out_type< T4 >::value )
		{
			m_response << m_param4;
		}
	}

	virtual void run()
	{
		m_return = ( m_obj->*m_func )( m_param1, m_param2, m_param3, m_param4 );
	}

protected:
	class_type* m_obj;
	func_type   m_func;
	return_type m_return;
	param1_type m_param1;
	param2_type m_param2;
	param3_type m_param3;
	param4_type m_param4;
};

template< typename R, typename C, typename T1, typename T2, typename T3, typename T4, typename T5 >
class command_impl< R, C, T1, T2, T3, T4, T5 > : public command
{
public:
	typedef R( C::*func_type )( T1, T2, T3, T4, T5 );
	typedef C class_type;
	typedef typename type_traits< R >::type  return_type;
	typedef typename type_traits< T1 >::type param1_type;
	typedef typename type_traits< T2 >::type param2_type;
	typedef typename type_traits< T3 >::type param3_type;
	typedef typename type_traits< T4 >::type param4_type;
	typedef typename type_traits< T5 >::type param5_type;

public:
	command_impl( message &request, message &response, class_type *obj, func_type func )
		: command( request, response ), m_obj( obj ), m_func( func )
	{
	}

private:
	command_impl( const command_impl &rhs ) = delete;
	command_impl& operator=( const command_impl &rhs ) = delete;

public:
	virtual void decode()
	{
		if( is_in_type< T1 >::value )
		{
			m_request >> m_param1;
		}

		if( is_in_type< T2 >::value )
		{
			m_request >> m_param2;
		}

		if( is_in_type< T3 >::value )
		{
			m_request >> m_param3;
		}

		if( is_in_type< T4 >::value )
		{
			m_request >> m_param4;
		}

		if( is_in_type< T5 >::value )
		{
			m_request >> m_param5;
		}
	}

	virtual void encode()
	{
		m_response << m_return;
		if( is_out_type< T1 >::value )
		{
			m_response << m_param1;
		}

		if( is_out_type< T2 >::value )
		{
			m_response << m_param2;
		}

		if( is_out_type< T3 >::value )
		{
			m_response << m_param3;
		}

		if( is_out_type< T4 >::value )
		{
			m_response << m_param4;
		}

		if( is_out_type< T5 >::value )
		{
			m_response << m_param5;
		}
	}

	virtual void run()
	{
		m_return = ( m_obj->*m_func )( m_param1, m_param2, m_param3, m_param4, m_param5 );
	}

protected:
	class_type* m_obj;
	func_type   m_func;
	return_type m_return;
	param1_type m_param1;
	param2_type m_param2;
	param3_type m_param3;
	param4_type m_param4;
	param5_type m_param5;
};

template< typename R, typename C, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6 >
class command_impl< R, C, T1, T2, T3, T4, T5, T6 > : public command
{
public:
	typedef R( C::*func_type )( T1, T2, T3, T4, T5, T6 );
	typedef C class_type;
	typedef typename type_traits< R >::type  return_type;
	typedef typename type_traits< T1 >::type param1_type;
	typedef typename type_traits< T2 >::type param2_type;
	typedef typename type_traits< T3 >::type param3_type;
	typedef typename type_traits< T4 >::type param4_type;
	typedef typename type_traits< T5 >::type param5_type;
	typedef typename type_traits< T6 >::type param6_type;

public:
	command_impl( message &request, message &response, class_type *obj, func_type func )
		: command( request, response ), m_obj( obj ), m_func( func )
	{
	}

private:
	command_impl( const command_impl &rhs ) = delete;
	command_impl& operator=( const command_impl &rhs ) = delete;

public:
	virtual void decode()
	{
		if( is_in_type< T1 >::value )
		{
			m_request >> m_param1;
		}

		if( is_in_type< T2 >::value )
		{
			m_request >> m_param2;
		}

		if( is_in_type< T3 >::value )
		{
			m_request >> m_param3;
		}

		if( is_in_type< T4 >::value )
		{
			m_request >> m_param4;
		}

		if( is_in_type< T5 >::value )
		{
			m_request >> m_param5;
		}

		if( is_in_type< T6 >::value )
		{
			m_request >> m_param6;
		}
	}

	virtual void encode()
	{
		m_response << m_return;
		if( is_out_type< T1 >::value )
		{
			m_response << m_param1;
		}

		if( is_out_type< T2 >::value )
		{
			m_response << m_param2;
		}

		if( is_out_type< T3 >::value )
		{
			m_response << m_param3;
		}

		if( is_out_type< T4 >::value )
		{
			m_response << m_param4;
		}

		if( is_out_type< T5 >::value )
		{
			m_response << m_param5;
		}

		if( is_out_type< T6 >::value )
		{
			m_response << m_param6;
		}
	}

	virtual void run()
	{
		m_return = ( m_obj->*m_func )( m_param1, m_param2, m_param3, m_param4, m_param5, m_param6 );
	}

protected:
	class_type* m_obj;
	func_type   m_func;
	return_type m_return;
	param1_type m_param1;
	param2_type m_param2;
	param3_type m_param3;
	param4_type m_param4;
	param5_type m_param5;
	param6_type m_param6;
};

template< typename R, typename C, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7 >
class command_impl< R, C, T1, T2, T3, T4, T5, T6, T7 > : public command
{
public:
	typedef R( C::*func_type )( T1, T2, T3, T4, T5, T6, T7 );
	typedef C class_type;
	typedef typename type_traits< R >::type  return_type;
	typedef typename type_traits< T1 >::type param1_type;
	typedef typename type_traits< T2 >::type param2_type;
	typedef typename type_traits< T3 >::type param3_type;
	typedef typename type_traits< T4 >::type param4_type;
	typedef typename type_traits< T5 >::type param5_type;
	typedef typename type_traits< T6 >::type param6_type;
	typedef typename type_traits< T7 >::type param7_type;

public:
	command_impl( message &request, message &response, class_type *obj, func_type func )
		: command( request, response ), m_obj( obj ), m_func( func )
	{
	}

private:
	command_impl( const command_impl &rhs ) = delete;
	command_impl& operator=( const command_impl &rhs ) = delete;

public:
	virtual void decode()
	{
		if( is_in_type< T1 >::value )
		{
			m_request >> m_param1;
		}

		if( is_in_type< T2 >::value )
		{
			m_request >> m_param2;
		}

		if( is_in_type< T3 >::value )
		{
			m_request >> m_param3;
		}

		if( is_in_type< T4 >::value )
		{
			m_request >> m_param4;
		}

		if( is_in_type< T5 >::value )
		{
			m_request >> m_param5;
		}

		if( is_in_type< T6 >::value )
		{
			m_request >> m_param6;
		}

		if( is_in_type< T7 >::value )
		{
			m_request >> m_param7;
		}
	}

	virtual void encode()
	{
		m_response << m_return;
		if( is_out_type< T1 >::value )
		{
			m_response << m_param1;
		}

		if( is_out_type< T2 >::value )
		{
			m_response << m_param2;
		}

		if( is_out_type< T3 >::value )
		{
			m_response << m_param3;
		}

		if( is_out_type< T4 >::value )
		{
			m_response << m_param4;
		}

		if( is_out_type< T5 >::value )
		{
			m_response << m_param5;
		}

		if( is_out_type< T6 >::value )
		{
			m_response << m_param6;
		}

		if( is_out_type< T7 >::value )
		{
			m_response << m_param7;
		}
	}

	virtual void run()
	{
		m_return = ( m_obj->*m_func )( m_param1, m_param2, m_param3, m_param4, m_param5, m_param6, m_param7 );
	}

protected:
	class_type* m_obj;
	func_type   m_func;
	return_type m_return;
	param1_type m_param1;
	param2_type m_param2;
	param3_type m_param3;
	param4_type m_param4;
	param5_type m_param5;
	param6_type m_param6;
	param7_type m_param7;
};

template< typename R, typename C, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8 >
class command_impl< R, C, T1, T2, T3, T4, T5, T6, T7, T8 > : public command
{
public:
	typedef R( C::*func_type )( T1, T2, T3, T4, T5, T6, T7, T8 );
	typedef C class_type;
	typedef typename type_traits< R >::type  return_type;
	typedef typename type_traits< T1 >::type param1_type;
	typedef typename type_traits< T2 >::type param2_type;
	typedef typename type_traits< T3 >::type param3_type;
	typedef typename type_traits< T4 >::type param4_type;
	typedef typename type_traits< T5 >::type param5_type;
	typedef typename type_traits< T6 >::type param6_type;
	typedef typename type_traits< T7 >::type param7_type;
	typedef typename type_traits< T8 >::type param8_type;

public:
	command_impl( message &request, message &response, class_type *obj, func_type func )
		: command( request, response ), m_obj( obj ), m_func( func )
	{
	}

private:
	command_impl( const command_impl &rhs ) = delete;
	command_impl& operator=( const command_impl &rhs ) = delete;

public:
	virtual void decode()
	{
		if( is_in_type< T1 >::value )
		{
			m_request >> m_param1;
		}

		if( is_in_type< T2 >::value )
		{
			m_request >> m_param2;
		}

		if( is_in_type< T3 >::value )
		{
			m_request >> m_param3;
		}

		if( is_in_type< T4 >::value )
		{
			m_request >> m_param4;
		}

		if( is_in_type< T5 >::value )
		{
			m_request >> m_param5;
		}

		if( is_in_type< T6 >::value )
		{
			m_request >> m_param6;
		}

		if( is_in_type< T7 >::value )
		{
			m_request >> m_param7;
		}

		if( is_in_type< T8 >::value )
		{
			m_request >> m_param8;
		}
	}

	virtual void encode()
	{
		m_response << m_return;
		if( is_out_type< T1 >::value )
		{
			m_response << m_param1;
		}

		if( is_out_type< T2 >::value )
		{
			m_response << m_param2;
		}

		if( is_out_type< T3 >::value )
		{
			m_response << m_param3;
		}

		if( is_out_type< T4 >::value )
		{
			m_response << m_param4;
		}

		if( is_out_type< T5 >::value )
		{
			m_response << m_param5;
		}

		if( is_out_type< T6 >::value )
		{
			m_response << m_param6;
		}

		if( is_out_type< T7 >::value )
		{
			m_response << m_param7;
		}

		if( is_out_type< T8 >::value )
		{
			m_response << m_param8;
		}
	}

	virtual void run()
	{
		m_return = ( m_obj->*m_func )( m_param1, m_param2, m_param3, m_param4, m_param5, m_param6, m_param7, m_param8 );
	}

protected:
	class_type* m_obj;
	func_type   m_func;
	return_type m_return;
	param1_type m_param1;
	param2_type m_param2;
	param3_type m_param3;
	param4_type m_param4;
	param5_type m_param5;
	param6_type m_param6;
	param7_type m_param7;
	param8_type m_param8;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
template< typename C >
class command_impl< void, C > : public command
{
public:
	typedef void ( C::*func_type )( );
	typedef C class_type;

public:
	command_impl( message &request, message &response, class_type *obj, func_type func )
		: command( request, response ), m_obj( obj ), m_func( func )
	{
	}

private:
	command_impl( const command_impl &rhs ) = delete;
	command_impl& operator=( const command_impl &rhs ) = delete;

public:
	virtual void decode()
	{
	}

	virtual void encode()
	{
	}

	virtual void run()
	{
		( m_obj->*m_func )( );
	}

protected:
	class_type* m_obj;
	func_type   m_func;
};

template< typename C, typename T1 >
class command_impl< void, C, T1 > : public command
{
public:
	typedef void ( C::*func_type )( T1 );
	typedef C class_type;
	typedef typename type_traits< T1 >::type param1_type;

public:
	command_impl( message &request, message &response, class_type *obj, func_type func ) 
		: command( request, response ), m_obj( obj ), m_func( func )
	{
	}

private:
	command_impl( const command_impl &rhs ) = delete;
	command_impl& operator=( const command_impl &rhs ) = delete;

public:
	virtual void decode()
	{
		if( is_in_type< T1 >::value )
		{
			m_request >> m_param1;
		}
	}

	virtual void encode()
	{
		if( is_out_type< T1 >::value )
		{
			m_response << m_param1;
		}
	}

	virtual void run()
	{
		( m_obj->*m_func )( m_param1 );
	}

protected:
	class_type* m_obj;
	func_type   m_func;
	param1_type m_param1;
};

template< typename C, typename T1, typename T2 >
class command_impl< void, C, T1, T2 > : public command
{
public:
	typedef void( C::*func_type )( T1, T2 );
	typedef C class_type;
	typedef typename type_traits< T1 >::type param1_type;
	typedef typename type_traits< T2 >::type param2_type;

public:
	command_impl( message &request, message &response, class_type *obj, func_type func )
		: command( request, response ), m_obj( obj ), m_func( func )
	{
	}

private:
	command_impl( const command_impl &rhs ) = delete;
	command_impl& operator=( const command_impl &rhs ) = delete;

public:
	virtual void decode()
	{
		if( is_in_type< T1 >::value )
		{
			m_request >> m_param1;
		}

		if( is_in_type< T2 >::value )
		{
			m_request >> m_param2;
		}
	}

	virtual void encode()
	{
		if( is_out_type< T1 >::value )
		{
			m_response << m_param1;
		}

		if( is_out_type< T2 >::value )
		{
			m_response << m_param2;
		}
	}

	virtual void run()
	{
		( m_obj->*m_func )( m_param1, m_param2 );
	}

protected:
	class_type* m_obj;
	func_type   m_func;
	param1_type m_param1;
	param2_type m_param2;
};

template< typename C, typename T1, typename T2, typename T3 >
class command_impl< void, C, T1, T2, T3 > : public command
{
public:
	typedef void( C::*func_type )( T1, T2, T3 );
	typedef C class_type;
	typedef typename type_traits< T1 >::type param1_type;
	typedef typename type_traits< T2 >::type param2_type;
	typedef typename type_traits< T3 >::type param3_type;

public:
	command_impl( message &request, message &response, class_type *obj, func_type func )
		: command( request, response ), m_obj( obj ), m_func( func )
	{
	}

private:
	command_impl( const command_impl &rhs ) = delete;
	command_impl& operator=( const command_impl &rhs ) = delete;

public:
	virtual void decode()
	{
		if( is_in_type< T1 >::value )
		{
			m_request >> m_param1;
		}

		if( is_in_type< T2 >::value )
		{
			m_request >> m_param2;
		}

		if( is_in_type< T3 >::value )
		{
			m_request >> m_param3;
		}
	}

	virtual void encode()
	{
		if( is_out_type< T1 >::value )
		{
			m_response << m_param1;
		}

		if( is_out_type< T2 >::value )
		{
			m_response << m_param2;
		}

		if( is_out_type< T3 >::value )
		{
			m_response << m_param3;
		}
	}

	virtual void run()
	{
		( m_obj->*m_func )( m_param1, m_param2, m_param3 );
	}

protected:
	class_type* m_obj;
	func_type   m_func;
	param1_type m_param1;
	param2_type m_param2;
	param3_type m_param3;
};

template< typename C, typename T1, typename T2, typename T3, typename T4 >
class command_impl< void, C, T1, T2, T3, T4 > : public command
{
public:
	typedef void( C::*func_type )( T1, T2, T3, T4 );
	typedef C class_type;
	typedef typename type_traits< T1 >::type param1_type;
	typedef typename type_traits< T2 >::type param2_type;
	typedef typename type_traits< T3 >::type param3_type;
	typedef typename type_traits< T4 >::type param4_type;

public:
	command_impl( message &request, message &response, class_type *obj, func_type func )
		: command( request, response ), m_obj( obj ), m_func( func )
	{
	}

private:
	command_impl( const command_impl &rhs ) = delete;
	command_impl& operator=( const command_impl &rhs ) = delete;

public:
	virtual void decode()
	{
		if( is_in_type< T1 >::value )
		{
			m_request >> m_param1;
		}

		if( is_in_type< T2 >::value )
		{
			m_request >> m_param2;
		}

		if( is_in_type< T3 >::value )
		{
			m_request >> m_param3;
		}

		if( is_in_type< T4 >::value )
		{
			m_request >> m_param4;
		}
	}

	virtual void encode()
	{
		if( is_out_type< T1 >::value )
		{
			m_response << m_param1;
		}

		if( is_out_type< T2 >::value )
		{
			m_response << m_param2;
		}

		if( is_out_type< T3 >::value )
		{
			m_response << m_param3;
		}

		if( is_out_type< T4 >::value )
		{
			m_response << m_param4;
		}
	}

	virtual void run()
	{
		( m_obj->*m_func )( m_param1, m_param2, m_param3, m_param4 );
	}

protected:
	class_type* m_obj;
	func_type   m_func;
	param1_type m_param1;
	param2_type m_param2;
	param3_type m_param3;
	param4_type m_param4;
};

template< typename C, typename T1, typename T2, typename T3, typename T4, typename T5 >
class command_impl< void, C, T1, T2, T3, T4, T5 > : public command
{
public:
	typedef void( C::*func_type )( T1, T2, T3, T4, T5 );
	typedef C class_type;
	typedef typename type_traits< T1 >::type param1_type;
	typedef typename type_traits< T2 >::type param2_type;
	typedef typename type_traits< T3 >::type param3_type;
	typedef typename type_traits< T4 >::type param4_type;
	typedef typename type_traits< T5 >::type param5_type;

public:
	command_impl( message &request, message &response, class_type *obj, func_type func )
		: command( request, response ), m_obj( obj ), m_func( func )
	{
	}

private:
	command_impl( const command_impl &rhs ) = delete;
	command_impl& operator=( const command_impl &rhs ) = delete;

public:
	virtual void decode()
	{
		if( is_in_type< T1 >::value )
		{
			m_request >> m_param1;
		}

		if( is_in_type< T2 >::value )
		{
			m_request >> m_param2;
		}

		if( is_in_type< T3 >::value )
		{
			m_request >> m_param3;
		}

		if( is_in_type< T4 >::value )
		{
			m_request >> m_param4;
		}

		if( is_in_type< T5 >::value )
		{
			m_request >> m_param5;
		}
	}

	virtual void encode()
	{
		if( is_out_type< T1 >::value )
		{
			m_response << m_param1;
		}

		if( is_out_type< T2 >::value )
		{
			m_response << m_param2;
		}

		if( is_out_type< T3 >::value )
		{
			m_response << m_param3;
		}

		if( is_out_type< T4 >::value )
		{
			m_response << m_param4;
		}

		if( is_out_type< T5 >::value )
		{
			m_response << m_param5;
		}
	}

	virtual void run()
	{
		( m_obj->*m_func )( m_param1, m_param2, m_param3, m_param4, m_param5 );
	}

protected:
	class_type* m_obj;
	func_type   m_func;
	param1_type m_param1;
	param2_type m_param2;
	param3_type m_param3;
	param4_type m_param4;
	param5_type m_param5;
};

template< typename C, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6 >
class command_impl< void, C, T1, T2, T3, T4, T5, T6 > : public command
{
public:
	typedef void( C::*func_type )( T1, T2, T3, T4, T5, T6 );
	typedef C class_type;
	typedef typename type_traits< T1 >::type param1_type;
	typedef typename type_traits< T2 >::type param2_type;
	typedef typename type_traits< T3 >::type param3_type;
	typedef typename type_traits< T4 >::type param4_type;
	typedef typename type_traits< T5 >::type param5_type;
	typedef typename type_traits< T6 >::type param6_type;

public:
	command_impl( message &request, message &response, class_type *obj, func_type func )
		: command( request, response ), m_obj( obj ), m_func( func )
	{
	}

private:
	command_impl( const command_impl &rhs ) = delete;
	command_impl& operator=( const command_impl &rhs ) = delete;

public:
	virtual void decode()
	{
		if( is_in_type< T1 >::value )
		{
			m_request >> m_param1;
		}

		if( is_in_type< T2 >::value )
		{
			m_request >> m_param2;
		}

		if( is_in_type< T3 >::value )
		{
			m_request >> m_param3;
		}

		if( is_in_type< T4 >::value )
		{
			m_request >> m_param4;
		}

		if( is_in_type< T5 >::value )
		{
			m_request >> m_param5;
		}

		if( is_in_type< T6 >::value )
		{
			m_request >> m_param6;
		}
	}

	virtual void encode()
	{
		if( is_out_type< T1 >::value )
		{
			m_response << m_param1;
		}

		if( is_out_type< T2 >::value )
		{
			m_response << m_param2;
		}

		if( is_out_type< T3 >::value )
		{
			m_response << m_param3;
		}

		if( is_out_type< T4 >::value )
		{
			m_response << m_param4;
		}

		if( is_out_type< T5 >::value )
		{
			m_response << m_param5;
		}

		if( is_out_type< T6 >::value )
		{
			m_response << m_param6;
		}
	}

	virtual void run()
	{
		( m_obj->*m_func )( m_param1, m_param2, m_param3, m_param4, m_param5, m_param6 );
	}

protected:
	class_type* m_obj;
	func_type   m_func;
	param1_type m_param1;
	param2_type m_param2;
	param3_type m_param3;
	param4_type m_param4;
	param5_type m_param5;
	param6_type m_param6;
};

template< typename C, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7 >
class command_impl< void, C, T1, T2, T3, T4, T5, T6, T7 > : public command
{
public:
	typedef void( C::*func_type )( T1, T2, T3, T4, T5, T6, T7 );
	typedef C class_type;
	typedef typename type_traits< T1 >::type param1_type;
	typedef typename type_traits< T2 >::type param2_type;
	typedef typename type_traits< T3 >::type param3_type;
	typedef typename type_traits< T4 >::type param4_type;
	typedef typename type_traits< T5 >::type param5_type;
	typedef typename type_traits< T6 >::type param6_type;
	typedef typename type_traits< T7 >::type param7_type;

public:
	command_impl( message &request, message &response, class_type *obj, func_type func )
		: command( request, response ), m_obj( obj ), m_func( func )
	{
	}

private:
	command_impl( const command_impl &rhs ) = delete;
	command_impl& operator=( const command_impl &rhs ) = delete;

public:
	virtual void decode()
	{
		if( is_in_type< T1 >::value )
		{
			m_request >> m_param1;
		}

		if( is_in_type< T2 >::value )
		{
			m_request >> m_param2;
		}

		if( is_in_type< T3 >::value )
		{
			m_request >> m_param3;
		}

		if( is_in_type< T4 >::value )
		{
			m_request >> m_param4;
		}

		if( is_in_type< T5 >::value )
		{
			m_request >> m_param5;
		}

		if( is_in_type< T6 >::value )
		{
			m_request >> m_param6;
		}

		if( is_in_type< T7 >::value )
		{
			m_request >> m_param7;
		}
	}

	virtual void encode()
	{
		if( is_out_type< T1 >::value )
		{
			m_response << m_param1;
		}

		if( is_out_type< T2 >::value )
		{
			m_response << m_param2;
		}

		if( is_out_type< T3 >::value )
		{
			m_response << m_param3;
		}

		if( is_out_type< T4 >::value )
		{
			m_response << m_param4;
		}

		if( is_out_type< T5 >::value )
		{
			m_response << m_param5;
		}

		if( is_out_type< T6 >::value )
		{
			m_response << m_param6;
		}

		if( is_out_type< T7 >::value )
		{
			m_response << m_param7;
		}
	}

	virtual void run()
	{
		( m_obj->*m_func )( m_param1, m_param2, m_param3, m_param4, m_param5, m_param6, m_param7 );
	}

protected:
	class_type* m_obj;
	func_type   m_func;
	param1_type m_param1;
	param2_type m_param2;
	param3_type m_param3;
	param4_type m_param4;
	param5_type m_param5;
	param6_type m_param6;
	param7_type m_param7;
};

template< typename C, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8 >
class command_impl< void, C, T1, T2, T3, T4, T5, T6, T7, T8 > : public command
{
public:
	typedef void( C::*func_type )( T1, T2, T3, T4, T5, T6, T7, T8 );
	typedef C class_type;
	typedef typename type_traits< T1 >::type param1_type;
	typedef typename type_traits< T2 >::type param2_type;
	typedef typename type_traits< T3 >::type param3_type;
	typedef typename type_traits< T4 >::type param4_type;
	typedef typename type_traits< T5 >::type param5_type;
	typedef typename type_traits< T6 >::type param6_type;
	typedef typename type_traits< T7 >::type param7_type;
	typedef typename type_traits< T8 >::type param8_type;

public:
	command_impl( message &request, message &response, class_type *obj, func_type func )
		: command( request, response ), m_obj( obj ), m_func( func )
	{
	}

private:
	command_impl( const command_impl &rhs ) = delete;
	command_impl& operator=( const command_impl &rhs ) = delete;

public:
	virtual void decode()
	{
		if( is_in_type< T1 >::value )
		{
			m_request >> m_param1;
		}

		if( is_in_type< T2 >::value )
		{
			m_request >> m_param2;
		}

		if( is_in_type< T3 >::value )
		{
			m_request >> m_param3;
		}

		if( is_in_type< T4 >::value )
		{
			m_request >> m_param4;
		}

		if( is_in_type< T5 >::value )
		{
			m_request >> m_param5;
		}

		if( is_in_type< T6 >::value )
		{
			m_request >> m_param6;
		}

		if( is_in_type< T7 >::value )
		{
			m_request >> m_param7;
		}

		if( is_in_type< T8 >::value )
		{
			m_request >> m_param8;
		}
	}

	virtual void encode()
	{
		if( is_out_type< T1 >::value )
		{
			m_response << m_param1;
		}

		if( is_out_type< T2 >::value )
		{
			m_response << m_param2;
		}

		if( is_out_type< T3 >::value )
		{
			m_response << m_param3;
		}

		if( is_out_type< T4 >::value )
		{
			m_response << m_param4;
		}

		if( is_out_type< T5 >::value )
		{
			m_response << m_param5;
		}

		if( is_out_type< T6 >::value )
		{
			m_response << m_param6;
		}

		if( is_out_type< T7 >::value )
		{
			m_response << m_param7;
		}

		if( is_out_type< T8 >::value )
		{
			m_response << m_param8;
		}
	}

	virtual void run()
	{
		( m_obj->*m_func )( m_param1, m_param2, m_param3, m_param4, m_param5, m_param6, m_param7, m_param8 );
	}

protected:
	class_type* m_obj;
	func_type   m_func;
	param1_type m_param1;
	param2_type m_param2;
	param3_type m_param3;
	param4_type m_param4;
	param5_type m_param5;
	param6_type m_param6;
	param7_type m_param7;
	param8_type m_param8;
};
//////////////////////////////////////////////////////////////////////////
} // namespace rmi
//////////////////////////////////////////////////////////////////////////
#endif // WHL_RMI_HPP_

