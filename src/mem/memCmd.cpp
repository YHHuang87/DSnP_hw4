/****************************************************************************
  FileName     [ memCmd.cpp ]
  PackageName  [ mem ]
  Synopsis     [ Define memory test commands ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <iomanip>
#include "memCmd.h"
#include "memTest.h"
#include "cmdParser.h"
#include "util.h"

using namespace std;

extern MemTest mtest;  // defined in memTest.cpp

bool
initMemCmd()
{
   if (!(cmdMgr->regCmd("MTReset", 3, new MTResetCmd) &&
         cmdMgr->regCmd("MTNew", 3, new MTNewCmd) &&
         cmdMgr->regCmd("MTDelete", 3, new MTDeleteCmd) &&
         cmdMgr->regCmd("MTPrint", 3, new MTPrintCmd)
      )) {
      cerr << "Registering \"mem\" commands fails... exiting" << endl;
      return false;
   }
   return true;
}


//----------------------------------------------------------------------
//    MTReset [(size_t blockSize)]
//----------------------------------------------------------------------
CmdExecStatus
MTResetCmd::exec(const string& option)
{
   // check option
   string token;
   if (!CmdExec::lexSingleOption(option, token))
      return CMD_EXEC_ERROR;
   if (token.size()) {
      int b;
      if (!myStr2Int(token, b) || b < int(toSizeT(sizeof(MemTestObj)))) {
         cerr << "Illegal block size (" << token << ")!!" << endl;
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
      }
      #ifdef MEM_MGR_H
      mtest.reset(toSizeT(b));
      #else
      mtest.reset();
      #endif // MEM_MGR_H
   }
   else
      mtest.reset();
   return CMD_EXEC_DONE;
}

void
MTResetCmd::usage(ostream& os) const
{  
   os << "Usage: MTReset [(size_t blockSize)]" << endl;
}

void
MTResetCmd::help() const
{  
   cout << setw(15) << left << "MTReset: " 
        << "(memory test) reset memory manager" << endl;
}


//----------------------------------------------------------------------
//    MTNew <(size_t numObjects)> [-Array (size_t arraySize)]
//----------------------------------------------------------------------
CmdExecStatus
MTNewCmd::exec(const string& option)
{
   // TODO
   vector<string>tokens;
   const string a_name = "-array";
   lexOptions(option, tokens);
   string temp;
   int num = 0;
   int n = 0;
   if(tokens.size() == 0)                     return errorOption(CMD_OPT_MISSING, temp);
   else if(tokens.size() == 1){
     if(myStr2Int(tokens[0],num) && num > 0){
        try{
          mtest.newObjs(num);
        }catch(bad_alloc& exception){
          //throw exception;
        }
        return CMD_EXEC_DONE;
     }
     else if(myStrNCmp(a_name, tokens[0], 2) == 0) return errorOption(CMD_OPT_MISSING, tokens[0]);
     else                                     return errorOption(CMD_OPT_ILLEGAL, tokens[0]);
   }
   else if(tokens.size() == 2){
     if(myStr2Int(tokens[0],num) && num > 0){
        if(myStrNCmp(a_name, tokens[1], 2) == 0)   return errorOption(CMD_OPT_MISSING, tokens[1]);
        else                                  return errorOption(CMD_OPT_EXTRA, tokens[1]);
     }
     else if(myStrNCmp(a_name, tokens[0], 2)){
        if(myStr2Int(tokens[1],num) && num > 0)return errorOption(CMD_OPT_MISSING, temp);
        else                                  return errorOption(CMD_OPT_ILLEGAL, tokens[1]);
     }
     else                                     return errorOption(CMD_OPT_ILLEGAL, tokens[0]);
   }
   else if(tokens.size() >= 3){
     if(myStr2Int(tokens[0],num) && num > 0){
        if(myStrNCmp(a_name, tokens[1], 2) == 0){
          if(myStr2Int(tokens[2],n) && n > 0){
            if(tokens.size() == 3){
              try{
              mtest.newArrs(num, n);
              }catch(bad_alloc& exception){
                //throw exception;
              }
              return CMD_EXEC_DONE;
            }
            else                              return errorOption(CMD_OPT_EXTRA, tokens[3]);
          }
          else                                return errorOption(CMD_OPT_ILLEGAL, tokens[2]);
        }   
        else                                  return errorOption(CMD_OPT_EXTRA, tokens[1]);
     }
     else if(myStrNCmp(a_name, tokens[0], 2) == 0){
        if(myStr2Int(tokens[1],num) && num > 0){
          if(myStr2Int(tokens[2],n) && n > 0){
            if(tokens.size() == 3){
              try{
                mtest.newArrs(n, num);
              }catch(bad_alloc& exception){
                 //throw exception;     
              }
              return CMD_EXEC_DONE;
            }
            else                              return errorOption(CMD_OPT_EXTRA, tokens[3]);
          }
          else                                return errorOption(CMD_OPT_ILLEGAL, tokens[2]);
        }          
        else                                  return errorOption(CMD_OPT_ILLEGAL, tokens[1]);
     }
     else                                     return errorOption(CMD_OPT_ILLEGAL, tokens[0]);
   }
   // Use try-catch to catch the bad_alloc exception
   return CMD_EXEC_DONE;
}

void
MTNewCmd::usage(ostream& os) const
{  
   os << "Usage: MTNew <(size_t numObjects)> [-Array (size_t arraySize)]\n";
}

void
MTNewCmd::help() const
{  
   cout << setw(15) << left << "MTNew: " 
        << "(memory test) new objects" << endl;
}


//----------------------------------------------------------------------
//    MTDelete <-Index (size_t objId) | -Random (size_t numRandId)> [-Array]
//----------------------------------------------------------------------
CmdExecStatus
MTDeleteCmd::exec(const string& option)
{
   // TODO
   vector<string> tokens;
   int num = 0;
   lexOptions(option, tokens);
   string a_name = "-Array";
   string i_name = "-Index";
   string r_name = "-Random";
   string temp;
   //cout << ' ' << tokens.size() << endl;
   if(tokens.size() == 0)                                   return errorOption(CMD_OPT_MISSING, temp);
   else if(myStrNCmp(a_name, tokens[0], 2) == 0){
     //cout << "Error functioning" << endl;
     if(tokens.size() == 1)                                 return errorOption(CMD_OPT_MISSING, temp);
     else if(myStrNCmp(i_name, tokens[1], 2) == 0){
       if(tokens.size() == 2)                               return errorOption(CMD_OPT_MISSING, tokens[1]);
       else if(myStr2Int(tokens[2], num) && num >= 0){
         if(tokens.size() > 3){
           if(myStrNCmp(a_name, tokens[3], 2) == 0 
           || myStrNCmp(r_name, tokens[3], 2) == 0 
           || myStrNCmp(i_name, tokens[3], 2) == 0)          return errorOption(CMD_OPT_EXTRA, tokens[3]);
           else                                              return errorOption(CMD_OPT_ILLEGAL, tokens[3]);
         }  
         else{
           if(num < mtest.getArrListSize()) { mtest.deleteArr(num); return CMD_EXEC_DONE;}
           else{ 
             cerr << "Size of array list (" << mtest.getArrListSize() << ") is <= " << num << "!!" << endl;
             return errorOption(CMD_OPT_ILLEGAL, tokens[2]);
           }
         }
       }
       else                                                 return errorOption(CMD_OPT_ILLEGAL, tokens[2]);
     }
     else if(myStrNCmp(r_name, tokens[1], 2) == 0){
       if(tokens.size() == 2)                               return errorOption(CMD_OPT_MISSING, tokens[1]);
       else if(myStr2Int(tokens[2], num) && num > 0){
         if(tokens.size() > 3){
           if(myStrNCmp(a_name, tokens[3], 2) == 0 
           || myStrNCmp(r_name, tokens[3], 2) == 0 
           || myStrNCmp(i_name, tokens[3], 2) == 0)          return errorOption(CMD_OPT_EXTRA, tokens[3]);
           else                                              return errorOption(CMD_OPT_ILLEGAL, tokens[3]);
         }                              
         else{
           if(mtest.getArrListSize() > 0){
              int idx = 0;
              for(int i = 0; i < num; i++){
                idx = rnGen(mtest.getArrListSize());                 
                mtest.deleteArr(idx);
              }
            }
            else{ cerr << "Size of array list is 0!!" << endl; errorOption(CMD_OPT_ILLEGAL, tokens[1]);}
           return CMD_EXEC_DONE;
         }
       }
       else                                                 return errorOption(CMD_OPT_ILLEGAL, tokens[2]);
     }
     else                                                   return errorOption(CMD_OPT_ILLEGAL, tokens[1]);
   }
   //cout << "Still functioning" << endl;
   else if(myStrNCmp(i_name, tokens[0], 2) == 0){
     if(tokens.size() == 1)                                 return errorOption(CMD_OPT_MISSING, tokens[0]);
     else if(myStr2Int(tokens[1], num) && num >= 0){
        if(tokens.size() == 2){
           if(num < mtest.getObjListSize()) { mtest.deleteObj(num); return CMD_EXEC_DONE;}
           else{ 
             cerr << "Size of object list (" << mtest.getObjListSize() << ") is <= " << num << "!!" << endl;
             return errorOption(CMD_OPT_ILLEGAL, tokens[1]);;
           }
        }
        else if(myStrNCmp(a_name, tokens[2], 2) == 0){
          if(tokens.size() == 3){
            if(num < mtest.getArrListSize()) { mtest.deleteArr(num); return CMD_EXEC_DONE;}
            else{ 
              cerr << "Size of array list (" << mtest.getArrListSize() << ") is <= " << num << "!!" << endl;
              return errorOption(CMD_OPT_ILLEGAL, tokens[1]);;
            }
          }
          else if(myStrNCmp(a_name, tokens[3], 2) == 0 
          || myStrNCmp(r_name, tokens[3], 2) == 0 
          || myStrNCmp(i_name, tokens[3], 2) == 0)          return errorOption(CMD_OPT_EXTRA, tokens[3]);
          else                                              return errorOption(CMD_OPT_ILLEGAL, tokens[3]);
        }
        else                                                return errorOption(CMD_OPT_ILLEGAL, tokens[2]);
     }
     else                                                   return errorOption(CMD_OPT_ILLEGAL, tokens[1]);
   }
   else if(myStrNCmp(r_name, tokens[0], 2) == 0){
     if(tokens.size() == 1)                                 return errorOption(CMD_OPT_MISSING, tokens[0]);
     else if(myStr2Int(tokens[1], num) && num > 0){
        if(tokens.size() == 2){
         int idx = 0;
         if(mtest.getObjListSize() > 0){
           for(int i = 0; i < num; i++){
             idx = rnGen(mtest.getObjListSize());
             mtest.deleteObj(idx);
           }
         }
         else{ cerr << "Size of object list is 0!!" << endl; return errorOption(CMD_OPT_ILLEGAL, tokens[0]);}
         return CMD_EXEC_DONE;
        }
        else if(myStrNCmp(a_name, tokens[2], 2) == 0){
          if(tokens.size() == 3){
            if(mtest.getArrListSize() > 0){
              int idx = 0;
              for(int i = 0; i < num; i++){
                idx = rnGen(mtest.getArrListSize());                 
                mtest.deleteArr(idx);
              }
            }
            else{ cerr << "Size of array list is 0!!" << endl; errorOption(CMD_OPT_ILLEGAL, tokens[0]);}
            return CMD_EXEC_DONE;
          }
          else if(myStrNCmp(a_name, tokens[3], 2) == 0 
          || myStrNCmp(r_name, tokens[3], 2) == 0 
          || myStrNCmp(i_name, tokens[3], 2) == 0)          return errorOption(CMD_OPT_EXTRA, tokens[3]);
          else                                              return errorOption(CMD_OPT_ILLEGAL, tokens[3]);
        }
        else                                                return errorOption(CMD_OPT_ILLEGAL, tokens[2]);
     }
     else                                                   return errorOption(CMD_OPT_ILLEGAL, tokens[1]);
   }
   else                                                     return errorOption(CMD_OPT_ILLEGAL, tokens[0]);
   return CMD_EXEC_DONE;
}

void
MTDeleteCmd::usage(ostream& os) const
{  
   os << "Usage: MTDelete <-Index (size_t objId) | "
      << "-Random (size_t numRandId)> [-Array]" << endl;
}

void
MTDeleteCmd::help() const
{  
   cout << setw(15) << left << "MTDelete: " 
        << "(memory test) delete objects" << endl;
}


//----------------------------------------------------------------------
//    MTPrint
//----------------------------------------------------------------------
CmdExecStatus
MTPrintCmd::exec(const string& option)
{
   // check option
   if (option.size())
      return CmdExec::errorOption(CMD_OPT_EXTRA, option);
   mtest.print();

   return CMD_EXEC_DONE;
}

void
MTPrintCmd::usage(ostream& os) const
{  
   os << "Usage: MTPrint" << endl;
}

void
MTPrintCmd::help() const
{  
   cout << setw(15) << left << "MTPrint: " 
        << "(memory test) print memory manager info" << endl;
}


