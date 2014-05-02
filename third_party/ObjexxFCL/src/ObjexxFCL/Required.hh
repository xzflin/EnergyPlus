#ifndef ObjexxFCL_Required_hh_INCLUDED
#define ObjexxFCL_Required_hh_INCLUDED

// Required Argument Wrapper
//
// Project: Objexx Fortran Compatibility Library (ObjexxFCL)
//
// Version: 4.0.0
//
// Language: C++
//
// Copyright (c) 2000-2014 Objexx Engineering, Inc. All Rights Reserved.
// Use of this source code or any derivative of it is restricted by license.
// Licensing is available from Objexx Engineering, Inc.:  http://objexx.com

// ObjexxFCL Headers
#include <ObjexxFCL/Required.fwd.hh>
#include <ObjexxFCL/Omit.hh>

// C++ Headers
#include <cassert>
#include <type_traits>

namespace ObjexxFCL {

// Required Argument Wrapper
template< typename T, typename Enable >
class Required
{

private: // Friend

	template< typename, typename > friend class Required;

public: // Types

	typedef  T  Value;

public: // Creation

	// Default Constructor
	inline
	Required() :
		ptr_( nullptr ),
		own_( false )
	{
		assert( false ); // Required object not present
	}

	// Copy Constructor
	inline
	Required( Required const & r ) :
		ptr_( r.own_ ? new Value( r() ) : r.ptr_ ),
		own_( r.own_ )
	{
		assert( ptr_ != nullptr ); // Required object must be present
	}

	// Required Constructor Template
	template< typename U >
	inline
	Required( Required< U, Enable > const & o, typename std::enable_if< std::is_const< T >::value && std::is_same< U, typename std::remove_const< T >::type >::value >::type * = 0 ) :
		ptr_( o.own_ ? new Value( o() ) : o.ptr_ ),
		own_( o.own_ )
	{
		assert( ptr_ != nullptr ); // Required object must be present
	}

	// Value Constructor
	inline
	Required( Value const & val ) :
		ptr_( const_cast< Value * >( &val ) ),
		own_( false )
	{}

	// Value Constructor Template
	template< typename U >
	inline
	Required( U const & val ) :
		ptr_( new Value( val ) ), // Requires Value( U ) constructor
		own_( true )
	{}

	// rvalue Constructor
	inline
	Required( Value && val ) :
		ptr_( new Value( val ) ), // Requires Value copy constructor
		own_( true )
	{}

	// Omit Constructor
	inline
	Required( Omit ) :
		ptr_( nullptr ),
		own_( false )
	{
		assert( false ); // Required object not present
	}

	// Destructor
	inline
	~Required()
	{
		if ( own_ ) delete ptr_;
	}

public: // Assignment

	// Copy Assignment
	inline
	Required &
	operator =( Required const & r )
	{
		if ( own_ ) delete ptr_;
		ptr_ = r.own_ ? new Value( r() ) : r.ptr_;
		assert( ptr_ != nullptr ); // Required object must be present
		own_ = r.own_;
		return *this;
	}

	// Value Assignment
	inline
	Required &
	operator =( Value const & val )
	{
		assert( ptr_ != nullptr );
		*ptr_ = val;
		return *this;
	}

	// Value Assignment Template
	template< typename U >
	inline
	Required &
	operator =( U const & val )
	{
		assert( ptr_ != nullptr );
		*ptr_ = val;
		return *this;
	}

	// rvalue Assignment
	inline
	Required &
	operator =( Value && val )
	{
		assert( ptr_ != nullptr );
		*ptr_ = val;
		return *this;
	}

public: // Conversion

	// Value Conversion
	inline
	operator Value const &() const
	{
		assert( ptr_ != nullptr );
		return *ptr_;
	}

	// Value Conversion
	inline
	operator Value &()
	{
		assert( ptr_ != nullptr );
		return *ptr_;
	}

public: // Operators

	// Value
	inline
	Value const &
	operator ()() const
	{
		assert( ptr_ != nullptr );
		return *ptr_;
	}

	// Value
	inline
	Value &
	operator ()()
	{
		assert( ptr_ != nullptr );
		return *ptr_;
	}

public: // Properties

	// Present?
	inline
	bool
	present() const
	{
		return ( ptr_ != nullptr );
	}

	// Own?
	inline
	bool
	own() const
	{
		return own_;
	}

public: // Comparison

	// Required == Required
	inline
	friend
	bool
	operator ==( Required const & a, Required const & b )
	{
		return ( ( a.ptr_ != nullptr ) && ( b.ptr_ != nullptr ) ? ( *a.ptr_ == *b.ptr_ ) : ( a.ptr_ == b.ptr_ ) );
	}

	// Required != Required
	inline
	friend
	bool
	operator !=( Required const & a, Required const & b )
	{
		return !( a == b );
	}

	// Required == Value
	inline
	friend
	bool
	operator ==( Required const & a, Value const & b )
	{
		return ( ( a.ptr_ != nullptr ) && ( *a.ptr_ == b ) );
	}

	// Required != Value
	inline
	friend
	bool
	operator !=( Required const & a, Value const & b )
	{
		return !( a == b );
	}

	// Value == Required
	inline
	friend
	bool
	operator ==( Value const & a, Required const & b )
	{
		return ( ( b.ptr_ != nullptr ) && ( a == *b.ptr_ ) );
	}

	// Value != Required
	inline
	friend
	bool
	operator !=( Value const & a, Required const & b )
	{
		return !( a == b );
	}

private: // Data

	Value * ptr_; // Pointer to object
	bool own_; // Own the object?

}; // Required

// Required Argument Wrapper: Abstract Type Specialization
template< typename T >
class Required< T, typename std::enable_if< std::is_abstract< T >::value >::type >
{

private: // Friend

	template< typename, typename > friend class Required;

public: // Types

	typedef  T  Value;
	typedef  typename std::enable_if< std::is_abstract< T >::value >::type  EnableType;

public: // Creation

	// Default Constructor
	inline
	Required() :
		ptr_( nullptr )
	{
		assert( false ); // Required object not present
	}

	// Copy Constructor
	inline
	Required( Required const & r ) :
		ptr_( r.ptr_ )
	{
		assert( ptr_ != nullptr ); // Required object must be present
	}

	// Required Constructor Template
	template< typename U >
	inline
	Required( Required< U, EnableType > const & o, typename std::enable_if< std::is_const< T >::value && std::is_same< U, typename std::remove_const< T >::type >::value >::type * = 0 ) :
		ptr_( o.ptr_ )
	{
		assert( ptr_ != nullptr ); // Required object must be present
	}

	// Value Constructor
	inline
	Required( Value const & val ) :
		ptr_( const_cast< Value * >( &val ) )
	{}

	// Omit Constructor
	inline
	Required( Omit ) :
		ptr_( nullptr )
	{
		assert( false ); // Required object not present
	}

	// Destructor
	inline
	~Required()
	{}

public: // Assignment

	// Copy Assignment
	inline
	Required &
	operator =( Required const & r )
	{
		ptr_ = r.ptr_;
		assert( ptr_ != nullptr ); // Required object must be present
		return *this;
	}

	// Value Assignment
	inline
	Required &
	operator =( Value const & val )
	{
		assert( ptr_ != nullptr );
		*ptr_ = val;
		return *this;
	}

	// Value Assignment Template
	template< typename U >
	inline
	Required &
	operator =( U const & val )
	{
		assert( ptr_ != nullptr );
		*ptr_ = val;
		return *this;
	}

	// rvalue Assignment
	inline
	Required &
	operator =( Value && val )
	{
		assert( ptr_ != nullptr );
		*ptr_ = val;
		return *this;
	}

public: // Conversion

	// Value Conversion
	inline
	operator Value const &() const
	{
		assert( ptr_ != nullptr );
		return *ptr_;
	}

	// Value Conversion
	inline
	operator Value &()
	{
		assert( ptr_ != nullptr );
		return *ptr_;
	}

public: // Operators

	// Value
	inline
	Value const &
	operator ()() const
	{
		assert( ptr_ != nullptr );
		return *ptr_;
	}

	// Value
	inline
	Value &
	operator ()()
	{
		assert( ptr_ != nullptr );
		return *ptr_;
	}

public: // Properties

	// Present?
	inline
	bool
	present() const
	{
		return ( ptr_ != nullptr );
	}

public: // Comparison

	// Required == Required
	inline
	friend
	bool
	operator ==( Required const & a, Required const & b )
	{
		return ( ( a.ptr_ != nullptr ) && ( b.ptr_ != nullptr ) ? ( *a.ptr_ == *b.ptr_ ) : ( a.ptr_ == b.ptr_ ) );
	}

	// Required != Required
	inline
	friend
	bool
	operator !=( Required const & a, Required const & b )
	{
		return !( a == b );
	}

	// Required == Value
	inline
	friend
	bool
	operator ==( Required const & a, Value const & b )
	{
		return ( ( a.ptr_ != nullptr ) && ( *a.ptr_ == b ) );
	}

	// Required != Value
	inline
	friend
	bool
	operator !=( Required const & a, Value const & b )
	{
		return !( a == b );
	}

	// Value == Required
	inline
	friend
	bool
	operator ==( Value const & a, Required const & b )
	{
		return ( ( b.ptr_ != nullptr ) && ( a == *b.ptr_ ) );
	}

	// Value != Required
	inline
	friend
	bool
	operator !=( Value const & a, Required const & b )
	{
		return !( a == b );
	}

private: // Data

	Value * ptr_; // Pointer

}; // Required

// Argument Present?
template< typename T >
inline
bool
present( Required< T > const & r )
{
	return r.present();
}

// Argument Present?
template< typename T >
inline
bool
PRESENT( Required< T > const & r )
{
	return r.present();
}

} // ObjexxFCL

#endif // ObjexxFCL_Required_hh_INCLUDED
