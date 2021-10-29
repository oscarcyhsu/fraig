/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir optimization functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include <algorithm>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::sweep()" and "CirMgr::optimize()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/**************************************************/
/*   Public member functions about optimization   */
/**************************************************/
// Remove unused gates
// DFS list should NOT be changed
// UNDEF, float and unused list may be changed
void
CirMgr::sweep()
{
   _GateList[0]->setGlobalRef(); //static 
   for(int i = 0; i < O; i++)
   {
      _GateList[_IdList[I + 1 + i]]->recursiveSetRef();
   }
   int count = 0;
   // for(GateList::iterator it = _GateList.begin(); it != _GateList.end(); it++)
   // {
   //    if(*it != 0 && !(*it)->isTraversed() && ((*it)->getTypeStr() == "AIG" || (*it)->getTypeStr() == "UNDEF")) //only delete in _Gatelist
   //    {
   //       cout << "Sweeping: " << (*it)->getTypeStr() << "(" << (*it)->getId() << ") removed..." << endl;
   //       if((*it)->getTypeStr() == "AIG")
   //       {
   //          A--;
   //          CirGate* in1 = _GateList[((*it)->getFanin(1)) / 2];
   //          CirGate* in2 = _GateList[((*it)->getFanin(2)) / 2];
   //          if(in1)
   //          {
   //             in1->getFanout().erase(std::remove(in1->getFanout().begin(), in1->getFanout().end(), (*it)->getId()*2), in1->getFanout().end());
   //             in1->getFanout().erase(std::remove(in1->getFanout().begin(), in1->getFanout().end(), (*it)->getId()*2+1), in1->getFanout().end());
   //          }
   //          if(in2)
   //          {
   //             in2->getFanout().erase(std::remove(in2->getFanout().begin(), in2->getFanout().end(), (*it)->getId()*2), in2->getFanout().end());
   //             in2->getFanout().erase(std::remove(in2->getFanout().begin(), in2->getFanout().end(), (*it)->getId()*2+1), in2->getFanout().end());
   //          }
   //       }
   //       _IdList.erase(std::remove(_IdList.begin(), _IdList.end(), (*it)->getId()), _IdList.end());
   //       delete *it;
   //       *it = 0;
   //    }
   // }
   
   // some gate will become unused
   // => check isUnused()
   _WithFloating.clear();
   _UnUsed.clear();
   for(IdList::iterator it = _IdList.begin(); it != _IdList.end(); it++)
   {
      if(_GateList[*it]->haveFloatingFanin())
      {
         _WithFloating.push_back(*it);
         //cout << *it << endl;
      }
      if(_GateList[*it]->isUnused())
      {
         _UnUsed.push_back(*it);
         //cout << *it << endl;
      }
   }
}

// Recursively simplifying from POs;
// _dfsList needs to be reconstructed afterwards
// UNDEF gates may be delete if its fanout becomes empty...
void
CirMgr::optimize()
{
   IdList delList;
   _GateList[0]->setGlobalRef();
   for(int i = 0; i < O; i++)
   {
      _GateList[_IdList[I + 1 + i]]->recursiveOptimize(delList);
   }
   // delete gate in delList
   for(IdList::iterator it = delList.begin(); it != delList.end(); it++)
      //cout << *it << " ";
      if(_GateList[*it] != 0)
      {
         A--;
         delete _GateList[*it];
         _GateList[*it] = 0;
         _IdList.erase(std::remove(_IdList.begin(), _IdList.end(), *it), _IdList.end());
      }
   // some gate will become unused
   // => check isUnused()
   _WithFloating.clear();
   _UnUsed.clear();
   for(IdList::iterator it = _IdList.begin(); it != _IdList.end(); it++)
   {
      if(_GateList[*it]->haveFloatingFanin())
      {
         _WithFloating.push_back(*it);
         //cout << *it << endl;
      }
      if(_GateList[*it]->isUnused())
      {
         _UnUsed.push_back(*it);
         //cout << *it << endl;
      }
   }
}

/***************************************************/
/*   Private member functions about optimization   */
/***************************************************/
