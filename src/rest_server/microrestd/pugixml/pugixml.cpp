// Modified by Milan Straka <straka@ufal.mff.cuni.cz>
// Changes: - added ufal::microrestd namespace
//          - matching changed to ignore XML namespaces
//          - XPATH and STL module completely removed

/**
 * pugixml parser - version 1.5
 * --------------------------------------------------------
 * Copyright (C) 2006-2014, by Arseny Kapoulkine (arseny.kapoulkine@gmail.com)
 * Report bugs and download new versions at http://pugixml.org/
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 *
 * This work is based on the pugxml parser, which is:
 * Copyright (C) 2003, by Kristen Wegner (kristen@tima.net)
 */

#ifndef SOURCE_PUGIXML_CPP
#define SOURCE_PUGIXML_CPP

#include "pugixml.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#ifdef PUGIXML_WCHAR_MODE
#	include <wchar.h>
#endif

#ifndef PUGIXML_NO_XPATH
// Removed the XPATH and STL module, by Milan Straka
#endif

#ifndef PUGIXML_NO_STL
// Removed the XPATH and STL module, by Milan Straka
#endif

// For placement new
#include <new>

#ifdef _MSC_VER
#	pragma warning(push)
#	pragma warning(disable: 4127) // conditional expression is constant
#	pragma warning(disable: 4324) // structure was padded due to __declspec(align())
#	pragma warning(disable: 4611) // interaction between '_setjmp' and C++ object destruction is non-portable
#	pragma warning(disable: 4702) // unreachable code
#	pragma warning(disable: 4996) // this function or variable may be unsafe
#	pragma warning(disable: 4793) // function compiled as native: presence of '_setjmp' makes a function unmanaged
#endif

#ifdef __INTEL_COMPILER
#	pragma warning(disable: 177) // function was declared but never referenced 
#	pragma warning(disable: 279) // controlling expression is constant
#	pragma warning(disable: 1478 1786) // function was declared "deprecated"
#	pragma warning(disable: 1684) // conversion from pointer to same-sized integral type
#endif

#if defined(__BORLANDC__) && defined(PUGIXML_HEADER_ONLY)
#	pragma warn -8080 // symbol is declared but never used; disabling this inside push/pop bracket does not make the warning go away
#endif

#ifdef __BORLANDC__
#	pragma option push
#	pragma warn -8008 // condition is always false
#	pragma warn -8066 // unreachable code
#endif

#ifdef __SNC__
// Using diag_push/diag_pop does not disable the warnings inside templates due to a compiler bug
#	pragma diag_suppress=178 // function was declared but never referenced
#	pragma diag_suppress=237 // controlling expression is constant
#endif

// Inlining controls
#if defined(_MSC_VER) && _MSC_VER >= 1300
#	define PUGI__NO_INLINE __declspec(noinline)
#elif defined(__GNUC__)
#	define PUGI__NO_INLINE __attribute__((noinline))
#else
#	define PUGI__NO_INLINE 
#endif

// Branch weight controls
#if defined(__GNUC__)
#	define PUGI__UNLIKELY(cond) __builtin_expect(cond, 0)
#else
#	define PUGI__UNLIKELY(cond) (cond)
#endif

// Simple static assertion
#define PUGI__STATIC_ASSERT(cond) { static const char condition_failed[(cond) ? 1 : -1] = {0}; (void)condition_failed[0]; }

// Digital Mars C++ bug workaround for passing char loaded from memory via stack
#ifdef __DMC__
#	define PUGI__DMC_VOLATILE volatile
#else
#	define PUGI__DMC_VOLATILE
#endif

// Borland C++ bug workaround for not defining ::memcpy depending on header include order (can't always use std::memcpy because some compilers don't have it at all)
#if defined(__BORLANDC__) && !defined(__MEM_H_USING_LIST)
using std::memcpy;
using std::memmove;
#endif

// In some environments MSVC is a compiler but the CRT lacks certain MSVC-specific features
#if defined(_MSC_VER) && !defined(__S3E__)
#	define PUGI__MSVC_CRT_VERSION _MSC_VER
#endif

#ifdef PUGIXML_HEADER_ONLY
#	define PUGI__NS_BEGIN namespace ufal { namespace microrestd { namespace pugi { namespace impl {
#	define PUGI__NS_END } } } }
#	define PUGI__FN inline
#	define PUGI__FN_NO_INLINE inline
#else
#	if defined(_MSC_VER) && _MSC_VER < 1300 // MSVC6 seems to have an amusing bug with anonymous namespaces inside namespaces
#		define PUGI__NS_BEGIN namespace ufal { namespace microrestd { namespace pugi { namespace impl {
#		define PUGI__NS_END } } } }
#	else
#		define PUGI__NS_BEGIN namespace ufal { namespace microrestd { namespace pugi { namespace impl { namespace {
#		define PUGI__NS_END } } } } }
#	endif
#	define PUGI__FN
#	define PUGI__FN_NO_INLINE PUGI__NO_INLINE
#endif

// uintptr_t
#if !defined(_MSC_VER) || _MSC_VER >= 1600
#	include <stdint.h>
#else
#	ifndef _UINTPTR_T_DEFINED
// No native uintptr_t in MSVC6 and in some WinCE versions
typedef size_t uintptr_t;
#define _UINTPTR_T_DEFINED
#	endif
PUGI__NS_BEGIN
	typedef unsigned __int8 uint8_t;
	typedef unsigned __int16 uint16_t;
	typedef unsigned __int32 uint32_t;
PUGI__NS_END
#endif

// Memory allocation
PUGI__NS_BEGIN
	PUGI__FN void* default_allocate(size_t size)
	{
		return malloc(size);
	}

	PUGI__FN void default_deallocate(void* ptr)
	{
		free(ptr);
	}

	template <typename T>
	struct xml_memory_management_function_storage
	{
		static allocation_function allocate;
		static deallocation_function deallocate;
	};

	// Global allocation functions are stored in class statics so that in header mode linker deduplicates them
	// Without a template<> we'll get multiple definitions of the same static
	template <typename T> allocation_function xml_memory_management_function_storage<T>::allocate = default_allocate;
	template <typename T> deallocation_function xml_memory_management_function_storage<T>::deallocate = default_deallocate;

	typedef xml_memory_management_function_storage<int> xml_memory;
PUGI__NS_END

// String utilities
PUGI__NS_BEGIN
	// Get string length
	PUGI__FN size_t strlength(const char_t* s)
	{
		assert(s);

	#ifdef PUGIXML_WCHAR_MODE
		return wcslen(s);
	#else
		return strlen(s);
	#endif
	}

	// Compare two strings
	PUGI__FN bool strequal(const char_t* src, const char_t* dst)
	{
		assert(src && dst);

		const char_t* colon; // Ignore XML namespaces, by Milan Straka.
	#ifdef PUGIXML_WCHAR_MODE
		if ((colon = wcschr(src, ':'))) src = colon; // Ignore XML namespaces, by Milan Straka.
		if ((colon = wcschr(dst, ':'))) dst = colon; // Ignore XML namespaces, by Milan Straka.
		return wcscmp(src, dst) == 0;
	#else
		if ((colon = strchr(src, ':'))) src = colon; // Ignore XML namespaces, by Milan Straka.
		if ((colon = strchr(dst, ':'))) dst = colon; // Ignore XML namespaces, by Milan Straka.
		return strcmp(src, dst) == 0;
	#endif
	}

	// Compare lhs with [rhs_begin, rhs_end)
	PUGI__FN bool strequalrange(const char_t* lhs, const char_t* rhs, size_t count)
	{
		for (size_t i = 0; i < count; ++i)
			if (lhs[i] != rhs[i])
				return false;
	
		return lhs[count] == 0;
	}

// Define the following functions only when using them, by Milan Straka
#if defined(PUGI__MSVC_CRT_VERSION) || defined(__BORLANDC__) || (defined(__MINGW32__) && (!defined(__STRICT_ANSI__) || defined(__MINGW64_VERSION_MAJOR)))
#else
	// Get length of wide string, even if CRT lacks wide character support
	PUGI__FN size_t strlength_wide(const wchar_t* s)
	{
		assert(s);

	#ifdef PUGIXML_WCHAR_MODE
		return wcslen(s);
	#else
		const wchar_t* end = s;
		while (*end) end++;
		return static_cast<size_t>(end - s);
	#endif
	}
#endif

#ifdef PUGIXML_WCHAR_MODE
	// Convert string to wide string, assuming all symbols are ASCII
	PUGI__FN void widen_ascii(wchar_t* dest, const char* source)
	{
		for (const char* i = source; *i; ++i) *dest++ = *i;
		*dest = 0;
	}
#endif
PUGI__NS_END

#if !defined(PUGIXML_NO_STL) || !defined(PUGIXML_NO_XPATH)
// Removed the XPATH and STL module, by Milan Straka
#endif

PUGI__NS_BEGIN
	static const size_t xml_memory_page_size =
	#ifdef PUGIXML_MEMORY_PAGE_SIZE
		PUGIXML_MEMORY_PAGE_SIZE
	#else
		32768
	#endif
		;

	static const uintptr_t xml_memory_page_alignment = 64;
	static const uintptr_t xml_memory_page_pointer_mask = ~(xml_memory_page_alignment - 1);
	static const uintptr_t xml_memory_page_contents_shared_mask = 32;
	static const uintptr_t xml_memory_page_name_allocated_mask = 16;
	static const uintptr_t xml_memory_page_value_allocated_mask = 8;
	static const uintptr_t xml_memory_page_type_mask = 7;
	static const uintptr_t xml_memory_page_name_allocated_or_shared_mask = xml_memory_page_name_allocated_mask | xml_memory_page_contents_shared_mask;
	static const uintptr_t xml_memory_page_value_allocated_or_shared_mask = xml_memory_page_value_allocated_mask | xml_memory_page_contents_shared_mask;

	#define PUGI__NODETYPE(n) static_cast<xml_node_type>(((n)->header & impl::xml_memory_page_type_mask) + 1)

	struct xml_allocator;

	struct xml_memory_page
	{
		static xml_memory_page* construct(void* memory)
		{
			xml_memory_page* result = static_cast<xml_memory_page*>(memory);

			result->allocator = 0;
			result->prev = 0;
			result->next = 0;
			result->busy_size = 0;
			result->freed_size = 0;

			return result;
		}

		xml_allocator* allocator;

		xml_memory_page* prev;
		xml_memory_page* next;

		size_t busy_size;
		size_t freed_size;
	};

	struct xml_memory_string_header
	{
		uint16_t page_offset; // offset from page->data
		uint16_t full_size; // 0 if string occupies whole page
	};

	struct xml_allocator
	{
		xml_allocator(xml_memory_page* root): _root(root), _busy_size(root->busy_size)
		{
		}

		xml_memory_page* allocate_page(size_t data_size)
		{
			size_t size = sizeof(xml_memory_page) + data_size;

			// allocate block with some alignment, leaving memory for worst-case padding
			void* memory = xml_memory::allocate(size + xml_memory_page_alignment);
			if (!memory) return 0;

			// align to next page boundary (note: this guarantees at least 1 usable byte before the page)
			char* page_memory = reinterpret_cast<char*>((reinterpret_cast<uintptr_t>(memory) + xml_memory_page_alignment) & ~(xml_memory_page_alignment - 1));

			// prepare page structure
			xml_memory_page* page = xml_memory_page::construct(page_memory);
			assert(page);

			page->allocator = _root->allocator;

			// record the offset for freeing the memory block
			assert(page_memory > memory && page_memory - static_cast<char*>(memory) <= 127);
			page_memory[-1] = static_cast<char>(page_memory - static_cast<char*>(memory));

			return page;
		}

		static void deallocate_page(xml_memory_page* page)
		{
			char* page_memory = reinterpret_cast<char*>(page);

			xml_memory::deallocate(page_memory - page_memory[-1]);
		}

		void* allocate_memory_oob(size_t size, xml_memory_page*& out_page);

		void* allocate_memory(size_t size, xml_memory_page*& out_page)
		{
			if (_busy_size + size > xml_memory_page_size) return allocate_memory_oob(size, out_page);

			void* buf = reinterpret_cast<char*>(_root) + sizeof(xml_memory_page) + _busy_size;

			_busy_size += size;

			out_page = _root;

			return buf;
		}

		void deallocate_memory(void* ptr, size_t size, xml_memory_page* page)
		{
			if (page == _root) page->busy_size = _busy_size;

			assert(ptr >= reinterpret_cast<char*>(page) + sizeof(xml_memory_page) && ptr < reinterpret_cast<char*>(page) + sizeof(xml_memory_page) + page->busy_size);
			(void)!ptr;

			page->freed_size += size;
			assert(page->freed_size <= page->busy_size);

			if (page->freed_size == page->busy_size)
			{
				if (page->next == 0)
				{
					assert(_root == page);

					// top page freed, just reset sizes
					page->busy_size = page->freed_size = 0;
					_busy_size = 0;
				}
				else
				{
					assert(_root != page);
					assert(page->prev);

					// remove from the list
					page->prev->next = page->next;
					page->next->prev = page->prev;

					// deallocate
					deallocate_page(page);
				}
			}
		}

		char_t* allocate_string(size_t length)
		{
			PUGI__STATIC_ASSERT(xml_memory_page_size <= (1 << 16));

			// allocate memory for string and header block
			size_t size = sizeof(xml_memory_string_header) + length * sizeof(char_t);
			
			// round size up to pointer alignment boundary
			size_t full_size = (size + (sizeof(void*) - 1)) & ~(sizeof(void*) - 1);

			xml_memory_page* page;
			xml_memory_string_header* header = static_cast<xml_memory_string_header*>(allocate_memory(full_size, page));

			if (!header) return 0;

			// setup header
			ptrdiff_t page_offset = reinterpret_cast<char*>(header) - reinterpret_cast<char*>(page) - sizeof(xml_memory_page);

			assert(page_offset >= 0 && page_offset < (1 << 16));
			header->page_offset = static_cast<uint16_t>(page_offset);

			// full_size == 0 for large strings that occupy the whole page
			assert(full_size < (1 << 16) || (page->busy_size == full_size && page_offset == 0));
			header->full_size = static_cast<uint16_t>(full_size < (1 << 16) ? full_size : 0);

			// round-trip through void* to avoid 'cast increases required alignment of target type' warning
			// header is guaranteed a pointer-sized alignment, which should be enough for char_t
			return static_cast<char_t*>(static_cast<void*>(header + 1));
		}

		void deallocate_string(char_t* string)
		{
			// this function casts pointers through void* to avoid 'cast increases required alignment of target type' warnings
			// we're guaranteed the proper (pointer-sized) alignment on the input string if it was allocated via allocate_string

			// get header
			xml_memory_string_header* header = static_cast<xml_memory_string_header*>(static_cast<void*>(string)) - 1;
			assert(header);

			// deallocate
			size_t page_offset = sizeof(xml_memory_page) + header->page_offset;
			xml_memory_page* page = reinterpret_cast<xml_memory_page*>(static_cast<void*>(reinterpret_cast<char*>(header) - page_offset));

			// if full_size == 0 then this string occupies the whole page
			size_t full_size = header->full_size == 0 ? page->busy_size : header->full_size;

			deallocate_memory(header, full_size, page);
		}

		xml_memory_page* _root;
		size_t _busy_size;
	};

	PUGI__FN_NO_INLINE void* xml_allocator::allocate_memory_oob(size_t size, xml_memory_page*& out_page)
	{
		const size_t large_allocation_threshold = xml_memory_page_size / 4;

		xml_memory_page* page = allocate_page(size <= large_allocation_threshold ? xml_memory_page_size : size);
		out_page = page;

		if (!page) return 0;

		if (size <= large_allocation_threshold)
		{
			_root->busy_size = _busy_size;

			// insert page at the end of linked list
			page->prev = _root;
			_root->next = page;
			_root = page;

			_busy_size = size;
		}
		else
		{
			// insert page before the end of linked list, so that it is deleted as soon as possible
			// the last page is not deleted even if it's empty (see deallocate_memory)
			assert(_root->prev);

			page->prev = _root->prev;
			page->next = _root;

			_root->prev->next = page;
			_root->prev = page;
		}

		// allocate inside page
		page->busy_size = size;

		return reinterpret_cast<char*>(page) + sizeof(xml_memory_page);
	}
PUGI__NS_END

namespace ufal {
namespace microrestd {
namespace pugi
{
	/// A 'name=value' XML attribute structure.
	struct xml_attribute_struct
	{
		/// Default ctor
		xml_attribute_struct(impl::xml_memory_page* page): header(reinterpret_cast<uintptr_t>(page)), name(0), value(0), prev_attribute_c(0), next_attribute(0)
		{
		}

		uintptr_t header;

		char_t* name;	///< Pointer to attribute name.
		char_t*	value;	///< Pointer to attribute value.

		xml_attribute_struct* prev_attribute_c;	///< Previous attribute (cyclic list)
		xml_attribute_struct* next_attribute;	///< Next attribute
	};

	/// An XML document tree node.
	struct xml_node_struct
	{
		/// Default ctor
		/// \param type - node type
		xml_node_struct(impl::xml_memory_page* page, xml_node_type type): header(reinterpret_cast<uintptr_t>(page) | (type - 1)), parent(0), name(0), value(0), first_child(0), prev_sibling_c(0), next_sibling(0), first_attribute(0)
		{
		}

		uintptr_t header;

		xml_node_struct*		parent;					///< Pointer to parent

		char_t*					name;					///< Pointer to element name.
		char_t*					value;					///< Pointer to any associated string data.

		xml_node_struct*		first_child;			///< First child
		
		xml_node_struct*		prev_sibling_c;			///< Left brother (cyclic list)
		xml_node_struct*		next_sibling;			///< Right brother
		
		xml_attribute_struct*	first_attribute;		///< First attribute
	};
}
} // namespace microrestd
} // namespace ufal

PUGI__NS_BEGIN
	struct xml_extra_buffer
	{
		char_t* buffer;
		xml_extra_buffer* next;
	};

	struct xml_document_struct: public xml_node_struct, public xml_allocator
	{
		xml_document_struct(xml_memory_page* page): xml_node_struct(page, node_document), xml_allocator(page), buffer(0), extra_buffers(0)
		{
		}

		const char_t* buffer;

		xml_extra_buffer* extra_buffers;
	};

	inline xml_allocator& get_allocator(const xml_node_struct* node)
	{
		assert(node);

		return *reinterpret_cast<xml_memory_page*>(node->header & xml_memory_page_pointer_mask)->allocator;
	}

	template <typename Object> inline xml_document_struct& get_document(const Object* object)
	{
		assert(object);

		return *static_cast<xml_document_struct*>(reinterpret_cast<xml_memory_page*>(object->header & xml_memory_page_pointer_mask)->allocator);
	}
PUGI__NS_END

// Low-level DOM operations
PUGI__NS_BEGIN
	inline xml_attribute_struct* allocate_attribute(xml_allocator& alloc)
	{
		xml_memory_page* page;
		void* memory = alloc.allocate_memory(sizeof(xml_attribute_struct), page);

		return new (memory) xml_attribute_struct(page);
	}

	inline xml_node_struct* allocate_node(xml_allocator& alloc, xml_node_type type)
	{
		xml_memory_page* page;
		void* memory = alloc.allocate_memory(sizeof(xml_node_struct), page);

		return new (memory) xml_node_struct(page, type);
	}

	inline void destroy_attribute(xml_attribute_struct* a, xml_allocator& alloc)
	{
		uintptr_t header = a->header;

		if (header & impl::xml_memory_page_name_allocated_mask) alloc.deallocate_string(a->name);
		if (header & impl::xml_memory_page_value_allocated_mask) alloc.deallocate_string(a->value);

		alloc.deallocate_memory(a, sizeof(xml_attribute_struct), reinterpret_cast<xml_memory_page*>(header & xml_memory_page_pointer_mask));
	}

	inline void destroy_node(xml_node_struct* n, xml_allocator& alloc)
	{
		uintptr_t header = n->header;

		if (header & impl::xml_memory_page_name_allocated_mask) alloc.deallocate_string(n->name);
		if (header & impl::xml_memory_page_value_allocated_mask) alloc.deallocate_string(n->value);

		for (xml_attribute_struct* attr = n->first_attribute; attr; )
		{
			xml_attribute_struct* next = attr->next_attribute;

			destroy_attribute(attr, alloc);

			attr = next;
		}

		for (xml_node_struct* child = n->first_child; child; )
		{
			xml_node_struct* next = child->next_sibling;

			destroy_node(child, alloc);

			child = next;
		}

		alloc.deallocate_memory(n, sizeof(xml_node_struct), reinterpret_cast<xml_memory_page*>(header & xml_memory_page_pointer_mask));
	}

	inline void append_node(xml_node_struct* child, xml_node_struct* node)
	{
		child->parent = node;

		xml_node_struct* head = node->first_child;

		if (head)
		{
			xml_node_struct* tail = head->prev_sibling_c;

			tail->next_sibling = child;
			child->prev_sibling_c = tail;
			head->prev_sibling_c = child;
		}
		else
		{
			node->first_child = child;
			child->prev_sibling_c = child;
		}
	}

	inline void prepend_node(xml_node_struct* child, xml_node_struct* node)
	{
		child->parent = node;

		xml_node_struct* head = node->first_child;

		if (head)
		{
			child->prev_sibling_c = head->prev_sibling_c;
			head->prev_sibling_c = child;
		}
		else
			child->prev_sibling_c = child;

		child->next_sibling = head;
		node->first_child = child;
	}

	inline void insert_node_after(xml_node_struct* child, xml_node_struct* node)
	{
		xml_node_struct* parent = node->parent;

		child->parent = parent;

		if (node->next_sibling)
			node->next_sibling->prev_sibling_c = child;
		else
			parent->first_child->prev_sibling_c = child;

		child->next_sibling = node->next_sibling;
		child->prev_sibling_c = node;

		node->next_sibling = child;
	}

	inline void insert_node_before(xml_node_struct* child, xml_node_struct* node)
	{
		xml_node_struct* parent = node->parent;

		child->parent = parent;

		if (node->prev_sibling_c->next_sibling)
			node->prev_sibling_c->next_sibling = child;
		else
			parent->first_child = child;

		child->prev_sibling_c = node->prev_sibling_c;
		child->next_sibling = node;

		node->prev_sibling_c = child;
	}

	inline void remove_node(xml_node_struct* node)
	{
		xml_node_struct* parent = node->parent;

		if (node->next_sibling)
			node->next_sibling->prev_sibling_c = node->prev_sibling_c;
		else
			parent->first_child->prev_sibling_c = node->prev_sibling_c;

		if (node->prev_sibling_c->next_sibling)
			node->prev_sibling_c->next_sibling = node->next_sibling;
		else
			parent->first_child = node->next_sibling;

		node->parent = 0;
		node->prev_sibling_c = 0;
		node->next_sibling = 0;
	}

	inline void append_attribute(xml_attribute_struct* attr, xml_node_struct* node)
	{
		xml_attribute_struct* head = node->first_attribute;

		if (head)
		{
			xml_attribute_struct* tail = head->prev_attribute_c;

			tail->next_attribute = attr;
			attr->prev_attribute_c = tail;
			head->prev_attribute_c = attr;
		}
		else
		{
			node->first_attribute = attr;
			attr->prev_attribute_c = attr;
		}
	}

	inline void prepend_attribute(xml_attribute_struct* attr, xml_node_struct* node)
	{
		xml_attribute_struct* head = node->first_attribute;

		if (head)
		{
			attr->prev_attribute_c = head->prev_attribute_c;
			head->prev_attribute_c = attr;
		}
		else
			attr->prev_attribute_c = attr;

		attr->next_attribute = head;
		node->first_attribute = attr;
	}

	inline void insert_attribute_after(xml_attribute_struct* attr, xml_attribute_struct* place, xml_node_struct* node)
	{
		if (place->next_attribute)
			place->next_attribute->prev_attribute_c = attr;
		else
			node->first_attribute->prev_attribute_c = attr;

		attr->next_attribute = place->next_attribute;
		attr->prev_attribute_c = place;
		place->next_attribute = attr;
	}

	inline void insert_attribute_before(xml_attribute_struct* attr, xml_attribute_struct* place, xml_node_struct* node)
	{
		if (place->prev_attribute_c->next_attribute)
			place->prev_attribute_c->next_attribute = attr;
		else
			node->first_attribute = attr;

		attr->prev_attribute_c = place->prev_attribute_c;
		attr->next_attribute = place;
		place->prev_attribute_c = attr;
	}

	inline void remove_attribute(xml_attribute_struct* attr, xml_node_struct* node)
	{
		if (attr->next_attribute)
			attr->next_attribute->prev_attribute_c = attr->prev_attribute_c;
		else
			node->first_attribute->prev_attribute_c = attr->prev_attribute_c;

		if (attr->prev_attribute_c->next_attribute)
			attr->prev_attribute_c->next_attribute = attr->next_attribute;
		else
			node->first_attribute = attr->next_attribute;

		attr->prev_attribute_c = 0;
		attr->next_attribute = 0;
	}

	PUGI__FN_NO_INLINE xml_node_struct* append_new_node(xml_node_struct* node, xml_allocator& alloc, xml_node_type type = node_element)
	{
		xml_node_struct* child = allocate_node(alloc, type);
		if (!child) return 0;

		append_node(child, node);

		return child;
	}

	PUGI__FN_NO_INLINE xml_attribute_struct* append_new_attribute(xml_node_struct* node, xml_allocator& alloc)
	{
		xml_attribute_struct* attr = allocate_attribute(alloc);
		if (!attr) return 0;

		append_attribute(attr, node);

		return attr;
	}
PUGI__NS_END

// Helper classes for code generation
PUGI__NS_BEGIN
	struct opt_false
	{
		enum { value = 0 };
	};

	struct opt_true
	{
		enum { value = 1 };
	};
PUGI__NS_END

// Unicode utilities
PUGI__NS_BEGIN
	inline uint16_t endian_swap(uint16_t value)
	{
		return static_cast<uint16_t>(((value & 0xff) << 8) | (value >> 8));
	}

	inline uint32_t endian_swap(uint32_t value)
	{
		return ((value & 0xff) << 24) | ((value & 0xff00) << 8) | ((value & 0xff0000) >> 8) | (value >> 24);
	}

	struct utf8_counter
	{
		typedef size_t value_type;

		static value_type low(value_type result, uint32_t ch)
		{
			// U+0000..U+007F
			if (ch < 0x80) return result + 1;
			// U+0080..U+07FF
			else if (ch < 0x800) return result + 2;
			// U+0800..U+FFFF
			else return result + 3;
		}

		static value_type high(value_type result, uint32_t)
		{
			// U+10000..U+10FFFF
			return result + 4;
		}
	};

	struct utf8_writer
	{
		typedef uint8_t* value_type;

		static value_type low(value_type result, uint32_t ch)
		{
			// U+0000..U+007F
			if (ch < 0x80)
			{
				*result = static_cast<uint8_t>(ch);
				return result + 1;
			}
			// U+0080..U+07FF
			else if (ch < 0x800)
			{
				result[0] = static_cast<uint8_t>(0xC0 | (ch >> 6));
				result[1] = static_cast<uint8_t>(0x80 | (ch & 0x3F));
				return result + 2;
			}
			// U+0800..U+FFFF
			else
			{
				result[0] = static_cast<uint8_t>(0xE0 | (ch >> 12));
				result[1] = static_cast<uint8_t>(0x80 | ((ch >> 6) & 0x3F));
				result[2] = static_cast<uint8_t>(0x80 | (ch & 0x3F));
				return result + 3;
			}
		}

		static value_type high(value_type result, uint32_t ch)
		{
			// U+10000..U+10FFFF
			result[0] = static_cast<uint8_t>(0xF0 | (ch >> 18));
			result[1] = static_cast<uint8_t>(0x80 | ((ch >> 12) & 0x3F));
			result[2] = static_cast<uint8_t>(0x80 | ((ch >> 6) & 0x3F));
			result[3] = static_cast<uint8_t>(0x80 | (ch & 0x3F));
			return result + 4;
		}

		static value_type any(value_type result, uint32_t ch)
		{
			return (ch < 0x10000) ? low(result, ch) : high(result, ch);
		}
	};

	struct utf16_counter
	{
		typedef size_t value_type;

		static value_type low(value_type result, uint32_t)
		{
			return result + 1;
		}

		static value_type high(value_type result, uint32_t)
		{
			return result + 2;
		}
	};

	struct utf16_writer
	{
		typedef uint16_t* value_type;

		static value_type low(value_type result, uint32_t ch)
		{
			*result = static_cast<uint16_t>(ch);

			return result + 1;
		}

		static value_type high(value_type result, uint32_t ch)
		{
			uint32_t msh = static_cast<uint32_t>(ch - 0x10000) >> 10;
			uint32_t lsh = static_cast<uint32_t>(ch - 0x10000) & 0x3ff;

			result[0] = static_cast<uint16_t>(0xD800 + msh);
			result[1] = static_cast<uint16_t>(0xDC00 + lsh);

			return result + 2;
		}

		static value_type any(value_type result, uint32_t ch)
		{
			return (ch < 0x10000) ? low(result, ch) : high(result, ch);
		}
	};

	struct utf32_counter
	{
		typedef size_t value_type;

		static value_type low(value_type result, uint32_t)
		{
			return result + 1;
		}

		static value_type high(value_type result, uint32_t)
		{
			return result + 1;
		}
	};

	struct utf32_writer
	{
		typedef uint32_t* value_type;

		static value_type low(value_type result, uint32_t ch)
		{
			*result = ch;

			return result + 1;
		}

		static value_type high(value_type result, uint32_t ch)
		{
			*result = ch;

			return result + 1;
		}

		static value_type any(value_type result, uint32_t ch)
		{
			*result = ch;

			return result + 1;
		}
	};

	struct latin1_writer
	{
		typedef uint8_t* value_type;

		static value_type low(value_type result, uint32_t ch)
		{
			*result = static_cast<uint8_t>(ch > 255 ? '?' : ch);

			return result + 1;
		}

		static value_type high(value_type result, uint32_t ch)
		{
			(void)ch;

			*result = '?';

			return result + 1;
		}
	};

	template <size_t size> struct wchar_selector;

	template <> struct wchar_selector<2>
	{
		typedef uint16_t type;
		typedef utf16_counter counter;
		typedef utf16_writer writer;
	};

	template <> struct wchar_selector<4>
	{
		typedef uint32_t type;
		typedef utf32_counter counter;
		typedef utf32_writer writer;
	};

	typedef wchar_selector<sizeof(wchar_t)>::counter wchar_counter;
	typedef wchar_selector<sizeof(wchar_t)>::writer wchar_writer;

	template <typename Traits, typename opt_swap = opt_false> struct utf_decoder
	{
		static inline typename Traits::value_type decode_utf8_block(const uint8_t* data, size_t size, typename Traits::value_type result)
		{
			const uint8_t utf8_byte_mask = 0x3f;

			while (size)
			{
				uint8_t lead = *data;

				// 0xxxxxxx -> U+0000..U+007F
				if (lead < 0x80)
				{
					result = Traits::low(result, lead);
					data += 1;
					size -= 1;

					// process aligned single-byte (ascii) blocks
					if ((reinterpret_cast<uintptr_t>(data) & 3) == 0)
					{
						// round-trip through void* to silence 'cast increases required alignment of target type' warnings
						while (size >= 4 && (*static_cast<const uint32_t*>(static_cast<const void*>(data)) & 0x80808080) == 0)
						{
							result = Traits::low(result, data[0]);
							result = Traits::low(result, data[1]);
							result = Traits::low(result, data[2]);
							result = Traits::low(result, data[3]);
							data += 4;
							size -= 4;
						}
					}
				}
				// 110xxxxx -> U+0080..U+07FF
				else if (static_cast<unsigned int>(lead - 0xC0) < 0x20 && size >= 2 && (data[1] & 0xc0) == 0x80)
				{
					result = Traits::low(result, ((lead & ~0xC0) << 6) | (data[1] & utf8_byte_mask));
					data += 2;
					size -= 2;
				}
				// 1110xxxx -> U+0800-U+FFFF
				else if (static_cast<unsigned int>(lead - 0xE0) < 0x10 && size >= 3 && (data[1] & 0xc0) == 0x80 && (data[2] & 0xc0) == 0x80)
				{
					result = Traits::low(result, ((lead & ~0xE0) << 12) | ((data[1] & utf8_byte_mask) << 6) | (data[2] & utf8_byte_mask));
					data += 3;
					size -= 3;
				}
				// 11110xxx -> U+10000..U+10FFFF
				else if (static_cast<unsigned int>(lead - 0xF0) < 0x08 && size >= 4 && (data[1] & 0xc0) == 0x80 && (data[2] & 0xc0) == 0x80 && (data[3] & 0xc0) == 0x80)
				{
					result = Traits::high(result, ((lead & ~0xF0) << 18) | ((data[1] & utf8_byte_mask) << 12) | ((data[2] & utf8_byte_mask) << 6) | (data[3] & utf8_byte_mask));
					data += 4;
					size -= 4;
				}
				// 10xxxxxx or 11111xxx -> invalid
				else
				{
					data += 1;
					size -= 1;
				}
			}

			return result;
		}

		static inline typename Traits::value_type decode_utf16_block(const uint16_t* data, size_t size, typename Traits::value_type result)
		{
			const uint16_t* end = data + size;

			while (data < end)
			{
				unsigned int lead = opt_swap::value ? endian_swap(*data) : *data;

				// U+0000..U+D7FF
				if (lead < 0xD800)
				{
					result = Traits::low(result, lead);
					data += 1;
				}
				// U+E000..U+FFFF
				else if (static_cast<unsigned int>(lead - 0xE000) < 0x2000)
				{
					result = Traits::low(result, lead);
					data += 1;
				}
				// surrogate pair lead
				else if (static_cast<unsigned int>(lead - 0xD800) < 0x400 && data + 1 < end)
				{
					uint16_t next = opt_swap::value ? endian_swap(data[1]) : data[1];

					if (static_cast<unsigned int>(next - 0xDC00) < 0x400)
					{
						result = Traits::high(result, 0x10000 + ((lead & 0x3ff) << 10) + (next & 0x3ff));
						data += 2;
					}
					else
					{
						data += 1;
					}
				}
				else
				{
					data += 1;
				}
			}

			return result;
		}

		static inline typename Traits::value_type decode_utf32_block(const uint32_t* data, size_t size, typename Traits::value_type result)
		{
			const uint32_t* end = data + size;

			while (data < end)
			{
				uint32_t lead = opt_swap::value ? endian_swap(*data) : *data;

				// U+0000..U+FFFF
				if (lead < 0x10000)
				{
					result = Traits::low(result, lead);
					data += 1;
				}
				// U+10000..U+10FFFF
				else
				{
					result = Traits::high(result, lead);
					data += 1;
				}
			}

			return result;
		}

		static inline typename Traits::value_type decode_latin1_block(const uint8_t* data, size_t size, typename Traits::value_type result)
		{
			for (size_t i = 0; i < size; ++i)
			{
				result = Traits::low(result, data[i]);
			}

			return result;
		}

		static inline typename Traits::value_type decode_wchar_block_impl(const uint16_t* data, size_t size, typename Traits::value_type result)
		{
			return decode_utf16_block(data, size, result);
		}

		static inline typename Traits::value_type decode_wchar_block_impl(const uint32_t* data, size_t size, typename Traits::value_type result)
		{
			return decode_utf32_block(data, size, result);
		}

		static inline typename Traits::value_type decode_wchar_block(const wchar_t* data, size_t size, typename Traits::value_type result)
		{
			return decode_wchar_block_impl(reinterpret_cast<const wchar_selector<sizeof(wchar_t)>::type*>(data), size, result);
		}
	};

	template <typename T> PUGI__FN void convert_utf_endian_swap(T* result, const T* data, size_t length)
	{
		for (size_t i = 0; i < length; ++i) result[i] = endian_swap(data[i]);
	}

#ifdef PUGIXML_WCHAR_MODE
	PUGI__FN void convert_wchar_endian_swap(wchar_t* result, const wchar_t* data, size_t length)
	{
		for (size_t i = 0; i < length; ++i) result[i] = static_cast<wchar_t>(endian_swap(static_cast<wchar_selector<sizeof(wchar_t)>::type>(data[i])));
	}
#endif
PUGI__NS_END

PUGI__NS_BEGIN
	enum chartype_t
	{
		ct_parse_pcdata = 1,	// \0, &, \r, <
		ct_parse_attr = 2,		// \0, &, \r, ', "
		ct_parse_attr_ws = 4,	// \0, &, \r, ', ", \n, tab
		ct_space = 8,			// \r, \n, space, tab
		ct_parse_cdata = 16,	// \0, ], >, \r
		ct_parse_comment = 32,	// \0, -, >, \r
		ct_symbol = 64,			// Any symbol > 127, a-z, A-Z, 0-9, _, :, -, .
		ct_start_symbol = 128	// Any symbol > 127, a-z, A-Z, _, :
	};

	static const unsigned char chartype_table[256] =
	{
		55,  0,   0,   0,   0,   0,   0,   0,      0,   12,  12,  0,   0,   63,  0,   0,   // 0-15
		0,   0,   0,   0,   0,   0,   0,   0,      0,   0,   0,   0,   0,   0,   0,   0,   // 16-31
		8,   0,   6,   0,   0,   0,   7,   6,      0,   0,   0,   0,   0,   96,  64,  0,   // 32-47
		64,  64,  64,  64,  64,  64,  64,  64,     64,  64,  192, 0,   1,   0,   48,  0,   // 48-63
		0,   192, 192, 192, 192, 192, 192, 192,    192, 192, 192, 192, 192, 192, 192, 192, // 64-79
		192, 192, 192, 192, 192, 192, 192, 192,    192, 192, 192, 0,   0,   16,  0,   192, // 80-95
		0,   192, 192, 192, 192, 192, 192, 192,    192, 192, 192, 192, 192, 192, 192, 192, // 96-111
		192, 192, 192, 192, 192, 192, 192, 192,    192, 192, 192, 0, 0, 0, 0, 0,           // 112-127

		192, 192, 192, 192, 192, 192, 192, 192,    192, 192, 192, 192, 192, 192, 192, 192, // 128+
		192, 192, 192, 192, 192, 192, 192, 192,    192, 192, 192, 192, 192, 192, 192, 192,
		192, 192, 192, 192, 192, 192, 192, 192,    192, 192, 192, 192, 192, 192, 192, 192,
		192, 192, 192, 192, 192, 192, 192, 192,    192, 192, 192, 192, 192, 192, 192, 192,
		192, 192, 192, 192, 192, 192, 192, 192,    192, 192, 192, 192, 192, 192, 192, 192,
		192, 192, 192, 192, 192, 192, 192, 192,    192, 192, 192, 192, 192, 192, 192, 192,
		192, 192, 192, 192, 192, 192, 192, 192,    192, 192, 192, 192, 192, 192, 192, 192,
		192, 192, 192, 192, 192, 192, 192, 192,    192, 192, 192, 192, 192, 192, 192, 192
	};

	enum chartypex_t
	{
		ctx_special_pcdata = 1,   // Any symbol >= 0 and < 32 (except \t, \r, \n), &, <, >
		ctx_special_attr = 2,     // Any symbol >= 0 and < 32 (except \t), &, <, >, "
		ctx_start_symbol = 4,	  // Any symbol > 127, a-z, A-Z, _
		ctx_digit = 8,			  // 0-9
		ctx_symbol = 16			  // Any symbol > 127, a-z, A-Z, 0-9, _, -, .
	};
	
	static const unsigned char chartypex_table[256] =
	{
		3,  3,  3,  3,  3,  3,  3,  3,     3,  0,  2,  3,  3,  2,  3,  3,     // 0-15
		3,  3,  3,  3,  3,  3,  3,  3,     3,  3,  3,  3,  3,  3,  3,  3,     // 16-31
		0,  0,  2,  0,  0,  0,  3,  0,     0,  0,  0,  0,  0, 16, 16,  0,     // 32-47
		24, 24, 24, 24, 24, 24, 24, 24,    24, 24, 0,  0,  3,  0,  3,  0,     // 48-63

		0,  20, 20, 20, 20, 20, 20, 20,    20, 20, 20, 20, 20, 20, 20, 20,    // 64-79
		20, 20, 20, 20, 20, 20, 20, 20,    20, 20, 20, 0,  0,  0,  0,  20,    // 80-95
		0,  20, 20, 20, 20, 20, 20, 20,    20, 20, 20, 20, 20, 20, 20, 20,    // 96-111
		20, 20, 20, 20, 20, 20, 20, 20,    20, 20, 20, 0,  0,  0,  0,  0,     // 112-127

		20, 20, 20, 20, 20, 20, 20, 20,    20, 20, 20, 20, 20, 20, 20, 20,    // 128+
		20, 20, 20, 20, 20, 20, 20, 20,    20, 20, 20, 20, 20, 20, 20, 20,
		20, 20, 20, 20, 20, 20, 20, 20,    20, 20, 20, 20, 20, 20, 20, 20,
		20, 20, 20, 20, 20, 20, 20, 20,    20, 20, 20, 20, 20, 20, 20, 20,
		20, 20, 20, 20, 20, 20, 20, 20,    20, 20, 20, 20, 20, 20, 20, 20,
		20, 20, 20, 20, 20, 20, 20, 20,    20, 20, 20, 20, 20, 20, 20, 20,
		20, 20, 20, 20, 20, 20, 20, 20,    20, 20, 20, 20, 20, 20, 20, 20,
		20, 20, 20, 20, 20, 20, 20, 20,    20, 20, 20, 20, 20, 20, 20, 20
	};
	
#ifdef PUGIXML_WCHAR_MODE
	#define PUGI__IS_CHARTYPE_IMPL(c, ct, table) ((static_cast<unsigned int>(c) < 128 ? table[static_cast<unsigned int>(c)] : table[128]) & (ct))
#else
	#define PUGI__IS_CHARTYPE_IMPL(c, ct, table) (table[static_cast<unsigned char>(c)] & (ct))
#endif

	#define PUGI__IS_CHARTYPE(c, ct) PUGI__IS_CHARTYPE_IMPL(c, ct, chartype_table)
	#define PUGI__IS_CHARTYPEX(c, ct) PUGI__IS_CHARTYPE_IMPL(c, ct, chartypex_table)

	PUGI__FN bool is_little_endian()
	{
		unsigned int ui = 1;

		return *reinterpret_cast<unsigned char*>(&ui) == 1;
	}

	PUGI__FN xml_encoding get_wchar_encoding()
	{
		PUGI__STATIC_ASSERT(sizeof(wchar_t) == 2 || sizeof(wchar_t) == 4);

		if (sizeof(wchar_t) == 2)
			return is_little_endian() ? encoding_utf16_le : encoding_utf16_be;
		else 
			return is_little_endian() ? encoding_utf32_le : encoding_utf32_be;
	}

	PUGI__FN xml_encoding guess_buffer_encoding(uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3)
	{
		// look for BOM in first few bytes
		if (d0 == 0 && d1 == 0 && d2 == 0xfe && d3 == 0xff) return encoding_utf32_be;
		if (d0 == 0xff && d1 == 0xfe && d2 == 0 && d3 == 0) return encoding_utf32_le;
		if (d0 == 0xfe && d1 == 0xff) return encoding_utf16_be;
		if (d0 == 0xff && d1 == 0xfe) return encoding_utf16_le;
		if (d0 == 0xef && d1 == 0xbb && d2 == 0xbf) return encoding_utf8;

		// look for <, <? or <?xm in various encodings
		if (d0 == 0 && d1 == 0 && d2 == 0 && d3 == 0x3c) return encoding_utf32_be;
		if (d0 == 0x3c && d1 == 0 && d2 == 0 && d3 == 0) return encoding_utf32_le;
		if (d0 == 0 && d1 == 0x3c && d2 == 0 && d3 == 0x3f) return encoding_utf16_be;
		if (d0 == 0x3c && d1 == 0 && d2 == 0x3f && d3 == 0) return encoding_utf16_le;
		if (d0 == 0x3c && d1 == 0x3f && d2 == 0x78 && d3 == 0x6d) return encoding_utf8;

		// look for utf16 < followed by node name (this may fail, but is better than utf8 since it's zero terminated so early)
		if (d0 == 0 && d1 == 0x3c) return encoding_utf16_be;
		if (d0 == 0x3c && d1 == 0) return encoding_utf16_le;

		// no known BOM detected, assume utf8
		return encoding_utf8;
	}

	PUGI__FN xml_encoding get_buffer_encoding(xml_encoding encoding, const void* contents, size_t size)
	{
		// replace wchar encoding with utf implementation
		if (encoding == encoding_wchar) return get_wchar_encoding();

		// replace utf16 encoding with utf16 with specific endianness
		if (encoding == encoding_utf16) return is_little_endian() ? encoding_utf16_le : encoding_utf16_be;

		// replace utf32 encoding with utf32 with specific endianness
		if (encoding == encoding_utf32) return is_little_endian() ? encoding_utf32_le : encoding_utf32_be;

		// only do autodetection if no explicit encoding is requested
		if (encoding != encoding_auto) return encoding;

		// skip encoding autodetection if input buffer is too small
		if (size < 4) return encoding_utf8;

		// try to guess encoding (based on XML specification, Appendix F.1)
		const uint8_t* data = static_cast<const uint8_t*>(contents);

		PUGI__DMC_VOLATILE uint8_t d0 = data[0], d1 = data[1], d2 = data[2], d3 = data[3];

		return guess_buffer_encoding(d0, d1, d2, d3);
	}

	PUGI__FN bool get_mutable_buffer(char_t*& out_buffer, size_t& out_length, const void* contents, size_t size, bool is_mutable)
	{
		size_t length = size / sizeof(char_t);

		if (is_mutable)
		{
			out_buffer = static_cast<char_t*>(const_cast<void*>(contents));
			out_length = length;
		}
		else
		{
			char_t* buffer = static_cast<char_t*>(xml_memory::allocate((length + 1) * sizeof(char_t)));
			if (!buffer) return false;

			if (contents)
				memcpy(buffer, contents, length * sizeof(char_t));
			else
				assert(length == 0);

			buffer[length] = 0;

			out_buffer = buffer;
			out_length = length + 1;
		}

		return true;
	}

#ifdef PUGIXML_WCHAR_MODE
	PUGI__FN bool need_endian_swap_utf(xml_encoding le, xml_encoding re)
	{
		return (le == encoding_utf16_be && re == encoding_utf16_le) || (le == encoding_utf16_le && re == encoding_utf16_be) ||
			   (le == encoding_utf32_be && re == encoding_utf32_le) || (le == encoding_utf32_le && re == encoding_utf32_be);
	}

	PUGI__FN bool convert_buffer_endian_swap(char_t*& out_buffer, size_t& out_length, const void* contents, size_t size, bool is_mutable)
	{
		const char_t* data = static_cast<const char_t*>(contents);
		size_t length = size / sizeof(char_t);

		if (is_mutable)
		{
			char_t* buffer = const_cast<char_t*>(data);

			convert_wchar_endian_swap(buffer, data, length);

			out_buffer = buffer;
			out_length = length;
		}
		else
		{
			char_t* buffer = static_cast<char_t*>(xml_memory::allocate((length + 1) * sizeof(char_t)));
			if (!buffer) return false;

			convert_wchar_endian_swap(buffer, data, length);
			buffer[length] = 0;

			out_buffer = buffer;
			out_length = length + 1;
		}

		return true;
	}

	PUGI__FN bool convert_buffer_utf8(char_t*& out_buffer, size_t& out_length, const void* contents, size_t size)
	{
		const uint8_t* data = static_cast<const uint8_t*>(contents);
		size_t data_length = size;

		// first pass: get length in wchar_t units
		size_t length = utf_decoder<wchar_counter>::decode_utf8_block(data, data_length, 0);

		// allocate buffer of suitable length
		char_t* buffer = static_cast<char_t*>(xml_memory::allocate((length + 1) * sizeof(char_t)));
		if (!buffer) return false;

		// second pass: convert utf8 input to wchar_t
		wchar_writer::value_type obegin = reinterpret_cast<wchar_writer::value_type>(buffer);
		wchar_writer::value_type oend = utf_decoder<wchar_writer>::decode_utf8_block(data, data_length, obegin);

		assert(oend == obegin + length);
		*oend = 0;

		out_buffer = buffer;
		out_length = length + 1;

		return true;
	}

	template <typename opt_swap> PUGI__FN bool convert_buffer_utf16(char_t*& out_buffer, size_t& out_length, const void* contents, size_t size, opt_swap)
	{
		const uint16_t* data = static_cast<const uint16_t*>(contents);
		size_t data_length = size / sizeof(uint16_t);

		// first pass: get length in wchar_t units
		size_t length = utf_decoder<wchar_counter, opt_swap>::decode_utf16_block(data, data_length, 0);

		// allocate buffer of suitable length
		char_t* buffer = static_cast<char_t*>(xml_memory::allocate((length + 1) * sizeof(char_t)));
		if (!buffer) return false;

		// second pass: convert utf16 input to wchar_t
		wchar_writer::value_type obegin = reinterpret_cast<wchar_writer::value_type>(buffer);
		wchar_writer::value_type oend = utf_decoder<wchar_writer, opt_swap>::decode_utf16_block(data, data_length, obegin);

		assert(oend == obegin + length);
		*oend = 0;

		out_buffer = buffer;
		out_length = length + 1;

		return true;
	}

	template <typename opt_swap> PUGI__FN bool convert_buffer_utf32(char_t*& out_buffer, size_t& out_length, const void* contents, size_t size, opt_swap)
	{
		const uint32_t* data = static_cast<const uint32_t*>(contents);
		size_t data_length = size / sizeof(uint32_t);

		// first pass: get length in wchar_t units
		size_t length = utf_decoder<wchar_counter, opt_swap>::decode_utf32_block(data, data_length, 0);

		// allocate buffer of suitable length
		char_t* buffer = static_cast<char_t*>(xml_memory::allocate((length + 1) * sizeof(char_t)));
		if (!buffer) return false;

		// second pass: convert utf32 input to wchar_t
		wchar_writer::value_type obegin = reinterpret_cast<wchar_writer::value_type>(buffer);
		wchar_writer::value_type oend = utf_decoder<wchar_writer, opt_swap>::decode_utf32_block(data, data_length, obegin);

		assert(oend == obegin + length);
		*oend = 0;

		out_buffer = buffer;
		out_length = length + 1;

		return true;
	}

	PUGI__FN bool convert_buffer_latin1(char_t*& out_buffer, size_t& out_length, const void* contents, size_t size)
	{
		const uint8_t* data = static_cast<const uint8_t*>(contents);
		size_t data_length = size;

		// get length in wchar_t units
		size_t length = data_length;

		// allocate buffer of suitable length
		char_t* buffer = static_cast<char_t*>(xml_memory::allocate((length + 1) * sizeof(char_t)));
		if (!buffer) return false;

		// convert latin1 input to wchar_t
		wchar_writer::value_type obegin = reinterpret_cast<wchar_writer::value_type>(buffer);
		wchar_writer::value_type oend = utf_decoder<wchar_writer>::decode_latin1_block(data, data_length, obegin);

		assert(oend == obegin + length);
		*oend = 0;

		out_buffer = buffer;
		out_length = length + 1;

		return true;
	}

	PUGI__FN bool convert_buffer(char_t*& out_buffer, size_t& out_length, xml_encoding encoding, const void* contents, size_t size, bool is_mutable)
	{
		// get native encoding
		xml_encoding wchar_encoding = get_wchar_encoding();

		// fast path: no conversion required
		if (encoding == wchar_encoding) return get_mutable_buffer(out_buffer, out_length, contents, size, is_mutable);

		// only endian-swapping is required
		if (need_endian_swap_utf(encoding, wchar_encoding)) return convert_buffer_endian_swap(out_buffer, out_length, contents, size, is_mutable);

		// source encoding is utf8
		if (encoding == encoding_utf8) return convert_buffer_utf8(out_buffer, out_length, contents, size);

		// source encoding is utf16
		if (encoding == encoding_utf16_be || encoding == encoding_utf16_le)
		{
			xml_encoding native_encoding = is_little_endian() ? encoding_utf16_le : encoding_utf16_be;

			return (native_encoding == encoding) ?
				convert_buffer_utf16(out_buffer, out_length, contents, size, opt_false()) :
				convert_buffer_utf16(out_buffer, out_length, contents, size, opt_true());
		}

		// source encoding is utf32
		if (encoding == encoding_utf32_be || encoding == encoding_utf32_le)
		{
			xml_encoding native_encoding = is_little_endian() ? encoding_utf32_le : encoding_utf32_be;

			return (native_encoding == encoding) ?
				convert_buffer_utf32(out_buffer, out_length, contents, size, opt_false()) :
				convert_buffer_utf32(out_buffer, out_length, contents, size, opt_true());
		}

		// source encoding is latin1
		if (encoding == encoding_latin1) return convert_buffer_latin1(out_buffer, out_length, contents, size);

		assert(!"Invalid encoding");
		return false;
	}
#else
	template <typename opt_swap> PUGI__FN bool convert_buffer_utf16(char_t*& out_buffer, size_t& out_length, const void* contents, size_t size, opt_swap)
	{
		const uint16_t* data = static_cast<const uint16_t*>(contents);
		size_t data_length = size / sizeof(uint16_t);

		// first pass: get length in utf8 units
		size_t length = utf_decoder<utf8_counter, opt_swap>::decode_utf16_block(data, data_length, 0);

		// allocate buffer of suitable length
		char_t* buffer = static_cast<char_t*>(xml_memory::allocate((length + 1) * sizeof(char_t)));
		if (!buffer) return false;

		// second pass: convert utf16 input to utf8
		uint8_t* obegin = reinterpret_cast<uint8_t*>(buffer);
		uint8_t* oend = utf_decoder<utf8_writer, opt_swap>::decode_utf16_block(data, data_length, obegin);

		assert(oend == obegin + length);
		*oend = 0;

		out_buffer = buffer;
		out_length = length + 1;

		return true;
	}

	template <typename opt_swap> PUGI__FN bool convert_buffer_utf32(char_t*& out_buffer, size_t& out_length, const void* contents, size_t size, opt_swap)
	{
		const uint32_t* data = static_cast<const uint32_t*>(contents);
		size_t data_length = size / sizeof(uint32_t);

		// first pass: get length in utf8 units
		size_t length = utf_decoder<utf8_counter, opt_swap>::decode_utf32_block(data, data_length, 0);

		// allocate buffer of suitable length
		char_t* buffer = static_cast<char_t*>(xml_memory::allocate((length + 1) * sizeof(char_t)));
		if (!buffer) return false;

		// second pass: convert utf32 input to utf8
		uint8_t* obegin = reinterpret_cast<uint8_t*>(buffer);
		uint8_t* oend = utf_decoder<utf8_writer, opt_swap>::decode_utf32_block(data, data_length, obegin);

		assert(oend == obegin + length);
		*oend = 0;

		out_buffer = buffer;
		out_length = length + 1;

		return true;
	}

	PUGI__FN size_t get_latin1_7bit_prefix_length(const uint8_t* data, size_t size)
	{
		for (size_t i = 0; i < size; ++i)
			if (data[i] > 127)
				return i;

		return size;
	}

	PUGI__FN bool convert_buffer_latin1(char_t*& out_buffer, size_t& out_length, const void* contents, size_t size, bool is_mutable)
	{
		const uint8_t* data = static_cast<const uint8_t*>(contents);
		size_t data_length = size;

		// get size of prefix that does not need utf8 conversion
		size_t prefix_length = get_latin1_7bit_prefix_length(data, data_length);
		assert(prefix_length <= data_length);

		const uint8_t* postfix = data + prefix_length;
		size_t postfix_length = data_length - prefix_length;

		// if no conversion is needed, just return the original buffer
		if (postfix_length == 0) return get_mutable_buffer(out_buffer, out_length, contents, size, is_mutable);

		// first pass: get length in utf8 units
		size_t length = prefix_length + utf_decoder<utf8_counter>::decode_latin1_block(postfix, postfix_length, 0);

		// allocate buffer of suitable length
		char_t* buffer = static_cast<char_t*>(xml_memory::allocate((length + 1) * sizeof(char_t)));
		if (!buffer) return false;

		// second pass: convert latin1 input to utf8
		memcpy(buffer, data, prefix_length);

		uint8_t* obegin = reinterpret_cast<uint8_t*>(buffer);
		uint8_t* oend = utf_decoder<utf8_writer>::decode_latin1_block(postfix, postfix_length, obegin + prefix_length);

		assert(oend == obegin + length);
		*oend = 0;

		out_buffer = buffer;
		out_length = length + 1;

		return true;
	}

	PUGI__FN bool convert_buffer(char_t*& out_buffer, size_t& out_length, xml_encoding encoding, const void* contents, size_t size, bool is_mutable)
	{
		// fast path: no conversion required
		if (encoding == encoding_utf8) return get_mutable_buffer(out_buffer, out_length, contents, size, is_mutable);

		// source encoding is utf16
		if (encoding == encoding_utf16_be || encoding == encoding_utf16_le)
		{
			xml_encoding native_encoding = is_little_endian() ? encoding_utf16_le : encoding_utf16_be;

			return (native_encoding == encoding) ?
				convert_buffer_utf16(out_buffer, out_length, contents, size, opt_false()) :
				convert_buffer_utf16(out_buffer, out_length, contents, size, opt_true());
		}

		// source encoding is utf32
		if (encoding == encoding_utf32_be || encoding == encoding_utf32_le)
		{
			xml_encoding native_encoding = is_little_endian() ? encoding_utf32_le : encoding_utf32_be;

			return (native_encoding == encoding) ?
				convert_buffer_utf32(out_buffer, out_length, contents, size, opt_false()) :
				convert_buffer_utf32(out_buffer, out_length, contents, size, opt_true());
		}

		// source encoding is latin1
		if (encoding == encoding_latin1) return convert_buffer_latin1(out_buffer, out_length, contents, size, is_mutable);

		assert(!"Invalid encoding");
		return false;
	}
#endif

// Define the following functions only when using them, by Milan Straka
#if defined(PUGI__MSVC_CRT_VERSION) || defined(__BORLANDC__) || (defined(__MINGW32__) && (!defined(__STRICT_ANSI__) || defined(__MINGW64_VERSION_MAJOR)))
#else
	PUGI__FN size_t as_utf8_begin(const wchar_t* str, size_t length)
	{
		// get length in utf8 characters
		return utf_decoder<utf8_counter>::decode_wchar_block(str, length, 0);
	}

	PUGI__FN void as_utf8_end(char* buffer, size_t size, const wchar_t* str, size_t length)
	{
		// convert to utf8
		uint8_t* begin = reinterpret_cast<uint8_t*>(buffer);
		uint8_t* end = utf_decoder<utf8_writer>::decode_wchar_block(str, length, begin);
	
		assert(begin + size == end);
		(void)!end;

		// zero-terminate
		buffer[size] = 0;
	}
#endif
	
#ifndef PUGIXML_NO_STL
// Removed the XPATH and STL module, by Milan Straka
#endif

	inline bool strcpy_insitu_allow(size_t length, uintptr_t header, uintptr_t header_mask, char_t* target)
	{
		// never reuse shared memory
		if (header & xml_memory_page_contents_shared_mask) return false;

		size_t target_length = strlength(target);

		// always reuse document buffer memory if possible
		if ((header & header_mask) == 0) return target_length >= length;

		// reuse heap memory if waste is not too great
		const size_t reuse_threshold = 32;

		return target_length >= length && (target_length < reuse_threshold || target_length - length < target_length / 2);
	}

	PUGI__FN bool strcpy_insitu(char_t*& dest, uintptr_t& header, uintptr_t header_mask, const char_t* source)
	{
		assert(header);

		size_t source_length = strlength(source);

		if (source_length == 0)
		{
			// empty string and null pointer are equivalent, so just deallocate old memory
			xml_allocator* alloc = reinterpret_cast<xml_memory_page*>(header & xml_memory_page_pointer_mask)->allocator;

			if (header & header_mask) alloc->deallocate_string(dest);
			
			// mark the string as not allocated
			dest = 0;
			header &= ~header_mask;

			return true;
		}
		else if (dest && strcpy_insitu_allow(source_length, header, header_mask, dest))
		{
			// we can reuse old buffer, so just copy the new data (including zero terminator)
			memcpy(dest, source, (source_length + 1) * sizeof(char_t));
			
			return true;
		}
		else
		{
			xml_allocator* alloc = reinterpret_cast<xml_memory_page*>(header & xml_memory_page_pointer_mask)->allocator;

			// allocate new buffer
			char_t* buf = alloc->allocate_string(source_length + 1);
			if (!buf) return false;

			// copy the string (including zero terminator)
			memcpy(buf, source, (source_length + 1) * sizeof(char_t));

			// deallocate old buffer (*after* the above to protect against overlapping memory and/or allocation failures)
			if (header & header_mask) alloc->deallocate_string(dest);
			
			// the string is now allocated, so set the flag
			dest = buf;
			header |= header_mask;

			return true;
		}
	}

	struct gap
	{
		char_t* end;
		size_t size;
			
		gap(): end(0), size(0)
		{
		}
			
		// Push new gap, move s count bytes further (skipping the gap).
		// Collapse previous gap.
		void push(char_t*& s, size_t count)
		{
			if (end) // there was a gap already; collapse it
			{
				// Move [old_gap_end, new_gap_start) to [old_gap_start, ...)
				assert(s >= end);
				memmove(end - size, end, reinterpret_cast<char*>(s) - reinterpret_cast<char*>(end));
			}
				
			s += count; // end of current gap
				
			// "merge" two gaps
			end = s;
			size += count;
		}
			
		// Collapse all gaps, return past-the-end pointer
		char_t* flush(char_t* s)
		{
			if (end)
			{
				// Move [old_gap_end, current_pos) to [old_gap_start, ...)
				assert(s >= end);
				memmove(end - size, end, reinterpret_cast<char*>(s) - reinterpret_cast<char*>(end));

				return s - size;
			}
			else return s;
		}
	};
	
	PUGI__FN char_t* strconv_escape(char_t* s, gap& g)
	{
		char_t* stre = s + 1;

		switch (*stre)
		{
			case '#':	// &#...
			{
				unsigned int ucsc = 0;

				if (stre[1] == 'x') // &#x... (hex code)
				{
					stre += 2;

					char_t ch = *stre;

					if (ch == ';') return stre;

					for (;;)
					{
						if (static_cast<unsigned int>(ch - '0') <= 9)
							ucsc = 16 * ucsc + (ch - '0');
						else if (static_cast<unsigned int>((ch | ' ') - 'a') <= 5)
							ucsc = 16 * ucsc + ((ch | ' ') - 'a' + 10);
						else if (ch == ';')
							break;
						else // cancel
							return stre;

						ch = *++stre;
					}
					
					++stre;
				}
				else	// &#... (dec code)
				{
					char_t ch = *++stre;

					if (ch == ';') return stre;

					for (;;)
					{
						if (static_cast<unsigned int>(static_cast<unsigned int>(ch) - '0') <= 9)
							ucsc = 10 * ucsc + (ch - '0');
						else if (ch == ';')
							break;
						else // cancel
							return stre;

						ch = *++stre;
					}
					
					++stre;
				}

			#ifdef PUGIXML_WCHAR_MODE
				s = reinterpret_cast<char_t*>(wchar_writer::any(reinterpret_cast<wchar_writer::value_type>(s), ucsc));
			#else
				s = reinterpret_cast<char_t*>(utf8_writer::any(reinterpret_cast<uint8_t*>(s), ucsc));
			#endif
					
				g.push(s, stre - s);
				return stre;
			}

			case 'a':	// &a
			{
				++stre;

				if (*stre == 'm') // &am
				{
					if (*++stre == 'p' && *++stre == ';') // &amp;
					{
						*s++ = '&';
						++stre;
							
						g.push(s, stre - s);
						return stre;
					}
				}
				else if (*stre == 'p') // &ap
				{
					if (*++stre == 'o' && *++stre == 's' && *++stre == ';') // &apos;
					{
						*s++ = '\'';
						++stre;

						g.push(s, stre - s);
						return stre;
					}
				}
				break;
			}

			case 'g': // &g
			{
				if (*++stre == 't' && *++stre == ';') // &gt;
				{
					*s++ = '>';
					++stre;
					
					g.push(s, stre - s);
					return stre;
				}
				break;
			}

			case 'l': // &l
			{
				if (*++stre == 't' && *++stre == ';') // &lt;
				{
					*s++ = '<';
					++stre;
						
					g.push(s, stre - s);
					return stre;
				}
				break;
			}

			case 'q': // &q
			{
				if (*++stre == 'u' && *++stre == 'o' && *++stre == 't' && *++stre == ';') // &quot;
				{
					*s++ = '"';
					++stre;
					
					g.push(s, stre - s);
					return stre;
				}
				break;
			}

			default:
				break;
		}
		
		return stre;
	}

	// Parser utilities
	#define PUGI__ENDSWITH(c, e)        ((c) == (e) || ((c) == 0 && endch == (e)))
	#define PUGI__SKIPWS()              { while (PUGI__IS_CHARTYPE(*s, ct_space)) ++s; }
	#define PUGI__OPTSET(OPT)           ( optmsk & (OPT) )
	#define PUGI__PUSHNODE(TYPE)        { cursor = append_new_node(cursor, alloc, TYPE); if (!cursor) PUGI__THROW_ERROR(status_out_of_memory, s); }
	#define PUGI__POPNODE()             { cursor = cursor->parent; }
	#define PUGI__SCANFOR(X)            { while (*s != 0 && !(X)) ++s; }
	#define PUGI__SCANWHILE(X)          { while (X) ++s; }
	#define PUGI__SCANWHILE_UNROLL(X)   { for (;;) { char_t ss = s[0]; if (PUGI__UNLIKELY(!(X))) { break; } ss = s[1]; if (PUGI__UNLIKELY(!(X))) { s += 1; break; } ss = s[2]; if (PUGI__UNLIKELY(!(X))) { s += 2; break; } ss = s[3]; if (PUGI__UNLIKELY(!(X))) { s += 3; break; } s += 4; } }
	#define PUGI__ENDSEG()              { ch = *s; *s = 0; ++s; }
	#define PUGI__THROW_ERROR(err, m)   return error_offset = m, error_status = err, static_cast<char_t*>(0)
	#define PUGI__CHECK_ERROR(err, m)   { if (*s == 0) PUGI__THROW_ERROR(err, m); }

	PUGI__FN char_t* strconv_comment(char_t* s, char_t endch)
	{
		gap g;
		
		while (true)
		{
			PUGI__SCANWHILE_UNROLL(!PUGI__IS_CHARTYPE(ss, ct_parse_comment));
		
			if (*s == '\r') // Either a single 0x0d or 0x0d 0x0a pair
			{
				*s++ = '\n'; // replace first one with 0x0a
				
				if (*s == '\n') g.push(s, 1);
			}
			else if (s[0] == '-' && s[1] == '-' && PUGI__ENDSWITH(s[2], '>')) // comment ends here
			{
				*g.flush(s) = 0;
				
				return s + (s[2] == '>' ? 3 : 2);
			}
			else if (*s == 0)
			{
				return 0;
			}
			else ++s;
		}
	}

	PUGI__FN char_t* strconv_cdata(char_t* s, char_t endch)
	{
		gap g;
			
		while (true)
		{
			PUGI__SCANWHILE_UNROLL(!PUGI__IS_CHARTYPE(ss, ct_parse_cdata));
			
			if (*s == '\r') // Either a single 0x0d or 0x0d 0x0a pair
			{
				*s++ = '\n'; // replace first one with 0x0a
				
				if (*s == '\n') g.push(s, 1);
			}
			else if (s[0] == ']' && s[1] == ']' && PUGI__ENDSWITH(s[2], '>')) // CDATA ends here
			{
				*g.flush(s) = 0;
				
				return s + 1;
			}
			else if (*s == 0)
			{
				return 0;
			}
			else ++s;
		}
	}
	
	typedef char_t* (*strconv_pcdata_t)(char_t*);
		
	template <typename opt_trim, typename opt_eol, typename opt_escape> struct strconv_pcdata_impl
	{
		static char_t* parse(char_t* s)
		{
			gap g;

			char_t* begin = s;

			while (true)
			{
				PUGI__SCANWHILE_UNROLL(!PUGI__IS_CHARTYPE(ss, ct_parse_pcdata));

				if (*s == '<') // PCDATA ends here
				{
					char_t* end = g.flush(s);

					if (opt_trim::value)
						while (end > begin && PUGI__IS_CHARTYPE(end[-1], ct_space))
							--end;

					*end = 0;
					
					return s + 1;
				}
				else if (opt_eol::value && *s == '\r') // Either a single 0x0d or 0x0d 0x0a pair
				{
					*s++ = '\n'; // replace first one with 0x0a
					
					if (*s == '\n') g.push(s, 1);
				}
				else if (opt_escape::value && *s == '&')
				{
					s = strconv_escape(s, g);
				}
				else if (*s == 0)
				{
					char_t* end = g.flush(s);

					if (opt_trim::value)
						while (end > begin && PUGI__IS_CHARTYPE(end[-1], ct_space))
							--end;

					*end = 0;

					return s;
				}
				else ++s;
			}
		}
	};
	
	PUGI__FN strconv_pcdata_t get_strconv_pcdata(unsigned int optmask)
	{
		PUGI__STATIC_ASSERT(parse_escapes == 0x10 && parse_eol == 0x20 && parse_trim_pcdata == 0x0800);

		switch (((optmask >> 4) & 3) | ((optmask >> 9) & 4)) // get bitmask for flags (eol escapes trim)
		{
		case 0: return strconv_pcdata_impl<opt_false, opt_false, opt_false>::parse;
		case 1: return strconv_pcdata_impl<opt_false, opt_false, opt_true>::parse;
		case 2: return strconv_pcdata_impl<opt_false, opt_true, opt_false>::parse;
		case 3: return strconv_pcdata_impl<opt_false, opt_true, opt_true>::parse;
		case 4: return strconv_pcdata_impl<opt_true, opt_false, opt_false>::parse;
		case 5: return strconv_pcdata_impl<opt_true, opt_false, opt_true>::parse;
		case 6: return strconv_pcdata_impl<opt_true, opt_true, opt_false>::parse;
		case 7: return strconv_pcdata_impl<opt_true, opt_true, opt_true>::parse;
		default: assert(false); return 0; // should not get here
		}
	}

	typedef char_t* (*strconv_attribute_t)(char_t*, char_t);
	
	template <typename opt_escape> struct strconv_attribute_impl
	{
		static char_t* parse_wnorm(char_t* s, char_t end_quote)
		{
			gap g;

			// trim leading whitespaces
			if (PUGI__IS_CHARTYPE(*s, ct_space))
			{
				char_t* str = s;
				
				do ++str;
				while (PUGI__IS_CHARTYPE(*str, ct_space));
				
				g.push(s, str - s);
			}

			while (true)
			{
				PUGI__SCANWHILE_UNROLL(!PUGI__IS_CHARTYPE(ss, ct_parse_attr_ws | ct_space));
				
				if (*s == end_quote)
				{
					char_t* str = g.flush(s);
					
					do *str-- = 0;
					while (PUGI__IS_CHARTYPE(*str, ct_space));
				
					return s + 1;
				}
				else if (PUGI__IS_CHARTYPE(*s, ct_space))
				{
					*s++ = ' ';
		
					if (PUGI__IS_CHARTYPE(*s, ct_space))
					{
						char_t* str = s + 1;
						while (PUGI__IS_CHARTYPE(*str, ct_space)) ++str;
						
						g.push(s, str - s);
					}
				}
				else if (opt_escape::value && *s == '&')
				{
					s = strconv_escape(s, g);
				}
				else if (!*s)
				{
					return 0;
				}
				else ++s;
			}
		}

		static char_t* parse_wconv(char_t* s, char_t end_quote)
		{
			gap g;

			while (true)
			{
				PUGI__SCANWHILE_UNROLL(!PUGI__IS_CHARTYPE(ss, ct_parse_attr_ws));
				
				if (*s == end_quote)
				{
					*g.flush(s) = 0;
				
					return s + 1;
				}
				else if (PUGI__IS_CHARTYPE(*s, ct_space))
				{
					if (*s == '\r')
					{
						*s++ = ' ';
				
						if (*s == '\n') g.push(s, 1);
					}
					else *s++ = ' ';
				}
				else if (opt_escape::value && *s == '&')
				{
					s = strconv_escape(s, g);
				}
				else if (!*s)
				{
					return 0;
				}
				else ++s;
			}
		}

		static char_t* parse_eol(char_t* s, char_t end_quote)
		{
			gap g;

			while (true)
			{
				PUGI__SCANWHILE_UNROLL(!PUGI__IS_CHARTYPE(ss, ct_parse_attr));
				
				if (*s == end_quote)
				{
					*g.flush(s) = 0;
				
					return s + 1;
				}
				else if (*s == '\r')
				{
					*s++ = '\n';
					
					if (*s == '\n') g.push(s, 1);
				}
				else if (opt_escape::value && *s == '&')
				{
					s = strconv_escape(s, g);
				}
				else if (!*s)
				{
					return 0;
				}
				else ++s;
			}
		}

		static char_t* parse_simple(char_t* s, char_t end_quote)
		{
			gap g;

			while (true)
			{
				PUGI__SCANWHILE_UNROLL(!PUGI__IS_CHARTYPE(ss, ct_parse_attr));
				
				if (*s == end_quote)
				{
					*g.flush(s) = 0;
				
					return s + 1;
				}
				else if (opt_escape::value && *s == '&')
				{
					s = strconv_escape(s, g);
				}
				else if (!*s)
				{
					return 0;
				}
				else ++s;
			}
		}
	};

	PUGI__FN strconv_attribute_t get_strconv_attribute(unsigned int optmask)
	{
		PUGI__STATIC_ASSERT(parse_escapes == 0x10 && parse_eol == 0x20 && parse_wconv_attribute == 0x40 && parse_wnorm_attribute == 0x80);
		
		switch ((optmask >> 4) & 15) // get bitmask for flags (wconv wnorm eol escapes)
		{
		case 0:  return strconv_attribute_impl<opt_false>::parse_simple;
		case 1:  return strconv_attribute_impl<opt_true>::parse_simple;
		case 2:  return strconv_attribute_impl<opt_false>::parse_eol;
		case 3:  return strconv_attribute_impl<opt_true>::parse_eol;
		case 4:  return strconv_attribute_impl<opt_false>::parse_wconv;
		case 5:  return strconv_attribute_impl<opt_true>::parse_wconv;
		case 6:  return strconv_attribute_impl<opt_false>::parse_wconv;
		case 7:  return strconv_attribute_impl<opt_true>::parse_wconv;
		case 8:  return strconv_attribute_impl<opt_false>::parse_wnorm;
		case 9:  return strconv_attribute_impl<opt_true>::parse_wnorm;
		case 10: return strconv_attribute_impl<opt_false>::parse_wnorm;
		case 11: return strconv_attribute_impl<opt_true>::parse_wnorm;
		case 12: return strconv_attribute_impl<opt_false>::parse_wnorm;
		case 13: return strconv_attribute_impl<opt_true>::parse_wnorm;
		case 14: return strconv_attribute_impl<opt_false>::parse_wnorm;
		case 15: return strconv_attribute_impl<opt_true>::parse_wnorm;
		default: assert(false); return 0; // should not get here
		}
	}

	inline xml_parse_result make_parse_result(xml_parse_status status, ptrdiff_t offset = 0)
	{
		xml_parse_result result;
		result.status = status;
		result.offset = offset;

		return result;
	}

	struct xml_parser
	{
		xml_allocator alloc;
		char_t* error_offset;
		xml_parse_status error_status;
		
		xml_parser(const xml_allocator& alloc_): alloc(alloc_), error_offset(0), error_status(status_ok)
		{
		}

		// DOCTYPE consists of nested sections of the following possible types:
		// <!-- ... -->, <? ... ?>, "...", '...'
		// <![...]]>
		// <!...>
		// First group can not contain nested groups
		// Second group can contain nested groups of the same type
		// Third group can contain all other groups
		char_t* parse_doctype_primitive(char_t* s)
		{
			if (*s == '"' || *s == '\'')
			{
				// quoted string
				char_t ch = *s++;
				PUGI__SCANFOR(*s == ch);
				if (!*s) PUGI__THROW_ERROR(status_bad_doctype, s);

				s++;
			}
			else if (s[0] == '<' && s[1] == '?')
			{
				// <? ... ?>
				s += 2;
				PUGI__SCANFOR(s[0] == '?' && s[1] == '>'); // no need for ENDSWITH because ?> can't terminate proper doctype
				if (!*s) PUGI__THROW_ERROR(status_bad_doctype, s);

				s += 2;
			}
			else if (s[0] == '<' && s[1] == '!' && s[2] == '-' && s[3] == '-')
			{
				s += 4;
				PUGI__SCANFOR(s[0] == '-' && s[1] == '-' && s[2] == '>'); // no need for ENDSWITH because --> can't terminate proper doctype
				if (!*s) PUGI__THROW_ERROR(status_bad_doctype, s);

				s += 4;
			}
			else PUGI__THROW_ERROR(status_bad_doctype, s);

			return s;
		}

		char_t* parse_doctype_ignore(char_t* s)
		{
			assert(s[0] == '<' && s[1] == '!' && s[2] == '[');
			s++;

			while (*s)
			{
				if (s[0] == '<' && s[1] == '!' && s[2] == '[')
				{
					// nested ignore section
					s = parse_doctype_ignore(s);
					if (!s) return s;
				}
				else if (s[0] == ']' && s[1] == ']' && s[2] == '>')
				{
					// ignore section end
					s += 3;

					return s;
				}
				else s++;
			}

			PUGI__THROW_ERROR(status_bad_doctype, s);
		}

		char_t* parse_doctype_group(char_t* s, char_t endch, bool toplevel)
		{
			assert((s[0] == '<' || s[0] == 0) && s[1] == '!');
			s++;

			while (*s)
			{
				if (s[0] == '<' && s[1] == '!' && s[2] != '-')
				{
					if (s[2] == '[')
					{
						// ignore
						s = parse_doctype_ignore(s);
						if (!s) return s;
					}
					else
					{
						// some control group
						s = parse_doctype_group(s, endch, false);
						if (!s) return s;

						// skip >
						assert(*s == '>');
						s++;
					}
				}
				else if (s[0] == '<' || s[0] == '"' || s[0] == '\'')
				{
					// unknown tag (forbidden), or some primitive group
					s = parse_doctype_primitive(s);
					if (!s) return s;
				}
				else if (*s == '>')
				{
					return s;
				}
				else s++;
			}

			if (!toplevel || endch != '>') PUGI__THROW_ERROR(status_bad_doctype, s);

			return s;
		}

		char_t* parse_exclamation(char_t* s, xml_node_struct* cursor, unsigned int optmsk, char_t endch)
		{
			// parse node contents, starting with exclamation mark
			++s;

			if (*s == '-') // '<!-...'
			{
				++s;

				if (*s == '-') // '<!--...'
				{
					++s;

					if (PUGI__OPTSET(parse_comments))
					{
						PUGI__PUSHNODE(node_comment); // Append a new node on the tree.
						cursor->value = s; // Save the offset.
					}

					if (PUGI__OPTSET(parse_eol) && PUGI__OPTSET(parse_comments))
					{
						s = strconv_comment(s, endch);

						if (!s) PUGI__THROW_ERROR(status_bad_comment, cursor->value);
					}
					else
					{
						// Scan for terminating '-->'.
						PUGI__SCANFOR(s[0] == '-' && s[1] == '-' && PUGI__ENDSWITH(s[2], '>'));
						PUGI__CHECK_ERROR(status_bad_comment, s);

						if (PUGI__OPTSET(parse_comments))
							*s = 0; // Zero-terminate this segment at the first terminating '-'.

						s += (s[2] == '>' ? 3 : 2); // Step over the '\0->'.
					}
				}
				else PUGI__THROW_ERROR(status_bad_comment, s);
			}
			else if (*s == '[')
			{
				// '<![CDATA[...'
				if (*++s=='C' && *++s=='D' && *++s=='A' && *++s=='T' && *++s=='A' && *++s == '[')
				{
					++s;

					if (PUGI__OPTSET(parse_cdata))
					{
						PUGI__PUSHNODE(node_cdata); // Append a new node on the tree.
						cursor->value = s; // Save the offset.

						if (PUGI__OPTSET(parse_eol))
						{
							s = strconv_cdata(s, endch);

							if (!s) PUGI__THROW_ERROR(status_bad_cdata, cursor->value);
						}
						else
						{
							// Scan for terminating ']]>'.
							PUGI__SCANFOR(s[0] == ']' && s[1] == ']' && PUGI__ENDSWITH(s[2], '>'));
							PUGI__CHECK_ERROR(status_bad_cdata, s);

							*s++ = 0; // Zero-terminate this segment.
						}
					}
					else // Flagged for discard, but we still have to scan for the terminator.
					{
						// Scan for terminating ']]>'.
						PUGI__SCANFOR(s[0] == ']' && s[1] == ']' && PUGI__ENDSWITH(s[2], '>'));
						PUGI__CHECK_ERROR(status_bad_cdata, s);

						++s;
					}

					s += (s[1] == '>' ? 2 : 1); // Step over the last ']>'.
				}
				else PUGI__THROW_ERROR(status_bad_cdata, s);
			}
			else if (s[0] == 'D' && s[1] == 'O' && s[2] == 'C' && s[3] == 'T' && s[4] == 'Y' && s[5] == 'P' && PUGI__ENDSWITH(s[6], 'E'))
			{
				s -= 2;

				if (cursor->parent) PUGI__THROW_ERROR(status_bad_doctype, s);

				char_t* mark = s + 9;

				s = parse_doctype_group(s, endch, true);
				if (!s) return s;

				assert((*s == 0 && endch == '>') || *s == '>');
				if (*s) *s++ = 0;

				if (PUGI__OPTSET(parse_doctype))
				{
					while (PUGI__IS_CHARTYPE(*mark, ct_space)) ++mark;

					PUGI__PUSHNODE(node_doctype);

					cursor->value = mark;
				}
			}
			else if (*s == 0 && endch == '-') PUGI__THROW_ERROR(status_bad_comment, s);
			else if (*s == 0 && endch == '[') PUGI__THROW_ERROR(status_bad_cdata, s);
			else PUGI__THROW_ERROR(status_unrecognized_tag, s);

			return s;
		}

		char_t* parse_question(char_t* s, xml_node_struct*& ref_cursor, unsigned int optmsk, char_t endch)
		{
			// load into registers
			xml_node_struct* cursor = ref_cursor;
			char_t ch = 0;

			// parse node contents, starting with question mark
			++s;

			// read PI target
			char_t* target = s;

			if (!PUGI__IS_CHARTYPE(*s, ct_start_symbol)) PUGI__THROW_ERROR(status_bad_pi, s);

			PUGI__SCANWHILE(PUGI__IS_CHARTYPE(*s, ct_symbol));
			PUGI__CHECK_ERROR(status_bad_pi, s);

			// determine node type; stricmp / strcasecmp is not portable
			bool declaration = (target[0] | ' ') == 'x' && (target[1] | ' ') == 'm' && (target[2] | ' ') == 'l' && target + 3 == s;

			if (declaration ? PUGI__OPTSET(parse_declaration) : PUGI__OPTSET(parse_pi))
			{
				if (declaration)
				{
					// disallow non top-level declarations
					if (cursor->parent) PUGI__THROW_ERROR(status_bad_pi, s);

					PUGI__PUSHNODE(node_declaration);
				}
				else
				{
					PUGI__PUSHNODE(node_pi);
				}

				cursor->name = target;

				PUGI__ENDSEG();

				// parse value/attributes
				if (ch == '?')
				{
					// empty node
					if (!PUGI__ENDSWITH(*s, '>')) PUGI__THROW_ERROR(status_bad_pi, s);
					s += (*s == '>');

					PUGI__POPNODE();
				}
				else if (PUGI__IS_CHARTYPE(ch, ct_space))
				{
					PUGI__SKIPWS();

					// scan for tag end
					char_t* value = s;

					PUGI__SCANFOR(s[0] == '?' && PUGI__ENDSWITH(s[1], '>'));
					PUGI__CHECK_ERROR(status_bad_pi, s);

					if (declaration)
					{
						// replace ending ? with / so that 'element' terminates properly
						*s = '/';

						// we exit from this function with cursor at node_declaration, which is a signal to parse() to go to LOC_ATTRIBUTES
						s = value;
					}
					else
					{
						// store value and step over >
						cursor->value = value;
						PUGI__POPNODE();

						PUGI__ENDSEG();

						s += (*s == '>');
					}
				}
				else PUGI__THROW_ERROR(status_bad_pi, s);
			}
			else
			{
				// scan for tag end
				PUGI__SCANFOR(s[0] == '?' && PUGI__ENDSWITH(s[1], '>'));
				PUGI__CHECK_ERROR(status_bad_pi, s);

				s += (s[1] == '>' ? 2 : 1);
			}

			// store from registers
			ref_cursor = cursor;

			return s;
		}

		char_t* parse_tree(char_t* s, xml_node_struct* root, unsigned int optmsk, char_t endch)
		{
			strconv_attribute_t strconv_attribute = get_strconv_attribute(optmsk);
			strconv_pcdata_t strconv_pcdata = get_strconv_pcdata(optmsk);
			
			char_t ch = 0;
			xml_node_struct* cursor = root;
			char_t* mark = s;

			while (*s != 0)
			{
				if (*s == '<')
				{
					++s;

				LOC_TAG:
					if (PUGI__IS_CHARTYPE(*s, ct_start_symbol)) // '<#...'
					{
						PUGI__PUSHNODE(node_element); // Append a new node to the tree.

						cursor->name = s;

						PUGI__SCANWHILE_UNROLL(PUGI__IS_CHARTYPE(ss, ct_symbol)); // Scan for a terminator.
						PUGI__ENDSEG(); // Save char in 'ch', terminate & step over.

						if (ch == '>')
						{
							// end of tag
						}
						else if (PUGI__IS_CHARTYPE(ch, ct_space))
						{
						LOC_ATTRIBUTES:
							while (true)
							{
								PUGI__SKIPWS(); // Eat any whitespace.
						
								if (PUGI__IS_CHARTYPE(*s, ct_start_symbol)) // <... #...
								{
									xml_attribute_struct* a = append_new_attribute(cursor, alloc); // Make space for this attribute.
									if (!a) PUGI__THROW_ERROR(status_out_of_memory, s);

									a->name = s; // Save the offset.

									PUGI__SCANWHILE_UNROLL(PUGI__IS_CHARTYPE(ss, ct_symbol)); // Scan for a terminator.
									PUGI__ENDSEG(); // Save char in 'ch', terminate & step over.

									if (PUGI__IS_CHARTYPE(ch, ct_space))
									{
										PUGI__SKIPWS(); // Eat any whitespace.

										ch = *s;
										++s;
									}
									
									if (ch == '=') // '<... #=...'
									{
										PUGI__SKIPWS(); // Eat any whitespace.

										if (*s == '"' || *s == '\'') // '<... #="...'
										{
											ch = *s; // Save quote char to avoid breaking on "''" -or- '""'.
											++s; // Step over the quote.
											a->value = s; // Save the offset.

											s = strconv_attribute(s, ch);
										
											if (!s) PUGI__THROW_ERROR(status_bad_attribute, a->value);

											// After this line the loop continues from the start;
											// Whitespaces, / and > are ok, symbols and EOF are wrong,
											// everything else will be detected
											if (PUGI__IS_CHARTYPE(*s, ct_start_symbol)) PUGI__THROW_ERROR(status_bad_attribute, s);
										}
										else PUGI__THROW_ERROR(status_bad_attribute, s);
									}
									else PUGI__THROW_ERROR(status_bad_attribute, s);
								}
								else if (*s == '/')
								{
									++s;
									
									if (*s == '>')
									{
										PUGI__POPNODE();
										s++;
										break;
									}
									else if (*s == 0 && endch == '>')
									{
										PUGI__POPNODE();
										break;
									}
									else PUGI__THROW_ERROR(status_bad_start_element, s);
								}
								else if (*s == '>')
								{
									++s;

									break;
								}
								else if (*s == 0 && endch == '>')
								{
									break;
								}
								else PUGI__THROW_ERROR(status_bad_start_element, s);
							}

							// !!!
						}
						else if (ch == '/') // '<#.../'
						{
							if (!PUGI__ENDSWITH(*s, '>')) PUGI__THROW_ERROR(status_bad_start_element, s);

							PUGI__POPNODE(); // Pop.

							s += (*s == '>');
						}
						else if (ch == 0)
						{
							// we stepped over null terminator, backtrack & handle closing tag
							--s;
							
							if (endch != '>') PUGI__THROW_ERROR(status_bad_start_element, s);
						}
						else PUGI__THROW_ERROR(status_bad_start_element, s);
					}
					else if (*s == '/')
					{
						++s;

						char_t* name = cursor->name;
						if (!name) PUGI__THROW_ERROR(status_end_element_mismatch, s);
						
						while (PUGI__IS_CHARTYPE(*s, ct_symbol))
						{
							if (*s++ != *name++) PUGI__THROW_ERROR(status_end_element_mismatch, s);
						}

						if (*name)
						{
							if (*s == 0 && name[0] == endch && name[1] == 0) PUGI__THROW_ERROR(status_bad_end_element, s);
							else PUGI__THROW_ERROR(status_end_element_mismatch, s);
						}
							
						PUGI__POPNODE(); // Pop.

						PUGI__SKIPWS();

						if (*s == 0)
						{
							if (endch != '>') PUGI__THROW_ERROR(status_bad_end_element, s);
						}
						else
						{
							if (*s != '>') PUGI__THROW_ERROR(status_bad_end_element, s);
							++s;
						}
					}
					else if (*s == '?') // '<?...'
					{
						s = parse_question(s, cursor, optmsk, endch);
						if (!s) return s;

						assert(cursor);
						if (PUGI__NODETYPE(cursor) == node_declaration) goto LOC_ATTRIBUTES;
					}
					else if (*s == '!') // '<!...'
					{
						s = parse_exclamation(s, cursor, optmsk, endch);
						if (!s) return s;
					}
					else if (*s == 0 && endch == '?') PUGI__THROW_ERROR(status_bad_pi, s);
					else PUGI__THROW_ERROR(status_unrecognized_tag, s);
				}
				else
				{
					mark = s; // Save this offset while searching for a terminator.

					PUGI__SKIPWS(); // Eat whitespace if no genuine PCDATA here.

					if (*s == '<' || !*s)
					{
						// We skipped some whitespace characters because otherwise we would take the tag branch instead of PCDATA one
						assert(mark != s);

						if (!PUGI__OPTSET(parse_ws_pcdata | parse_ws_pcdata_single) || PUGI__OPTSET(parse_trim_pcdata))
						{
							continue;
						}
						else if (PUGI__OPTSET(parse_ws_pcdata_single))
						{
							if (s[0] != '<' || s[1] != '/' || cursor->first_child) continue;
						}
					}

					if (!PUGI__OPTSET(parse_trim_pcdata))
						s = mark;
							
					if (cursor->parent || PUGI__OPTSET(parse_fragment))
					{
						PUGI__PUSHNODE(node_pcdata); // Append a new node on the tree.
						cursor->value = s; // Save the offset.

						s = strconv_pcdata(s);
								
						PUGI__POPNODE(); // Pop since this is a standalone.
						
						if (!*s) break;
					}
					else
					{
						PUGI__SCANFOR(*s == '<'); // '...<'
						if (!*s) break;
						
						++s;
					}

					// We're after '<'
					goto LOC_TAG;
				}
			}

			// check that last tag is closed
			if (cursor != root) PUGI__THROW_ERROR(status_end_element_mismatch, s);

			return s;
		}

	#ifdef PUGIXML_WCHAR_MODE
		static char_t* parse_skip_bom(char_t* s)
		{
			unsigned int bom = 0xfeff;
			return (s[0] == static_cast<wchar_t>(bom)) ? s + 1 : s;
		}
	#else
		static char_t* parse_skip_bom(char_t* s)
		{
			return (s[0] == '\xef' && s[1] == '\xbb' && s[2] == '\xbf') ? s + 3 : s;
		}
	#endif

		static bool has_element_node_siblings(xml_node_struct* node)
		{
			while (node)
			{
				if (PUGI__NODETYPE(node) == node_element) return true;

				node = node->next_sibling;
			}

			return false;
		}

		static xml_parse_result parse(char_t* buffer, size_t length, xml_document_struct* xmldoc, xml_node_struct* root, unsigned int optmsk)
		{
			// allocator object is a part of document object
			xml_allocator& alloc_ = *static_cast<xml_allocator*>(xmldoc);

			// early-out for empty documents
			if (length == 0)
				return make_parse_result(PUGI__OPTSET(parse_fragment) ? status_ok : status_no_document_element);

			// get last child of the root before parsing
			xml_node_struct* last_root_child = root->first_child ? root->first_child->prev_sibling_c : 0;
	
			// create parser on stack
			xml_parser parser(alloc_);

			// save last character and make buffer zero-terminated (speeds up parsing)
			char_t endch = buffer[length - 1];
			buffer[length - 1] = 0;
			
			// skip BOM to make sure it does not end up as part of parse output
			char_t* buffer_data = parse_skip_bom(buffer);

			// perform actual parsing
			parser.parse_tree(buffer_data, root, optmsk, endch);

			// update allocator state
			alloc_ = parser.alloc;

			xml_parse_result result = make_parse_result(parser.error_status, parser.error_offset ? parser.error_offset - buffer : 0);
			assert(result.offset >= 0 && static_cast<size_t>(result.offset) <= length);

			if (result)
			{
				// since we removed last character, we have to handle the only possible false positive (stray <)
				if (endch == '<')
					return make_parse_result(status_unrecognized_tag, length - 1);

				// check if there are any element nodes parsed
				xml_node_struct* first_root_child_parsed = last_root_child ? last_root_child->next_sibling : root->first_child;

				if (!PUGI__OPTSET(parse_fragment) && !has_element_node_siblings(first_root_child_parsed))
					return make_parse_result(status_no_document_element, length - 1);
			}
			else
			{
				// roll back offset if it occurs on a null terminator in the source buffer
				if (result.offset > 0 && static_cast<size_t>(result.offset) == length - 1 && endch == 0)
					result.offset--;
			}

			return result;
		}
	};

	// Output facilities
	PUGI__FN xml_encoding get_write_native_encoding()
	{
	#ifdef PUGIXML_WCHAR_MODE
		return get_wchar_encoding();
	#else
		return encoding_utf8;
	#endif
	}

	PUGI__FN xml_encoding get_write_encoding(xml_encoding encoding)
	{
		// replace wchar encoding with utf implementation
		if (encoding == encoding_wchar) return get_wchar_encoding();

		// replace utf16 encoding with utf16 with specific endianness
		if (encoding == encoding_utf16) return is_little_endian() ? encoding_utf16_le : encoding_utf16_be;

		// replace utf32 encoding with utf32 with specific endianness
		if (encoding == encoding_utf32) return is_little_endian() ? encoding_utf32_le : encoding_utf32_be;

		// only do autodetection if no explicit encoding is requested
		if (encoding != encoding_auto) return encoding;

		// assume utf8 encoding
		return encoding_utf8;
	}

#ifdef PUGIXML_WCHAR_MODE
	PUGI__FN size_t get_valid_length(const char_t* data, size_t length)
	{
		if (length < 1) return 0;

		// discard last character if it's the lead of a surrogate pair 
		return (sizeof(wchar_t) == 2 && static_cast<unsigned int>(static_cast<uint16_t>(data[length - 1]) - 0xD800) < 0x400) ? length - 1 : length;
	}

	PUGI__FN size_t convert_buffer_output(char_t* r_char, uint8_t* r_u8, uint16_t* r_u16, uint32_t* r_u32, const char_t* data, size_t length, xml_encoding encoding)
	{
		// only endian-swapping is required
		if (need_endian_swap_utf(encoding, get_wchar_encoding()))
		{
			convert_wchar_endian_swap(r_char, data, length);

			return length * sizeof(char_t);
		}
	
		// convert to utf8
		if (encoding == encoding_utf8)
		{
			uint8_t* dest = r_u8;
			uint8_t* end = utf_decoder<utf8_writer>::decode_wchar_block(data, length, dest);

			return static_cast<size_t>(end - dest);
		}

		// convert to utf16
		if (encoding == encoding_utf16_be || encoding == encoding_utf16_le)
		{
			uint16_t* dest = r_u16;

			// convert to native utf16
			uint16_t* end = utf_decoder<utf16_writer>::decode_wchar_block(data, length, dest);

			// swap if necessary
			xml_encoding native_encoding = is_little_endian() ? encoding_utf16_le : encoding_utf16_be;

			if (native_encoding != encoding) convert_utf_endian_swap(dest, dest, static_cast<size_t>(end - dest));

			return static_cast<size_t>(end - dest) * sizeof(uint16_t);
		}

		// convert to utf32
		if (encoding == encoding_utf32_be || encoding == encoding_utf32_le)
		{
			uint32_t* dest = r_u32;

			// convert to native utf32
			uint32_t* end = utf_decoder<utf32_writer>::decode_wchar_block(data, length, dest);

			// swap if necessary
			xml_encoding native_encoding = is_little_endian() ? encoding_utf32_le : encoding_utf32_be;

			if (native_encoding != encoding) convert_utf_endian_swap(dest, dest, static_cast<size_t>(end - dest));

			return static_cast<size_t>(end - dest) * sizeof(uint32_t);
		}

		// convert to latin1
		if (encoding == encoding_latin1)
		{
			uint8_t* dest = r_u8;
			uint8_t* end = utf_decoder<latin1_writer>::decode_wchar_block(data, length, dest);

			return static_cast<size_t>(end - dest);
		}

		assert(!"Invalid encoding");
		return 0;
	}
#else
	PUGI__FN size_t get_valid_length(const char_t* data, size_t length)
	{
		if (length < 5) return 0;

		for (size_t i = 1; i <= 4; ++i)
		{
			uint8_t ch = static_cast<uint8_t>(data[length - i]);

			// either a standalone character or a leading one
			if ((ch & 0xc0) != 0x80) return length - i;
		}

		// there are four non-leading characters at the end, sequence tail is broken so might as well process the whole chunk
		return length;
	}

	PUGI__FN size_t convert_buffer_output(char_t* /* r_char */, uint8_t* r_u8, uint16_t* r_u16, uint32_t* r_u32, const char_t* data, size_t length, xml_encoding encoding)
	{
		if (encoding == encoding_utf16_be || encoding == encoding_utf16_le)
		{
			uint16_t* dest = r_u16;

			// convert to native utf16
			uint16_t* end = utf_decoder<utf16_writer>::decode_utf8_block(reinterpret_cast<const uint8_t*>(data), length, dest);

			// swap if necessary
			xml_encoding native_encoding = is_little_endian() ? encoding_utf16_le : encoding_utf16_be;

			if (native_encoding != encoding) convert_utf_endian_swap(dest, dest, static_cast<size_t>(end - dest));

			return static_cast<size_t>(end - dest) * sizeof(uint16_t);
		}

		if (encoding == encoding_utf32_be || encoding == encoding_utf32_le)
		{
			uint32_t* dest = r_u32;

			// convert to native utf32
			uint32_t* end = utf_decoder<utf32_writer>::decode_utf8_block(reinterpret_cast<const uint8_t*>(data), length, dest);

			// swap if necessary
			xml_encoding native_encoding = is_little_endian() ? encoding_utf32_le : encoding_utf32_be;

			if (native_encoding != encoding) convert_utf_endian_swap(dest, dest, static_cast<size_t>(end - dest));

			return static_cast<size_t>(end - dest) * sizeof(uint32_t);
		}

		if (encoding == encoding_latin1)
		{
			uint8_t* dest = r_u8;
			uint8_t* end = utf_decoder<latin1_writer>::decode_utf8_block(reinterpret_cast<const uint8_t*>(data), length, dest);

			return static_cast<size_t>(end - dest);
		}

		assert(!"Invalid encoding");
		return 0;
	}
#endif

	class xml_buffered_writer
	{
		xml_buffered_writer(const xml_buffered_writer&);
		xml_buffered_writer& operator=(const xml_buffered_writer&);

	public:
		xml_buffered_writer(xml_writer& writer_, xml_encoding user_encoding): writer(writer_), bufsize(0), encoding(get_write_encoding(user_encoding))
		{
			PUGI__STATIC_ASSERT(bufcapacity >= 8);
		}

		~xml_buffered_writer()
		{
			flush();
		}

		size_t flush()
		{
			flush(buffer, bufsize);
			bufsize = 0;
			return 0;
		}

		void flush(const char_t* data, size_t size)
		{
			if (size == 0) return;

			// fast path, just write data
			if (encoding == get_write_native_encoding())
				writer.write(data, size * sizeof(char_t));
			else
			{
				// convert chunk
				size_t result = convert_buffer_output(scratch.data_char, scratch.data_u8, scratch.data_u16, scratch.data_u32, data, size, encoding);
				assert(result <= sizeof(scratch));

				// write data
				writer.write(scratch.data_u8, result);
			}
		}

		void write_direct(const char_t* data, size_t length)
		{
			// flush the remaining buffer contents
			flush();

			// handle large chunks
			if (length > bufcapacity)
			{
				if (encoding == get_write_native_encoding())
				{
					// fast path, can just write data chunk
					writer.write(data, length * sizeof(char_t));
					return;
				}

				// need to convert in suitable chunks
				while (length > bufcapacity)
				{
					// get chunk size by selecting such number of characters that are guaranteed to fit into scratch buffer
					// and form a complete codepoint sequence (i.e. discard start of last codepoint if necessary)
					size_t chunk_size = get_valid_length(data, bufcapacity);
					assert(chunk_size);

					// convert chunk and write
					flush(data, chunk_size);

					// iterate
					data += chunk_size;
					length -= chunk_size;
				}

				// small tail is copied below
				bufsize = 0;
			}

			memcpy(buffer + bufsize, data, length * sizeof(char_t));
			bufsize += length;
		}

		void write_buffer(const char_t* data, size_t length)
		{
			size_t offset = bufsize;

			if (offset + length <= bufcapacity)
			{
				memcpy(buffer + offset, data, length * sizeof(char_t));
				bufsize = offset + length;
			}
			else
			{
				write_direct(data, length);
			}
		}

		void write_string(const char_t* data)
		{
			// write the part of the string that fits in the buffer
			size_t offset = bufsize;

			while (*data && offset < bufcapacity)
				buffer[offset++] = *data++;

			// write the rest
			if (offset < bufcapacity)
			{
				bufsize = offset;
			}
			else
			{
				// backtrack a bit if we have split the codepoint
				size_t length = offset - bufsize;
				size_t extra = length - get_valid_length(data - length, length);

				bufsize = offset - extra;

				write_direct(data - extra, strlength(data) + extra);
			}
		}

		void write(char_t d0)
		{
			size_t offset = bufsize;
			if (offset > bufcapacity - 1) offset = flush();

			buffer[offset + 0] = d0;
			bufsize = offset + 1;
		}

		void write(char_t d0, char_t d1)
		{
			size_t offset = bufsize;
			if (offset > bufcapacity - 2) offset = flush();

			buffer[offset + 0] = d0;
			buffer[offset + 1] = d1;
			bufsize = offset + 2;
		}

		void write(char_t d0, char_t d1, char_t d2)
		{
			size_t offset = bufsize;
			if (offset > bufcapacity - 3) offset = flush();

			buffer[offset + 0] = d0;
			buffer[offset + 1] = d1;
			buffer[offset + 2] = d2;
			bufsize = offset + 3;
		}

		void write(char_t d0, char_t d1, char_t d2, char_t d3)
		{
			size_t offset = bufsize;
			if (offset > bufcapacity - 4) offset = flush();

			buffer[offset + 0] = d0;
			buffer[offset + 1] = d1;
			buffer[offset + 2] = d2;
			buffer[offset + 3] = d3;
			bufsize = offset + 4;
		}

		void write(char_t d0, char_t d1, char_t d2, char_t d3, char_t d4)
		{
			size_t offset = bufsize;
			if (offset > bufcapacity - 5) offset = flush();

			buffer[offset + 0] = d0;
			buffer[offset + 1] = d1;
			buffer[offset + 2] = d2;
			buffer[offset + 3] = d3;
			buffer[offset + 4] = d4;
			bufsize = offset + 5;
		}

		void write(char_t d0, char_t d1, char_t d2, char_t d3, char_t d4, char_t d5)
		{
			size_t offset = bufsize;
			if (offset > bufcapacity - 6) offset = flush();

			buffer[offset + 0] = d0;
			buffer[offset + 1] = d1;
			buffer[offset + 2] = d2;
			buffer[offset + 3] = d3;
			buffer[offset + 4] = d4;
			buffer[offset + 5] = d5;
			bufsize = offset + 6;
		}

		// utf8 maximum expansion: x4 (-> utf32)
		// utf16 maximum expansion: x2 (-> utf32)
		// utf32 maximum expansion: x1
		enum
		{
			bufcapacitybytes =
			#ifdef PUGIXML_MEMORY_OUTPUT_STACK
				PUGIXML_MEMORY_OUTPUT_STACK
			#else
				10240
			#endif
			,
			bufcapacity = bufcapacitybytes / (sizeof(char_t) + 4)
		};

		char_t buffer[bufcapacity];

		union
		{
			uint8_t data_u8[4 * bufcapacity];
			uint16_t data_u16[2 * bufcapacity];
			uint32_t data_u32[bufcapacity];
			char_t data_char[bufcapacity];
		} scratch;

		xml_writer& writer;
		size_t bufsize;
		xml_encoding encoding;
	};

	PUGI__FN void text_output_escaped(xml_buffered_writer& writer, const char_t* s, chartypex_t type)
	{
		while (*s)
		{
			const char_t* prev = s;
			
			// While *s is a usual symbol
			PUGI__SCANWHILE_UNROLL(!PUGI__IS_CHARTYPEX(ss, type));
		
			writer.write_buffer(prev, static_cast<size_t>(s - prev));

			switch (*s)
			{
				case 0: break;
				case '&':
					writer.write('&', 'a', 'm', 'p', ';');
					++s;
					break;
				case '<':
					writer.write('&', 'l', 't', ';');
					++s;
					break;
				case '>':
					writer.write('&', 'g', 't', ';');
					++s;
					break;
				case '"':
					writer.write('&', 'q', 'u', 'o', 't', ';');
					++s;
					break;
				default: // s is not a usual symbol
				{
					unsigned int ch = static_cast<unsigned int>(*s++);
					assert(ch < 32);

					writer.write('&', '#', static_cast<char_t>((ch / 10) + '0'), static_cast<char_t>((ch % 10) + '0'), ';');
				}
			}
		}
	}

	PUGI__FN void text_output(xml_buffered_writer& writer, const char_t* s, chartypex_t type, unsigned int flags)
	{
		if (flags & format_no_escapes)
			writer.write_string(s);
		else
			text_output_escaped(writer, s, type);
	}

	PUGI__FN void text_output_cdata(xml_buffered_writer& writer, const char_t* s)
	{
		do
		{
			writer.write('<', '!', '[', 'C', 'D');
			writer.write('A', 'T', 'A', '[');

			const char_t* prev = s;

			// look for ]]> sequence - we can't output it as is since it terminates CDATA
			while (*s && !(s[0] == ']' && s[1] == ']' && s[2] == '>')) ++s;

			// skip ]] if we stopped at ]]>, > will go to the next CDATA section
			if (*s) s += 2;

			writer.write_buffer(prev, static_cast<size_t>(s - prev));

			writer.write(']', ']', '>');
		}
		while (*s);
	}

	PUGI__FN void text_output_indent(xml_buffered_writer& writer, const char_t* indent, size_t indent_length, unsigned int depth)
	{
		switch (indent_length)
		{
		case 1:
		{
			for (unsigned int i = 0; i < depth; ++i)
				writer.write(indent[0]);
			break;
		}

		case 2:
		{
			for (unsigned int i = 0; i < depth; ++i)
				writer.write(indent[0], indent[1]);
			break;
		}

		case 3:
		{
			for (unsigned int i = 0; i < depth; ++i)
				writer.write(indent[0], indent[1], indent[2]);
			break;
		}

		case 4:
		{
			for (unsigned int i = 0; i < depth; ++i)
				writer.write(indent[0], indent[1], indent[2], indent[3]);
			break;
		}

		default:
		{
			for (unsigned int i = 0; i < depth; ++i)
				writer.write_buffer(indent, indent_length);
		}
		}
	}

	PUGI__FN void node_output_comment(xml_buffered_writer& writer, const char_t* s)
	{
		writer.write('<', '!', '-', '-');

		while (*s)
		{
			const char_t* prev = s;

			// look for -\0 or -- sequence - we can't output it since -- is illegal in comment body
			while (*s && !(s[0] == '-' && (s[1] == '-' || s[1] == 0))) ++s;

			writer.write_buffer(prev, static_cast<size_t>(s - prev));

			if (*s)
			{
				assert(*s == '-');

				writer.write('-', ' ');
				++s;
			}
		}

		writer.write('-', '-', '>');
	}

	PUGI__FN void node_output_attributes(xml_buffered_writer& writer, xml_node_struct* node, unsigned int flags)
	{
		const char_t* default_name = PUGIXML_TEXT(":anonymous");

		for (xml_attribute_struct* a = node->first_attribute; a; a = a->next_attribute)
		{
			writer.write(' ');
			writer.write_string(a->name ? a->name : default_name);
			writer.write('=', '"');

			if (a->value)
				text_output(writer, a->value, ctx_special_attr, flags);

			writer.write('"');
		}
	}

	PUGI__FN bool node_output_start(xml_buffered_writer& writer, xml_node_struct* node, unsigned int flags)
	{
		const char_t* default_name = PUGIXML_TEXT(":anonymous");
		const char_t* name = node->name ? node->name : default_name;

		writer.write('<');
		writer.write_string(name);

		if (node->first_attribute)
			node_output_attributes(writer, node, flags);

		if (flags & format_raw)
		{
			if (!node->first_child)
				writer.write(' ', '/', '>');
			else
			{
				writer.write('>');

				return true;
			}
		}
		else
		{
			xml_node_struct* first = node->first_child;

			if (!first)
				writer.write(' ', '/', '>', '\n');
			else if (!first->next_sibling && (PUGI__NODETYPE(first) == node_pcdata || PUGI__NODETYPE(first) == node_cdata))
			{
				writer.write('>');

				const char_t* value = first->value ? first->value : PUGIXML_TEXT("");

				if (PUGI__NODETYPE(first) == node_pcdata)
					text_output(writer, value, ctx_special_pcdata, flags);
				else
					text_output_cdata(writer, value);

				writer.write('<', '/');
				writer.write_string(name);
				writer.write('>', '\n');
			}
			else
			{
				writer.write('>', '\n');

				return true;
			}
		}

		return false;
	}

	PUGI__FN void node_output_end(xml_buffered_writer& writer, xml_node_struct* node, unsigned int flags)
	{
		const char_t* default_name = PUGIXML_TEXT(":anonymous");
		const char_t* name = node->name ? node->name : default_name;

		writer.write('<', '/');
		writer.write_string(name);

		if (flags & format_raw)
			writer.write('>');
		else
			writer.write('>', '\n');
	}

	PUGI__FN void node_output_simple(xml_buffered_writer& writer, xml_node_struct* node, unsigned int flags)
	{
		const char_t* default_name = PUGIXML_TEXT(":anonymous");

		switch (PUGI__NODETYPE(node))
		{
			case node_pcdata:
				text_output(writer, node->value ? node->value : PUGIXML_TEXT(""), ctx_special_pcdata, flags);
				if ((flags & format_raw) == 0) writer.write('\n');
				break;

			case node_cdata:
				text_output_cdata(writer, node->value ? node->value : PUGIXML_TEXT(""));
				if ((flags & format_raw) == 0) writer.write('\n');
				break;

			case node_comment:
				node_output_comment(writer, node->value ? node->value : PUGIXML_TEXT(""));
				if ((flags & format_raw) == 0) writer.write('\n');
				break;

			case node_pi:
				writer.write('<', '?');
				writer.write_string(node->name ? node->name : default_name);

				if (node->value)
				{
					writer.write(' ');
					writer.write_string(node->value);
				}

				writer.write('?', '>');
				if ((flags & format_raw) == 0) writer.write('\n');
				break;

			case node_declaration:
				writer.write('<', '?');
				writer.write_string(node->name ? node->name : default_name);
				node_output_attributes(writer, node, flags);
				writer.write('?', '>');
				if ((flags & format_raw) == 0) writer.write('\n');
				break;

			case node_doctype:
				writer.write('<', '!', 'D', 'O', 'C');
				writer.write('T', 'Y', 'P', 'E');

				if (node->value)
				{
					writer.write(' ');
					writer.write_string(node->value);
				}

				writer.write('>');
				if ((flags & format_raw) == 0) writer.write('\n');
				break;

			default:
				assert(!"Invalid node type");
		}
	}

	PUGI__FN void node_output(xml_buffered_writer& writer, xml_node_struct* root, const char_t* indent, unsigned int flags, unsigned int depth)
	{
		size_t indent_length = ((flags & (format_indent | format_raw)) == format_indent) ? strlength(indent) : 0;

		xml_node_struct* node = root;

		do
		{
			assert(node);

			// begin writing current node
			if (indent_length)
				text_output_indent(writer, indent, indent_length, depth);

			if (PUGI__NODETYPE(node) == node_element)
			{
				if (node_output_start(writer, node, flags))
				{
					node = node->first_child;
					depth++;
					continue;
				}
			}
			else if (PUGI__NODETYPE(node) == node_document)
			{
				if (node->first_child)
				{
					node = node->first_child;
					continue;
				}
			}
			else
			{
				node_output_simple(writer, node, flags);
			}

			// continue to the next node
			while (node != root)
			{
				if (node->next_sibling)
				{
					node = node->next_sibling;
					break;
				}

				node = node->parent;

				// write closing node
				if (PUGI__NODETYPE(node) == node_element)
				{
					depth--;

					if (indent_length)
						text_output_indent(writer, indent, indent_length, depth);

					node_output_end(writer, node, flags);
				}
			}
		}
		while (node != root);
	}

	PUGI__FN bool has_declaration(xml_node_struct* node)
	{
		for (xml_node_struct* child = node->first_child; child; child = child->next_sibling)
		{
			xml_node_type type = PUGI__NODETYPE(child);

			if (type == node_declaration) return true;
			if (type == node_element) return false;
		}

		return false;
	}

	PUGI__FN bool is_attribute_of(xml_attribute_struct* attr, xml_node_struct* node)
	{
		for (xml_attribute_struct* a = node->first_attribute; a; a = a->next_attribute)
			if (a == attr)
				return true;

		return false;
	}

	PUGI__FN bool allow_insert_attribute(xml_node_type parent)
	{
		return parent == node_element || parent == node_declaration;
	}

	PUGI__FN bool allow_insert_child(xml_node_type parent, xml_node_type child)
	{
		if (parent != node_document && parent != node_element) return false;
		if (child == node_document || child == node_null) return false;
		if (parent != node_document && (child == node_declaration || child == node_doctype)) return false;

		return true;
	}

	PUGI__FN bool allow_move(xml_node parent, xml_node child)
	{
		// check that child can be a child of parent
		if (!allow_insert_child(parent.type(), child.type()))
			return false;

		// check that node is not moved between documents
		if (parent.root() != child.root())
			return false;

		// check that new parent is not in the child subtree
		xml_node cur = parent;

		while (cur)
		{
			if (cur == child)
				return false;

			cur = cur.parent();
		}

		return true;
	}

	PUGI__FN void node_copy_string(char_t*& dest, uintptr_t& header, uintptr_t header_mask, char_t* source, uintptr_t& source_header, xml_allocator* alloc)
	{
		assert(!dest && (header & header_mask) == 0);

		if (source)
		{
			if (alloc && (source_header & header_mask) == 0)
			{
				dest = source;

				// since strcpy_insitu can reuse document buffer memory we need to mark both source and dest as shared
				header |= xml_memory_page_contents_shared_mask;
				source_header |= xml_memory_page_contents_shared_mask;
			}
			else
				strcpy_insitu(dest, header, header_mask, source);
		}
	}

	PUGI__FN void node_copy_contents(xml_node_struct* dn, xml_node_struct* sn, xml_allocator* shared_alloc)
	{
		node_copy_string(dn->name, dn->header, xml_memory_page_name_allocated_mask, sn->name, sn->header, shared_alloc);
		node_copy_string(dn->value, dn->header, xml_memory_page_value_allocated_mask, sn->value, sn->header, shared_alloc);

		for (xml_attribute_struct* sa = sn->first_attribute; sa; sa = sa->next_attribute)
		{
			xml_attribute_struct* da = append_new_attribute(dn, get_allocator(dn));

			if (da)
			{
				node_copy_string(da->name, da->header, xml_memory_page_name_allocated_mask, sa->name, sa->header, shared_alloc);
				node_copy_string(da->value, da->header, xml_memory_page_value_allocated_mask, sa->value, sa->header, shared_alloc);
			}
		}
	}

	PUGI__FN void node_copy_tree(xml_node_struct* dn, xml_node_struct* sn)
	{
		xml_allocator& alloc = get_allocator(dn);
		xml_allocator* shared_alloc = (&alloc == &get_allocator(sn)) ? &alloc : 0;

		node_copy_contents(dn, sn, shared_alloc);

		xml_node_struct* dit = dn;
		xml_node_struct* sit = sn->first_child;

		while (sit && sit != sn)
		{
			if (sit != dn)
			{
				xml_node_struct* copy = append_new_node(dit, alloc, PUGI__NODETYPE(sit));

				if (copy)
				{
					node_copy_contents(copy, sit, shared_alloc);

					if (sit->first_child)
					{
						dit = copy;
						sit = sit->first_child;
						continue;
					}
				}
			}

			// continue to the next node
			do
			{
				if (sit->next_sibling)
				{
					sit = sit->next_sibling;
					break;
				}

				sit = sit->parent;
				dit = dit->parent;
			}
			while (sit != sn);
		}
	}

	inline bool is_text_node(xml_node_struct* node)
	{
		xml_node_type type = PUGI__NODETYPE(node);

		return type == node_pcdata || type == node_cdata;
	}

	// get value with conversion functions
	PUGI__FN int get_integer_base(const char_t* value)
	{
		const char_t* s = value;

		while (PUGI__IS_CHARTYPE(*s, ct_space))
			s++;

		if (*s == '-')
			s++;

		return (s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) ? 16 : 10;
	}

	PUGI__FN int get_value_int(const char_t* value, int def)
	{
		if (!value) return def;

		int base = get_integer_base(value);

	#ifdef PUGIXML_WCHAR_MODE
		return static_cast<int>(wcstol(value, 0, base));
	#else
		return static_cast<int>(strtol(value, 0, base));
	#endif
	}

	PUGI__FN unsigned int get_value_uint(const char_t* value, unsigned int def)
	{
		if (!value) return def;

		int base = get_integer_base(value);

	#ifdef PUGIXML_WCHAR_MODE
		return static_cast<unsigned int>(wcstoul(value, 0, base));
	#else
		return static_cast<unsigned int>(strtoul(value, 0, base));
	#endif
	}

	PUGI__FN double get_value_double(const char_t* value, double def)
	{
		if (!value) return def;

	#ifdef PUGIXML_WCHAR_MODE
		return wcstod(value, 0);
	#else
		return strtod(value, 0);
	#endif
	}

	PUGI__FN float get_value_float(const char_t* value, float def)
	{
		if (!value) return def;

	#ifdef PUGIXML_WCHAR_MODE
		return static_cast<float>(wcstod(value, 0));
	#else
		return static_cast<float>(strtod(value, 0));
	#endif
	}

	PUGI__FN bool get_value_bool(const char_t* value, bool def)
	{
		if (!value) return def;

		// only look at first char
		char_t first = *value;

		// 1*, t* (true), T* (True), y* (yes), Y* (YES)
		return (first == '1' || first == 't' || first == 'T' || first == 'y' || first == 'Y');
	}

#ifdef PUGIXML_HAS_LONG_LONG
	PUGI__FN long long get_value_llong(const char_t* value, long long def)
	{
		if (!value) return def;

		int base = get_integer_base(value);

	#ifdef PUGIXML_WCHAR_MODE
		#ifdef PUGI__MSVC_CRT_VERSION
			return _wcstoi64(value, 0, base);
		#else
			return wcstoll(value, 0, base);
		#endif
	#else
		#ifdef PUGI__MSVC_CRT_VERSION
			return _strtoi64(value, 0, base);
		#else
			return strtoll(value, 0, base);
		#endif
	#endif
	}

	PUGI__FN unsigned long long get_value_ullong(const char_t* value, unsigned long long def)
	{
		if (!value) return def;

		int base = get_integer_base(value);

	#ifdef PUGIXML_WCHAR_MODE
		#ifdef PUGI__MSVC_CRT_VERSION
			return _wcstoui64(value, 0, base);
		#else
			return wcstoull(value, 0, base);
		#endif
	#else
		#ifdef PUGI__MSVC_CRT_VERSION
			return _strtoui64(value, 0, base);
		#else
			return strtoull(value, 0, base);
		#endif
	#endif
	}
#endif

	// set value with conversion functions
	PUGI__FN bool set_value_buffer(char_t*& dest, uintptr_t& header, uintptr_t header_mask, char (&buf)[128])
	{
	#ifdef PUGIXML_WCHAR_MODE
		char_t wbuf[128];
		impl::widen_ascii(wbuf, buf);

		return strcpy_insitu(dest, header, header_mask, wbuf);
	#else
		return strcpy_insitu(dest, header, header_mask, buf);
	#endif
	}

	PUGI__FN bool set_value_convert(char_t*& dest, uintptr_t& header, uintptr_t header_mask, int value)
	{
		char buf[128];
		snprintf(buf, sizeof(buf), "%d", value);
	
		return set_value_buffer(dest, header, header_mask, buf);
	}

	PUGI__FN bool set_value_convert(char_t*& dest, uintptr_t& header, uintptr_t header_mask, unsigned int value)
	{
		char buf[128];
		snprintf(buf, sizeof(buf), "%u", value);

		return set_value_buffer(dest, header, header_mask, buf);
	}

	PUGI__FN bool set_value_convert(char_t*& dest, uintptr_t& header, uintptr_t header_mask, double value)
	{
		char buf[128];
		snprintf(buf, sizeof(buf), "%g", value);

		return set_value_buffer(dest, header, header_mask, buf);
	}
	
	PUGI__FN bool set_value_convert(char_t*& dest, uintptr_t& header, uintptr_t header_mask, bool value)
	{
		return strcpy_insitu(dest, header, header_mask, value ? PUGIXML_TEXT("true") : PUGIXML_TEXT("false"));
	}

#ifdef PUGIXML_HAS_LONG_LONG
	PUGI__FN bool set_value_convert(char_t*& dest, uintptr_t& header, uintptr_t header_mask, long long value)
	{
		char buf[128];
		snprintf(buf, sizeof(buf), "%lld", value);
	
		return set_value_buffer(dest, header, header_mask, buf);
	}

	PUGI__FN bool set_value_convert(char_t*& dest, uintptr_t& header, uintptr_t header_mask, unsigned long long value)
	{
		char buf[128];
		snprintf(buf, sizeof(buf), "%llu", value);
	
		return set_value_buffer(dest, header, header_mask, buf);
	}
#endif

	// we need to get length of entire file to load it in memory; the only (relatively) sane way to do it is via seek/tell trick
	PUGI__FN xml_parse_status get_file_size(FILE* file, size_t& out_result)
	{
	#if defined(PUGI__MSVC_CRT_VERSION) && PUGI__MSVC_CRT_VERSION >= 1400 && !defined(_WIN32_WCE)
		// there are 64-bit versions of fseek/ftell, let's use them
		typedef __int64 length_type;

		_fseeki64(file, 0, SEEK_END);
		length_type length = _ftelli64(file);
		_fseeki64(file, 0, SEEK_SET);
	#elif defined(__MINGW32__) && !defined(__NO_MINGW_LFS) && (!defined(__STRICT_ANSI__) || defined(__MINGW64_VERSION_MAJOR))
		// there are 64-bit versions of fseek/ftell, let's use them
		typedef off64_t length_type;

		fseeko64(file, 0, SEEK_END);
		length_type length = ftello64(file);
		fseeko64(file, 0, SEEK_SET);
	#else
		// if this is a 32-bit OS, long is enough; if this is a unix system, long is 64-bit, which is enough; otherwise we can't do anything anyway.
		typedef long length_type;

		fseek(file, 0, SEEK_END);
		length_type length = ftell(file);
		fseek(file, 0, SEEK_SET);
	#endif

		// check for I/O errors
		if (length < 0) return status_io_error;
		
		// check for overflow
		size_t result = static_cast<size_t>(length);

		if (static_cast<length_type>(result) != length) return status_out_of_memory;

		// finalize
		out_result = result;

		return status_ok;
	}

	PUGI__FN size_t zero_terminate_buffer(void* buffer, size_t size, xml_encoding encoding) 
	{
		// We only need to zero-terminate if encoding conversion does not do it for us
	#ifdef PUGIXML_WCHAR_MODE
		xml_encoding wchar_encoding = get_wchar_encoding();

		if (encoding == wchar_encoding || need_endian_swap_utf(encoding, wchar_encoding))
		{
			size_t length = size / sizeof(char_t);

			static_cast<char_t*>(buffer)[length] = 0;
			return (length + 1) * sizeof(char_t);
		}
	#else
		if (encoding == encoding_utf8)
		{
			static_cast<char*>(buffer)[size] = 0;
			return size + 1;
		}
	#endif

		return size;
	}

	PUGI__FN xml_parse_result load_file_impl(xml_document& doc, FILE* file, unsigned int options, xml_encoding encoding)
	{
		if (!file) return make_parse_result(status_file_not_found);

		// get file size (can result in I/O errors)
		size_t size = 0;
		xml_parse_status size_status = get_file_size(file, size);

		if (size_status != status_ok)
		{
			fclose(file);
			return make_parse_result(size_status);
		}
		
		size_t max_suffix_size = sizeof(char_t);

		// allocate buffer for the whole file
		char* contents = static_cast<char*>(xml_memory::allocate(size + max_suffix_size));

		if (!contents)
		{
			fclose(file);
			return make_parse_result(status_out_of_memory);
		}

		// read file in memory
		size_t read_size = fread(contents, 1, size, file);
		fclose(file);

		if (read_size != size)
		{
			xml_memory::deallocate(contents);
			return make_parse_result(status_io_error);
		}

		xml_encoding real_encoding = get_buffer_encoding(encoding, contents, size);
		
		return doc.load_buffer_inplace_own(contents, zero_terminate_buffer(contents, size, real_encoding), options, real_encoding);
	}

#ifndef PUGIXML_NO_STL
// Removed the XPATH and STL module, by Milan Straka
#endif

#if defined(PUGI__MSVC_CRT_VERSION) || defined(__BORLANDC__) || (defined(__MINGW32__) && (!defined(__STRICT_ANSI__) || defined(__MINGW64_VERSION_MAJOR)))
	PUGI__FN FILE* open_file_wide(const wchar_t* path, const wchar_t* mode)
	{
		return _wfopen(path, mode);
	}
#else
	PUGI__FN char* convert_path_heap(const wchar_t* str)
	{
		assert(str);

		// first pass: get length in utf8 characters
		size_t length = strlength_wide(str);
		size_t size = as_utf8_begin(str, length);

		// allocate resulting string
		char* result = static_cast<char*>(xml_memory::allocate(size + 1));
		if (!result) return 0;

		// second pass: convert to utf8
		as_utf8_end(result, size, str, length);

		return result;
	}

	PUGI__FN FILE* open_file_wide(const wchar_t* path, const wchar_t* mode)
	{
		// there is no standard function to open wide paths, so our best bet is to try utf8 path
		char* path_utf8 = convert_path_heap(path);
		if (!path_utf8) return 0;

		// convert mode to ASCII (we mirror _wfopen interface)
		char mode_ascii[4] = {0};
		for (size_t i = 0; mode[i]; ++i) mode_ascii[i] = static_cast<char>(mode[i]);

		// try to open the utf8 path
		FILE* result = fopen(path_utf8, mode_ascii);

		// free dummy buffer
		xml_memory::deallocate(path_utf8);

		return result;
	}
#endif

	PUGI__FN bool save_file_impl(const xml_document& doc, FILE* file, const char_t* indent, unsigned int flags, xml_encoding encoding)
	{
		if (!file) return false;

		xml_writer_file writer(file);
		doc.save(writer, indent, flags, encoding);

		int result = ferror(file);

		fclose(file);

		return result == 0;
	}

	PUGI__FN xml_parse_result load_buffer_impl(xml_document_struct* doc, xml_node_struct* root, void* contents, size_t size, unsigned int options, xml_encoding encoding, bool is_mutable, bool own, char_t** out_buffer)
	{
		// check input buffer
		assert(contents || size == 0);

		// get actual encoding
		xml_encoding buffer_encoding = impl::get_buffer_encoding(encoding, contents, size);

		// get private buffer
		char_t* buffer = 0;
		size_t length = 0;

		if (!impl::convert_buffer(buffer, length, buffer_encoding, contents, size, is_mutable)) return impl::make_parse_result(status_out_of_memory);
		
		// delete original buffer if we performed a conversion
		if (own && buffer != contents && contents) impl::xml_memory::deallocate(contents);

		// store buffer for offset_debug
		doc->buffer = buffer;

		// parse
		xml_parse_result res = impl::xml_parser::parse(buffer, length, doc, root, options);

		// remember encoding
		res.encoding = buffer_encoding;

		// grab onto buffer if it's our buffer, user is responsible for deallocating contents himself
		if (own || buffer != contents) *out_buffer = buffer;

		return res;
	}
PUGI__NS_END

namespace ufal {
namespace microrestd {
namespace pugi
{
	PUGI__FN xml_writer_file::xml_writer_file(void* file_): file(file_)
	{
	}

	PUGI__FN void xml_writer_file::write(const void* data, size_t size)
	{
		size_t result = fwrite(data, 1, size, static_cast<FILE*>(file));
		(void)!result; // unfortunately we can't do proper error handling here
	}

#ifndef PUGIXML_NO_STL
// Removed the XPATH and STL module, by Milan Straka
#endif

	PUGI__FN xml_tree_walker::xml_tree_walker(): _depth(0)
	{
	}
	
	PUGI__FN xml_tree_walker::~xml_tree_walker()
	{
	}

	PUGI__FN int xml_tree_walker::depth() const
	{
		return _depth;
	}

	PUGI__FN bool xml_tree_walker::begin(xml_node&)
	{
		return true;
	}

	PUGI__FN bool xml_tree_walker::end(xml_node&)
	{
		return true;
	}

	PUGI__FN xml_attribute::xml_attribute(): _attr(0)
	{
	}

	PUGI__FN xml_attribute::xml_attribute(xml_attribute_struct* attr): _attr(attr)
	{
	}

	PUGI__FN static void unspecified_bool_xml_attribute(xml_attribute***)
	{
	}

	PUGI__FN xml_attribute::operator xml_attribute::unspecified_bool_type() const
	{
		return _attr ? unspecified_bool_xml_attribute : 0;
	}

	PUGI__FN bool xml_attribute::operator!() const
	{
		return !_attr;
	}

	PUGI__FN bool xml_attribute::operator==(const xml_attribute& r) const
	{
		return (_attr == r._attr);
	}
	
	PUGI__FN bool xml_attribute::operator!=(const xml_attribute& r) const
	{
		return (_attr != r._attr);
	}

	PUGI__FN bool xml_attribute::operator<(const xml_attribute& r) const
	{
		return (_attr < r._attr);
	}
	
	PUGI__FN bool xml_attribute::operator>(const xml_attribute& r) const
	{
		return (_attr > r._attr);
	}
	
	PUGI__FN bool xml_attribute::operator<=(const xml_attribute& r) const
	{
		return (_attr <= r._attr);
	}
	
	PUGI__FN bool xml_attribute::operator>=(const xml_attribute& r) const
	{
		return (_attr >= r._attr);
	}

	PUGI__FN xml_attribute xml_attribute::next_attribute() const
	{
		return _attr ? xml_attribute(_attr->next_attribute) : xml_attribute();
	}

	PUGI__FN xml_attribute xml_attribute::previous_attribute() const
	{
		return _attr && _attr->prev_attribute_c->next_attribute ? xml_attribute(_attr->prev_attribute_c) : xml_attribute();
	}

	PUGI__FN const char_t* xml_attribute::as_string(const char_t* def) const
	{
		return (_attr && _attr->value) ? _attr->value : def;
	}

	PUGI__FN int xml_attribute::as_int(int def) const
	{
		return impl::get_value_int(_attr ? _attr->value : 0, def);
	}

	PUGI__FN unsigned int xml_attribute::as_uint(unsigned int def) const
	{
		return impl::get_value_uint(_attr ? _attr->value : 0, def);
	}

	PUGI__FN double xml_attribute::as_double(double def) const
	{
		return impl::get_value_double(_attr ? _attr->value : 0, def);
	}

	PUGI__FN float xml_attribute::as_float(float def) const
	{
		return impl::get_value_float(_attr ? _attr->value : 0, def);
	}

	PUGI__FN bool xml_attribute::as_bool(bool def) const
	{
		return impl::get_value_bool(_attr ? _attr->value : 0, def);
	}

#ifdef PUGIXML_HAS_LONG_LONG
	PUGI__FN long long xml_attribute::as_llong(long long def) const
	{
		return impl::get_value_llong(_attr ? _attr->value : 0, def);
	}

	PUGI__FN unsigned long long xml_attribute::as_ullong(unsigned long long def) const
	{
		return impl::get_value_ullong(_attr ? _attr->value : 0, def);
	}
#endif

	PUGI__FN bool xml_attribute::empty() const
	{
		return !_attr;
	}

	PUGI__FN const char_t* xml_attribute::name() const
	{
		return (_attr && _attr->name) ? _attr->name : PUGIXML_TEXT("");
	}

	PUGI__FN const char_t* xml_attribute::value() const
	{
		return (_attr && _attr->value) ? _attr->value : PUGIXML_TEXT("");
	}

	PUGI__FN size_t xml_attribute::hash_value() const
	{
		return static_cast<size_t>(reinterpret_cast<uintptr_t>(_attr) / sizeof(xml_attribute_struct));
	}

	PUGI__FN xml_attribute_struct* xml_attribute::internal_object() const
	{
		return _attr;
	}

	PUGI__FN xml_attribute& xml_attribute::operator=(const char_t* rhs)
	{
		set_value(rhs);
		return *this;
	}
	
	PUGI__FN xml_attribute& xml_attribute::operator=(int rhs)
	{
		set_value(rhs);
		return *this;
	}

	PUGI__FN xml_attribute& xml_attribute::operator=(unsigned int rhs)
	{
		set_value(rhs);
		return *this;
	}

	PUGI__FN xml_attribute& xml_attribute::operator=(double rhs)
	{
		set_value(rhs);
		return *this;
	}
	
	PUGI__FN xml_attribute& xml_attribute::operator=(bool rhs)
	{
		set_value(rhs);
		return *this;
	}

#ifdef PUGIXML_HAS_LONG_LONG
	PUGI__FN xml_attribute& xml_attribute::operator=(long long rhs)
	{
		set_value(rhs);
		return *this;
	}

	PUGI__FN xml_attribute& xml_attribute::operator=(unsigned long long rhs)
	{
		set_value(rhs);
		return *this;
	}
#endif

	PUGI__FN bool xml_attribute::set_name(const char_t* rhs)
	{
		if (!_attr) return false;
		
		return impl::strcpy_insitu(_attr->name, _attr->header, impl::xml_memory_page_name_allocated_mask, rhs);
	}
		
	PUGI__FN bool xml_attribute::set_value(const char_t* rhs)
	{
		if (!_attr) return false;

		return impl::strcpy_insitu(_attr->value, _attr->header, impl::xml_memory_page_value_allocated_mask, rhs);
	}

	PUGI__FN bool xml_attribute::set_value(int rhs)
	{
		if (!_attr) return false;

		return impl::set_value_convert(_attr->value, _attr->header, impl::xml_memory_page_value_allocated_mask, rhs);
	}

	PUGI__FN bool xml_attribute::set_value(unsigned int rhs)
	{
		if (!_attr) return false;

		return impl::set_value_convert(_attr->value, _attr->header, impl::xml_memory_page_value_allocated_mask, rhs);
	}

	PUGI__FN bool xml_attribute::set_value(double rhs)
	{
		if (!_attr) return false;

		return impl::set_value_convert(_attr->value, _attr->header, impl::xml_memory_page_value_allocated_mask, rhs);
	}
	
	PUGI__FN bool xml_attribute::set_value(bool rhs)
	{
		if (!_attr) return false;

		return impl::set_value_convert(_attr->value, _attr->header, impl::xml_memory_page_value_allocated_mask, rhs);
	}

#ifdef PUGIXML_HAS_LONG_LONG
	PUGI__FN bool xml_attribute::set_value(long long rhs)
	{
		if (!_attr) return false;

		return impl::set_value_convert(_attr->value, _attr->header, impl::xml_memory_page_value_allocated_mask, rhs);
	}

	PUGI__FN bool xml_attribute::set_value(unsigned long long rhs)
	{
		if (!_attr) return false;

		return impl::set_value_convert(_attr->value, _attr->header, impl::xml_memory_page_value_allocated_mask, rhs);
	}
#endif

#ifdef __BORLANDC__
	PUGI__FN bool operator&&(const xml_attribute& lhs, bool rhs)
	{
		return (bool)lhs && rhs;
	}

	PUGI__FN bool operator||(const xml_attribute& lhs, bool rhs)
	{
		return (bool)lhs || rhs;
	}
#endif

	PUGI__FN xml_node::xml_node(): _root(0)
	{
	}

	PUGI__FN xml_node::xml_node(xml_node_struct* p): _root(p)
	{
	}
	
	PUGI__FN static void unspecified_bool_xml_node(xml_node***)
	{
	}

	PUGI__FN xml_node::operator xml_node::unspecified_bool_type() const
	{
		return _root ? unspecified_bool_xml_node : 0;
	}

	PUGI__FN bool xml_node::operator!() const
	{
		return !_root;
	}

	PUGI__FN xml_node::iterator xml_node::begin() const
	{
		return iterator(_root ? _root->first_child : 0, _root);
	}

	PUGI__FN xml_node::iterator xml_node::end() const
	{
		return iterator(0, _root);
	}
	
	PUGI__FN xml_node::attribute_iterator xml_node::attributes_begin() const
	{
		return attribute_iterator(_root ? _root->first_attribute : 0, _root);
	}

	PUGI__FN xml_node::attribute_iterator xml_node::attributes_end() const
	{
		return attribute_iterator(0, _root);
	}
	
	PUGI__FN xml_object_range<xml_node_iterator> xml_node::children() const
	{
		return xml_object_range<xml_node_iterator>(begin(), end());
	}

	PUGI__FN xml_object_range<xml_named_node_iterator> xml_node::children(const char_t* name_) const
	{
		return xml_object_range<xml_named_node_iterator>(xml_named_node_iterator(child(name_)._root, _root, name_), xml_named_node_iterator(0, _root, name_));
	}

	PUGI__FN xml_object_range<xml_attribute_iterator> xml_node::attributes() const
	{
		return xml_object_range<xml_attribute_iterator>(attributes_begin(), attributes_end());
	}

	PUGI__FN bool xml_node::operator==(const xml_node& r) const
	{
		return (_root == r._root);
	}

	PUGI__FN bool xml_node::operator!=(const xml_node& r) const
	{
		return (_root != r._root);
	}

	PUGI__FN bool xml_node::operator<(const xml_node& r) const
	{
		return (_root < r._root);
	}
	
	PUGI__FN bool xml_node::operator>(const xml_node& r) const
	{
		return (_root > r._root);
	}
	
	PUGI__FN bool xml_node::operator<=(const xml_node& r) const
	{
		return (_root <= r._root);
	}
	
	PUGI__FN bool xml_node::operator>=(const xml_node& r) const
	{
		return (_root >= r._root);
	}

	PUGI__FN bool xml_node::empty() const
	{
		return !_root;
	}
	
	PUGI__FN const char_t* xml_node::name() const
	{
		return (_root && _root->name) ? _root->name : PUGIXML_TEXT("");
	}

	PUGI__FN xml_node_type xml_node::type() const
	{
		return _root ? PUGI__NODETYPE(_root) : node_null;
	}
	
	PUGI__FN const char_t* xml_node::value() const
	{
		return (_root && _root->value) ? _root->value : PUGIXML_TEXT("");
	}
	
	PUGI__FN xml_node xml_node::child(const char_t* name_) const
	{
		if (!_root) return xml_node();

		for (xml_node_struct* i = _root->first_child; i; i = i->next_sibling)
			if (i->name && impl::strequal(name_, i->name)) return xml_node(i);

		return xml_node();
	}

	PUGI__FN xml_attribute xml_node::attribute(const char_t* name_) const
	{
		if (!_root) return xml_attribute();

		for (xml_attribute_struct* i = _root->first_attribute; i; i = i->next_attribute)
			if (i->name && impl::strequal(name_, i->name))
				return xml_attribute(i);
		
		return xml_attribute();
	}
	
	PUGI__FN xml_node xml_node::next_sibling(const char_t* name_) const
	{
		if (!_root) return xml_node();
		
		for (xml_node_struct* i = _root->next_sibling; i; i = i->next_sibling)
			if (i->name && impl::strequal(name_, i->name)) return xml_node(i);

		return xml_node();
	}

	PUGI__FN xml_node xml_node::next_sibling() const
	{
		return _root ? xml_node(_root->next_sibling) : xml_node();
	}

	PUGI__FN xml_node xml_node::previous_sibling(const char_t* name_) const
	{
		if (!_root) return xml_node();
		
		for (xml_node_struct* i = _root->prev_sibling_c; i->next_sibling; i = i->prev_sibling_c)
			if (i->name && impl::strequal(name_, i->name)) return xml_node(i);

		return xml_node();
	}

	PUGI__FN xml_node xml_node::previous_sibling() const
	{
		if (!_root) return xml_node();
		
		if (_root->prev_sibling_c->next_sibling) return xml_node(_root->prev_sibling_c);
		else return xml_node();
	}

	PUGI__FN xml_node xml_node::parent() const
	{
		return _root ? xml_node(_root->parent) : xml_node();
	}

	PUGI__FN xml_node xml_node::root() const
	{
		return _root ? xml_node(&impl::get_document(_root)) : xml_node();
	}

	PUGI__FN xml_text xml_node::text() const
	{
		return xml_text(_root);
	}

	PUGI__FN const char_t* xml_node::child_value() const
	{
		if (!_root) return PUGIXML_TEXT("");
		
		for (xml_node_struct* i = _root->first_child; i; i = i->next_sibling)
			if (i->value && impl::is_text_node(i))
				return i->value;

		return PUGIXML_TEXT("");
	}

	PUGI__FN const char_t* xml_node::child_value(const char_t* name_) const
	{
		return child(name_).child_value();
	}

	PUGI__FN xml_attribute xml_node::first_attribute() const
	{
		return _root ? xml_attribute(_root->first_attribute) : xml_attribute();
	}

	PUGI__FN xml_attribute xml_node::last_attribute() const
	{
		return _root && _root->first_attribute ? xml_attribute(_root->first_attribute->prev_attribute_c) : xml_attribute();
	}

	PUGI__FN xml_node xml_node::first_child() const
	{
		return _root ? xml_node(_root->first_child) : xml_node();
	}

	PUGI__FN xml_node xml_node::last_child() const
	{
		return _root && _root->first_child ? xml_node(_root->first_child->prev_sibling_c) : xml_node();
	}

	PUGI__FN bool xml_node::set_name(const char_t* rhs)
	{
		switch (type())
		{
		case node_pi:
		case node_declaration:
		case node_element:
			return impl::strcpy_insitu(_root->name, _root->header, impl::xml_memory_page_name_allocated_mask, rhs);

		default:
			return false;
		}
	}
		
	PUGI__FN bool xml_node::set_value(const char_t* rhs)
	{
		switch (type())
		{
		case node_pi:
		case node_cdata:
		case node_pcdata:
		case node_comment:
		case node_doctype:
			return impl::strcpy_insitu(_root->value, _root->header, impl::xml_memory_page_value_allocated_mask, rhs);

		default:
			return false;
		}
	}

	PUGI__FN xml_attribute xml_node::append_attribute(const char_t* name_)
	{
		if (!impl::allow_insert_attribute(type())) return xml_attribute();
		
		xml_attribute a(impl::allocate_attribute(impl::get_allocator(_root)));
		if (!a) return xml_attribute();

		impl::append_attribute(a._attr, _root);

		a.set_name(name_);
		
		return a;
	}

	PUGI__FN xml_attribute xml_node::prepend_attribute(const char_t* name_)
	{
		if (!impl::allow_insert_attribute(type())) return xml_attribute();
		
		xml_attribute a(impl::allocate_attribute(impl::get_allocator(_root)));
		if (!a) return xml_attribute();

		impl::prepend_attribute(a._attr, _root);

		a.set_name(name_);

		return a;
	}

	PUGI__FN xml_attribute xml_node::insert_attribute_after(const char_t* name_, const xml_attribute& attr)
	{
		if (!impl::allow_insert_attribute(type())) return xml_attribute();
		if (!attr || !impl::is_attribute_of(attr._attr, _root)) return xml_attribute();
		
		xml_attribute a(impl::allocate_attribute(impl::get_allocator(_root)));
		if (!a) return xml_attribute();

		impl::insert_attribute_after(a._attr, attr._attr, _root);

		a.set_name(name_);

		return a;
	}

	PUGI__FN xml_attribute xml_node::insert_attribute_before(const char_t* name_, const xml_attribute& attr)
	{
		if (!impl::allow_insert_attribute(type())) return xml_attribute();
		if (!attr || !impl::is_attribute_of(attr._attr, _root)) return xml_attribute();
		
		xml_attribute a(impl::allocate_attribute(impl::get_allocator(_root)));
		if (!a) return xml_attribute();

		impl::insert_attribute_before(a._attr, attr._attr, _root);

		a.set_name(name_);

		return a;
	}

	PUGI__FN xml_attribute xml_node::append_copy(const xml_attribute& proto)
	{
		if (!proto) return xml_attribute();

		xml_attribute result = append_attribute(proto.name());
		result.set_value(proto.value());

		return result;
	}

	PUGI__FN xml_attribute xml_node::prepend_copy(const xml_attribute& proto)
	{
		if (!proto) return xml_attribute();

		xml_attribute result = prepend_attribute(proto.name());
		result.set_value(proto.value());

		return result;
	}

	PUGI__FN xml_attribute xml_node::insert_copy_after(const xml_attribute& proto, const xml_attribute& attr)
	{
		if (!proto) return xml_attribute();

		xml_attribute result = insert_attribute_after(proto.name(), attr);
		result.set_value(proto.value());

		return result;
	}

	PUGI__FN xml_attribute xml_node::insert_copy_before(const xml_attribute& proto, const xml_attribute& attr)
	{
		if (!proto) return xml_attribute();

		xml_attribute result = insert_attribute_before(proto.name(), attr);
		result.set_value(proto.value());

		return result;
	}

	PUGI__FN xml_node xml_node::append_child(xml_node_type type_)
	{
		if (!impl::allow_insert_child(type(), type_)) return xml_node();
		
		xml_node n(impl::allocate_node(impl::get_allocator(_root), type_));
		if (!n) return xml_node();

		impl::append_node(n._root, _root);

		if (type_ == node_declaration) n.set_name(PUGIXML_TEXT("xml"));

		return n;
	}

	PUGI__FN xml_node xml_node::prepend_child(xml_node_type type_)
	{
		if (!impl::allow_insert_child(type(), type_)) return xml_node();
		
		xml_node n(impl::allocate_node(impl::get_allocator(_root), type_));
		if (!n) return xml_node();

		impl::prepend_node(n._root, _root);
				
		if (type_ == node_declaration) n.set_name(PUGIXML_TEXT("xml"));

		return n;
	}

	PUGI__FN xml_node xml_node::insert_child_before(xml_node_type type_, const xml_node& node)
	{
		if (!impl::allow_insert_child(type(), type_)) return xml_node();
		if (!node._root || node._root->parent != _root) return xml_node();
	
		xml_node n(impl::allocate_node(impl::get_allocator(_root), type_));
		if (!n) return xml_node();

		impl::insert_node_before(n._root, node._root);

		if (type_ == node_declaration) n.set_name(PUGIXML_TEXT("xml"));

		return n;
	}

	PUGI__FN xml_node xml_node::insert_child_after(xml_node_type type_, const xml_node& node)
	{
		if (!impl::allow_insert_child(type(), type_)) return xml_node();
		if (!node._root || node._root->parent != _root) return xml_node();
	
		xml_node n(impl::allocate_node(impl::get_allocator(_root), type_));
		if (!n) return xml_node();

		impl::insert_node_after(n._root, node._root);

		if (type_ == node_declaration) n.set_name(PUGIXML_TEXT("xml"));

		return n;
	}

	PUGI__FN xml_node xml_node::append_child(const char_t* name_)
	{
		xml_node result = append_child(node_element);

		result.set_name(name_);

		return result;
	}

	PUGI__FN xml_node xml_node::prepend_child(const char_t* name_)
	{
		xml_node result = prepend_child(node_element);

		result.set_name(name_);

		return result;
	}

	PUGI__FN xml_node xml_node::insert_child_after(const char_t* name_, const xml_node& node)
	{
		xml_node result = insert_child_after(node_element, node);

		result.set_name(name_);

		return result;
	}

	PUGI__FN xml_node xml_node::insert_child_before(const char_t* name_, const xml_node& node)
	{
		xml_node result = insert_child_before(node_element, node);

		result.set_name(name_);

		return result;
	}

	PUGI__FN xml_node xml_node::append_copy(const xml_node& proto)
	{
		xml_node_type type_ = proto.type();
		if (!impl::allow_insert_child(type(), type_)) return xml_node();

		xml_node n(impl::allocate_node(impl::get_allocator(_root), type_));
		if (!n) return xml_node();

		impl::append_node(n._root, _root);
		impl::node_copy_tree(n._root, proto._root);

		return n;
	}

	PUGI__FN xml_node xml_node::prepend_copy(const xml_node& proto)
	{
		xml_node_type type_ = proto.type();
		if (!impl::allow_insert_child(type(), type_)) return xml_node();

		xml_node n(impl::allocate_node(impl::get_allocator(_root), type_));
		if (!n) return xml_node();

		impl::prepend_node(n._root, _root);
		impl::node_copy_tree(n._root, proto._root);

		return n;
	}

	PUGI__FN xml_node xml_node::insert_copy_after(const xml_node& proto, const xml_node& node)
	{
		xml_node_type type_ = proto.type();
		if (!impl::allow_insert_child(type(), type_)) return xml_node();
		if (!node._root || node._root->parent != _root) return xml_node();

		xml_node n(impl::allocate_node(impl::get_allocator(_root), type_));
		if (!n) return xml_node();

		impl::insert_node_after(n._root, node._root);
		impl::node_copy_tree(n._root, proto._root);

		return n;
	}

	PUGI__FN xml_node xml_node::insert_copy_before(const xml_node& proto, const xml_node& node)
	{
		xml_node_type type_ = proto.type();
		if (!impl::allow_insert_child(type(), type_)) return xml_node();
		if (!node._root || node._root->parent != _root) return xml_node();

		xml_node n(impl::allocate_node(impl::get_allocator(_root), type_));
		if (!n) return xml_node();

		impl::insert_node_before(n._root, node._root);
		impl::node_copy_tree(n._root, proto._root);

		return n;
	}

	PUGI__FN xml_node xml_node::append_move(const xml_node& moved)
	{
		if (!impl::allow_move(*this, moved)) return xml_node();

		// disable document_buffer_order optimization since moving nodes around changes document order without changing buffer pointers
		impl::get_document(_root).header |= impl::xml_memory_page_contents_shared_mask;

		impl::remove_node(moved._root);
		impl::append_node(moved._root, _root);

		return moved;
	}

	PUGI__FN xml_node xml_node::prepend_move(const xml_node& moved)
	{
		if (!impl::allow_move(*this, moved)) return xml_node();

		// disable document_buffer_order optimization since moving nodes around changes document order without changing buffer pointers
		impl::get_document(_root).header |= impl::xml_memory_page_contents_shared_mask;

		impl::remove_node(moved._root);
		impl::prepend_node(moved._root, _root);

		return moved;
	}

	PUGI__FN xml_node xml_node::insert_move_after(const xml_node& moved, const xml_node& node)
	{
		if (!impl::allow_move(*this, moved)) return xml_node();
		if (!node._root || node._root->parent != _root) return xml_node();
		if (moved._root == node._root) return xml_node();

		// disable document_buffer_order optimization since moving nodes around changes document order without changing buffer pointers
		impl::get_document(_root).header |= impl::xml_memory_page_contents_shared_mask;

		impl::remove_node(moved._root);
		impl::insert_node_after(moved._root, node._root);

		return moved;
	}

	PUGI__FN xml_node xml_node::insert_move_before(const xml_node& moved, const xml_node& node)
	{
		if (!impl::allow_move(*this, moved)) return xml_node();
		if (!node._root || node._root->parent != _root) return xml_node();
		if (moved._root == node._root) return xml_node();

		// disable document_buffer_order optimization since moving nodes around changes document order without changing buffer pointers
		impl::get_document(_root).header |= impl::xml_memory_page_contents_shared_mask;

		impl::remove_node(moved._root);
		impl::insert_node_before(moved._root, node._root);

		return moved;
	}

	PUGI__FN bool xml_node::remove_attribute(const char_t* name_)
	{
		return remove_attribute(attribute(name_));
	}

	PUGI__FN bool xml_node::remove_attribute(const xml_attribute& a)
	{
		if (!_root || !a._attr) return false;
		if (!impl::is_attribute_of(a._attr, _root)) return false;

		impl::remove_attribute(a._attr, _root);
		impl::destroy_attribute(a._attr, impl::get_allocator(_root));

		return true;
	}

	PUGI__FN bool xml_node::remove_child(const char_t* name_)
	{
		return remove_child(child(name_));
	}

	PUGI__FN bool xml_node::remove_child(const xml_node& n)
	{
		if (!_root || !n._root || n._root->parent != _root) return false;

		impl::remove_node(n._root);
		impl::destroy_node(n._root, impl::get_allocator(_root));

		return true;
	}

	PUGI__FN xml_parse_result xml_node::append_buffer(const void* contents, size_t size, unsigned int options, xml_encoding encoding)
	{
		// append_buffer is only valid for elements/documents
		if (!impl::allow_insert_child(type(), node_element)) return impl::make_parse_result(status_append_invalid_root);

		// get document node
		impl::xml_document_struct* doc = &impl::get_document(_root);

		// disable document_buffer_order optimization since in a document with multiple buffers comparing buffer pointers does not make sense
		doc->header |= impl::xml_memory_page_contents_shared_mask;
		
		// get extra buffer element (we'll store the document fragment buffer there so that we can deallocate it later)
		impl::xml_memory_page* page = 0;
		impl::xml_extra_buffer* extra = static_cast<impl::xml_extra_buffer*>(doc->allocate_memory(sizeof(impl::xml_extra_buffer), page));
		(void)page;

		if (!extra) return impl::make_parse_result(status_out_of_memory);

		// save name; name of the root has to be NULL before parsing - otherwise closing node mismatches will not be detected at the top level
		char_t* rootname = _root->name;
		_root->name = 0;

		// parse
		char_t* buffer = 0;
		xml_parse_result res = impl::load_buffer_impl(doc, _root, const_cast<void*>(contents), size, options, encoding, false, false, &buffer);

		// restore name
		_root->name = rootname;

		// add extra buffer to the list
		extra->buffer = buffer;
		extra->next = doc->extra_buffers;
		doc->extra_buffers = extra;

		return res;
	}

	PUGI__FN xml_node xml_node::find_child_by_attribute(const char_t* name_, const char_t* attr_name, const char_t* attr_value) const
	{
		if (!_root) return xml_node();
		
		for (xml_node_struct* i = _root->first_child; i; i = i->next_sibling)
			if (i->name && impl::strequal(name_, i->name))
			{
				for (xml_attribute_struct* a = i->first_attribute; a; a = a->next_attribute)
					if (a->name && impl::strequal(attr_name, a->name) && impl::strequal(attr_value, a->value ? a->value : PUGIXML_TEXT("")))
						return xml_node(i);
			}

		return xml_node();
	}

	PUGI__FN xml_node xml_node::find_child_by_attribute(const char_t* attr_name, const char_t* attr_value) const
	{
		if (!_root) return xml_node();
		
		for (xml_node_struct* i = _root->first_child; i; i = i->next_sibling)
			for (xml_attribute_struct* a = i->first_attribute; a; a = a->next_attribute)
				if (a->name && impl::strequal(attr_name, a->name) && impl::strequal(attr_value, a->value ? a->value : PUGIXML_TEXT("")))
					return xml_node(i);

		return xml_node();
	}

#ifndef PUGIXML_NO_STL
// Removed the XPATH and STL module, by Milan Straka
#endif

	PUGI__FN xml_node xml_node::first_element_by_path(const char_t* path_, char_t delimiter) const
	{
		xml_node found = *this; // Current search context.

		if (!_root || !path_ || !path_[0]) return found;

		if (path_[0] == delimiter)
		{
			// Absolute path; e.g. '/foo/bar'
			found = found.root();
			++path_;
		}

		const char_t* path_segment = path_;

		while (*path_segment == delimiter) ++path_segment;

		const char_t* path_segment_end = path_segment;

		while (*path_segment_end && *path_segment_end != delimiter) ++path_segment_end;

		if (path_segment == path_segment_end) return found;

		const char_t* next_segment = path_segment_end;

		while (*next_segment == delimiter) ++next_segment;

		if (*path_segment == '.' && path_segment + 1 == path_segment_end)
			return found.first_element_by_path(next_segment, delimiter);
		else if (*path_segment == '.' && *(path_segment+1) == '.' && path_segment + 2 == path_segment_end)
			return found.parent().first_element_by_path(next_segment, delimiter);
		else
		{
			for (xml_node_struct* j = found._root->first_child; j; j = j->next_sibling)
			{
				if (j->name && impl::strequalrange(j->name, path_segment, static_cast<size_t>(path_segment_end - path_segment)))
				{
					xml_node subsearch = xml_node(j).first_element_by_path(next_segment, delimiter);

					if (subsearch) return subsearch;
				}
			}

			return xml_node();
		}
	}

	PUGI__FN bool xml_node::traverse(xml_tree_walker& walker)
	{
		walker._depth = -1;
		
		xml_node arg_begin = *this;
		if (!walker.begin(arg_begin)) return false;

		xml_node cur = first_child();
				
		if (cur)
		{
			++walker._depth;

			do 
			{
				xml_node arg_for_each = cur;
				if (!walker.for_each(arg_for_each))
					return false;
						
				if (cur.first_child())
				{
					++walker._depth;
					cur = cur.first_child();
				}
				else if (cur.next_sibling())
					cur = cur.next_sibling();
				else
				{
					// Borland C++ workaround
					while (!cur.next_sibling() && cur != *this && !cur.parent().empty())
					{
						--walker._depth;
						cur = cur.parent();
					}
						
					if (cur != *this)
						cur = cur.next_sibling();
				}
			}
			while (cur && cur != *this);
		}

		assert(walker._depth == -1);

		xml_node arg_end = *this;
		return walker.end(arg_end);
	}

	PUGI__FN size_t xml_node::hash_value() const
	{
		return static_cast<size_t>(reinterpret_cast<uintptr_t>(_root) / sizeof(xml_node_struct));
	}

	PUGI__FN xml_node_struct* xml_node::internal_object() const
	{
		return _root;
	}

	PUGI__FN void xml_node::print(xml_writer& writer, const char_t* indent, unsigned int flags, xml_encoding encoding, unsigned int depth) const
	{
		if (!_root) return;

		impl::xml_buffered_writer buffered_writer(writer, encoding);

		impl::node_output(buffered_writer, _root, indent, flags, depth);
	}

#ifndef PUGIXML_NO_STL
// Removed the XPATH and STL module, by Milan Straka
#endif

	PUGI__FN ptrdiff_t xml_node::offset_debug() const
	{
		if (!_root) return -1;

		impl::xml_document_struct& doc = impl::get_document(_root);

		// we can determine the offset reliably only if there is exactly once parse buffer
		if (!doc.buffer || doc.extra_buffers) return -1;

		switch (type())
		{
		case node_document:
			return 0;

		case node_element:
		case node_declaration:
		case node_pi:
			return _root->name && (_root->header & impl::xml_memory_page_name_allocated_or_shared_mask) == 0 ? _root->name - doc.buffer : -1;

		case node_pcdata:
		case node_cdata:
		case node_comment:
		case node_doctype:
			return _root->value && (_root->header & impl::xml_memory_page_value_allocated_or_shared_mask) == 0 ? _root->value - doc.buffer : -1;

		default:
			return -1;
		}
	}

#ifdef __BORLANDC__
	PUGI__FN bool operator&&(const xml_node& lhs, bool rhs)
	{
		return (bool)lhs && rhs;
	}

	PUGI__FN bool operator||(const xml_node& lhs, bool rhs)
	{
		return (bool)lhs || rhs;
	}
#endif

	PUGI__FN xml_text::xml_text(xml_node_struct* root): _root(root)
	{
	}

	PUGI__FN xml_node_struct* xml_text::_data() const
	{
		if (!_root || impl::is_text_node(_root)) return _root;

		for (xml_node_struct* node = _root->first_child; node; node = node->next_sibling)
			if (impl::is_text_node(node))
				return node;

		return 0;
	}

	PUGI__FN xml_node_struct* xml_text::_data_new()
	{
		xml_node_struct* d = _data();
		if (d) return d;

		return xml_node(_root).append_child(node_pcdata).internal_object();
	}

	PUGI__FN xml_text::xml_text(): _root(0)
	{
	}

	PUGI__FN static void unspecified_bool_xml_text(xml_text***)
	{
	}

	PUGI__FN xml_text::operator xml_text::unspecified_bool_type() const
	{
		return _data() ? unspecified_bool_xml_text : 0;
	}

	PUGI__FN bool xml_text::operator!() const
	{
		return !_data();
	}

	PUGI__FN bool xml_text::empty() const
	{
		return _data() == 0;
	}

	PUGI__FN const char_t* xml_text::get() const
	{
		xml_node_struct* d = _data();

		return (d && d->value) ? d->value : PUGIXML_TEXT("");
	}

	PUGI__FN const char_t* xml_text::as_string(const char_t* def) const
	{
		xml_node_struct* d = _data();

		return (d && d->value) ? d->value : def;
	}

	PUGI__FN int xml_text::as_int(int def) const
	{
		xml_node_struct* d = _data();

		return impl::get_value_int(d ? d->value : 0, def);
	}

	PUGI__FN unsigned int xml_text::as_uint(unsigned int def) const
	{
		xml_node_struct* d = _data();

		return impl::get_value_uint(d ? d->value : 0, def);
	}

	PUGI__FN double xml_text::as_double(double def) const
	{
		xml_node_struct* d = _data();

		return impl::get_value_double(d ? d->value : 0, def);
	}

	PUGI__FN float xml_text::as_float(float def) const
	{
		xml_node_struct* d = _data();

		return impl::get_value_float(d ? d->value : 0, def);
	}

	PUGI__FN bool xml_text::as_bool(bool def) const
	{
		xml_node_struct* d = _data();

		return impl::get_value_bool(d ? d->value : 0, def);
	}

#ifdef PUGIXML_HAS_LONG_LONG
	PUGI__FN long long xml_text::as_llong(long long def) const
	{
		xml_node_struct* d = _data();

		return impl::get_value_llong(d ? d->value : 0, def);
	}

	PUGI__FN unsigned long long xml_text::as_ullong(unsigned long long def) const
	{
		xml_node_struct* d = _data();

		return impl::get_value_ullong(d ? d->value : 0, def);
	}
#endif

	PUGI__FN bool xml_text::set(const char_t* rhs)
	{
		xml_node_struct* dn = _data_new();

		return dn ? impl::strcpy_insitu(dn->value, dn->header, impl::xml_memory_page_value_allocated_mask, rhs) : false;
	}

	PUGI__FN bool xml_text::set(int rhs)
	{
		xml_node_struct* dn = _data_new();

		return dn ? impl::set_value_convert(dn->value, dn->header, impl::xml_memory_page_value_allocated_mask, rhs) : false;
	}

	PUGI__FN bool xml_text::set(unsigned int rhs)
	{
		xml_node_struct* dn = _data_new();

		return dn ? impl::set_value_convert(dn->value, dn->header, impl::xml_memory_page_value_allocated_mask, rhs) : false;
	}

	PUGI__FN bool xml_text::set(double rhs)
	{
		xml_node_struct* dn = _data_new();

		return dn ? impl::set_value_convert(dn->value, dn->header, impl::xml_memory_page_value_allocated_mask, rhs) : false;
	}

	PUGI__FN bool xml_text::set(bool rhs)
	{
		xml_node_struct* dn = _data_new();

		return dn ? impl::set_value_convert(dn->value, dn->header, impl::xml_memory_page_value_allocated_mask, rhs) : false;
	}

#ifdef PUGIXML_HAS_LONG_LONG
	PUGI__FN bool xml_text::set(long long rhs)
	{
		xml_node_struct* dn = _data_new();

		return dn ? impl::set_value_convert(dn->value, dn->header, impl::xml_memory_page_value_allocated_mask, rhs) : false;
	}

	PUGI__FN bool xml_text::set(unsigned long long rhs)
	{
		xml_node_struct* dn = _data_new();

		return dn ? impl::set_value_convert(dn->value, dn->header, impl::xml_memory_page_value_allocated_mask, rhs) : false;
	}
#endif

	PUGI__FN xml_text& xml_text::operator=(const char_t* rhs)
	{
		set(rhs);
		return *this;
	}

	PUGI__FN xml_text& xml_text::operator=(int rhs)
	{
		set(rhs);
		return *this;
	}

	PUGI__FN xml_text& xml_text::operator=(unsigned int rhs)
	{
		set(rhs);
		return *this;
	}

	PUGI__FN xml_text& xml_text::operator=(double rhs)
	{
		set(rhs);
		return *this;
	}

	PUGI__FN xml_text& xml_text::operator=(bool rhs)
	{
		set(rhs);
		return *this;
	}

#ifdef PUGIXML_HAS_LONG_LONG
	PUGI__FN xml_text& xml_text::operator=(long long rhs)
	{
		set(rhs);
		return *this;
	}

	PUGI__FN xml_text& xml_text::operator=(unsigned long long rhs)
	{
		set(rhs);
		return *this;
	}
#endif

	PUGI__FN xml_node xml_text::data() const
	{
		return xml_node(_data());
	}

#ifdef __BORLANDC__
	PUGI__FN bool operator&&(const xml_text& lhs, bool rhs)
	{
		return (bool)lhs && rhs;
	}

	PUGI__FN bool operator||(const xml_text& lhs, bool rhs)
	{
		return (bool)lhs || rhs;
	}
#endif

	PUGI__FN xml_node_iterator::xml_node_iterator()
	{
	}

	PUGI__FN xml_node_iterator::xml_node_iterator(const xml_node& node): _wrap(node), _parent(node.parent())
	{
	}

	PUGI__FN xml_node_iterator::xml_node_iterator(xml_node_struct* ref, xml_node_struct* parent): _wrap(ref), _parent(parent)
	{
	}

	PUGI__FN bool xml_node_iterator::operator==(const xml_node_iterator& rhs) const
	{
		return _wrap._root == rhs._wrap._root && _parent._root == rhs._parent._root;
	}
	
	PUGI__FN bool xml_node_iterator::operator!=(const xml_node_iterator& rhs) const
	{
		return _wrap._root != rhs._wrap._root || _parent._root != rhs._parent._root;
	}

	PUGI__FN xml_node& xml_node_iterator::operator*() const
	{
		assert(_wrap._root);
		return _wrap;
	}

	PUGI__FN xml_node* xml_node_iterator::operator->() const
	{
		assert(_wrap._root);
		return const_cast<xml_node*>(&_wrap); // BCC32 workaround
	}

	PUGI__FN const xml_node_iterator& xml_node_iterator::operator++()
	{
		assert(_wrap._root);
		_wrap._root = _wrap._root->next_sibling;
		return *this;
	}

	PUGI__FN xml_node_iterator xml_node_iterator::operator++(int)
	{
		xml_node_iterator temp = *this;
		++*this;
		return temp;
	}

	PUGI__FN const xml_node_iterator& xml_node_iterator::operator--()
	{
		_wrap = _wrap._root ? _wrap.previous_sibling() : _parent.last_child();
		return *this;
	}

	PUGI__FN xml_node_iterator xml_node_iterator::operator--(int)
	{
		xml_node_iterator temp = *this;
		--*this;
		return temp;
	}

	PUGI__FN xml_attribute_iterator::xml_attribute_iterator()
	{
	}

	PUGI__FN xml_attribute_iterator::xml_attribute_iterator(const xml_attribute& attr, const xml_node& parent): _wrap(attr), _parent(parent)
	{
	}

	PUGI__FN xml_attribute_iterator::xml_attribute_iterator(xml_attribute_struct* ref, xml_node_struct* parent): _wrap(ref), _parent(parent)
	{
	}

	PUGI__FN bool xml_attribute_iterator::operator==(const xml_attribute_iterator& rhs) const
	{
		return _wrap._attr == rhs._wrap._attr && _parent._root == rhs._parent._root;
	}
	
	PUGI__FN bool xml_attribute_iterator::operator!=(const xml_attribute_iterator& rhs) const
	{
		return _wrap._attr != rhs._wrap._attr || _parent._root != rhs._parent._root;
	}

	PUGI__FN xml_attribute& xml_attribute_iterator::operator*() const
	{
		assert(_wrap._attr);
		return _wrap;
	}

	PUGI__FN xml_attribute* xml_attribute_iterator::operator->() const
	{
		assert(_wrap._attr);
		return const_cast<xml_attribute*>(&_wrap); // BCC32 workaround
	}

	PUGI__FN const xml_attribute_iterator& xml_attribute_iterator::operator++()
	{
		assert(_wrap._attr);
		_wrap._attr = _wrap._attr->next_attribute;
		return *this;
	}

	PUGI__FN xml_attribute_iterator xml_attribute_iterator::operator++(int)
	{
		xml_attribute_iterator temp = *this;
		++*this;
		return temp;
	}

	PUGI__FN const xml_attribute_iterator& xml_attribute_iterator::operator--()
	{
		_wrap = _wrap._attr ? _wrap.previous_attribute() : _parent.last_attribute();
		return *this;
	}

	PUGI__FN xml_attribute_iterator xml_attribute_iterator::operator--(int)
	{
		xml_attribute_iterator temp = *this;
		--*this;
		return temp;
	}

	PUGI__FN xml_named_node_iterator::xml_named_node_iterator(): _name(0)
	{
	}

	PUGI__FN xml_named_node_iterator::xml_named_node_iterator(const xml_node& node, const char_t* name): _wrap(node), _parent(node.parent()), _name(name)
	{
	}

	PUGI__FN xml_named_node_iterator::xml_named_node_iterator(xml_node_struct* ref, xml_node_struct* parent, const char_t* name): _wrap(ref), _parent(parent), _name(name)
	{
	}

	PUGI__FN bool xml_named_node_iterator::operator==(const xml_named_node_iterator& rhs) const
	{
		return _wrap._root == rhs._wrap._root && _parent._root == rhs._parent._root;
	}

	PUGI__FN bool xml_named_node_iterator::operator!=(const xml_named_node_iterator& rhs) const
	{
		return _wrap._root != rhs._wrap._root || _parent._root != rhs._parent._root;
	}

	PUGI__FN xml_node& xml_named_node_iterator::operator*() const
	{
		assert(_wrap._root);
		return _wrap;
	}

	PUGI__FN xml_node* xml_named_node_iterator::operator->() const
	{
		assert(_wrap._root);
		return const_cast<xml_node*>(&_wrap); // BCC32 workaround
	}

	PUGI__FN const xml_named_node_iterator& xml_named_node_iterator::operator++()
	{
		assert(_wrap._root);
		_wrap = _wrap.next_sibling(_name);
		return *this;
	}

	PUGI__FN xml_named_node_iterator xml_named_node_iterator::operator++(int)
	{
		xml_named_node_iterator temp = *this;
		++*this;
		return temp;
	}

	PUGI__FN const xml_named_node_iterator& xml_named_node_iterator::operator--()
	{
		if (_wrap._root)
			_wrap = _wrap.previous_sibling(_name);
		else
		{
			_wrap = _parent.last_child();

			if (!impl::strequal(_wrap.name(), _name))
				_wrap = _wrap.previous_sibling(_name);
		}

		return *this;
	}

	PUGI__FN xml_named_node_iterator xml_named_node_iterator::operator--(int)
	{
		xml_named_node_iterator temp = *this;
		--*this;
		return temp;
	}

	PUGI__FN xml_parse_result::xml_parse_result(): status(status_internal_error), offset(0), encoding(encoding_auto)
	{
	}

	PUGI__FN xml_parse_result::operator bool() const
	{
		return status == status_ok;
	}

	PUGI__FN const char* xml_parse_result::description() const
	{
		switch (status)
		{
		case status_ok: return "No error";

		case status_file_not_found: return "File was not found";
		case status_io_error: return "Error reading from file/stream";
		case status_out_of_memory: return "Could not allocate memory";
		case status_internal_error: return "Internal error occurred";

		case status_unrecognized_tag: return "Could not determine tag type";

		case status_bad_pi: return "Error parsing document declaration/processing instruction";
		case status_bad_comment: return "Error parsing comment";
		case status_bad_cdata: return "Error parsing CDATA section";
		case status_bad_doctype: return "Error parsing document type declaration";
		case status_bad_pcdata: return "Error parsing PCDATA section";
		case status_bad_start_element: return "Error parsing start element tag";
		case status_bad_attribute: return "Error parsing element attribute";
		case status_bad_end_element: return "Error parsing end element tag";
		case status_end_element_mismatch: return "Start-end tags mismatch";

		case status_append_invalid_root: return "Unable to append nodes: root is not an element or document";

		case status_no_document_element: return "No document element found";

		default: return "Unknown error";
		}
	}

	PUGI__FN xml_document::xml_document(): _buffer(0)
	{
		create();
	}

	PUGI__FN xml_document::~xml_document()
	{
		destroy();
	}

	PUGI__FN void xml_document::reset()
	{
		destroy();
		create();
	}

	PUGI__FN void xml_document::reset(const xml_document& proto)
	{
		reset();

		for (xml_node cur = proto.first_child(); cur; cur = cur.next_sibling())
			append_copy(cur);
	}

	PUGI__FN void xml_document::create()
	{
		assert(!_root);

		// initialize sentinel page
		PUGI__STATIC_ASSERT(sizeof(impl::xml_memory_page) + sizeof(impl::xml_document_struct) + impl::xml_memory_page_alignment - sizeof(void*) <= sizeof(_memory));

		// align upwards to page boundary
		void* page_memory = reinterpret_cast<void*>((reinterpret_cast<uintptr_t>(_memory) + (impl::xml_memory_page_alignment - 1)) & ~(impl::xml_memory_page_alignment - 1));

		// prepare page structure
		impl::xml_memory_page* page = impl::xml_memory_page::construct(page_memory);
		assert(page);

		page->busy_size = impl::xml_memory_page_size;

		// allocate new root
		_root = new (reinterpret_cast<char*>(page) + sizeof(impl::xml_memory_page)) impl::xml_document_struct(page);
		_root->prev_sibling_c = _root;

		// setup sentinel page
		page->allocator = static_cast<impl::xml_document_struct*>(_root);

		// verify the document allocation
		assert(reinterpret_cast<char*>(_root) + sizeof(impl::xml_document_struct) <= _memory + sizeof(_memory));
	}

	PUGI__FN void xml_document::destroy()
	{
		assert(_root);

		// destroy static storage
		if (_buffer)
		{
			impl::xml_memory::deallocate(_buffer);
			_buffer = 0;
		}

		// destroy extra buffers (note: no need to destroy linked list nodes, they're allocated using document allocator)
		for (impl::xml_extra_buffer* extra = static_cast<impl::xml_document_struct*>(_root)->extra_buffers; extra; extra = extra->next)
		{
			if (extra->buffer) impl::xml_memory::deallocate(extra->buffer);
		}

		// destroy dynamic storage, leave sentinel page (it's in static memory)
		impl::xml_memory_page* root_page = reinterpret_cast<impl::xml_memory_page*>(_root->header & impl::xml_memory_page_pointer_mask);
		assert(root_page && !root_page->prev);
		assert(reinterpret_cast<char*>(root_page) >= _memory && reinterpret_cast<char*>(root_page) < _memory + sizeof(_memory));

		for (impl::xml_memory_page* page = root_page->next; page; )
		{
			impl::xml_memory_page* next = page->next;

			impl::xml_allocator::deallocate_page(page);

			page = next;
		}

		_root = 0;
	}

#ifndef PUGIXML_NO_STL
// Removed the XPATH and STL module, by Milan Straka
#endif

	PUGI__FN xml_parse_result xml_document::load_string(const char_t* contents, unsigned int options)
	{
		// Force native encoding (skip autodetection)
	#ifdef PUGIXML_WCHAR_MODE
		xml_encoding encoding = encoding_wchar;
	#else
		xml_encoding encoding = encoding_utf8;
	#endif

		return load_buffer(contents, impl::strlength(contents) * sizeof(char_t), options, encoding);
	}

	PUGI__FN xml_parse_result xml_document::load(const char_t* contents, unsigned int options)
	{
		return load_string(contents, options);
	}

	PUGI__FN xml_parse_result xml_document::load_file(const char* path_, unsigned int options, xml_encoding encoding)
	{
		reset();

		FILE* file = fopen(path_, "rb");

		return impl::load_file_impl(*this, file, options, encoding);
	}

	PUGI__FN xml_parse_result xml_document::load_file(const wchar_t* path_, unsigned int options, xml_encoding encoding)
	{
		reset();

		FILE* file = impl::open_file_wide(path_, L"rb");

		return impl::load_file_impl(*this, file, options, encoding);
	}

	PUGI__FN xml_parse_result xml_document::load_buffer(const void* contents, size_t size, unsigned int options, xml_encoding encoding)
	{
		reset();

		return impl::load_buffer_impl(static_cast<impl::xml_document_struct*>(_root), _root, const_cast<void*>(contents), size, options, encoding, false, false, &_buffer);
	}

	PUGI__FN xml_parse_result xml_document::load_buffer_inplace(void* contents, size_t size, unsigned int options, xml_encoding encoding)
	{
		reset();

		return impl::load_buffer_impl(static_cast<impl::xml_document_struct*>(_root), _root, contents, size, options, encoding, true, false, &_buffer);
	}

	PUGI__FN xml_parse_result xml_document::load_buffer_inplace_own(void* contents, size_t size, unsigned int options, xml_encoding encoding)
	{
		reset();

		return impl::load_buffer_impl(static_cast<impl::xml_document_struct*>(_root), _root, contents, size, options, encoding, true, true, &_buffer);
	}

	PUGI__FN void xml_document::save(xml_writer& writer, const char_t* indent, unsigned int flags, xml_encoding encoding) const
	{
		impl::xml_buffered_writer buffered_writer(writer, encoding);

		if ((flags & format_write_bom) && encoding != encoding_latin1)
		{
			// BOM always represents the codepoint U+FEFF, so just write it in native encoding
		#ifdef PUGIXML_WCHAR_MODE
			unsigned int bom = 0xfeff;
			buffered_writer.write(static_cast<wchar_t>(bom));
		#else
			buffered_writer.write('\xef', '\xbb', '\xbf');
		#endif
		}

		if (!(flags & format_no_declaration) && !impl::has_declaration(_root))
		{
			buffered_writer.write_string(PUGIXML_TEXT("<?xml version=\"1.0\""));
			if (encoding == encoding_latin1) buffered_writer.write_string(PUGIXML_TEXT(" encoding=\"ISO-8859-1\""));
			buffered_writer.write('?', '>');
			if (!(flags & format_raw)) buffered_writer.write('\n');
		}

		impl::node_output(buffered_writer, _root, indent, flags, 0);
	}

#ifndef PUGIXML_NO_STL
// Removed the XPATH and STL module, by Milan Straka
#endif

	PUGI__FN bool xml_document::save_file(const char* path_, const char_t* indent, unsigned int flags, xml_encoding encoding) const
	{
		FILE* file = fopen(path_, (flags & format_save_file_text) ? "w" : "wb");
		return impl::save_file_impl(*this, file, indent, flags, encoding);
	}

	PUGI__FN bool xml_document::save_file(const wchar_t* path_, const char_t* indent, unsigned int flags, xml_encoding encoding) const
	{
		FILE* file = impl::open_file_wide(path_, (flags & format_save_file_text) ? L"w" : L"wb");
		return impl::save_file_impl(*this, file, indent, flags, encoding);
	}

	PUGI__FN xml_node xml_document::document_element() const
	{
		assert(_root);

		for (xml_node_struct* i = _root->first_child; i; i = i->next_sibling)
			if (PUGI__NODETYPE(i) == node_element)
				return xml_node(i);

		return xml_node();
	}

#ifndef PUGIXML_NO_STL
// Removed the XPATH and STL module, by Milan Straka
#endif

	PUGI__FN void PUGIXML_FUNCTION set_memory_management_functions(allocation_function allocate, deallocation_function deallocate)
	{
		impl::xml_memory::allocate = allocate;
		impl::xml_memory::deallocate = deallocate;
	}

	PUGI__FN allocation_function PUGIXML_FUNCTION get_memory_allocation_function()
	{
		return impl::xml_memory::allocate;
	}

	PUGI__FN deallocation_function PUGIXML_FUNCTION get_memory_deallocation_function()
	{
		return impl::xml_memory::deallocate;
	}
}
} // namespace microrestd
} // namespace ufal

#if !defined(PUGIXML_NO_STL) && (defined(_MSC_VER) || defined(__ICC))
// Removed the XPATH and STL module, by Milan Straka
#endif

#if !defined(PUGIXML_NO_STL) && defined(__SUNPRO_CC)
// Removed the XPATH and STL module, by Milan Straka
#endif

#ifndef PUGIXML_NO_XPATH
// Removed the XPATH and STL module, by Milan Straka
#endif

#ifdef __BORLANDC__
#	pragma option pop
#endif

// Intel C++ does not properly keep warning state for function templates,
// so popping warning state at the end of translation unit leads to warnings in the middle.
#if defined(_MSC_VER) && !defined(__INTEL_COMPILER)
#	pragma warning(pop)
#endif

// Undefine all local macros (makes sure we're not leaking macros in header-only mode)
#undef PUGI__NO_INLINE
#undef PUGI__UNLIKELY
#undef PUGI__STATIC_ASSERT
#undef PUGI__DMC_VOLATILE
#undef PUGI__MSVC_CRT_VERSION
#undef PUGI__NS_BEGIN
#undef PUGI__NS_END
#undef PUGI__FN
#undef PUGI__FN_NO_INLINE
#undef PUGI__NODETYPE
#undef PUGI__IS_CHARTYPE_IMPL
#undef PUGI__IS_CHARTYPE
#undef PUGI__IS_CHARTYPEX
#undef PUGI__ENDSWITH
#undef PUGI__SKIPWS
#undef PUGI__OPTSET
#undef PUGI__PUSHNODE
#undef PUGI__POPNODE
#undef PUGI__SCANFOR
#undef PUGI__SCANWHILE
#undef PUGI__SCANWHILE_UNROLL
#undef PUGI__ENDSEG
#undef PUGI__THROW_ERROR
#undef PUGI__CHECK_ERROR

#endif

/**
 * Copyright (c) 2006-2014 Arseny Kapoulkine
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */
