#ifndef __PRETTY_FUNCTION__
#include "pretty.h"
#endif

#include <iostream>
#include <map>
#include <vector>

#include "MyList.h"

//#define USE_PRETTY 1
const size_t MEMORY_PAGE_SIZE = 512;

template <typename T, int N = 0>
struct memory_allocator 
{
private:
	struct block
	{
		char* head;
		size_t size;
	};
	std::vector<block> deallocated; // блоки, помеченные как "свободные"
	std::vector<block> pages; 		// выделенные страницы
	const bool resizable;
	const size_t page_size;			// размер страницы
	char *ptr_in_page;				// начало следующего свободного блока
	
	size_t page_available_memory()
	{
		if(pages.empty())
		{
			return 0;
		}
		else
		{
			auto b = pages.back();
			return b.size - (ptr_in_page - b.head);
		}
	}
	char *allocate_page(size_t num) 
	{
		auto size = page_size * num;
		auto p = std::malloc(size);
		if (!p)
			throw std::bad_alloc();
		
		block b;
		b.head = reinterpret_cast<char*>(p);
		b.size = size;
		pages.push_back(b);
			
		return b.head; 
	}
	T* find_in_deallocated(size_t size) 
	{
		T* p = nullptr;
		for(auto i : deallocated)
		{
			if(i.size >= size)
			{
				p = reinterpret_cast<T*>(i.head);
				i.head += size;
				i.size -= size;
				if(i.size == 0) 
				{
					std::swap(i, deallocated.back());
					deallocated.pop_back();
				}
				break;
			}
		}
		return p;
	}

public:
	using value_type = T;
	using pointer = T *;
	using const_pointer = const T *;
	using reference = T &;
	using const_reference = const T &;

	template <typename U>
	struct rebind {
		using other = memory_allocator<U, N>;
	};

	memory_allocator():resizable(N == 0), page_size(resizable ? MEMORY_PAGE_SIZE : N * sizeof(T))
	{
		ptr_in_page = nullptr;
		if(!resizable) 
			ptr_in_page = allocate_page(page_size);
	};
	~memory_allocator()
	{
		for(auto i: pages)
			std::free(i.head);	
	};

	template <typename U>
	memory_allocator(const memory_allocator<U, N> &copy) 
	{
		page_size = copy.page_size;
		ptr_in_page = nullptr;
	}

	T *allocate(std::size_t n) 
	{
#ifdef USE_PRETTY
		std::cout << __PRETTY_FUNCTION__ << "[n = " << n << "]" << std::endl;
#endif
		size_t need = n * sizeof(T);
		T* p = find_in_deallocated(need);
		if(p == nullptr)
		{
			size_t available = page_available_memory();
			if (need > available)
			{
				if(available > 0)
				{
					block b = {ptr_in_page, available};
					deallocated.push_back(b);
				}
				if(resizable)
					ptr_in_page = allocate_page((need / page_size) + 1);
				else
					throw std::bad_alloc();
			}	

			p = reinterpret_cast<T *>(ptr_in_page);
			ptr_in_page += need;
		}
		return p;
	}

	void deallocate(T *p, std::size_t n) {
#ifdef USE_PRETTY
		std::cout << __PRETTY_FUNCTION__ << "[n = " << n << "]" << std::endl;
#endif
		block b = {reinterpret_cast<char*>(p), n};
		deallocated.push_back(b);
	}

	template <typename U, typename... Args>
	void construct(U *p, Args &&...args) {
#ifdef USE_PRETTY
		std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif
		new (p) U(std::forward<Args>(args)...);
	};

	template <typename U>
	void destroy(U *p) {
#ifdef USE_PRETTY
		std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif
		p->~U();
	}
};

double fact(int n)
{
    if(n < 0)  return 0; 
    if(n == 0) return 1; 
    else       return n * fact(n - 1);
}

int main(int, char *[]) 
{
	std::cout << "std::map with std::allocator" <<std::endl;
	auto m1 = std::map<int, int>{};
	for (int i = 0; i < 10; i++)
		m1[i] = static_cast<int>(fact(i));

	for(auto i : m1)
		std::cout << i.first << " " << i.second << std::endl;

	typedef memory_allocator<std::pair<int, int>, 10> pair_alloc;
	auto m2 = std::map<int, int, std::less<int>, pair_alloc>{};

	std::cout << "std::map with memory_allocator" << std::endl;
	for (int i = 0; i < 10; i++)
		m2[i] = static_cast<int>(fact(i));

	for(auto i : m2)
		std::cout << i.first << " " << i.second << std::endl;
	
	std::cout << "MyList with memory_allocator" << std::endl;
	MyList<int, memory_allocator<int>> list;
	for(int i = 0; i < 10; i++)
		list.push_back(i);
	
	list.print();

	return 0;
}
