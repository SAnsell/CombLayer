/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   old/FuncDataBase.h
*
 * Copyright (c) 2004-2013 by Stuart Ansell
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. 
 *
 ****************************************************************************/
#ifndef FuncDataBase_h
#define FuncDataBase_h


class FItem;

/*!
  \class FuncDataBase 
  \brief holds Variables and FuncComponents etc.
  \version 1.0
  \author S. Ansell
  \date April 2006
  
*/

class FuncDataBase 
{
 private:
  
  Code Build;              ///< Current total-bytecode

  int compileExpression(const std::string&,const int);
  int compileFunctionParams(const std::string&,const int,const int);
  int compileElement(const std::string&,const int);

  int compileAdd(const std::string&,const int);
  int compileMult(const std::string&,const int);
  int compilePow(const std::string&,const int);
  int compileUnaryMinus(const std::string&,const int);

  std::pair<int,int> CheckSyntax(const std::string&) const;
  int Compile(const std::string&);
  std::string getItem(const std::string&) const;

 public:

  FuncDataBase();
  FuncDataBase(const FuncDataBase&);
  FuncDataBase& operator=(const FuncDataBase&);
  ~FuncDataBase();
  
  //  int hasItem(const std::string&) const;
  const FItem* findItem(const std::string&) const;
  //  void setFuncParser(const std::string&,const FuncDataBase&);
  
  int Parse(const std::string&);
  double Eval();
  double EvalVar(const std::string&) const;      

  /// Add variable
  void addVariable(const std::string&,const double) const;
  void addVariable(const std::string&) const;

  /// Debug print function
  void printByteCode(std::ostream& OX) const { Build.printByteCode(OX); }
};


#endif

