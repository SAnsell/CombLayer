/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   inputInc/inputParam.h
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#ifndef mainSystem_inputParam_h
#define mainSystem_inputParam_h

class FuncDataBase;

namespace mainSystem
{
  class IItemBase;
/*!
  \class inputParam
  \version 1.0
  \author S. Ansell
  \date March 2011
  \brief All the input parameter system
*/

class inputParam
{
 private:

  /// Keys/Names type
  typedef std::map<std::string,IItemBase*> MTYPE;

  std::map<std::string,IItemBase*> Keys;         ///< Simple search key
  std::map<std::string,IItemBase*> Names;        ///< Full name [optional]

  void copyMaps(const inputParam&);

  void deleteMaps();
  IItemBase* getIndex(const std::string&);
  const IItemBase* getIndex(const std::string&) const;
  const IItemBase* findKey(const std::string&) const;
  const IItemBase* findShortKey(const std::string&) const;
  const IItemBase* findLongKey(const std::string&) const;

  IItemBase* findKey(const std::string&);
  IItemBase* findShortKey(const std::string&);
  IItemBase* findLongKey(const std::string&);
  
 public:

  inputParam();
  inputParam(const inputParam&);
  inputParam& operator=(const inputParam&);
  ~inputParam();
  
  void regFlag(const std::string&,const std::string&);

  template<typename T>
  void regItem(const std::string&,const std::string&,
	       const size_t =1);
  template<typename T>
  void regMulti(const std::string&,const std::string&,const size_t,
		const long int = -1);

  template<typename T>
  void regDefItemList(const std::string&,const std::string&,
		      const size_t,const std::vector<T>&);

  template<typename T>
  void regDefItem(const std::string&,const std::string&,
		  const size_t,const T&);
  template<typename T>
  void regDefItem(const std::string&,const std::string&,
		  const size_t,const T&,const T&);
  template<typename T>
  void regDefItem(const std::string&,const std::string&,
		  const size_t,const T&,const T&,const T&);

 
  size_t dataCnt(const std::string&) const;
  size_t grpCnt(const std::string&) const;
  size_t itemCnt(const std::string&,const size_t) const;

  bool hasKey(const std::string&) const;

  bool flag(const std::string&) const;

  std::string getFull(const std::string&,const size_t =0) const;
  template<typename T>
  const T& getValue(const std::string&,const size_t =0) const;

  template<typename T>
  const T& getCompValue(const std::string&,const size_t,const size_t) const;


  bool compNoCaseValue(const std::string&,const std::string&) const;
  template<typename T>
  bool compValue(const std::string&,const T&) const;

  template<typename T>
  T getFlagDef(const std::string&,const FuncDataBase&,
		      const std::string&,const size_t =0) const;

  void setDesc(const std::string&,const std::string&);
  void setFlag(const std::string&);
  template<typename T>
  void setValue(const std::string&,const T&,const size_t =0);

  void processMainInput(std::vector<std::string>&);
  
  void writeDescription(std::ostream&) const;
  void write(std::ostream&) const;



};

}

#endif
 
