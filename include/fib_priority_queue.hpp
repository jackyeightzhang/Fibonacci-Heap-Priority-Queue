#ifndef FIB_PRIORITY_QUEUE_HPP_
#define FIB_PRIORITY_QUEUE_HPP_

#include <cmath>

#include <string>
#include <iostream>
#include <sstream>
#include <initializer_list>
#include "courselib/ics_exceptions.hpp"
#include <utility>							//For std::swap function
#include "array_stack.hpp"			//See operator <<
#include "array_set.hpp"

namespace ics {


//Instantiate the templated class supplying tgt(a,b): true, iff a has higher priority than b.
//If tgt is defaulted to nullptr in the template, then a constructor must supply cgt.
//If both tgt and cgt are supplied, then they must be the same (by ==) function.
//If neither is supplied, or both are supplied but different, TemplateFunctionError is raised.
//The (unique) non-nullptr value supplied by tgt/cgt is stored in the instance variable gt.
template<class T, bool (*tgt)(const T& a, const T& b) = nullptr> 
class FibPriorityQueue {
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
		bool empty		() const;
		int	size		() const;
		T&	peek		() const;
		std::string str	() const; //supplies useful debugging information; contrast to operator <<


		//Commands
		int	enqueue	(const T& element);
		T dequeue	();
		void clear	();

		//Iterable class must support "for-each" loop: .begin()/.end() and prefix ++ on returned result
		//template <class Iterable>
		//int enqueue_all (const Iterable& i);


		//Operators
		FibPriorityQueue<T,tgt>& operator = (const FibPriorityQueue<T,tgt>& rhs);
		bool operator == (const FibPriorityQueue<T,tgt>& rhs) const;
		bool operator != (const FibPriorityQueue<T,tgt>& rhs) const;

		template<class T2, bool (*gt2)(const T2& a, const T2& b)>
		friend std::ostream& operator << (std::ostream& outs, const FibPriorityQueue<T2,gt2>& pq);


		/*
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


		Iterator begin	() const;
		Iterator end	() const;*/
		
	private:
		class HN {
		public:
	   	    HN(){}
			HN(const HN& toCopy)	: value(toCopy.value) { childNodes = toCopy.childNodes; }
			HN(const T& value)		: value(value) {}
			
			int addChild(HN* newChildNode) { return childNodes.insert(newChildNode); }
			const ArraySet<HN*>& getChildNodes() { return childNodes; }
			const T& getValue() { return value; }

		private:	
			ArraySet<HN*> childNodes;
	    	T value;			
		};

		class DLN {
	    public:
	        DLN() 					: heapNode(nullptr) { prevNode = this; nextNode = this; }
			DLN(const DLN& toCopy) 	: heapNode(toCopy.heapNode), prevNode(toCopy.prevNode), nextNode(toCopy.nextNode) {}
			DLN(HN* heapNode)		: heapNode(heapNode) { prevNode = this; nextNode = this; }	
						
			HN* heapNode;
	   		DLN* prevNode;
			DLN* nextNode;
		};
		
		bool (*gt) (const T& a, const T& b);	// The gt used by enqueue (from template or constructor)
		int nodeCount		= 0;				// The number of nodes in the heap
		int rootNodeCount	= 0;				// The number of root nodes in the root node list
		DLN* minRootNode	= nullptr;			// A pointer to the minimum value 
		
		//Helper methods
		void addRootNode(DLN* toAdd);			// Adds a root node to the root list
		void consolidateRank();					// Ensures no two root nodes have the same rank
};





////////////////////////////////////////////////////////////////////////////////
//
//FibPriorityQueue class and related definitions

//Destructor/Constructors

template<class T, bool (*tgt)(const T& a, const T& b)>
FibPriorityQueue<T,tgt>::~FibPriorityQueue() {
	while(minRootNode != nullptr) dequeue();
}


template<class T, bool (*tgt)(const T& a, const T& b)>
FibPriorityQueue<T,tgt>::FibPriorityQueue(bool (*cgt)(const T& a, const T& b))
: gt(tgt != nullptr ? tgt : cgt) {
	if (gt == nullptr)
		throw TemplateFunctionError("FibPriorityQueue::default constructor: neither specified");
	if (tgt != nullptr && cgt != nullptr && tgt != cgt)
		throw TemplateFunctionError("FibPriorityQueue::default constructor: both specified and different");
}

template<class T, bool (*tgt)(const T& a, const T& b)>
FibPriorityQueue<T,tgt>::FibPriorityQueue(const FibPriorityQueue<T,tgt>& toCopy, bool (*cgt)(const T& a, const T& b))
: gt(tgt != nullptr ? tgt : cgt), nodeCount(toCopy.nodeCount) {
	if (gt == nullptr)
		gt = toCopy.gt; throw TemplateFunctionError("FibPriorityQueue::copy constructor: neither specified");
	if (tgt != nullptr && cgt != nullptr && tgt != cgt)
		throw TemplateFunctionError("FibPriorityQueue::copy constructor: both specified and different");

}


template<class T, bool (*tgt)(const T& a, const T& b)>
FibPriorityQueue<T,tgt>::FibPriorityQueue(const std::initializer_list<T>& il, bool (*cgt)(const T& a, const T& b))
: gt(tgt != nullptr ? tgt : cgt), nodeCount(il.size()) {
	if (gt == nullptr)
		throw TemplateFunctionError("FibPriorityQueue::initializer_list constructor: neither specified");
	if (tgt != nullptr && cgt != nullptr && tgt != cgt)
		throw TemplateFunctionError("FibPriorityQueue::initializer_list constructor: both specified and different");
}


template<class T, bool (*tgt)(const T& a, const T& b)>
template<class Iterable>
FibPriorityQueue<T,tgt>::FibPriorityQueue(const Iterable& i, bool (*cgt)(const T& a, const T& b))
: gt(tgt != nullptr ? tgt : cgt), nodeCount(i.size()) {
	if (gt == nullptr)
		throw TemplateFunctionError("FibPriorityQueue::Iterable constructor: neither specified");
	if (tgt != nullptr && cgt != nullptr && tgt != cgt)
		throw TemplateFunctionError("FibPriorityQueue::Iterable constructor: both specified and different");

}


////////////////////////////////////////////////////////////////////////////////
//
//Queries

template<class T, bool (*tgt)(const T& a, const T& b)>
bool FibPriorityQueue<T,tgt>::empty() const {
	return nodeCount == 0; 
}


template<class T, bool (*tgt)(const T& a, const T& b)>
int FibPriorityQueue<T,tgt>::size() const {
	return nodeCount;
}


template<class T, bool (*tgt)(const T& a, const T& b)>
T& FibPriorityQueue<T,tgt>::peek () const {
	if(empty()) throw EmptyError("FibPriorityQueue::peek"); 
	return minRootNode->heapNode->getValue();
}


template<class T, bool (*tgt)(const T& a, const T& b)>
std::string FibPriorityQueue<T,tgt>::str() const {
	std::ostringstream answer;
	answer << "FibPriorityQueue" << std::endl;
	return answer.str();
}


////////////////////////////////////////////////////////////////////////////////
//
//Commands

template<class T, bool (*tgt)(const T& a, const T& b)>
int FibPriorityQueue<T,tgt>::enqueue(const T& element) {
	HN* tempHeapNode = new HN(element);
	DLN* tempRootNode = new DLN(tempHeapNode);

	addRootNode(tempRootNode);

	++rootNodeCount;
	++nodeCount; 
	return 1;
}


template<class T, bool (*tgt)(const T& a, const T& b)>
T FibPriorityQueue<T,tgt>::dequeue() {
	if (this->empty())
		throw EmptyError("FibPriorityQueue::dequeue");

	T minVal = minRootNode->heapNode->getValue();

	DLN* tempRootNode = nullptr;
	for(HN* currentChild : minRootNode->heapNode->getChildNodes()) {
		tempRootNode = new DLN(currentChild);
		addRootNode(tempRootNode);
	}

	DLN* oldMinRootNode = minRootNode;

	minRootNode = minRootNode->nextNode;
	if(minRootNode == oldMinRootNode) {
		minRootNode = nullptr;
	} else {		
		oldMinRootNode->nextNode->prevNode = oldMinRootNode->prevNode;
		oldMinRootNode->prevNode->nextNode = oldMinRootNode->nextNode;
	}

	delete oldMinRootNode->heapNode;
	delete oldMinRootNode;
	--rootNodeCount;

	consolidateRank();

	return minVal;
}


template<class T, bool (*tgt)(const T& a, const T& b)>
void FibPriorityQueue<T,tgt>::clear() {

}

/*
template<class T, bool (*tgt)(const T& a, const T& b)>
template <class Iterable>
int FibPriorityQueue<T,tgt>::enqueue_all (const Iterable& i) {
	int count = 0;
		return count;
}
*/

////////////////////////////////////////////////////////////////////////////////
//
//Operators

template<class T, bool (*tgt)(const T& a, const T& b)>
FibPriorityQueue<T,tgt>& FibPriorityQueue<T,tgt>::operator = (const FibPriorityQueue<T,tgt>& rhs) {
	return *this;
}


template<class T, bool (*tgt)(const T& a, const T& b)>
bool FibPriorityQueue<T,tgt>::operator == (const FibPriorityQueue<T,tgt>& rhs) const {
	return true;
}


template<class T, bool (*tgt)(const T& a, const T& b)>
bool FibPriorityQueue<T,tgt>::operator != (const FibPriorityQueue<T,tgt>& rhs) const {
	return !(*this == rhs);
}


template<class T, bool (*tgt)(const T& a, const T& b)>
std::ostream& operator << (std::ostream& outs, const FibPriorityQueue<T,tgt>& p) {	
	return outs;
}


////////////////////////////////////////////////////////////////////////////////
//
//Iterator constructors
/*
template<class T, bool (*tgt)(const T& a, const T& b)>
auto FibPriorityQueue<T,tgt>::begin () const -> FibPriorityQueue<T,tgt>::Iterator {
	return Iterator(const_cast<FibPriorityQueue<T,tgt>*>(this),true);
}


template<class T, bool (*tgt)(const T& a, const T& b)>
auto FibPriorityQueue<T,tgt>::end () const -> FibPriorityQueue<T,tgt>::Iterator {
	return Iterator(const_cast<FibPriorityQueue<T,tgt>*>(this));	//Create empty pq (size == 0)
}

*/
////////////////////////////////////////////////////////////////////////////////
//
//Private helper methods
template<class T, bool (*tgt)(const T& a, const T& b)>
void FibPriorityQueue<T,tgt>::addRootNode(DLN* toAdd){
	if(minRootNode == nullptr) {
		minRootNode = toAdd;
	} else {

		minRootNode->nextNode->prevNode = toAdd;
		toAdd->prevNode = minRootNode;

		toAdd->nextNode = minRootNode->nextNode;
		minRootNode->nextNode = toAdd;
	}
}

template<class T, bool (*tgt)(const T& a, const T& b)>
void FibPriorityQueue<T,tgt>::consolidateRank(){
	DLN* rankArray[static_cast<int>(log2(nodeCount))];

	for(int rootNodeIndex = 0; rootNodeIndex < rootNodeCount; ++rootNodeIndex);
		
}

////////////////////////////////////////////////////////////////////////////////
//
//Iterator class definitions
/*
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
*/
}

#endif /* FIB_PRIORITY_QUEUE_HPP_ */
