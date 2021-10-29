/****************************************************************************
  FileName     [ cirFraig.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir FRAIG functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2012-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include <algorithm>
#include "cirMgr.h"
#include "cirGate.h"
#include "sat.h"
#include "myHashMap.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::strash()" and "CirMgr::fraig()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/*******************************************/
/*   Public member functions about fraig   */
/*******************************************/
// _floatList may be changed.
// _unusedList and _undefList won't be changed
void
CirMgr::strash()
{
   HashMap<HashKey, size_t> map(A);
   IdList delList;
   _GateList[0]->setGlobalRef();
   for(int i = 0; i < O; i++)
   {
      _GateList[_IdList[I + 1 + i]]->recursiveStrash(map, delList);
   }
   for(IdList::iterator it = delList.begin(); it != delList.end(); it++)
      //cout << *it << " ";
      if(_GateList[*it] != 0)
      {
         A--;
         delete _GateList[*it];
         _GateList[*it] = 0;
         _IdList.erase(std::remove(_IdList.begin(), _IdList.end(), *it), _IdList.end());
      }
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

void
CirMgr::fraig()
{
}

/********************************************/
/*   Private member functions about fraig   */
/********************************************/
