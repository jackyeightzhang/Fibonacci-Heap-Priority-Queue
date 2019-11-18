#ifndef FIB_PRIORITY_QUEUE_HPP_
#define FIB_PRIORITY_QUEUE_HPP_

#include <string>
#include <iostream>
#include <sstream>
#include <initializer_list>
#include "courselib/ics_exceptions.hpp"
#include <utility>							//For std::swap function
#include "array_stack.hpp"			//See operator <<


namespace ics {


//Instantiate the templated class supplying tgt(a,b): true, iff a has higher priority than b.
//If tgt is defaulted to nullptr in the template, then a constructor must supply cgt.
//If both tgt and cgt are supplied, then they must be the same (by ==) function.
//If neither is supplied, or both are supplied but different, TemplateFunctionError is raised.
//The (unique) non-nullptr value supplied by tgt/cgt is stored in the instance variable gt.
template<class T, bool (*tgt)(const T& a, const T& b) = nullptr> class FibPriorityQueue {
	public:
		//Destructor/Constructors
		~FibPriorityQueue();

		FibPriorityQueue(bool (*cgt)(const T& a, const T& b) = nullptr);
		explicit FibPriorityQueue(int initial_length, bool (*cgt)(const T& a, const T& b));
		FibPriorityQueue(const FibPriorityQueue<T,tgt>& to_copy, bool (*cgt)(const T& a, const T& b) = nullptr);
		explicit FibPriorityQueue(const std::initializer_list<T>& il, bool (*cgt)(const T& a, const T& b) = nullptr);

		//Iterable class must support "for-each" loop: .begin()/.end() and prefix ++ on returned result
		template <class Iterable>
		explicit FibPriorityQueue (const Iterable& i, bool (*cgt)(const T& a, const T& b) = nullptr);


		//Queries
		bool empty			() const;
		int	size			 () const;
		T&	 peek			 () const;
		std::string str () const; //supplies useful debugging information; contrast to operator <<


		//Commands
		int	enqueue (const T& element);
		T		dequeue ();
		void clear	 ();

		//Iterable class must support "for-each" loop: .begin()/.end() and prefix ++ on returned result
		template <class Iterable>
		int enqueue_all (const Iterable& i);


		//Operators
		FibPriorityQueue<T,tgt>& operator = (const FibPriorityQueue<T,tgt>& rhs);
		bool operator == (const FibPriorityQueue<T,tgt>& rhs) const;
		bool operator != (const FibPriorityQueue<T,tgt>& rhs) const;

		template<class T2, bool (*gt2)(const T2& a, const T2& b)>
		friend std::ostream& operator << (std::ostream& outs, const FibPriorityQueue<T2,gt2>& pq);



		class Iterator {
			public:
				//Private constructor called in begin/end, which are friends of FibPriorityQueue<T,tgt>
				~Iterator();
				T					 erase();
				std::string str	() const;
				FibPriorityQueue<T,tgt>::Iterator& operator ++ ();
				FibPriorityQueue<T,tgt>::Iterator	operator ++ (int);
				bool operator == (const FibPriorityQueue<T,tgt>::Iterator& rhs) const;
				bool operator != (const FibPriorityQueue<T,tgt>::Iterator& rhs) const;
				T& operator *	() const;
				T* operator -> () const;
				friend std::ostream& operator << (std::ostream& outs, const FibPriorityQueue<T,tgt>::Iterator& i) {
					outs << i.str(); //Use the same meaning as the debugging .str() method
					return outs;
				}

				friend Iterator FibPriorityQueue<T,tgt>::begin () const;
				friend Iterator FibPriorityQueue<T,tgt>::end	 () const;

			private:
				//If can_erase is false, the value has been removed from "it" (++ does nothing)
				FibPriorityQueue<T,tgt>	it;								 //copy of HPQ (from begin), to use as iterator via dequeue
				FibPriorityQueue<T,tgt>* ref_pq;
				int											 expected_mod_count;
				bool											can_erase = true;

				//Called in friends begin/end
				//These constructors have different initializers (see it(...) in first one)
				Iterator(FibPriorityQueue<T,tgt>* iterate_over, bool from_begin);		// Called by begin
				Iterator(FibPriorityQueue<T,tgt>* iterate_over);										 // Called by end
		};


		Iterator begin () const;
		Iterator end	 () const;


	private:
		class FN {	
	    	public:
	    	    FN()				: parentNode(nullptr){}
				FN(const FN& fn)	: childNodes(ArraySet<FN*>(fn.childNodes)), parentNode(fn.parentNode), value(fn.value){}
				FN(ArraySet<FN*> cn, 
					FN* pn = nullptr, 
					T v) 			: childNodes(cn), parentNode(pn), value(v){}

				ArraySet<FN*> childNodes;
	    		FN* parentNode;
	    		T value;
		};

		class FRN {
	    	public:
	    	    FRN()				: prevRootNode(this), nextRootNode(this){} 
				FRN(const FRN& frn)	: childNodes(ArraySet<FN*>(frn.childNodes)), prevRootNode(frn.prevRootNode), nextRootNode(frn.nextRootNode), value(frn.value){}
				FN(ArraySet<FN*> cn, 
					FRN* prn = this, 
					FRN* nrn = this,
					T v) 			: childNodes(cn), prevRootNode(prn), nextRootNode(nrn), value(v){}

				ArraySet<FN*> childNodes;
	    		FRN* prevRootNode;
				FRN* nextRootNode;
	    		T value;
		};

		bool (*gt) (const T& a, const T& b); // The gt used by enqueue (from template or constructor)
		T*	pq;															// Smaller values in lower indexes (biggest is at used-1)
		int length		= 0;									 //Physical length of array: must be >= .size()
		int used			= 0;									 //Amount of array used:	invariant: 0 <= used <= length
		int mod_count = 0;									 //For sensing concurrent modification


		//Helper methods
		void ensure_length	(int new_length);
		int	left_child		 (int i) const;				 //Useful abstractions for heaps as arrays
		int	right_child		(int i) const;
		int	parent				 (int i) const;
		bool is_root				(int i) const;
		bool in_heap				(int i) const;
		void percolate_up	 (int i);
		void percolate_down (int i);
		void heapify				();									 // Percolate down all value is array (from indexes used-1 to 0): O(N)
};





////////////////////////////////////////////////////////////////////////////////
//
//FibPriorityQueue class and related definitions

//Destructor/Constructors

template<class T, bool (*tgt)(const T& a, const T& b)>
FibPriorityQueue<T,tgt>::~FibPriorityQueue() {
	delete[] pq;
}


template<class T, bool (*tgt)(const T& a, const T& b)>
FibPriorityQueue<T,tgt>::FibPriorityQueue(bool (*cgt)(const T& a, const T& b))
: gt(tgt != nullptr ? tgt : cgt) {
	if (gt == nullptr)
		throw TemplateFunctionError("FibPriorityQueue::default constructor: neither specified");
	if (tgt != nullptr && cgt != nullptr && tgt != cgt)
		throw TemplateFunctionError("FibPriorityQueue::default constructor: both specified and different");

	pq = new T[length];
}


template<class T, bool (*tgt)(const T& a, const T& b)>
FibPriorityQueue<T,tgt>::FibPriorityQueue(int initial_length, bool (*cgt)(const T& a, const T& b))
: gt(tgt != nullptr ? tgt : cgt), length(initial_length) {
	if (gt == nullptr)
		throw TemplateFunctionError("FibPriorityQueue::length constructor: neither specified");
	if (tgt != nullptr && cgt != nullptr && tgt != cgt)
		throw TemplateFunctionError("FibPriorityQueue::length constructor: both specified and different");

	if (length < 0)
		length = 0;
	pq = new T[length];
}


template<class T, bool (*tgt)(const T& a, const T& b)>
FibPriorityQueue<T,tgt>::FibPriorityQueue(const FibPriorityQueue<T,tgt>& to_copy, bool (*cgt)(const T& a, const T& b))
: gt(tgt != nullptr ? tgt : cgt), length(to_copy.length), used(to_copy.used) {
	if (gt == nullptr)
		gt = to_copy.gt;//throw TemplateFunctionError("FibPriorityQueue::copy constructor: neither specified");
	if (tgt != nullptr && cgt != nullptr && tgt != cgt)
		throw TemplateFunctionError("FibPriorityQueue::copy constructor: both specified and different");

	pq = new T[length];
	for (int i=0; i<to_copy.used; ++i)
		pq[i] = to_copy.pq[i];

	if (gt != to_copy.gt)
		heapify();
}


template<class T, bool (*tgt)(const T& a, const T& b)>
FibPriorityQueue<T,tgt>::FibPriorityQueue(const std::initializer_list<T>& il, bool (*cgt)(const T& a, const T& b))
: gt(tgt != nullptr ? tgt : cgt), length(il.size()) {
	if (gt == nullptr)
		throw TemplateFunctionError("FibPriorityQueue::initializer_list constructor: neither specified");
	if (tgt != nullptr && cgt != nullptr && tgt != cgt)
		throw TemplateFunctionError("FibPriorityQueue::initializer_list constructor: both specified and different");

	pq = new T[length];
	int i = 0;
	for (const T& pq_elem : il) {
		pq[i++] = pq_elem;
	}
	used = length;
	heapify();
}


template<class T, bool (*tgt)(const T& a, const T& b)>
template<class Iterable>
FibPriorityQueue<T,tgt>::FibPriorityQueue(const Iterable& i, bool (*cgt)(const T& a, const T& b))
: gt(tgt != nullptr ? tgt : cgt), length(i.size()) {
	if (gt == nullptr)
		throw TemplateFunctionError("FibPriorityQueue::Iterable constructor: neither specified");
	if (tgt != nullptr && cgt != nullptr && tgt != cgt)
		throw TemplateFunctionError("FibPriorityQueue::Iterable constructor: both specified and different");

	pq = new T[length];
	int j = 0;
	for (const T& pq_elem : i) {
		pq[j++] = pq_elem;
	}
	used = length;
	heapify();
}


////////////////////////////////////////////////////////////////////////////////
//
//Queries

template<class T, bool (*tgt)(const T& a, const T& b)>
bool FibPriorityQueue<T,tgt>::empty() const {
	return used == 0;
}


template<class T, bool (*tgt)(const T& a, const T& b)>
int FibPriorityQueue<T,tgt>::size() const {
	return used;
}


template<class T, bool (*tgt)(const T& a, const T& b)>
T& FibPriorityQueue<T,tgt>::peek () const {
	if (empty())
		throw EmptyError("FibPriorityQueue::peek");

	return pq[0];
}


template<class T, bool (*tgt)(const T& a, const T& b)>
std::string FibPriorityQueue<T,tgt>::str() const {
	std::ostringstream answer;
	answer << "FibPriorityQueue[";

	if (length != 0) {
		answer << "0:" << pq[0];
		for (int i = 1; i < length; ++i)
			answer << "," << i << ":" << pq[i];
	}

	answer << "](length=" << length << ",used=" << used << ",mod_count=" << mod_count << ")";
	return answer.str();
}


////////////////////////////////////////////////////////////////////////////////
//
//Commands

template<class T, bool (*tgt)(const T& a, const T& b)>
int FibPriorityQueue<T,tgt>::enqueue(const T& element) {
	this->ensure_length(used+1);
	pq[used++] = element;

	this->percolate_up(used-1);
	++mod_count;
	return 1;
}


template<class T, bool (*tgt)(const T& a, const T& b)>
T FibPriorityQueue<T,tgt>::dequeue() {
	if (this->empty())
		throw EmptyError("FibPriorityQueue::dequeue");

	T to_return = pq[0];
	pq[0] = pq[--used];

	percolate_down(0);

	++mod_count;
	return to_return;
}


template<class T, bool (*tgt)(const T& a, const T& b)>
void FibPriorityQueue<T,tgt>::clear() {
	used = 0;
	++mod_count;
}


template<class T, bool (*tgt)(const T& a, const T& b)>
template <class Iterable>
int FibPriorityQueue<T,tgt>::enqueue_all (const Iterable& i) {
	int count = 0;
	for (const T& v : i)
		 count += enqueue(v);

	return count;
}


////////////////////////////////////////////////////////////////////////////////
//
//Operators

template<class T, bool (*tgt)(const T& a, const T& b)>
FibPriorityQueue<T,tgt>& FibPriorityQueue<T,tgt>::operator = (const FibPriorityQueue<T,tgt>& rhs) {
	if (this == &rhs)
		return *this;

	gt = rhs.gt;	 // if tgt != nullptr, gts are already equal (or compiler error)
	this->ensure_length(rhs.used);
	used = rhs.used;
	for (int i=0; i<rhs.used; ++i)
		pq[i] = rhs.pq[i];

	++mod_count;
	return *this;
}


template<class T, bool (*tgt)(const T& a, const T& b)>
bool FibPriorityQueue<T,tgt>::operator == (const FibPriorityQueue<T,tgt>& rhs) const {
	if (this == &rhs)
		return true;
	if (gt != rhs.gt) //For PriorityQueues to be equal, they need the same gt function, and values
		return false;
	if (used != rhs.size())
		return false;
	FibPriorityQueue<T,tgt>::Iterator l = this->begin(), r = rhs.begin();
	for (int i=0; i<used; ++i, ++l, ++r)
		if (*l != *r)
			return false;

	return true;
}


template<class T, bool (*tgt)(const T& a, const T& b)>
bool FibPriorityQueue<T,tgt>::operator != (const FibPriorityQueue<T,tgt>& rhs) const {
	return !(*this == rhs);
}


template<class T, bool (*tgt)(const T& a, const T& b)>
std::ostream& operator << (std::ostream& outs, const FibPriorityQueue<T,tgt>& p) {
	outs << "priority_queue[";

	if (!p.empty()) {
		ArrayStack<T> temp(p);
		outs << temp.pop();
		for (int i = 1; i < p.used; ++i)
			outs << "," << temp.pop();
	}

	outs << "]:highest";
	return outs;
}


////////////////////////////////////////////////////////////////////////////////
//
//Iterator constructors

template<class T, bool (*tgt)(const T& a, const T& b)>
auto FibPriorityQueue<T,tgt>::begin () const -> FibPriorityQueue<T,tgt>::Iterator {
		return Iterator(const_cast<FibPriorityQueue<T,tgt>*>(this),true);
}


template<class T, bool (*tgt)(const T& a, const T& b)>
auto FibPriorityQueue<T,tgt>::end () const -> FibPriorityQueue<T,tgt>::Iterator {
	return Iterator(const_cast<FibPriorityQueue<T,tgt>*>(this));	//Create empty pq (size == 0)
}


////////////////////////////////////////////////////////////////////////////////
//
//Private helper methods

template<class T, bool (*tgt)(const T& a, const T& b)>
void FibPriorityQueue<T,tgt>::ensure_length(int new_length) {
	if (length >= new_length)
		return;
	T*	old_pq	= pq;
	length = std::max(new_length,2*length);
	pq = new T[length];
	for (int i=0; i<used; ++i)
		pq[i] = old_pq[i];

	delete [] old_pq;
}


template<class T, bool (*tgt)(const T& a, const T& b)>
int FibPriorityQueue<T,tgt>::left_child(int i) const
{return 2*i+1;}

template<class T, bool (*tgt)(const T& a, const T& b)>
int FibPriorityQueue<T,tgt>::right_child(int i) const
{return 2*i+2;}

template<class T, bool (*tgt)(const T& a, const T& b)>
int FibPriorityQueue<T,tgt>::parent(int i) const
{return (i-1)/2;}

template<class T, bool (*tgt)(const T& a, const T& b)>
bool FibPriorityQueue<T,tgt>::is_root(int i) const
{return i == 0;}

template<class T, bool (*tgt)(const T& a, const T& b)>
bool FibPriorityQueue<T,tgt>::in_heap(int i) const
{return i < used;}


template<class T, bool (*tgt)(const T& a, const T& b)>
void FibPriorityQueue<T,tgt>::percolate_up(int i) {
	for (/*parameter*/; !is_root(i) && gt(pq[i],pq[parent(i)]); i = parent(i))
		std::swap(pq[parent(i)],pq[i]);
}


template<class T, bool (*tgt)(const T& a, const T& b)>
void FibPriorityQueue<T,tgt>::percolate_down(int i) {
	for (int l = left_child(i); in_heap(l); l = left_child(i)) {
		int r = right_child(i);
		int max_child = (!in_heap(r) || gt(pq[l],pq[r]) ? l : r);
		if ( gt(pq[i],pq[max_child]) )
			 break;
		std::swap(pq[i],pq[max_child]);
		i = max_child;
	}
}



template<class T, bool (*tgt)(const T& a, const T& b)>
void FibPriorityQueue<T,tgt>::heapify() {
for (int i = used-1; i >= 0; --i)
	percolate_down(i);
}


////////////////////////////////////////////////////////////////////////////////
//
//Iterator class definitions

template<class T, bool (*tgt)(const T& a, const T& b)>
FibPriorityQueue<T,tgt>::Iterator::Iterator(FibPriorityQueue<T,tgt>* iterate_over, bool tgt_nullptr)
: it(*iterate_over,iterate_over->gt), ref_pq(iterate_over), expected_mod_count(iterate_over->mod_count) {
	// Full priority queue; use copy constructor
}


template<class T, bool (*tgt)(const T& a, const T& b)>
FibPriorityQueue<T,tgt>::Iterator::Iterator(FibPriorityQueue<T,tgt>* iterate_over)
: it(iterate_over->gt), ref_pq(iterate_over), expected_mod_count(iterate_over->mod_count) {
	// Empty priority queue; use default constructor (from declaration of "it")
}


template<class T, bool (*tgt)(const T& a, const T& b)>
FibPriorityQueue<T,tgt>::Iterator::~Iterator()
{}


template<class T, bool (*tgt)(const T& a, const T& b)>
T FibPriorityQueue<T,tgt>::Iterator::erase() {
	if (expected_mod_count != ref_pq->mod_count)
		throw ConcurrentModificationError("FibPriorityQueue::Iterator::erase");
	if (!can_erase)
		throw CannotEraseError("FibPriorityQueue::Iterator::erase Iterator cursor already erased");
	if (it.empty())
		throw CannotEraseError("FibPriorityQueue::Iterator::erase Iterator cursor beyond data structure");

	can_erase = false;
	T to_return = it.dequeue();

	//Find value from it (heap iterating over) in main heap; percolate it
	for (int i=0; i<ref_pq->used; ++i)
		if (ref_pq->pq[i] == to_return) {
			ref_pq->pq[i] = ref_pq->pq[--ref_pq->used];
			ref_pq->percolate_up(i);
			ref_pq->percolate_down(i);
			break;
		}

	expected_mod_count = ref_pq->mod_count;
	return to_return;
}


template<class T, bool (*tgt)(const T& a, const T& b)>
std::string FibPriorityQueue<T,tgt>::Iterator::str() const {
	std::ostringstream answer;
	answer << it.str() << "/expected_mod_count=" << expected_mod_count << "/can_erase=" << can_erase;
	return answer.str();
}



template<class T, bool (*tgt)(const T& a, const T& b)>
auto FibPriorityQueue<T,tgt>::Iterator::operator ++ () -> FibPriorityQueue<T,tgt>::Iterator& {
if (expected_mod_count != ref_pq->mod_count)
		throw ConcurrentModificationError("FibPriorityQueue::Iterator::operator ++");

	if (it.empty())
		return *this;

	if (can_erase)
		it.dequeue();
	else
		can_erase = true;

	return *this;
}


template<class T, bool (*tgt)(const T& a, const T& b)>
auto FibPriorityQueue<T,tgt>::Iterator::operator ++ (int) -> FibPriorityQueue<T,tgt>::Iterator {
	if (expected_mod_count != ref_pq->mod_count)
		throw ConcurrentModificationError("FibPriorityQueue::Iterator::operator ++(int)");

	if (it.empty())
		return *this;

	Iterator to_return(*this);
	if (can_erase)
		it.dequeue();
	else
		can_erase = true;

	return to_return;
}


template<class T, bool (*tgt)(const T& a, const T& b)>
bool FibPriorityQueue<T,tgt>::Iterator::operator == (const FibPriorityQueue<T,tgt>::Iterator& rhs) const {
	const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
	if (rhsASI == 0)
		throw IteratorTypeError("FibPriorityQueue::Iterator::operator ==");
	if (expected_mod_count != ref_pq->mod_count)
		throw ConcurrentModificationError("FibPriorityQueue::Iterator::operator ==");
	if (ref_pq != rhsASI->ref_pq)
		throw ComparingDifferentIteratorsError("FibPriorityQueue::Iterator::operator ==");

	//Two iterators on the same heap are equal if their sizes are equal
	return this->it.size() == rhsASI->it.size();
}


template<class T, bool (*tgt)(const T& a, const T& b)>
bool FibPriorityQueue<T,tgt>::Iterator::operator != (const FibPriorityQueue<T,tgt>::Iterator& rhs) const {
	const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
	if (rhsASI == 0)
		throw IteratorTypeError("FibPriorityQueue::Iterator::operator !=");
	if (expected_mod_count != ref_pq->mod_count)
		throw ConcurrentModificationError("FibPriorityQueue::Iterator::operator !=");
	if (ref_pq != rhsASI->ref_pq)
		throw ComparingDifferentIteratorsError("FibPriorityQueue::Iterator::operator !=");

	return this->it.size() != rhsASI->it.size();
}


template<class T, bool (*tgt)(const T& a, const T& b)>
T& FibPriorityQueue<T,tgt>::Iterator::operator *() const {
	if (expected_mod_count != ref_pq->mod_count)
		throw ConcurrentModificationError("FibPriorityQueue::Iterator::operator *");
	if (!can_erase || it.empty())
		throw IteratorPositionIllegal("FibPriorityQueue::Iterator::operator * Iterator illegal: exhausted");

	return it.peek();
}


template<class T, bool (*tgt)(const T& a, const T& b)>
T* FibPriorityQueue<T,tgt>::Iterator::operator ->() const {
	if (expected_mod_count != ref_pq->mod_count)
		throw ConcurrentModificationError("FibPriorityQueue::Iterator::operator *");
	if (!can_erase || it.empty())
		throw IteratorPositionIllegal("FibPriorityQueue::Iterator::operator -> Iterator illegal: exhausted");

	return &it.peek();
}

}

#endif /* FIB_PRIORITY_QUEUE_HPP_ */
