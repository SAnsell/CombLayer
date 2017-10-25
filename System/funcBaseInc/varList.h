/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   funcBaseInc/varList.h
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
#ifndef varList_h
#define varList_h 
  
/*!
  \class varList
  \brief Singleton class of all variables
  \author S. Ansell
  \version 1.0
  \date April 2006

  This class holds the variable name + number 
  relative to the actual variable type object. 
*/

class FItem;

class varList
{
 public:

  typedef std::map<std::string,FItem*> varStore;  ///< Store of variable items

 private:

  int varNum;                              ///< Current max var

  varStore varName;                        ///< Var by name
  std::map<int,FItem*> varItem;            ///< Var by number

  void deleteMem();

 public:

  varList();
  varList(const varList&);
  varList& operator=(const varList&);
  ~varList();

  const FItem* findVar(const std::string&) const;
  const FItem* findVar(const int) const;
  FItem* findVar(const std::string&);
  FItem* findVar(const int);

  void copyVar(const std::string&,const std::string&);
  void copyVarSet(const std::string&,const std::string&);
  
  int selectValue(const int,Geometry::Vec3D&,double&) const;

  template<typename T>
  T getValue(const int) const;

  template<typename T>
  void setValue(const int,const T&);

  template<typename T>
  void addVar(const std::string&,const T&);
  template<typename T>
  void setVar(const std::string&,const T&);
  void removeVar(const std::string&);

  /// Accessors to begin
  varStore::const_iterator begin() const { return varName.begin(); }
  /// Accessors to end
  varStore::const_iterator end() const { return varName.end(); }

  template<typename T>
  FItem* createFType(const int,const T&);

  void resetActive();
  
  std::vector<std::string> getKeys() const;
  void writeActive(std::ostream&) const;
  void writeAll(std::ostream&) const;

};



#endif
