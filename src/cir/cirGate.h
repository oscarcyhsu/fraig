/****************************************************************************
  FileName     [ cirGate.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic gate data structures ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_GATE_H
#define CIR_GATE_H

#include <string>
#include <sstream>
#include <vector>
#include <cassert>
#include <iostream>
#include "cirDef.h"
#include <climits>
#include "sat.h"

using namespace std;

// TODO: Feel free to define your own classes, variables, or functions.

class CirGate;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
class CirGate
{
public:
   friend class PI_CirGate;
   friend class PO_CirGate;
   friend class AIG_CirGate;
   friend class CONST_CirGate;
   CirGate(){}
   virtual ~CirGate() {}
   // Basic access methods
   string getTypeStr() const {
      if(gateType == UNDEF_GATE)
         return "UNDEF";
      if(gateType == CONST_GATE)
         return "CONST";
      if(gateType == PI_GATE)
         return "PI";
      if(gateType == PO_GATE)
         return "PO";
      if(gateType == AIG_GATE)
         return "AIG";
      return "NAN";
   }
   unsigned getLineNo() const { return line; }
   virtual IdList& getFanout() { assert(gateType != PO_GATE); }
   virtual size_t getFanin(int which = 0) { assert(gateType != PI_GATE && gateType != CONST_GATE); }
   bool isTraversed() const { return (ref == CirGate::globalRef); }
   size_t getId() const { return Id; }
   virtual bool isAig() const { return false; }
   virtual bool haveFloatingFanin() const = 0;
   virtual bool isUnused() const = 0;
   virtual size_t getFanin() { return SIZE_MAX; }
   size_t getSimValue() { return simValue; }
   // Printing functions
   virtual void printGate(int& i) const = 0;
   virtual void reportGate() const = 0;
   void reportFanin(int level) const;
   void reportFanout(int level) const;
   virtual void writeToSs(int& count, stringstream& ss) const = 0;

   virtual void recursiveReportIn(int nowlevel, int level, bool invert) const = 0;
   virtual void recursiveReportOut(int nowlevel, int level, bool invert) const = 0;
   // setting functions
   void setGlobalRef() const { CirGate::globalRef++; }
   void setSimValue(size_t num) { simValue = num; }
   virtual void dfs(IdList& v) { return; }
   virtual void recursiveSetRef() const { ref = CirGate::globalRef;}
   virtual void recursiveOptimize(IdList& delList) { return; }
   virtual void recursiveStrash(HashMap<HashKey, size_t>& map, IdList& delList) { return; }
   virtual bool recursiveSim() { return true; }
   void setLine(int l) { line = l;}
   virtual void addIn(size_t gate) = 0;
   virtual void addOut(size_t gate) = 0;
   virtual void sortFanout() = 0;
   virtual void setFanin(size_t gate, int which = 0) { return; }
   virtual void setGrp(IdList* p) { return; }

protected:
   static unsigned globalRef;
   mutable unsigned ref = 0;
   size_t simValue = 0;
   size_t Id;
   int line;
   int gateType;
};

class CONST_CirGate : public CirGate
{
public:
   friend class PI_CirGate;
   friend class PO_CirGate;
   friend class AIG_CirGate;
   CONST_CirGate() { Id = 0; gateType = CONST_GATE; line = 0; simValue = 0; }
   ~CONST_CirGate() {}
   // Basic access methods
   IdList& getFanout() { return fanout; }
   bool haveFloatingFanin() const { return false; }
   bool isUnused()          const { return false; }
   // Printing functions
   void reportGate() const;
   void printGate(int& i) const {
      if(ref == CirGate::globalRef)
         return;
      ref = CirGate::globalRef;
      cout << "[" << i << "] " << "CONST0" << endl;
      i++;
      return;
   }
   void recursiveReportIn(int nowlevel, int level, bool invert) const;
   void recursiveReportOut(int nowlevel, int level, bool invert) const;
   void writeToSs(int& count, stringstream& ss) const;
   // setting function
   void addIn(size_t gate) { return; }
   void addOut(size_t gate) { fanout.push_back(gate); }
   void setFanin(size_t gate, int which) { return; }
   void setGrp(IdList* p) { grp = p; }
   void sortFanout() {
      for(int i = fanout.size() - 1; i > 0; i--)
      {
        for(int j = 0; j < i; j++)
        {
           if(fanout[j] > fanout[j+1])
           {
             size_t temp = fanout[j];
              fanout[j] = fanout[j+1];
              fanout[j+1] = temp;
           }
        }
     }
   }
private:
   IdList fanout;
   IdList* grp = 0;
};

class PI_CirGate : public CirGate
{
public:
   friend class CONST_CirGate;
   friend class PO_CirGate;
   friend class AIG_CirGate;
   PI_CirGate(int id, int l = -1) { Id = id; gateType= PI_GATE; line = l; }
   ~PI_CirGate() {}
   // Basic access methods
   bool haveFloatingFanin() const { return false; }
   bool isUnused() const { return fanout.empty(); }
   string getSymbol() { return symbol; }
   IdList& getFanout() { return fanout; }
   // printing function
   void reportGate() const;
   void printGate(int& i) const {
      if(ref == CirGate::globalRef)
         return;
      ref = CirGate::globalRef;
      cout << "[" << i << "] " << "PI  " << Id;
      if(symbol != "")
         cout << " (" << symbol << ")";
      cout << endl;
      i++;
      return;
   }
   void recursiveReportIn(int nowlevel, int level, bool invert) const;
   void recursiveReportOut(int nowlevel, int level, bool invert) const;
   void writeToSs(int& count, stringstream& ss) const;
   // setting function
   void setSymbol(string s) { symbol = s; }
   void addIn(size_t gate) { return; }
   void addOut(size_t gate) { fanout.push_back(gate); }
   void sortFanout() {
      for(int i = fanout.size() - 1; i > 0; i--)
      {
        for(int j = 0; j < i; j++)
        {
           if(fanout[j] > fanout[j+1])
           {
             size_t temp = fanout[j];
              fanout[j] = fanout[j+1];
              fanout[j+1] = temp;
           }
        }
     }
   }
private:
   string symbol;
   IdList fanout;
};

class PO_CirGate : public CirGate
{
public:
   friend class CONST_CirGate;
   friend class PI_CirGate;
   friend class AIG_CirGate;
   PO_CirGate(int id, int l = -1) {Id = id; gateType = PO_GATE; line = l; }
   ~PO_CirGate() {}
   // Basic access methods
   bool haveFloatingFanin() const;
   bool isUnused() const { return false; }
   size_t getFanin(int which = 0) { return fanin; }
   string getSymbol() { return symbol; }
   // printing function
   void reportGate() const;
   void printGate(int& i) const;
   void recursiveReportIn(int nowlevel, int level, bool invert) const;
   void recursiveReportOut(int nowlevel, int level, bool invert) const;
   void writeToSs(int& count, stringstream& ss) const;
   // setting function
   void dfs(IdList& v);
   void recursiveOptimize(IdList& delList);
   void recursiveSetRef() const;
   void recursiveStrash(HashMap<HashKey, size_t>& map, IdList& delList);
   bool recursiveSim();
   void setSymbol(string s) { symbol = s; }
   void addIn(size_t gate) { fanin = gate; }
   void addOut(size_t gate) { return; }
   void setFanin(size_t gate, int which = 0) { fanin = gate; }
   void sortFanout() { return; }
private:
   string symbol;
   size_t fanin = SIZE_MAX;
};

class AIG_CirGate : public CirGate
{
public:
   friend class CONST_CirGate;
   friend class PI_CirGate;
   friend class PO_CirGate;
   AIG_CirGate(int id, int l) { Id = id; gateType = AIG_GATE; line = l; simValue = 0;}
   AIG_CirGate(int id) { Id = id; gateType = UNDEF_GATE; line = 0; simValue = 0;}
   ~AIG_CirGate() {}
   // basis access functions
   bool haveFloatingFanin() const;
   bool isUnused() const { return fanout.empty(); }
   IdList& getFanout() { return fanout; }
   size_t getFanin(int which = 0) {
      if(which == 1)
         return fanin1;
      else if(which == 2)
         return fanin2;
   }
   // printing functions
   void reportGate() const;
   void printGate(int& i) const;
   void recursiveReportIn(int nowlevel, int level, bool invert) const;
   void recursiveReportOut(int nowlevel, int level, bool invert) const;
   void writeToSs(int& count, stringstream& ss) const;
   // setting functions
   void dfs(IdList& v);
   void setGrp(IdList* p) { grp = p; }
   void recursiveOptimize(IdList& delList);
   void recursiveSetRef() const;
   void recursiveStrash(HashMap<HashKey, size_t>& map, IdList& delList);
   bool recursiveSim();
   void merge(size_t mergeGate);
   void addIn(size_t gate) {  //after setting two fanin AIG is defined
      if(fanin1 != SIZE_MAX and fanin2 != SIZE_MAX)
         return;
      if(fanin1 == SIZE_MAX)
         fanin1 = gate;
      else if(fanin1 != SIZE_MAX)
      {
         fanin2 = gate;
         gateType = AIG_GATE;
      }
   }
   void addOut(size_t gate) { fanout.push_back(gate);}
   void sortFanout() {
      for(int i = fanout.size() - 1; i > 0; i--)
      {
        for(int j = 0; j < i; j++)
        {
           if(fanout[j] > fanout[j+1])
           {
             size_t temp = fanout[j];
              fanout[j] = fanout[j+1];
              fanout[j+1] = temp;
           }
        }
     }
   }
   void setFanin(size_t gate, int which = 0) {
      if(which == 1)
         fanin1 = gate;
      else if(which == 2)
         fanin2 = gate;
   }
private:
   size_t fanin1 = SIZE_MAX;
   size_t fanin2 = SIZE_MAX;
   IdList fanout;
   IdList* grp = 0;
};
#endif // CIR_GATE_H
