/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   funcBaseInc/FuncDataBase.h
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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

  Based on the code from J. Nieminen from http://iki.fi/warp/
  Extended to deal with function groups and commands.

*/

class FuncDataBase 
{
 private:

  varList VList;           ///< Variable list
  Code Build;              ///< Current total-bytecode

  size_t compileExpression(const std::string&,const size_t);
  size_t compileFunctionParams(const std::string&,const size_t,const size_t);
  size_t compileElement(const std::string&,const size_t);
  size_t compileEqual(const std::string&,const size_t);

  size_t compileAdd(const std::string&,const size_t);
  size_t compileMult(const std::string&,const size_t);
  size_t compilePow(const std::string&,const size_t);
  size_t compileUnaryMinus(const std::string&,const size_t);

  std::pair<std::string,size_t> CheckSyntax(const std::string&) const;
  long int Compile(const std::string&);
  std::string getItem(const std::string&) const;

  size_t extractComponents(const std::string&,
			std::vector<std::string>&) const;
  int stripEqual(std::string&);
  std::string subProcVar(std::string&) const;

 public:

  FuncDataBase();
  FuncDataBase(const FuncDataBase&);
  FuncDataBase& operator=(const FuncDataBase&);
  ~FuncDataBase();
  
  /// Access varList
  const varList& getVarList() const { return VList; }
  
  //  int hasItem(const std::string&) const;
  const FItem* findItem(const std::string&) const;
  //  void setFuncParser(const std::string&,const FuncDataBase&);
  
  int Parse(const std::string&);
  template<typename T>
  T Eval();
  template<typename T>
  T EvalVar(const std::string&) const;      
  template<typename T>
  T EvalDefVar(const std::string&,const T&) const;      
  template<typename T>
  T EvalPair(const std::string&,const std::string&) const;      
  template<typename T>
  T EvalPair(const std::string&,const std::string&,
	     const std::string&) const;      
  template<typename T>
  T EvalDefPair(const std::string&,const std::string&,
		const T&) const;      
  template<typename T>
  T EvalDefPair(const std::string&,const std::string&,
		const std::string&,const T&) const;      

  template<typename T>
  T EvalTriple(const std::string&,const std::string&,
	       const std::string&) const;        
  template<typename T>
  T EvalTriple(const std::string&,const std::string&,
	       const std::string&,const std::string&) const;      
  

  template<typename T>
  void addParse(const std::string&,const std::string&);
  void copyVar(const std::string&,const std::string&);
  void copyVarSet(const std::string&,const std::string&);
  
  // Add variable
  template<typename T>
  void addVariable(const std::string&,const T&);
  void addVariable(const std::string&,const char*);
  void addVariable(const std::string&);

  template<typename T>
  void setVariable(const std::string&,const T&);
  void setVariable(const std::string&);

  void removeVariable(const std::string&);
  
  int hasVariable(const std::string&) const;

  void writeAll(const std::string&) const; 
  void processXML(const std::string&);
  void writeXML(const std::string&) const;
  /// Debug print function
  void printByteCode(std::ostream& OX) const { Build.printByteCode(OX); }

  /// access keys
  std::vector<std::string> getKeys() const { return VList.getKeys(); }
  std::string variableHash() const;

  // RESET of active
  void resetActive();
  
};

#endif

