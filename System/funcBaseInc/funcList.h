/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   funcBaseInc/funcList.h
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
#ifndef funcList_h
#define funcList_h 

/*!
  \class FuncDefinition 
  \brief Function information 
  \date August 2005
  \version 1.0
  
  Class to hold everything we 
  need to know about built in function 
*/

class FuncDefinition
{
  public:
    
    std::string name;        ///< Name of the function 
    size_t nameLength;       ///< length of the name of the function
    int opCode;              ///< code for this function
    size_t Np;               ///< number of parameters 
    
    FuncDefinition(const std::string&,const int,const size_t);
    FuncDefinition(const FuncDefinition&);
    FuncDefinition& operator=(const FuncDefinition&);
    ~FuncDefinition();
};
  
/*!
  \class funcList
  \brief Singleton class of all basic functions
  \author S. Ansell
  \date August 2005
  \version 1.0
*/

class funcList
{
 private:

  /// nullItem for zero references
  const FuncDefinition nullItem;                
  /// List of math functions
  std::map<std::string,FuncDefinition> mathFunc;      

  funcList();
  funcList(const funcList&);              ///< Private copy constructor 
  funcList& operator=(const funcList&);   ///< Private assignment constructor

 public:

  static funcList* Instance();                                
  ~funcList() { }  ///< Destructor

  const FuncDefinition& findFunction(const std::string&) const;
  const FuncDefinition& findFunction(const int) const;
};



#endif
