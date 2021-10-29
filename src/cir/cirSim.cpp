/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir simulation functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <fstream>
#include <bitset>
#include <iostream>
#include <cstring>
#include <string>
#include <iomanip>
#include <algorithm>
#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Keep "CirMgr::randimSim()" and "CirMgr::fileSim()" for cir cmd.
//       Feel free to define your own variables or functions
class simV
{
public:
simV(size_t num) { simValue = num; }
bool operator == (simV s) { return s.simValue == this->simValue || ~(s.simValue) == this->simValue; }
size_t operator () () const { return ~simValue < simValue? simValue: ~simValue; }

size_t simValue;
};
/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/************************************************/
/*   Public member functions about Simulation   */
/************************************************/
void
CirMgr::randomSim()
{
   if(firstSim)
   {
      firstSim = false;
      IdList* firstGrp = new IdList;
      _GateList[0]->setGlobalRef();
      firstGrp->push_back(0);
      for(int i = 0; i < O; i++)
      {
         _GateList[_IdList[I + 1 + i]]->dfs(*firstGrp);
      }
      grpList.push_back(firstGrp);
   }
   int count = 0;
   for(int time = 0; time < M/64 || time < 100; time++)
   {
      size_t* sim = new size_t[I];
      for(size_t i = 0 ; i < I; i++)
      {
         size_t s1 = rnGen(INT_MAX);
         size_t s2 = rnGen(INT_MAX);
         size_t ss = (s1 << 32)+ s2;
         sim[i] = ss;
      }
      for(int i = 0; i < I; i++)
      {
         _GateList[_IdList[1+i]]->setSimValue(sim[i]);
      }

      _GateList[0]->setGlobalRef();
      for(int i = 0; i < O; i++)
         _GateList[_IdList[1 + I + i]]->recursiveSim();
      _GateList[0]->setGlobalRef();
      
      count += 64;
      int size = grpList.size();
      
      for(int i = size - 1; i >= 0; i--)
      {
         
         HashMap<simV, IdList*> newGrpList(M / 1000 + 20);
         for(IdList::iterator it = grpList[i]->begin(); it != grpList[i]->end(); it++)
         {
            CirGate* gate = cirMgr->_GateList[*it];
            simV simv = simV(gate->getSimValue());
            IdList* grp;
            if(newGrpList.query(simv, grp))
               grp->push_back(*it);
            else
            {
               IdList* newGrp = new IdList;
               newGrp->push_back(*it);
               newGrpList.insert(simv, newGrp);
            }
         }
         for(HashMap<simV, IdList*>::iterator iter = newGrpList.begin(); iter != newGrpList.end(); iter++)
         {
            if((*iter).second->size() > 1)
            {
               grpList.push_back((*iter).second);
            }
            else
            {
               //cout << ((*iter).second) << endl;
               if(((*iter).second) != 0)
               {
                  delete ((*iter).second);
               }
            }
         }
         delete grpList[i];
         grpList[i] = *(--grpList.end());
         grpList.pop_back();
      }

      string s = "Total #FEC Group = " + to_string(grpList.size());
      cout << s << endl;
   }

   cout << count << " patterns simulated." << endl;
   
}

void
CirMgr::fileSim(ifstream& patternFile)
{

   if(!patternFile)
      return;
   // access from gate?
   // sim again?
   if(firstSim)
   {
      firstSim = false;
      IdList* firstGrp = new IdList;
      _GateList[0]->setGlobalRef();
      firstGrp->push_back(0);
      for(int i = 0; i < O; i++)
      {
         _GateList[_IdList[I + 1 + i]]->dfs(*firstGrp);
      }
      // firstGrp->push_back(0);

      // IdList::iterator ii = _IdList.begin(); 
      // for(int i = 0; i < 1 + I + O; i++)
      //    ii++;
      // for(;ii != _IdList.end(); ii++)
      // {
      //    firstGrp->push_back(*ii);
      // }
      grpList.push_back(firstGrp);
   }
   
   size_t* sim = new size_t[I];
   for(size_t i = 0 ; i < I; i++)
      sim[i] = 0;
   char* b = new char[10000];
   bool done = false;
   int count = 0;
   cout << endl;
   while(!done)
   {
      for(size_t i = 0 ; i < I; i++)
         sim[i] = 0;
      // a loop is a 64bits sim
      bool empty = false;
      int digit = 0; // length of patter in this loop, thus 0 ~ 64
      for(; digit < 64; digit++)
      {
         if(!(patternFile >> b))
         {
            if(digit == 0) // meet end of file when reading first digit
               empty = true;
            done = true;
            break;
         }
         if(strlen(b) > I)
         {
            cerr << "Error: Pattern(" << b << ") length(" << strlen(b) 
            << ") does not match the number of inputs(" << I << ") in a circuit!!" << endl;
            empty = true;
            done = true;
            break;
         }
         for(int i = 0; i < I; i++)
         {
            if(b[i] != 48 && b[i] != 49)
            {
               cerr << "Error: Pattern(" << b << ") contains a non-0/1 character(‘" << b[i] << "’)." << endl;
               empty = true;
               done = true;
               break;
            }
            sim[i] |= ((size_t)(b[i] - 48) << digit);
            //cout << sim[i] << endl;
         }
      }

      // if simValue is not empty or with wrong format, set seimValue and recursiveSim
      if(!empty)      {

         for(int i = 0; i < I; i++)
         {
            _GateList[_IdList[1+i]]->setSimValue(sim[i]);
         }

         _GateList[0]->setGlobalRef();
         for(int i = 0; i < O; i++)
            _GateList[_IdList[1 + I + i]]->recursiveSim();
         
         
         count += digit;
         int size = grpList.size();

         for(int i = size - 1; i >= 0; i--)
         {

            HashMap<simV, IdList*> newGrpList(M / 1000 + 20);
            for(IdList::iterator it = grpList[i]->begin(); it != grpList[i]->end(); it++)
            {
               CirGate* gate = cirMgr->_GateList[*it];
               simV simv = simV(gate->getSimValue());
               IdList* grp;
               if(newGrpList.query(simv, grp))
                  grp->push_back(*it);
               else
               {
                  IdList* newGrp = new IdList;
                  newGrp->push_back(*it);
                  newGrpList.insert(simv, newGrp);
               }
            }
            for(HashMap<simV, IdList*>::iterator iter = newGrpList.begin(); iter != newGrpList.end(); iter++)
            {
               
               if((*iter).second->size() > 1)
               {
                  grpList.push_back((*iter).second);
               }
               else
               {
                  //cout << ((*iter).second) << endl;
                  if(((*iter).second) != 0)
                  {
                     delete ((*iter).second);
                  }
               }
            }
            delete grpList[i];
            grpList[i] = *(--grpList.end());
            grpList.pop_back();
         }
         string s = "Total #FEC Group = " + to_string(grpList.size());
         cout << s << endl;
      }
   }
   // sort FEC group
   int size = grpList.size();
   for(int i = size - 1; i >= 0; i--)
   {
      if(grpList[i] != 0)
      {
         sort(grpList[i]->begin(), grpList[i]->end());
      }
   }
   // clear grp*
   _GateList[0]->setGrp(0);
   IdList::iterator jt = _IdList.begin();
   for(int i = 0; i < 1 + I + O; i++)
      jt++;
   for(; jt != _IdList.end(); jt++)
      _GateList[*jt]->setGrp(0);
   
   // set grp*
   for(int i = size - 1; i >= 0; i--)
   {
      if(grpList[i] != 0)
      {
         for(IdList::iterator it = grpList[i]->begin(); it != grpList[i]->end(); it++)
         {
            _GateList[*it]->setGrp(grpList[i]);
            // cout << cirMgr->_GateList[*it]->getSimValue() << " " << cirMgr->_GateList[*(grpList[i]->begin())]->getSimValue() << endl;
            // if(cirMgr->_GateList[*it]->getSimValue() != cirMgr->_GateList[*(grpList[i]->begin())]->getSimValue())
            //    cout << "!";
            // cout << *it << " ";
         }
         //cout << endl;
      }
   }
   // string s = "Total #FEC Group = " + to_string(grpList.size()) + '\n';
   // cout << endl;
   // cout << s;
   cout << count << " patterns simulated." << endl;
   delete[] b;
   delete[] sim;
}

/*************************************************/
/*   Private member functions about Simulation   */
/*************************************************/
