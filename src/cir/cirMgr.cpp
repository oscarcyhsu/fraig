/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cstdio>
#include <ctype.h>
#include <cassert>
#include <cstring>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Implement memeber functions for class CirMgr

/*******************************/
/*   Global variable and enum  */
/*******************************/
CirMgr* cirMgr = 0;

enum CirParseError {
   EXTRA_SPACE,
   MISSING_SPACE,
   ILLEGAL_WSPACE,
   ILLEGAL_NUM,
   ILLEGAL_IDENTIFIER,
   ILLEGAL_SYMBOL_TYPE,
   ILLEGAL_SYMBOL_NAME,
   MISSING_NUM,
   MISSING_IDENTIFIER,
   MISSING_NEWLINE,
   MISSING_DEF,
   CANNOT_INVERTED,
   MAX_LIT_ID,
   REDEF_GATE,
   REDEF_SYMBOLIC_NAME,
   REDEF_CONST,
   NUM_TOO_SMALL,
   NUM_TOO_BIG,

   DUMMY_END
};

/**************************************/
/*   Static varaibles and functions   */
/**************************************/
static unsigned lineNo = 0;  // in printint, lineNo needs to ++
static unsigned colNo  = 0;  // in printing, colNo needs to ++
static char buf[1024];
static string errMsg;
static int errInt;
static CirGate *errGate;

static bool
parseError(CirParseError err)
{
   switch (err) {
      case EXTRA_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Extra space character is detected!!" << endl;
         break;
      case MISSING_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing space character!!" << endl;
         break;
      case ILLEGAL_WSPACE: // for non-space white space character
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal white space char(" << errInt
              << ") is detected!!" << endl;
         break;
      case ILLEGAL_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal "
              << errMsg << "!!" << endl;
         break;
      case ILLEGAL_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal identifier \""
              << errMsg << "\"!!" << endl;
         break;
      case ILLEGAL_SYMBOL_TYPE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal symbol type (" << errMsg << ")!!" << endl;
         break;
      case ILLEGAL_SYMBOL_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Symbolic name contains un-printable char(" << errInt
              << ")!!" << endl;
         break;
      case MISSING_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing " << errMsg << "!!" << endl;
         break;
      case MISSING_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing \""
              << errMsg << "\"!!" << endl;
         break;
      case MISSING_NEWLINE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": A new line is expected here!!" << endl;
         break;
      case MISSING_DEF:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing " << errMsg
              << " definition!!" << endl;
         break;
      case CANNOT_INVERTED:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": " << errMsg << " " << errInt << "(" << errInt/2
              << ") cannot be inverted!!" << endl;
         break;
      case MAX_LIT_ID:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Literal \"" << errInt << "\" exceeds maximum valid ID!!"
              << endl;
         break;
      case REDEF_GATE:
         cerr << "[ERROR] Line " << lineNo+1 << ": Literal \"" << errInt
              << "\" is redefined, previously defined as "
              << errGate->getTypeStr() << " in line " << errGate->getLineNo()
              << "!!" << endl;
         break;
      case REDEF_SYMBOLIC_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ": Symbolic name for \""
              << errMsg << errInt << "\" is redefined!!" << endl;
         break;
      case REDEF_CONST:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Cannot redefine const (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_SMALL:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too small (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_BIG:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too big (" << errInt << ")!!" << endl;
         break;
      default: break;
   }
   return false;
}

/**************************************************************/
/*   class CirMgr member functions for circuit construction   */
/**************************************************************/
bool
CirMgr::readCircuit(const string& fileName)
{
   fstream inf(fileName);
   if(!inf)
   {
      cerr << "Cannot open design \"" << fileName << "\"!!" << endl;
      return false;
   }
   
   char   buffer[100] = {};
   inf >> buffer; //aag

   inf >> M >> I >> L >> O >> A;
   _GateList.resize(1 + M + O, NULL); //rand access
   _IdList.reserve(1 + M + O);
   _GateList[0] = new CONST_CirGate();
   _IdList.push_back(0);
   for(int i = 0; i < I; i++)
   {
      int gate;
      inf >> gate;
      _GateList[gate / 2] = new PI_CirGate(gate / 2, i + 2);
      _IdList.push_back(gate / 2);
   }
   for(int i = 0; i < O; i++)
   {
      int gate;
      inf >> gate;
      if(_GateList[gate / 2] == 0) //fanin not created
      {
         _GateList[gate / 2] = new AIG_CirGate(gate / 2); //create fanin(must be AIG)
      }
      _GateList[M + 1 + i] = new PO_CirGate(M + 1 + i, I + 2 + i); //create PO_gate
      _IdList.push_back(M + 1 + i);
      _GateList[M + 1 + i]->addIn(gate);
      _GateList[gate / 2]->addOut(2*(M + 1 + i) + (gate % 2));
   }
   for(int i = 0; i < A; i++)
   {
      int gate, fin1, fin2;
      inf >> gate >> fin1 >> fin2;
      if(_GateList[fin1 / 2] == 0) //fin1 not created
      {
         _GateList[fin1 / 2] = new AIG_CirGate(fin1 / 2);
      }
      if(_GateList[fin2 / 2] == 0) //fin2 not created
      {
         _GateList[fin2 / 2] = new AIG_CirGate(fin2 / 2);
      }
      if(_GateList[gate / 2] == 0) //gate not created
      {
         _GateList[gate / 2] = new AIG_CirGate(gate / 2, 2 + I + O + i);
      }
      else
         _GateList[gate / 2]->setLine(2 + I + O + i); // created before, thus line haven't been set
      _IdList.push_back(gate / 2);
      _GateList[fin1 / 2]->addOut(gate + (fin1 % 2));
      _GateList[fin2 / 2]->addOut(gate + (fin2 % 2));
      _GateList[gate / 2]->addIn(fin1);
      _GateList[gate / 2]->addIn(fin2);
   }
   while(inf.getline(buffer, 100))
   {
      if(buffer[0] == 'i')
      {
         string s = buffer;
         int i = 0;
         while(s[i] != ' ')
            i++;
         static_cast<PI_CirGate*> (_GateList[_IdList[1 + stoi(s.substr(1, i))]])->setSymbol(s.substr(i+1));
      }
      else if(buffer[0] == 'o')
      {
         string s = buffer;
         int i = 0;
         while(s[i] != ' ')
            i++;
         static_cast<PO_CirGate*> (_GateList[_IdList[1 + I + stoi(s.substr(1, i))]])->setSymbol(s.substr(i+1));
      }
   }
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
   sort(_WithFloating.begin(), _WithFloating.end());
   sort(_UnUsed.begin(), _UnUsed.end());
   for(auto it = _GateList.begin(); it != _GateList.end(); it++)
   {
      if(*it != 0)
         (*it)->sortFanout();
   }
   inf.close();
   return true;
}

/**********************************************************/
/*   class CirMgr member functions for circuit printing   */
/**********************************************************/
/*********************
Circuit Statistics
==================
  PI          20
  PO          12
  AIG        130
------------------
  Total      162
*********************/
void
CirMgr::printSummary() const
{
   cout << endl;
   cout << "Circuit Statistics" << endl;
   cout << "==================" << endl;
   cout << "  PI" << right << setw(12) << I << endl;
   cout << "  PO" << right << setw(12) << O << endl;
   cout << "  AIG" << right << setw(11) << A << endl;
   cout << "------------------" << endl;
   cout << "  Total" << right << setw(9) << I + O + A << endl;
}

void
CirMgr::printNetlist() const
{
/*
   cout << endl;
   for (unsigned i = 0, n = _dfsList.size(); i < n; ++i) {
      cout << "[" << i << "] ";
      _dfsList[i]->printGate();
   }
*/
   cout << endl;
   _GateList[0]->setGlobalRef(); //static 
   int count = 0;
   for(int i = 0; i < O; i++)
   {
      _GateList[_IdList[I + 1 + i]]->printGate(count);
   }
}

void
CirMgr::printPIs() const
{
   cout << "PIs of the circuit:";
   for(int i = 0; i < I; i++)
      cout << " " << _IdList[1+i];
   cout << endl;
}

void
CirMgr::printPOs() const
{
   cout << "POs of the circuit:";
   for(int i = 0; i < O; i++)
      cout << " " << _IdList[1 + I + i];
   cout << endl;
}

void
CirMgr::printFloatGates() const
{
   if(_WithFloating.size())
   {
      cout << "Gates with floating fanin(s):";
      for(auto it = _WithFloating.begin(); it != _WithFloating.end(); it++)
         cout << " " << *it;
      cout << endl;
   }
   if(_UnUsed.size())
   {
      cout << "Gates defined but not used  :";
      for(auto it = _UnUsed.begin(); it != _UnUsed.end(); it++)
         cout << " " << *it;
      cout << endl;
   }
}

void
CirMgr::printFECPairs() const
{
}

void
CirMgr::writeAag(ostream& outfile) const
{
   stringstream ss;
   _GateList[0]->setGlobalRef(); //static 
   int count = 0;
   for(int i = 0; i < O; i++)
   {
      _GateList[_IdList[I + 1 + i]]->writeToSs(count, ss);
   }
   outfile << "aag" << " " << M << " " << I << " " << L << " " << O << " " << count << endl;
   char buffer[200];
   for(int i = 0; i < I; i++)
   {
      cout << 2*_IdList[1 + i] << endl;
   }
   for(int i = 0; i < O; i++)
   {
      cout << ((PO_CirGate*)_GateList[_IdList[1 + I + i]])->getFanin() << endl;
   }
   while(ss.getline(buffer, 200))
   {
      cout << buffer << endl;
   }
   for(int i = 0; i < I; i++)
   {
      if(((PI_CirGate*)_GateList[_IdList[1 + i]])->getSymbol() != "")
         cout << 'i' << i << ' ' << ((PI_CirGate*)_GateList[_IdList[1 + i]])->getSymbol() << endl;
   }
   for(int i = 0; i < O; i++)
   {
      if(((PO_CirGate*)_GateList[_IdList[1 + I + i]])->getSymbol() != "")
         cout << 'o' << i << ' ' << ((PO_CirGate*)_GateList[_IdList[1 + I + i]])->getSymbol() << endl;
   }
   cout << "c" << endl;
   cout << "AAG output by Chung-Yang (Ric) Huang" << endl;
}

void
CirMgr::writeGate(ostream& outfile, CirGate *g) const
{
}

