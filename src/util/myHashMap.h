/****************************************************************************
  FileName     [ myHashMap.h ]
  PackageName  [ util ]
  Synopsis     [ Define HashMap and Cache ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2009-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_HASH_MAP_H
#define MY_HASH_MAP_H

#include <vector>
#include <util.h>

using namespace std;

// TODO: (Optionally) Implement your own HashMap and Cache classes.

//-----------------------
// Define HashMap classes
//-----------------------
// To use HashMap ADT, you should define your own HashKey class.
// It should at least overload the "()" and "==" operators.
//
// class HashKey
// {
// public:
//    HashKey() {}
// 
//    size_t operator() () const { return 0; }
// 
//    bool operator == (const HashKey& k) const { return true; }
// 
// private:
// };


// TODO: (Optionally) Implement your own HashMap and Cache classes.

//-----------------------
// Define HashMap classes
//-----------------------
// To use HashMap ADT, you should define your own HashKey class.
// It should at least overload the "()" and "==" operators.

class HashKey
{
public:
   HashKey(size_t in1, size_t in2) { fanin1 = in1; fanin2 = in2; }
   
   size_t operator() () const { return fanin2 * fanin1; }
   bool operator == (const HashKey& key) const { return (this->fanin1 == key.fanin1 && this->fanin2 == key.fanin2)
                                                     || (this->fanin1 == key.fanin2 && this->fanin2 == key.fanin1);
   }


private:
size_t fanin1;
size_t fanin2;
};

template <class HashKey, class HashData>
class HashMap
{
typedef pair<HashKey, HashData> HashNode;

public:
   HashMap(size_t b=0) : _numBuckets(0), _buckets(0) { if (b != 0) init(b); }
   ~HashMap() { reset(); }

   // [Optional] TODO: implement the HashMap<HashKey, HashData>::iterator
   // o An iterator should be able to go through all the valid HashNodes
   //   in the HashMap
   // o Functions to be implemented:
   //   - constructor(s), destructor
   //   - operator '*': return the HashNode
   //   - ++/--iterator, iterator++/--
   //   - operators '=', '==', !="
   //
   class iterator
   {
      friend class HashMap<HashKey, HashData>;

   public:
      iterator(const iterator& it): scope(it.scope){ iter = it.iter; myBucket = it.myBucket; }
      iterator(HashMap<HashKey, HashData>* s, size_t myB, bool begin): scope(s){ 
         if(begin)
            iter = (*s)[myB].begin();
         else
            iter = (*s)[myB].end();
         myBucket = myB;
      }
      ~iterator() { }
      const HashNode& operator * () const { return *iter; }
      bool operator != (const iterator& i) const { return (this->iter != i.iter) ||
                                                          (this->scope != i.scope) ||
                                                          (this->myBucket != i.myBucket); }
      bool operator == (const iterator& i) const { return (this->iter == i.iter) &&
                                                          (this->scope == i.scope)&&
                                                          (this->myBucket == i.myBucket); }
      iterator& operator -- () {
         while(iter == (*scope)[myBucket].begin() && myBucket != 0)
         {
            iter = (*scope)[--myBucket].end();
         }
         iter--;
         return *this;
      }
      iterator& operator -- (int) {
         iterator temp = *this;
         while(iter == (*scope)[myBucket].begin() && myBucket != 0)
         {
            iter = (*scope)[--myBucket].end();
         }
         iter--;
         return temp;
      }
      iterator& operator ++ () {
         iter++;
         //cout << myBucket << "/" << scope->numBuckets() << endl;
         while(iter == (*scope)[myBucket].end() && myBucket != (scope->numBuckets())-1)
         {
            iter = (*scope)[++myBucket].begin();
         }
         return (*this);
      }
      iterator& operator ++ (int) {
         iterator temp = *this;
         iter++;
         
         while(iter == (*scope)[myBucket].end() && myBucket != (scope->numBuckets())-1)
         {
            iter = (*scope)[++myBucket].begin();
         }
         return temp;
      }
   private:
      HashMap<HashKey, HashData>* scope;
      typename vector<HashNode>::iterator iter;
      size_t myBucket;
   };

   void init(size_t b) {
      reset(); _numBuckets = getHashSize(b); _buckets = new vector<HashNode>[_numBuckets]; }
   void reset() {
      _numBuckets = 0;
      if (_buckets) { delete [] _buckets; _buckets = 0; }
   }
   void clear() {
      for (size_t i = 0; i < _numBuckets; ++i) _buckets[i].clear();
   }
   size_t numBuckets() const { return _numBuckets; }

   vector<HashNode>& operator [] (size_t i) { return _buckets[i]; }
   const vector<HashNode>& operator [](size_t i) const { return _buckets[i]; }

   // TODO: implement these functions
   //
   // Point to the first valid data
   iterator begin() const {
      size_t num = 0;
      while(num < _numBuckets)
      {
         if(_buckets[num].size())
            return iterator(const_cast<HashMap<HashKey, HashData>*>(this), num, true);
         num++;
      }
      return end();
   }
   // Pass the end
   iterator end() const {
      return iterator(const_cast<HashMap<HashKey, HashData>*>(this), _numBuckets - 1, false);
   }
   // return true if no valid data
   bool empty() const { return begin() == end(); }
   // number of valid data
   size_t size() const {
      size_t s = 0;
      iterator it = begin();
      while(it != end())
      {
         s++;
         it++;
      }
      return s; 
   }

   // check if k is in the hash...
   // if yes, return true;
   // else return false;
   bool check(const HashKey& k) const {
      size_t num = bucketNum(k);
      typename vector<HashNode>::iterator it = _buckets[num].begin();
      while(it != _buckets[num].end())
      {
         if((*it).first == k)
            return true;
         it++;
      }
      return false;
   }
   
   // query if k is in the hash...
   // if yes, replace d with the data in the hash and return true;
   // else return false;
   bool query(const HashKey& k, HashData& d) const {
      size_t num = bucketNum(k);
      typename vector<HashNode>::iterator it = _buckets[num].begin();
      //cout << _buckets[num].size() << " ";
      while(it != _buckets[num].end())
      {
         if((*it).first == k)
         {
            d = (*it).second;
            return true;
         }
         it++;
      }
      return false;
   }

   // update the entry in hash that is equal to k (i.e. == return true)
   // if found, update that entry with d and return true;
   // else insert d into hash as a new entry and return false;
   bool update(const HashKey& k, HashData& d) {
      size_t num = bucketNum(k);
      typename vector<HashNode>::iterator it = _buckets[num].begin();
      while(it != _buckets[num].end())
      {
         if((*it).first == k)
         {
            (*it).second = d;
            return true;
         }
         it++;
      }
      _buckets[num].push_back(HashNode(k, d));
      return false;
   }

   // return true if inserted d successfully (i.e. k is not in the hash)
   // return false is k is already in the hash ==> will not insert
   bool insert(const HashKey& k, const HashData& d) {
      size_t num = bucketNum(k);
      typename vector<HashNode>::iterator it = _buckets[num].begin();
      while(it != _buckets[num].end())
      {
         if((*it).first == k)
            return false;
         it++;
      }
      _buckets[num].push_back(HashNode(k, d));
      return true;
   }

   // return true if removed successfully (i.e. k is in the hash)
   // return fasle otherwise (i.e. nothing is removed)
   bool remove(const HashKey& k) {
      size_t num = bucketNum(k);
      typename vector<HashNode>::iterator it = _buckets[num].begin();
      while(it != _buckets[num].end())
      {
         if((*it).first == k)
         {
            //_buckets[num].erase(it);
            *it = *(--_buckets[num].end());
            _buckets[num].pop_back();
            return true;
         }
         it++;
      }
      return false;
   }

private:
   // Do not add any extra data member
   size_t                   _numBuckets;
   vector<HashNode>*        _buckets;

   size_t bucketNum(const HashKey& k) const {
      return (k() % _numBuckets); }

};

// class HashKey
// {
// public:
//    HashKey() {}

//    size_t operator() () const { return 0; }

//    bool operator == (const HashKey& k) const { return true; }

// private:
// };
// template <class HashKey, class HashData>
// class HashMap
// {
// typedef pair<HashKey, HashData> HashNode;

// public:
//    HashMap(size_t b=0) : _numBuckets(0), _buckets(0) { if (b != 0) init(b); }
//    ~HashMap() { reset(); }

//    // [Optional] TODO: implement the HashMap<HashKey, HashData>::iterator
//    // o An iterator should be able to go through all the valid HashNodes
//    //   in the HashMap
//    // o Functions to be implemented:
//    //   - constructor(s), destructor
//    //   - operator '*': return the HashNode
//    //   - ++/--iterator, iterator++/--
//    //   - operators '=', '==', !="
//    //
//    class iterator
//    {
//       friend class HashMap<HashKey, HashData>;

//    public:

//    private:
//    };

//    void init(size_t b) {
//       reset(); _numBuckets = b; _buckets = new vector<HashNode>[b]; }
//    void reset() {
//       _numBuckets = 0;
//       if (_buckets) { delete [] _buckets; _buckets = 0; }
//    }
//    void clear() {
//       for (size_t i = 0; i < _numBuckets; ++i) _buckets[i].clear();
//    }
//    size_t numBuckets() const { return _numBuckets; }

//    vector<HashNode>& operator [] (size_t i) { return _buckets[i]; }
//    const vector<HashNode>& operator [](size_t i) const { return _buckets[i]; }

//    // TODO: implement these functions
//    //
//    // Point to the first valid data
//    iterator begin() const { return iterator(); }
//    // Pass the end
//    iterator end() const { return iterator(); }
//    // return true if no valid data
//    bool empty() const { return true; }
//    // number of valid data
//    size_t size() const { size_t s = 0; return s; }

//    // check if k is in the hash...
//    // if yes, return true;
//    // else return false;
//    bool check(const HashKey& k) const { return false; }

//    // query if k is in the hash...
//    // if yes, replace d with the data in the hash and return true;
//    // else return false;
//    bool query(const HashKey& k, HashData& d) const { return false; }

//    // update the entry in hash that is equal to k (i.e. == return true)
//    // if found, update that entry with d and return true;
//    // else insert d into hash as a new entry and return false;
//    bool update(const HashKey& k, HashData& d) { return false; }

//    // return true if inserted d successfully (i.e. k is not in the hash)
//    // return false is k is already in the hash ==> will not insert
//    bool insert(const HashKey& k, const HashData& d) { return true; }

//    // return true if removed successfully (i.e. k is in the hash)
//    // return fasle otherwise (i.e. nothing is removed)
//    bool remove(const HashKey& k) { return false; }

// private:
//    // Do not add any extra data member
//    size_t                   _numBuckets;
//    vector<HashNode>*        _buckets;

//    size_t bucketNum(const HashKey& k) const {
//       return (k() % _numBuckets); }

// };


//---------------------
// Define Cache classes
//---------------------
// To use Cache ADT, you should define your own HashKey class.
// It should at least overload the "()" and "==" operators.
//
// class CacheKey
// {
// public:
//    CacheKey() {}
//    
//    size_t operator() () const { return 0; }
//   
//    bool operator == (const CacheKey&) const { return true; }
//       
// private:
// }; 
// 
template <class CacheKey, class CacheData>
class Cache
{
typedef pair<CacheKey, CacheData> CacheNode;

public:
   Cache() : _size(0), _cache(0) {}
   Cache(size_t s) : _size(0), _cache(0) { init(s); }
   ~Cache() { reset(); }

   // NO NEED to implement Cache::iterator class

   // TODO: implement these functions
   //
   // Initialize _cache with size s
   void init(size_t s) { reset(); _size = s; _cache = new CacheNode[s]; }
   void reset() {  _size = 0; if (_cache) { delete [] _cache; _cache = 0; } }

   size_t size() const { return _size; }

   CacheNode& operator [] (size_t i) { return _cache[i]; }
   const CacheNode& operator [](size_t i) const { return _cache[i]; }

   // return false if cache miss
   bool read(const CacheKey& k, CacheData& d) const {
      size_t i = k() % _size;
      if (k == _cache[i].first) {
         d = _cache[i].second;
         return true;
      }
      return false;
   }
   // If k is already in the Cache, overwrite the CacheData
   void write(const CacheKey& k, const CacheData& d) {
      size_t i = k() % _size;
      _cache[i].first = k;
      _cache[i].second = d;
   }

private:
   // Do not add any extra data member
   size_t         _size;
   CacheNode*     _cache;
};


#endif // MY_HASH_H
