 ////////////////////////////////////////////////////////////////////////////////////////////////////
///	
///	@file		Typedefs.h	
///	@brief		Defining types and macros according to specific compiler
///	
///	@internal
///	@author		Shlomi Mor
///	@date		01/10/2006
///	@version	version 0.1
///
///		Copyright (C) 2006 DSP GROUP, INC .   All Rights Reserved
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _TYPEDEFS_H
#define _TYPEDEFS_H

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <limits.h>

///////////////////////////////////////////////////////////////////////////////
// DEBUG
///////////////////////////////////////////////////////////////////////////////

///
/// @brief endianess definitions
///
#define HOST_BIG_ENDIAN		0
#define HOST_LITTLE_ENDIAN	1
#define HOST_ENDIAN			HOST_LITTLE_ENDIAN

///
/// @brief MIN, MAX and RANGE macros
///
#ifndef MIN
#define MIN(a, b)	( (a) < (b) ? (a) : (b) )
#endif // MIN

#ifndef MAX
#define MAX(a, b)	( (a) > (b) ? (a) : (b) )
#endif // MAX

#ifndef RANGE
#define RANGE(min, x, max)	( (((min) > (x)) || ((x) > (max))) ? false : true )
#endif // RANGE

///
/// @brief IN, OUT and INOUT macros
///
#ifndef IN
	#define IN
#endif // IN

#ifndef OUT
	#define OUT
#endif // OUT

#ifndef INOUT
	#define INOUT
#endif // INOUT


#define NORETURN

#define RAM_FUNC
#define FAST_FUNC
#define VERY_FAST_FUNC
#define END_FUNC




///
/// @brief Obsolete PACKED macro. Use PACKED_STRUCT instead (see below)
///
#ifndef PACKED
	#define PACKED "Error: Use PACKED_STRUCT instead!!!"
#endif // PACKED

///
///	@brief	Macro to be used when decalring "packed" struct
///
///	Use this macro when you want to declare a "packed" struct. Usage:
///		typedef PACKED_STRUCT	_MyPackedStruct 
///		{
///			uint32 FourBytes;
///			uint16 Twobytes;
///			uint8  Onebyte;
///		} RESTORE_PACK(MyPackedStruct);
///
///	@note	Windows is not supported. If you want to use a "packed" struct
/// 		in a code that should be compiled for WIN32, take care of it 
///			in the code. Example:
///	
///		#ifdef WIN32
///			#pragma pack(1)
///		#endif
///		typedef PACKED_STRUCT	_MyPackedStruct 
///		{
///			uint32 FourBytes;
///			uint16 Twobytes;
///			uint8  Onebyte;
///		} RESTORE_PACK(MyPackedStruct);
///		#ifdef WIN32
///			#pragma pack()
///		#endif
/// 
#ifndef PACKED_STRUCT
	#if defined(WIN32)
		#define PACKED_STRUCT struct
	#elif defined(__GNUC__)
		#define PACKED_STRUCT struct __attribute__((packed))
	#elif defined(__CC_ARM)	// ARM_ADS
		#define PACKED_STRUCT __packed struct
	#elif defined(__ICCARM__) // ARM_IAR
		#define PACKED_STRUCT _Pragma("pack(push,1)") struct
	#endif
#endif // PACKED_STRUCT

///
///	@brief	PACKED_UNION macro. To be used when decalring "packed" union (same as for packed struct)
///
#ifndef PACKED_UNION
	#if defined(WIN32)
		#define PACKED_UNION union
	#elif defined(__GNUC__)
		#define PACKED_UNION union __attribute__((packed))
	#elif defined(__CC_ARM)	// ARM_ADS
		#define PACKED_UNION __packed union
	#elif defined(__ICCARM__) // ARM_IAR
		#define PACKED_UNION _Pragma("pack(push,1)") union
	#endif
#endif // PACKED_STRUCT

#ifndef RESTORE_PACK
	#if defined(WIN32)
		#define RESTORE_PACK(Name) Name
	#elif defined(__GNUC__)
		#define RESTORE_PACK(Name) Name
	#elif defined(__CC_ARM)	// ARM_ADS
		#define RESTORE_PACK(Name) Name
	#elif defined(__ICCARM__) // ARM_IAR
		#define RESTORE_PACK(Name) _Pragma("pack(pop)") Name
	#endif
#endif

///
///	@brief	DRAG_INTO_LINKAGE macro. To be used in order to force the linker inserting a symbol
/// @n		into the executable. Usually used for ISR functions (Interrupt Service Routine)
///
#define DRAG_INTO_LINKAGE(Symbol)					\
{													\
	volatile int DragIntoLinkage = (int)(Symbol);	\
	(void)DragIntoLinkage;							\
}



///
///	@brief		Size in bytes
///
#define SIZEOF_B(x)		( sizeof(x) )

///
///	@brief		Size in chars
///
#define SIZEOF_C(x)		( sizeof(x) )

///
///	@brief		Size in words
///
#define SIZEOF_W(x)		( SIZEOF_C(x) / SIZEOF_C(uint16) )

///
///	@brief	ARGUSED macro. Use in order to prevent compiler warning because of unused function parameter.
///
#define ARGUSED(x) (void)(x)

///
///	@brief	LENGTHOF macro. Use in order to get number of items in array.
///
#define LENGTHOF(x) (sizeof(x) / sizeof((x)[0]))

///
///	@brief	SIZE_IN_BYTES macro. Use in order to get the size of a variable/type in bytes.
///
#define SIZE_IN_BYTES(x) (sizeof(x)*CHAR_BIT/8)

///
///	@brief	ROUND_UP macros
///
#define ROUND_UP(x,n) 			( ( (uint32)(x) + (uint32)(n) - 1 ) & ( ~((n)-1)) )
#define ROUND_UP_TO_TYPE(x,n)	( ( (uint32)(x) + SIZE_IN_BYTES(n) - 1) & ~(SIZE_IN_BYTES(n)-1) )

///
///	@brief	ROUND_DOWN macros
///
#define ROUND_DOWN(x,n) 		( (uint32)(x) & ~((n)-1) )
#define ROUND_DOWN_TO_TYPE(x,n)	( (uint32)(x)  & ~(SIZE_IN_BYTES(n)-1) )

///
///	@brief	FIELD_MASK macro. Use in order to create a mask of bits in which bits Low..High are set.
///
#define FIELD_MASK( Low, High ) ( ((1 << ((High)-(Low)+1)) - 1) << (Low) )

///
///	@brief	OFFSET_OF macro. Use in order to get the offset of a field in a struct. Should not be
/// @n		used on packed structs.
///
#define OFFSET_OF( StructName, FieldName ) \
	((int)(&(((StructName*)0)->FieldName)))

///
///	@brief	UPWARD_CAST macro. Use in order to implement C++ using C.
///
#define UPWARD_CAST( BaseClassName, DerivedClassName, BaseClassMemberName, BaseClassPtr ) \
	((DerivedClassName*)(((int)BaseClassPtr) - OFFSET_OF( DerivedClassName, BaseClassMemberName )))	

///
///	@brief	Kb macro. Use to specify 1024 multiplier.
///
#define Kb	*1024

///
/// @brief	Integer types. Portable code MUST use the types below, and not the compiler internal types,
/// @n		since they might have different sizes when using different compilers/processors.
///
//#ifdef __LINUX__

#ifndef _TYPEDEFS_NORMAL
#define _TYPEDEFS_NORMAL
typedef signed char			int8;
typedef signed short			int16;
typedef signed long			int32;
#endif

typedef unsigned char			uint8;
typedef unsigned short			uint16;
typedef unsigned long			uint32;
typedef unsigned long long		uint64;

# ifndef __BIONIC__
typedef unsigned int			uint;
#endif // __BIONIC__
								
typedef signed char				sint8;
typedef signed short			sint16;
typedef signed long				sint32;
typedef signed long long		sint64;
typedef signed int				sint;

typedef signed long long		int64;

//typedef uint16 					bool_type;


#if 0
/// Define a 128 bit type
#if defined(__cplusplus)

PACKED_STRUCT uint128
{ 
	// IAR comiler supplies the default behaviour like this (binary operations).

	bool operator==(const uint128& other) const {
		for (unsigned int i = 0; i < sizeof(octets) / sizeof(uint8); i++)
			if (octets[i] != other.octets[i])
				return false;
		return true;
	}

	bool operator!=(const uint128& other) const {
		return !(*this == other);
	}

	/// Assignment
	uint128& operator=(const uint128 &rhs) {
		for (unsigned int i = 0; i < sizeof(octets) / sizeof(uint8); i++)
			octets[i] = rhs.octets[i];
		return *this;
	}

	uint128& operator=(uint8 initializer) {
		for (unsigned int i = 0; i < sizeof(octets) / sizeof(uint8); i++)
			octets[i] = initializer;
		return *this;
	}

	/// Conversion operator
	operator uint8* () { return &octets[0]; } 

	operator const uint8* () const { return &octets[0]; } 

	uint8 octets[16]; 

} RESTORE_PACK();

#else

typedef PACKED_STRUCT { uint8 octets[16]; } RESTORE_PACK(uint128);

#endif  /// __cplusplus
#endif


///
/// @brief	Boolean types
///
#ifndef true
#if !defined(__cplusplus)
	#if defined(WIN32)
		#if _MSC_VER >= 1310	// VC .NET 2003
		//typedef int32 bool_type;
		typedef signed char bool_type;
		#else
		typedef signed char bool_type;
		#endif
	#elif defined(__LINUX__)
		typedef signed char bool_type;
	#elif defined(__linux__)
		typedef signed char bool_type;
	#elif defined(__CC_ARM)
		typedef int32 bool_type;
	#elif defined(__ICCARM__)
		typedef uint8 bool_type;
	#endif

#ifndef TEAKLITE
	#define true	(1)
	#define false	(0)
#endif	
	#define bool	unsigned char
#endif	//	!defined(__cplusplus)
#endif

///
/// @brief	NULL definition
///
#ifndef NULL
	#define NULL 	0
#endif // ifndef NULL

///
/// @brief	Macros for REGISTERS / FIELDS definitions
///


typedef volatile uint8* Reg8Ptr;
typedef volatile uint16* Reg16Ptr;
typedef volatile uint32* Reg32Ptr;


///
/// @brief	DECLARE_OPAQUE macro definition. Use in order to export a (void*) type with compiler's
/// @n		type checking support.
///
#ifndef DECLARE_OPAQUE
#define DECLARE_OPAQUE(name) typedef struct name##__ __##name##__; \
typedef __##name##__ * name
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // _TYPEDEFS_H

