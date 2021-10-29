/****************************************************************************
  FileName     [ cirMgr.h ]
  PackageName  [ cir ]
  Synopsis     [ Define circuit manager ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_MGR_H
#define CIR_MGR_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include "../util/myHashSet.h"
using namespace std;

// TODO: Feel free to define your own classes, variables, or functions.

#include "cirDef.h"
#include "cirGate.h"
extern CirMgr *cirMgr;

class CirMgr
{
public:
   friend class CirGate;
   friend class CONST_CirGate;
   friend class PI_CirGate;
   friend class PO_CirGate;
   friend class AIG_CirGate;
   CirMgr() {}
   ~CirMgr()
   {
      for (size_t i = 0; i < _GateList.size(); i++)
      {
         if(_GateList[i])
            delete _GateList[i];
      }
   } 

   // Access functions
   // return '0' if "gid" corresponds to an undefined gate.
   CirGate* getGate(unsigned gid) const {
      if(gid < _GateList.size() and _GateList[gid] != 0)
         return _GateList[gid];
      return 0;
   }

   // Member functions about circuit construction
   bool readCircuit(const string&);

   // Member functions about circuit optimization
   void sweep();
   void optimize();

   // Member functions about simulation
   void randomSim();
   void fileSim(ifstream&);
   void setSimLog(ofstream *logFile) { _simLog = logFile; }

   // Member functions about fraig
   void strash();
   void printFEC() const;
   void fraig();

   // Member functions about circuit reporting
   void printSummary() const;
   void printNetlist() const;
   void printPIs() const;
   void printPOs() const;
   void printFloatGates() const;
   void printFECPairs() const;
   void writeAag(ostream&) const;
   void writeGate(ostream&, CirGate*) const;

private:
   ofstream           *_simLog;
   unsigned M = 0;
   unsigned I = 0;
   unsigned L = 0;
   unsigned O = 0;
   unsigned A = 0;
   GateList _GateList;
   IdList _IdList;
   IdList _WithFloating;
   IdList _UnUsed;
   vector<IdList*> grpList;

   bool firstSim = true;
};

#endif // CIR_MGR_H
