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
#include "hash_map.hpp"
#include "array_queue.hpp"
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
//				T					 erase();
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
				friend Iterator FibPriorityQueue<T,tgt>::end   () const;

			private:
				//If can_erase is false, the value has been removed from "it" (++ does nothing)
				FibPriorityQueue<T,tgt>		it; //copy of HPQ (from begin), to use as iterator via dequeue
				FibPriorityQueue<T,tgt>* 	ref_pq;
				int							expected_mod_count;
				bool						can_erase = true;

				//Called in friends begin/end
				//These constructors have different initializers (see it(...) in first one)
				Iterator(FibPriorityQueue<T,tgt>* iterate_over, bool from_begin);		// Called by begin
				Iterator(FibPriorityQueue<T,tgt>* iterate_over);						// Called by end
		};


		Iterator begin	() const;
		Iterator end	() const;
		
	private:
		class HN {
		public:
			HN(const HN& toCopy)	: value(toCopy.value) { childNodes = toCopy.childNodes; }
			HN(const T& value)		: value(value) {}
			
			int addChild(HN* newChildNode) { return childNodes.insert(newChildNode); }
			ArraySet<HN*>& getChildNodes() { return childNodes; }
			T& getValue() { return value; }

		private:	
			ArraySet<HN*> childNodes;
	    	T value;			
		};

		class DLN {
	    public:
			DLN(const DLN& toCopy) 	: heapNode(toCopy.heapNode), prevNode(toCopy.prevNode), nextNode(toCopy.nextNode) {}
			DLN(HN* heapNode)		: heapNode(heapNode) { prevNode = this; nextNode = this; }	


			int addChild(HN* newChildNode) { return heapNode->getChildNodes().insert(newChildNode); }
			ArraySet<HN*>& getChildNodes() { return heapNode->getChildNodes(); }
			T& getValue() { return heapNode->getValue(); }
						
			HN* heapNode;
	   		DLN* prevNode;
			DLN* nextNode;
		};
		
		bool (*gt) (const T& a, const T& b);				// The gt used by enqueue (from template or constructor)
		int nodeCount		= 0;							// The number of nodes in the heap
		int modCount		= 0;							// For sensing concurrent modification
		DLN* headRootNode	= nullptr;						// A pointer to the head value 

		
		//Helper methods
		void addRootNode(DLN* prevRootNode, DLN* toAdd);	//Adds a root node to the root list
		void removeRootNode(DLN* toRemove);					//removes a root node from the root list
		void consolidateRank();								//Ensures no two root nodes have the same rank
		DLN* copyFibTree(DLN* originalTree);
		HN* copyFibBranch(HN* originalBranch);
		void destroyFibTree(DLN* originalTree);
		void destroyFibBranch(HN* originalBranch);
};





////////////////////////////////////////////////////////////////////////////////
//
//FibPriorityQueue class and related definitions

//Destructor/Constructors

template<class T, bool (*tgt)(const T& a, const T& b)>
FibPriorityQueue<T,tgt>::~FibPriorityQueue() {
	destroyFibTree(headRootNode);
}


template<class T, bool (*tgt)(const T& a, const T& b)>
FibPriorityQueue<T,tgt>::FibPriorityQueue(bool (*cgt)(const T& a, const T& b))
: gt(tgt != nullptr ? tgt : cgt) {
	if(gt == nullptr)
		throw TemplateFunctionError("FibPriorityQueue::default constructor: neither specified");
	if(tgt != nullptr && cgt != nullptr && tgt != cgt)
		throw TemplateFunctionError("FibPriorityQueue::default constructor: both specified and different");
}

template<class T, bool (*tgt)(const T& a, const T& b)>
FibPriorityQueue<T,tgt>::FibPriorityQueue(const FibPriorityQueue<T,tgt>& toCopy, bool (*cgt)(const T& a, const T& b))
: gt(tgt != nullptr ? tgt : cgt), nodeCount(toCopy.nodeCount) {
	if(gt == nullptr)
		gt = toCopy.gt;//throw TemplateFunctionError("FibPriorityQueue::copy constructor: neither specified");
	if(tgt != nullptr && cgt != nullptr && tgt != cgt)
		throw TemplateFunctionError("FibPriorityQueue::copy constructor: both specified and different");

	headRootNode = copyFibTree(toCopy.headRootNode);
}


template<class T, bool (*tgt)(const T& a, const T& b)>
FibPriorityQueue<T,tgt>::FibPriorityQueue(const std::initializer_list<T>& il, bool (*cgt)(const T& a, const T& b))
: gt(tgt != nullptr ? tgt : cgt) {
	if(gt == nullptr)
		throw TemplateFunctionError("FibPriorityQueue::initializer_list constructor: neither specified");
	if(tgt != nullptr && cgt != nullptr && tgt != cgt)
		throw TemplateFunctionError("FibPriorityQueue::initializer_list constructor: both specified and different");

	for(const T& element : il) enqueue(element);
	consolidateRank();
	modCount = 0;
}


template<class T, bool (*tgt)(const T& a, const T& b)>
template<class Iterable>
FibPriorityQueue<T,tgt>::FibPriorityQueue(const Iterable& i, bool (*cgt)(const T& a, const T& b))
: gt(tgt != nullptr ? tgt : cgt) {
	if(gt == nullptr)
		throw TemplateFunctionError("FibPriorityQueue::Iterable constructor: neither specified");
	if(tgt != nullptr && cgt != nullptr && tgt != cgt)
		throw TemplateFunctionError("FibPriorityQueue::Iterable constructor: both specified and different");

	for(const T& element : i) enqueue(element);
	consolidateRank();
	modCount = 0;
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
T& FibPriorityQueue<T,tgt>::peek() const {
	if(empty()) throw EmptyError("FibPriorityQueue::peek"); 
	return headRootNode->getValue();
}


template<class T, bool (*tgt)(const T& a, const T& b)>
std::string FibPriorityQueue<T,tgt>::str() const {
	std::ostringstream answer;
	answer << "PriorityQueue" << std::endl;
	return answer.str();
}


////////////////////////////////////////////////////////////////////////////////
//
//Commands

template<class T, bool (*tgt)(const T& a, const T& b)>
int FibPriorityQueue<T,tgt>::enqueue(const T& element) {
	HN* tempHeapNode = new HN(element);
	DLN* tempRootNode = new DLN(tempHeapNode);

	if(headRootNode == nullptr) {
		headRootNode = tempRootNode;
	} else {
		addRootNode(headRootNode, tempRootNode);
	}

	if(gt(tempRootNode->getValue(), headRootNode->getValue())) {
		headRootNode = tempRootNode;
	}

	++nodeCount; 
	++modCount;
	return 1;
}


template<class T, bool (*tgt)(const T& a, const T& b)>
T FibPriorityQueue<T,tgt>::dequeue() {
	if (this->empty())
		throw EmptyError("FibPriorityQueue::dequeue");

	T headValue = headRootNode->getValue();

	DLN* tempRootNode = nullptr;
	for(HN* currentChild : headRootNode->getChildNodes()) {
		tempRootNode = new DLN(currentChild);
		addRootNode(headRootNode, tempRootNode);
	}

	DLN* oldHeadRootNode = headRootNode;

	headRootNode = headRootNode->nextNode;
	if(headRootNode == oldHeadRootNode) {
		headRootNode = nullptr;
	} else {
		removeRootNode(oldHeadRootNode);
	}

	delete oldHeadRootNode->heapNode;
	delete oldHeadRootNode;
	--nodeCount;
	++modCount;

	consolidateRank();
	return headValue;
}


template<class T, bool (*tgt)(const T& a, const T& b)>
void FibPriorityQueue<T,tgt>::clear() {
	destroyFibTree(headRootNode);
	headRootNode = nullptr;
	nodeCount = 0;
	++modCount;
}


template<class T, bool (*tgt)(const T& a, const T& b)>
template <class Iterable>
int FibPriorityQueue<T,tgt>::enqueue_all (const Iterable& i) {
 	int count = 0;
 	for (const T& v : i)
		count += enqueue(v);

	consolidateRank();
	return count;
}


////////////////////////////////////////////////////////////////////////////////
//
//Operators

template<class T, bool (*tgt)(const T& a, const T& b)>
FibPriorityQueue<T,tgt>& FibPriorityQueue<T,tgt>::operator = (const FibPriorityQueue<T,tgt>& rhs) {	
	//check if it is assigning into itself
	if(this == &rhs) return *this;
	
	//delete current fib tree
	destroyFibTree(headRootNode);
	
	//make copy of rhs fib tree
	headRootNode = copyFibTree(rhs.headRootNode);
	
	//update current fib tree's info
	nodeCount = rhs.nodeCount;
	gt = rhs.gt;
	return *this;
}


template<class T, bool (*tgt)(const T& a, const T& b)>
bool FibPriorityQueue<T,tgt>::operator == (const FibPriorityQueue<T,tgt>& rhs) const {
	//check if current comparing itself
	if(this == &rhs) return true;
	
	//check if gt function are the same
	if(this->gt != rhs.gt) return false;
	
	if(nodeCount != rhs.nodeCount) return false;
	FibPriorityQueue<T,tgt>::Iterator left = this->begin(), right = rhs.begin();
	for(; left != this->end(); ++left, ++right)
		if (*left != *right)
			return false;
	return true;		
	/*
	//check if either fib tree is empty
	DLN* branchCursor = headRootNode;
	if(branchCursor == nullptr) return rhs.headRootNode == branchCursor;
	if(rhs.headRootNode == nullptr) return false;

	//traverse through all branches and create hash map
	HashMap<T, int, > toCompare; 
	ArrayQueue<HN*> heapNodeQueue;
	HN* tempHeapNode= nullptr;
	for(int i = 1; i  < rootNodeCount; i++, branchCursor = branchCursor->nextNode) {
		heapNodeQueue.enqueue(branchCursor->heapNode);
		while(!heapNodeQueue.empty()) {
			tempHeapNode = heapNodeQueue.dequeue();
			if(toCompare.has_key(tempHeapNode->getValue())) ++toCompare[tempHeapNode->getValue()];
			else toCompare[tempHeapNode->getValue()] = 1;
			heapNodeQueue.enqueue_all(tempHeapNode->getChildNodes());
		}
	}

	//traverse through rhs and check all the values in the hash map
	branchCursor = rhs.headRootNode;
	for(int i = 1; i < rhs.rootNodeCount; i++, branchCursor = branchCursor->nextNode) {
		heapNodeQueue.enqueue(branchCursor->heapNode);
		while(!heapNodeQueue.empty()) {
			tempHeapNode = heapNodeQueue.dequeue(); 
			if(toCompare.has_key(tempHeapNode->getValue())) --toCompare[tempHeapNode->getValue()];
			//if it does not exist then they are not equivalent
			else return false;
			//erase if the occurances hit zero
			if(!toCompare[tempHeapNode->getValue()]) toCompare.erase(tempHeapNode->getValue());
			heapNodeQueue.enqueue_all(tempHeapNode->getChildNodes());
		}
	}
	//check if empty hash map
	return toCompare.empty();
	*/
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
		for (int i = 1; i < p.nodeCount; ++i)
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
void FibPriorityQueue<T,tgt>::addRootNode(DLN* prevRootNode, DLN* toAdd){
	prevRootNode->nextNode->prevNode = toAdd;
	toAdd->prevNode = prevRootNode;

	toAdd->nextNode = prevRootNode->nextNode;
	prevRootNode->nextNode = toAdd;
}

template<class T, bool (*tgt)(const T& a, const T& b)>
void FibPriorityQueue<T,tgt>::removeRootNode(DLN* toRemove){
	toRemove->prevNode->nextNode = toRemove->nextNode;
	toRemove->nextNode->prevNode = toRemove->prevNode;
}

template<class T, bool (*tgt)(const T& a, const T& b)>
void FibPriorityQueue<T,tgt>::consolidateRank() {
	if(headRootNode == nullptr || headRootNode->nextNode == headRootNode) return;

	int currentRank = -1;
	T headValue = headRootNode->getValue();
	DLN* stopRootNode = headRootNode;
	DLN* currentRootNode = headRootNode;
	DLN* rankArray[static_cast<int>(log2(nodeCount)) + 1] = { nullptr };

	//iterate through all root nodes
	do {
		currentRank = currentRootNode->getChildNodes().size();

		//merge fib branches until branch has unique rank
		while(rankArray[currentRank] != nullptr) {
			if(gt(rankArray[currentRank]->getValue(), currentRootNode->getValue())) {
				std::swap(rankArray[currentRank]->heapNode, currentRootNode->heapNode);
			}

			currentRootNode->addChild(rankArray[currentRank]->heapNode);

			//move stopRootNode forward if it is going to be deleted
			if(rankArray[currentRank] == stopRootNode) {
				stopRootNode = stopRootNode->nextNode;
			}

			removeRootNode(rankArray[currentRank]);
			delete rankArray[currentRank];
			rankArray[currentRank++] = nullptr;
		}

		//update headRootNode to point to max value
		if(!gt(headValue, currentRootNode->getValue())) {
			headRootNode = currentRootNode;
			headValue = currentRootNode->getValue();
		}

		//save unique fib branch in the rank array
		rankArray[currentRank] = currentRootNode;
		currentRootNode = currentRootNode->nextNode;
	} while(currentRootNode != stopRootNode);
}

template<class T, bool (*tgt)(const T& a, const T& b)>
typename FibPriorityQueue<T,tgt>::DLN* FibPriorityQueue<T,tgt>::copyFibTree(DLN* originalTree) {
	DLN* cursor = originalTree;
	if(cursor == nullptr) return cursor;
	//add headRootNode in order to use addRootNode
	//also make a deep copy of the fib branch and connect it to the root
	DLN* returnHeadRootNode = new DLN(copyFibBranch(cursor->heapNode));	
	cursor = cursor->nextNode;

	//traverse through every root node and add to the doublely linked list copy
	while(cursor != originalTree)	{	
		//make a deep copy of fib branch at cursor and connect to root
		DLN* tempRootNode = new DLN(copyFibBranch(cursor->heapNode));
		//connect this hanging root node to the doublely linked list
		addRootNode(returnHeadRootNode, tempRootNode);
		cursor = cursor->nextNode;
	}
	return returnHeadRootNode;
}
	
template<class T, bool (*tgt)(const T& a, const T& b)>
typename FibPriorityQueue<T,tgt>::HN* FibPriorityQueue<T,tgt>::copyFibBranch(HN* originalBranch) {
	//traverse recursively through fib branch
	HN* copyBranch = new HN(originalBranch->getValue());
	//make deep copies of the child nodes, else jump to return
	for(auto childNode : originalBranch->getChildNodes())	
		copyBranch->addChild(copyFibBranch(childNode));
	return copyBranch;
}

template<class T, bool (*tgt)(const T& a, const T& b)>
void FibPriorityQueue<T,tgt>::destroyFibTree(DLN* originalTree) {
	DLN* cursor = originalTree;

	//traverse through doublely linked list 
	while(cursor != nullptr) {		
		//delete entire fib heap
		destroyFibBranch(cursor->heapNode);
	
		//delete root node
		DLN* toDelete = cursor;
		//if there exist only on branch then set it to null to end loop
		if(cursor == cursor->nextNode) cursor = nullptr;
		else cursor = cursor->nextNode;
		removeRootNode(toDelete);
		delete toDelete;
	}
}

template<class T, bool (*tgt)(const T& a, const T& b)>
void FibPriorityQueue<T,tgt>::destroyFibBranch(HN* originalBranch) {	
	//recursively delete children before deleting node, else delete self
	for(auto childNode : originalBranch->getChildNodes())	
		destroyFibBranch(childNode);
	//delete node
	delete originalBranch;
}
////////////////////////////////////////////////////////////////////////////////
//
//Iterator class definitions

template<class T, bool (*tgt)(const T& a, const T& b)>
FibPriorityQueue<T,tgt>::Iterator::Iterator(FibPriorityQueue<T,tgt>* iterate_over, bool tgt_nullptr)
: it(*iterate_over,iterate_over->gt), ref_pq(iterate_over), expected_mod_count(iterate_over->modCount) {
	// Full priority queue; use copy constructor
}


template<class T, bool (*tgt)(const T& a, const T& b)>
FibPriorityQueue<T,tgt>::Iterator::Iterator(FibPriorityQueue<T,tgt>* iterate_over)
: it(iterate_over->gt), ref_pq(iterate_over), expected_mod_count(iterate_over->modCount) {
	// Empty priority queue; use default constructor (from declaration of "it")
}


template<class T, bool (*tgt)(const T& a, const T& b)>
FibPriorityQueue<T,tgt>::Iterator::~Iterator()
{}

/*
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
			ref_pq->percolate_up(i);originalBranch-
			ref_pq->percolate_down(i);
			break;
		}

	expected_mod_count = ref_pq->mod_count;
	return to_return;
}*/


template<class T, bool (*tgt)(const T& a, const T& b)>
std::string FibPriorityQueue<T,tgt>::Iterator::str() const {
	std::ostringstream answer;
	answer << it.str() << "/expected_mod_count=" << expected_mod_count << "/can_erase=" << can_erase;
	return answer.str();
}


template<class T, bool (*tgt)(const T& a, const T& b)>
auto FibPriorityQueue<T,tgt>::Iterator::operator ++ () -> FibPriorityQueue<T,tgt>::Iterator& {
if (expected_mod_count != ref_pq->modCount)
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
	if (expected_mod_count != ref_pq->modCount)
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
	if (expected_mod_count != ref_pq->modCount)
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
	if (expected_mod_count != ref_pq->modCount)
		throw ConcurrentModificationError("FibPriorityQueue::Iterator::operator !=");
	if (ref_pq != rhsASI->ref_pq)
		throw ComparingDifferentIteratorsError("FibPriorityQueue::Iterator::operator !=");

	return this->it.size() != rhsASI->it.size();
}


template<class T, bool (*tgt)(const T& a, const T& b)>
T& FibPriorityQueue<T,tgt>::Iterator::operator *() const {
	if (expected_mod_count != ref_pq->modCount)
		throw ConcurrentModificationError("FibPriorityQueue::Iterator::operator *");
	if (!can_erase || it.empty())
		throw IteratorPositionIllegal("FibPriorityQueue::Iterator::operator * Iterator illegal: exhausted");

	return it.peek();
}


template<class T, bool (*tgt)(const T& a, const T& b)>
T* FibPriorityQueue<T,tgt>::Iterator::operator ->() const {
	if (expected_mod_count != ref_pq->modCount)
		throw ConcurrentModificationError("FibPriorityQueue::Iterator::operator *");
	if (!can_erase || it.empty())
		throw IteratorPositionIllegal("FibPriorityQueue::Iterator::operator -> Iterator illegal: exhausted");

	return &it.peek();
}

}

#endif /* FIB_PRIORITY_QUEUE_HPP_ */
