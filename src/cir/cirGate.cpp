/****************************************************************************
  FileName     [ cirGate.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define class CirAigGate member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdarg.h>
#include <cassert>
#include <algorithm>
#include <bitset>
#include "cirGate.h"
#include "cirMgr.h"
#include "util.h"

using namespace std;

// TODO: Keep "CirGate::reportGate()", "CirGate::reportFanin()" and
//       "CirGate::reportFanout()" for cir cmds. Feel free to define
//       your own variables and functions.

extern string toBinary(size_t n)
{
         string r;
         string z;
         while(n!=0) 
         {
                 r=(n%2==0 ? "0":"1")+r; 
                 n/=2;
         }
         z.resize(64 - r.size(),'0');
         r = z+r;
         return r;
}
extern CirMgr *cirMgr;
unsigned CirGate::globalRef = 0;
/**************************************/
/*   class CirGate member functions   */
/**************************************/


void
CirGate::reportFanin(int level) const
{
   assert (level >= 0);
   setGlobalRef();
   recursiveReportIn(level, level, false);
}

void
CirGate::reportFanout(int level) const
{
   assert (level >= 0);
   setGlobalRef();
   recursiveReportOut(level, level, false);
}

// for CONST_CirGate
void
CONST_CirGate::reportGate() const
{
   cout << "================================================================================" << endl;
   cout << "= " << getTypeStr() << "(" << Id << ")";
   cout << ", " << "line " << line << endl;
   cout << "= FECs:";
   if(grp != 0)
   {
      for(IdList::iterator it = grp->begin(); it != grp->end(); it++)
      {
         if(*it != Id)
         {
            cout << " ";
            if(cirMgr->_GateList[*it]->getSimValue() != simValue)
               cout << "!";
            cout << *it;
         }
      }
   }
   cout << endl;
   cout << "= Value: ";
   for(int i = 7 ; i >= 0; i--)
   {
      if(i != 7)
         cout << "_";
      cout << bitset<8> ((simValue >> (8 * i)));
   }
   cout << endl;
   cout << "================================================================================" << endl;
}

void CONST_CirGate::recursiveReportIn(int nowlevel, int level, bool invert) const {
   ref = CirGate::globalRef;
   for(int i = 0; i < 2*(level - nowlevel); i++) //identation
      cout << ' ';
   if(invert)
      cout << '!';
   cout << getTypeStr() << " " << Id << endl;
   if(level == 0)
      return;
}


void CONST_CirGate::recursiveReportOut(int nowlevel, int level, bool invert) const {
   for(int i = 0; i < 2*(level - nowlevel); i++) //identation
      cout << ' ';
   if(invert)
      cout << '!';
   cout << getTypeStr() << " " << Id;
   if(nowlevel <= 0 or fanout.empty()) //last
   {
      cout << endl;
      return;
   }
   else
   {
      if(ref == CirGate::globalRef) //fanins have been reported
      {
         cout << " (*)";
         cout << endl;
         return;
      }
      cout << endl;
      for(int i = 0; i < fanout.size(); i++)
      {
         cirMgr->_GateList[fanout[i]/2]->recursiveReportOut(nowlevel - 1, level, fanout[i] % 2);
      }
      ref = CirGate::globalRef;
   }
   return;
}

void CONST_CirGate::writeToSs(int& count, stringstream& ss) const {
   return;
}

// for PI_CirGate

void
PI_CirGate::reportGate() const
{
   cout << "================================================================================" << endl;
   cout << "= " << getTypeStr() << "(" << Id << ")";
   if(symbol != "")
   {
      cout << "\"" << symbol << "\"";
   }
   cout << ", " << "line " << line << endl;
   cout << "= FECs:" << endl;
   cout << "= Value: ";
   for(int i = 7 ; i >= 0; i--)
   {
      if(i != 7)
         cout << "_";
      cout << bitset<8> ((simValue >> (8 * i)));
   }
   cout << endl;
   cout << "================================================================================" << endl;
}

void PI_CirGate::recursiveReportIn(int nowlevel, int level, bool invert) const {
   ref = CirGate::globalRef;
   for(int i = 0; i < 2*(level - nowlevel); i++) //identation
      cout << ' ';
   if(invert)
      cout << '!';
   cout << getTypeStr() << " " << Id << endl;
   if(level == 0)
      return;
}

void PI_CirGate::recursiveReportOut(int nowlevel, int level, bool invert) const {
   for(int i = 0; i < 2*(level - nowlevel); i++) //identation
      cout << ' ';
   if(invert)
      cout << '!';
   cout << getTypeStr() << " " << Id;
   if(nowlevel <= 0 or fanout.empty()) //last
   {
      cout << endl;
      return;
   }
   else
   {
      if(ref == CirGate::globalRef) //fanins have been reported
      {
         cout << " (*)";
         cout << endl;
         return;
      }
      cout << endl;
      for(int i = 0; i < fanout.size(); i++)
      {
         cirMgr->_GateList[fanout[i]/2]->recursiveReportOut(nowlevel - 1, level, fanout[i] % 2);
      }
      ref = CirGate::globalRef;
   }
   return;
}

void PI_CirGate::writeToSs(int& count, stringstream& ss) const {
   return; 
}
// for PO_gate
void
PO_CirGate::reportGate() const
{
   cout << "================================================================================" << endl;
   cout << "= " << getTypeStr() << "(" << Id << ")";
   if(symbol != "")
   {
      cout << "\"" << symbol << "\"";
   }
   cout << ", " << "line " << line << endl;
   cout << "= FECs:" << endl;
   cout << "= Value: ";
   for(int i = 7 ; i >= 0; i--)
   {
      if(i != 7)
         cout << "_";
      cout << bitset<8> ((simValue >> (8 * i)));
   }
   cout << endl;
   cout << "================================================================================" << endl;
}

bool PO_CirGate::haveFloatingFanin() const
{
  return cirMgr->_GateList[fanin/2]->getTypeStr() == "UNDEF";
}

void PO_CirGate::printGate(int& i) const
{
      if(ref == CirGate::globalRef)
         return;
      ref = CirGate::globalRef;
      cirMgr->_GateList[fanin/2]->printGate(i);
      cout << "[" << i << "] " << "PO  " << Id << " ";
      if(fanin % 2 != 0)
         cout << '!';
      cout << fanin / 2;
      if(symbol != "")
         cout << " (" << symbol << ")";
      cout << endl;
      i++;
      return;
}

void PO_CirGate::recursiveReportIn(int nowlevel, int level, bool invert) const {
   ref = CirGate::globalRef;
   for(int i = 0; i < 2*(level - nowlevel); i++) //identation
      cout << ' ';
   if(invert)
      cout << '!';
   cout << getTypeStr() << " " << Id << endl;
   if(level == 0)
      return;
   cirMgr->_GateList[fanin/2]->recursiveReportIn(nowlevel - 1, level, fanin % 2);
}

void PO_CirGate::recursiveReportOut(int nowlevel, int level, bool invert) const {
   for(int i = 0; i < 2*(level - nowlevel); i++) //identation
      cout << ' ';
   if(invert)
      cout << '!';
   cout << getTypeStr() << " " << Id;
   cout << endl;
   return;
}

void PO_CirGate::writeToSs(int& count, stringstream& ss) const {
   if(ref == CirGate::globalRef)
      return;
   if(fanin != SIZE_MAX)
   {
      cirMgr->_GateList[fanin / 2]->writeToSs(count, ss);
   }
}

void PO_CirGate::recursiveSetRef() const
{
   if(ref == CirGate::globalRef)
         return;
   ref = CirGate::globalRef;
   if(fanin != SIZE_MAX)
      cirMgr->_GateList[fanin / 2]->recursiveSetRef();
}

void PO_CirGate::recursiveOptimize(IdList& delList)
{
   if(fanin != SIZE_MAX)
      cirMgr->_GateList[fanin/2]->recursiveOptimize(delList);
}
void PO_CirGate::recursiveStrash(HashMap<HashKey, size_t>& map, IdList& delList){
   cirMgr->_GateList[fanin/2]->recursiveStrash(map, delList);
}

bool PO_CirGate::recursiveSim()
{
   if(ref == CirGate::globalRef)
      return true;
   ref = CirGate::globalRef;

   cirMgr->_GateList[fanin/2]->recursiveSim();
   //cout << Id << " " << cirMgr->_GateList[fanin/2]->simVcalue << endl;
   simValue = cirMgr->_GateList[fanin/2]->simValue;
   if(fanin % 2 != 0)
      simValue = ~simValue;
   return true;
}
void PO_CirGate::dfs(IdList& v)
{
   cirMgr->_GateList[fanin / 2]->dfs(v);
}
// for AIG_CirGate
void
AIG_CirGate::reportGate() const
{
   //int width = 39 - getTypeStr().size() - to_string(Id).size() - to_string(line).size();
   cout << "================================================================================" << endl;
   cout << "= " << getTypeStr() << "(" << Id << ")";
   cout << ", " << "line " << line << endl;
   cout << "= FECs:";
   if(grp != 0)
   {
      for(IdList::iterator it = grp->begin(); it != grp->end(); it++)
      {
         if(*it != Id)
         {
            cout << " ";
            if(cirMgr->_GateList[*it]->getSimValue() != simValue)
               cout << "!";
            cout << *it;
         }
      }
   }
   cout << endl;
   cout << "= Value: ";
   for(int i = 7 ; i >= 0; i--)
   {
      if(i != 7)
         cout << "_";
      cout << bitset<8> ((simValue >> (8 * i)));
   }
   cout << endl;
   cout << "================================================================================" << endl;
}

bool AIG_CirGate::haveFloatingFanin() const 
{
   return (cirMgr->_GateList[fanin1/2]->getTypeStr() == "UNDEF" or cirMgr->_GateList[fanin2/2]->getTypeStr() == "UNDEF");
}

void AIG_CirGate::printGate(int& i) const
{
   if(ref == CirGate::globalRef)
      return;
   ref = CirGate::globalRef;

   if(fanin1 != SIZE_MAX and fanin2 != SIZE_MAX)
   {
      cirMgr->_GateList[fanin1/2]->printGate(i);
      cirMgr->_GateList[fanin2/2]->printGate(i);
   }
   else
   {
      return;
   }  
   cout << "[" << i << "] " << "AIG " << Id << " ";
   if(cirMgr->_GateList[fanin1/2]->getTypeStr() == "UNDEF")
      cout << '*';
   if(fanin1 % 2 != 0)
      cout << '!';
   cout << fanin1 / 2 << " ";
   if(cirMgr->_GateList[fanin2/2]->getTypeStr() == "UNDEF")
      cout << '*';
   if(fanin2 % 2 != 0)
      cout << '!';
   cout << fanin2 / 2 << endl;
   i++;
   return;
}

void AIG_CirGate::recursiveReportIn(int nowlevel, int level, bool invert) const{
   for(int i = 0; i < 2*(level - nowlevel); i++) //identation
      cout << ' ';
   if(invert)
      cout << '!';
   cout << this->getTypeStr() << " " << Id;
   if(nowlevel <= 0 or gateType == UNDEF_GATE) //last
   {
      cout << endl;
      return;
   }
   else
   {
      if(ref == CirGate::globalRef) //fanins have been reported
      {
         cout << " (*)";
         cout << endl;
         return;
      }
      cout << endl;
      cirMgr->_GateList[fanin1/2]->recursiveReportIn(nowlevel - 1, level, fanin1 % 2);
      cirMgr->_GateList[fanin2/2]->recursiveReportIn(nowlevel - 1, level, fanin2 % 2);
      ref = CirGate::globalRef;
   }
   return;
}

void AIG_CirGate::recursiveReportOut(int nowlevel, int level, bool invert) const{
   for(int i = 0; i < 2*(level - nowlevel); i++) //identation
      cout << ' ';
   if(invert)
      cout << '!';
   cout << getTypeStr() << " " << Id;
   if(nowlevel <= 0 or fanout.empty()) //last
   {
      cout << endl;
      return;
   }
   else
   {
      if(ref == CirGate::globalRef) //fanins have been reported
      {
         cout << " (*)";
         cout << endl;
         return;
      }
      cout << endl;
      for(int i = 0; i < fanout.size(); i++)
      {
         //cout << fanout[i];
         cirMgr->_GateList[fanout[i]/2]->recursiveReportOut(nowlevel - 1, level, fanout[i]%2);
      }
      ref = CirGate::globalRef;
   }
   return;
}

void AIG_CirGate::writeToSs(int& count, stringstream& ss) const {
   if(ref == CirGate::globalRef)
      return;
   if(fanin1 != SIZE_MAX and fanin2 != SIZE_MAX)
   {
      cirMgr->_GateList[fanin1/2]->writeToSs(count, ss);
      cirMgr->_GateList[fanin2/2]->writeToSs(count, ss);
      ss << 2*Id << " " << fanin1 << " "
         << fanin2 << endl;
      count++;
      ref = CirGate::globalRef;
   }
}

void AIG_CirGate::recursiveSetRef() const
{
   if(ref == CirGate::globalRef)
         return;
   ref = CirGate::globalRef;
   //cout << Id << endl;
   if(!gateType == UNDEF_GATE)
   {
      cirMgr->_GateList[fanin1 / 2]->recursiveSetRef();
      cirMgr->_GateList[fanin2 / 2]->recursiveSetRef();
   }
}

void AIG_CirGate::recursiveOptimize(IdList& delList)
{
   if(ref == CirGate::globalRef)
      return;
   ref = CirGate::globalRef;

   if(gateType != UNDEF_GATE)
   {
      cirMgr->_GateList[fanin1 / 2]->recursiveOptimize(delList);
      cirMgr->_GateList[fanin2 / 2]->recursiveOptimize(delList);
   }
   else
      return;
   if(fanin1 == 0 || fanin2 == 0)
   {
      delList.push_back(Id);
      cout << "Simplifying: " << "0" << " merging " << Id << "..." << endl;
      // below is to modify fanout's fanin
      for(IdList::iterator it = fanout.begin(); it != fanout.end(); it++)
      {
         CirGate* out = cirMgr->_GateList[*it / 2];
         if(out->getTypeStr() == "PO")  // fanout is PO_GATE (not include UNDEF_GATE)
            out->setFanin(0 + (*it) % 2); // fanout's fanin = 0 + (invert)
         else if(out->getTypeStr() == "AIG") //fanout in AIG_GATE
         {
            if(((AIG_CirGate*) out)->fanin1 / 2 == Id) // AIG's fanin1 is this gate
               out->setFanin(0 + (*it) % 2, 1);
            if(((AIG_CirGate*) out)->fanin2 / 2 == Id) // // AIG's fanin2 is this gate
               out->setFanin(0 + (*it) % 2, 2);
         }
      }
      // below is to modify fanin's fanout  
      // note: fanin may be PI_GATE or AIG_GATE
      CirGate* in1 = cirMgr->_GateList[fanin1 / 2];
      CirGate* in2 = cirMgr->_GateList[fanin2 / 2];
      in1->getFanout().erase(std::remove(in1->getFanout().begin(), in1->getFanout().end(), Id*2), in1->getFanout().end());
      in1->getFanout().erase(std::remove(in1->getFanout().begin(), in1->getFanout().end(), Id*2+1), in1->getFanout().end());
      in2->getFanout().erase(std::remove(in2->getFanout().begin(), in2->getFanout().end(), Id*2), in2->getFanout().end());
      in2->getFanout().erase(std::remove(in2->getFanout().begin(), in2->getFanout().end(), Id*2+1), in2->getFanout().end());

      for(IdList::iterator it = fanout.begin(); it != fanout.end(); it++)
      {
         cirMgr->_GateList[0]->getFanout().push_back(*it);
      }
   }
   else if(fanin1 == 1 || fanin2 == 1)
   {
      delList.push_back(Id);
      if(fanin1 == 1)
      {
         cout << "Simplifying: " << (fanin2 / 2) << " merging " << (fanin2%2 != 0? "!" : "") << Id << "..." << endl;
         // below is to modify fanout's fanin
         for(IdList::iterator it = fanout.begin(); it != fanout.end(); it++)
         {
            CirGate* out = cirMgr->_GateList[*it / 2];
            if(out->getTypeStr() == "PO")  // fanout is PO_GAE
               out->setFanin((fanin2/2)*2 + (*it + fanin2 % 2) % 2); // fanout's fanin = 0 + (invert)
            else if(out->getTypeStr() == "AIG") //fanout in AIG_GATE
            {
               if(((AIG_CirGate*) out)->fanin1 / 2 == Id) // AIG's fanin1 is this gate
                  out->setFanin((fanin2/2)*2 + (*it + fanin2 % 2) % 2, 1);
               if(((AIG_CirGate*) out)->fanin2 / 2 == Id) // // AIG's fanin2 is this gate
                  out->setFanin((fanin2/2)*2 + (*it + fanin2 % 2) % 2, 2);
            }
         }
         // below is to modify fanin's fanout  
         // note: fanin may be PI_GATE or AIG_GATE
         CirGate* in1 = cirMgr->_GateList[fanin1 / 2];
         CirGate* in2 = cirMgr->_GateList[fanin2 / 2];
         //cout << fanin1/2 << " " << fanin2/2 << endl;

         in1->getFanout().erase(std::remove(in1->getFanout().begin(), in1->getFanout().end(), Id*2), in1->getFanout().end());
         in1->getFanout().erase(std::remove(in1->getFanout().begin(), in1->getFanout().end(), Id*2+1), in1->getFanout().end());
         in2->getFanout().erase(std::remove(in2->getFanout().begin(), in2->getFanout().end(), Id*2), in2->getFanout().end());
         in2->getFanout().erase(std::remove(in2->getFanout().begin(), in2->getFanout().end(), Id*2+1), in2->getFanout().end());
         for(IdList::iterator it = fanout.begin(); it != fanout.end(); it++)
         {
            cirMgr->_GateList[fanin2 / 2]->getFanout().push_back((*it/2)*2 + (*it + fanin2) % 2);
         }
      }
      else // fanin2 is 1
      {
         cout << "Simplifying: " << (fanin1 / 2) << " merging " << (fanin1%2 != 0? "!" : "") << Id << "..." << endl;
         // below is to modify fanout's fanin
         for(IdList::iterator it = fanout.begin(); it != fanout.end(); it++)
         {
            CirGate* out = cirMgr->_GateList[*it / 2];
            if(out->getTypeStr() == "PO")  // fanout is PO_GAE
               out->setFanin((fanin1/2)*2 + (*it + fanin1 % 2) % 2); // fanout's fanin = 0 + (invert)
            else if(out->getTypeStr() == "AIG") //fanout in AIG_GATE
            {
               if(((AIG_CirGate*) out)->fanin1 / 2 == Id) // AIG's fanin1 is this gate
                  out->setFanin((fanin1/2)*2 + (*it + fanin1 % 2) % 2, 1);
               if(((AIG_CirGate*) out)->fanin2 / 2 == Id) // // AIG's fanin2 is this gate
                  out->setFanin((fanin1/2)*2 + (*it + fanin1 % 2) % 2, 2);
            }
         }
         // below is to modify fanin's fanout  
         // note: fanin may be PI_GATE or AIG_GATE
         CirGate* in1 = cirMgr->_GateList[fanin1 / 2];
         CirGate* in2 = cirMgr->_GateList[fanin2 / 2];      
         in1->getFanout().erase(std::remove(in1->getFanout().begin(), in1->getFanout().end(), Id*2), in1->getFanout().end());
         in1->getFanout().erase(std::remove(in1->getFanout().begin(), in1->getFanout().end(), Id*2+1), in1->getFanout().end());
         in2->getFanout().erase(std::remove(in2->getFanout().begin(), in2->getFanout().end(), Id*2), in2->getFanout().end());
         in2->getFanout().erase(std::remove(in2->getFanout().begin(), in2->getFanout().end(), Id*2+1), in2->getFanout().end());
         for(IdList::iterator it = fanout.begin(); it != fanout.end(); it++)
         {
            cirMgr->_GateList[fanin1 / 2]->getFanout().push_back((*it/2)*2 + (*it + fanin1) % 2);
         }
      }
   }
   else if(fanin1 == fanin2)
   {
      delList.push_back(Id);
      cout << "Simplifying: " << (fanin1 / 2) << " merging " << (fanin1%2 != 0? "!" : "") << Id << "..." << endl;
      // below is to modify fanout's fanin
      for(IdList::iterator it = fanout.begin(); it != fanout.end(); it++)
      {
         CirGate* out = cirMgr->_GateList[*it / 2];
         if(out->getTypeStr() == "PO")  // fanout is PO_GAE
            out->setFanin((fanin2/2)*2 + (*it + fanin2 % 2) % 2); // fanout's fanin = 0 + (invert)
         else if(out->getTypeStr() == "AIG") //fanout in AIG_GATE
         {
            if(((AIG_CirGate*) out)->fanin1 / 2 == Id) // AIG's fanin1 is this gate
               out->setFanin((fanin2/2)*2 + (*it + fanin2 % 2) % 2, 1);
            if(((AIG_CirGate*) out)->fanin2 / 2 == Id) // // AIG's fanin2 is this gate
               out->setFanin((fanin2/2)*2 + (*it + fanin2 % 2) % 2, 2);
         }
      }
      // below is to modify fanin's fanout  
      // note: fanin may be PI_GATE or AIG_GATE
      CirGate* in = cirMgr->_GateList[fanin1 / 2]; 
      in->getFanout().erase(std::remove(in->getFanout().begin(), in->getFanout().end(), Id*2), in->getFanout().end());
      in->getFanout().erase(std::remove(in->getFanout().begin(), in->getFanout().end(), Id*2+1), in->getFanout().end());

      for(IdList::iterator it = fanout.begin(); it != fanout.end(); it++)
      {
         cirMgr->_GateList[fanin1 / 2]->getFanout().push_back((*it/2)*2 + (*it + fanin1) % 2);
      }
   }
   else if(fanin1 / 2 == fanin2 / 2 && fanin1 % 2 != fanin2 % 2)
   {
      delList.push_back(Id);
      cout << "Simplifying: " << "0" << " merging " << Id << "..." << endl;
      // below is to modify fanout's fanin
      for(IdList::iterator it = fanout.begin(); it != fanout.end(); it++)
      {
         CirGate* out = cirMgr->_GateList[*it / 2];
         if(out->getTypeStr() == "PO")  // fanout is PO_GATE (not include UNDEF_GATE)
            out->setFanin(0 + (*it) % 2); // fanout's fanin = 0 + (invert)
         else if(out->getTypeStr() == "AIG") //fanout in AIG_GATE
         {
            if(((AIG_CirGate*) out)->fanin1 / 2 == Id) // AIG's fanin1 is this gate
               out->setFanin(0 + (*it) % 2, 1);
            if(((AIG_CirGate*) out)->fanin2 / 2 == Id) // // AIG's fanin2 is this gate
               out->setFanin(0 + (*it) % 2, 2);
         }
      }
      // below is to modify fanin's fanout  
      // note: fanin may be PI_GATE or AIG_GATE
      CirGate* in = cirMgr->_GateList[fanin1 / 2]; 
      in->getFanout().erase(std::remove(in->getFanout().begin(), in->getFanout().end(), Id*2), in->getFanout().end());
      in->getFanout().erase(std::remove(in->getFanout().begin(), in->getFanout().end(), Id*2+1), in->getFanout().end());
      for(IdList::iterator it = fanout.begin(); it != fanout.end(); it++)
      {
         cirMgr->_GateList[0]->getFanout().push_back(*it);
      }
   }
}

void AIG_CirGate::recursiveStrash(HashMap<HashKey, size_t>& map, IdList &delList){
   if(ref == CirGate::globalRef)
      return;
   ref = CirGate::globalRef;

   if(gateType != UNDEF_GATE)
   {
      cirMgr->_GateList[fanin1/2]->recursiveStrash(map, delList);
      cirMgr->_GateList[fanin2/2]->recursiveStrash(map, delList);
   }
   else
      return;
   HashKey key(fanin1, fanin2);
   size_t mergeGate;
   if(map.query(key, mergeGate))
   {
      cout << "Strashing: " << mergeGate << " merging " << Id << "..." << endl;
      merge(mergeGate);
      delList.push_back(Id);
   }
   else
      map.insert(key, Id);
}

void AIG_CirGate::merge(size_t mergeGate)
{
   for(IdList::iterator it = fanout.begin(); it != fanout.end(); it++)
   {
      CirGate* out = cirMgr->_GateList[*it / 2];
      if(out->getTypeStr() == "PO")  // fanout is PO_GATE (not include UNDEF_GATE)
         out->setFanin(mergeGate*2 + (*it) % 2); // fanout's fanin = 0 + (invert)
      else if(out->getTypeStr() == "AIG") //fanout in AIG_GATE
      {
         if(((AIG_CirGate*) out)->fanin1 / 2 == Id) // AIG's fanin1 is this gate
            out->setFanin(mergeGate*2 + (*it) % 2, 1);
         if(((AIG_CirGate*) out)->fanin2 / 2 == Id) // // AIG's fanin2 is this gate
            out->setFanin(mergeGate*2 + (*it) % 2, 2);
      }
      // give AIG's fanout to mergeGate
      cirMgr->_GateList[mergeGate]->getFanout().push_back(*it); 
   }
   
   
   // below is to modify fanin's fanout  
   // note: fanin may be PI_GATE or AIG_GATE

   CirGate* in1 = cirMgr->_GateList[fanin1 / 2];
   CirGate* in2 = cirMgr->_GateList[fanin2 / 2];
   in1->getFanout().erase(std::remove(in1->getFanout().begin(), in1->getFanout().end(), Id*2), in1->getFanout().end());
   in1->getFanout().erase(std::remove(in1->getFanout().begin(), in1->getFanout().end(), Id*2+1), in1->getFanout().end());
   in2->getFanout().erase(std::remove(in2->getFanout().begin(), in2->getFanout().end(), Id*2), in2->getFanout().end());
   in2->getFanout().erase(std::remove(in2->getFanout().begin(), in2->getFanout().end(), Id*2+1), in2->getFanout().end());
   // for(IdList::iterator it = in1->getFanout().begin(); it != in1->getFanout().end(); it++)
   // {
   //    if(*it / 2 == Id)
   //    {
   //       *it = *it % 2 + mergeGate * 2;
   //    }
   // }
   // for(IdList::iterator it = in2->getFanout().begin(); it != in2->getFanout().end(); it++)
   // {
   //    if(*it / 2 == Id)
   //    {
   //       *it = *it % 2 + mergeGate * 2;
   //    }
   // }
}

bool AIG_CirGate::recursiveSim()
{
   if(ref == CirGate::globalRef)
      return true;
   ref = CirGate::globalRef;
   if(gateType == UNDEF_GATE)
   {
      simValue = 0;
      return true;
   }
   cirMgr->_GateList[fanin1/2]->recursiveSim();
   cirMgr->_GateList[fanin2/2]->recursiveSim();
   size_t v1 = fanin1 % 2 == 0? cirMgr->_GateList[fanin1/2]->simValue : ~(cirMgr->_GateList[fanin1/2]->simValue);
   size_t v2 = fanin2 % 2 == 0? cirMgr->_GateList[fanin2/2]->simValue : ~(cirMgr->_GateList[fanin2/2]->simValue);
   simValue = v1 & v2;
   //cout << simValue << endl;
   return true;
}

void AIG_CirGate::dfs(IdList& v)
{
   if(ref == CirGate::globalRef)
      return;
   ref = CirGate::globalRef;

   
   if(gateType != UNDEF_GATE)
   {
      cirMgr->_GateList[fanin1/2]->dfs(v);
      cirMgr->_GateList[fanin2/2]->dfs(v);
   }
   else
      return;
   v.push_back(Id);
}