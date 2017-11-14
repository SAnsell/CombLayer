/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   inputInc/inputParam.h
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
#ifndef mainSystem_inputParam_h
#define mainSystem_inputParam_h

class FuncDataBase;

namespace mainSystem
{
  class IItem;
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
  typedef std::map<std::string,IItem*> MTYPE;

  std::map<std::string,IItem*> Keys;         ///< Simple search key
  std::map<std::string,IItem*> Names;        ///< Full name [optional]

  void copyMaps(const inputParam&);

  void deleteMaps();
  IItem* getIndex(const std::string&);
  const IItem* getIndex(const std::string&) const;
  const IItem* findKey(const std::string&) const;
  const IItem* findShortKey(const std::string&) const;
  const IItem* findLongKey(const std::string&) const;

  IItem* findKey(const std::string&);
  IItem* findShortKey(const std::string&);
  IItem* findLongKey(const std::string&);

  void checkKeys(const std::string&,const std::string&) const;
  
 public:

  inputParam();
  inputParam(const inputParam&);
  inputParam& operator=(const inputParam&);
  ~inputParam();
  
  void regFlag(const std::string&,const std::string&);
  void regItem(const std::string&,const std::string&,const size_t =0,
	       const size_t = 10000);
  void regMulti(const std::string&,const std::string&,const size_t =10000,
		const size_t = 1,const size_t = 10000);
  

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
  size_t setCnt(const std::string&) const;
  size_t itemCnt(const std::string&,const size_t) const;

  bool hasKey(const std::string&) const;

  bool flag(const std::string&) const;

  std::string getFull(const std::string&,const size_t =0) const;
  template<typename T>
  T getValue(const std::string&,const size_t =0) const;
  template<typename T>
  T getValue(const std::string&,const size_t,const size_t) const;
  template<typename T>
  T getDefValue(const T&,const std::string&,const size_t =0) const;
  template<typename T>
    T getDefValue(const T&,const std::string&,const size_t,const size_t) const;
  template<typename T>
  T getValueError(const std::string&,const size_t,const size_t,
		  const std::string&) const;


  std::vector<std::string>
    getAllItems(const std::string&) const;

  const std::vector<std::string>&
    getObjectItems(const std::string&,const size_t) const;

  Geometry::Vec3D getCntVec3D(const std::string&,
			      const size_t,size_t&) const;
  Geometry::Vec3D getCntVec3D(const std::string&,
			      const size_t,size_t&,
			      const std::string&) const;
  template<typename T>
  T outputItem(const std::string&,const size_t,
	 const size_t,const std::string&) const;

  template<typename T>
  T outputDefItem(const std::string&,const size_t,
		  size_t&,const T&) const;
  
  template<typename T>
  int checkItem(const std::string&,const size_t,const size_t,T&) const;
  int checkCntVec3D(const std::string&,const size_t,
		    size_t&,Geometry::Vec3D&) const;
  
  
  bool compNoCaseValue(const std::string&,const std::string&) const;
  template<typename T>
  bool compValue(const std::string&,const T&) const;

  template<typename T>
  T getFlagDef(const std::string&,const FuncDataBase&,
		      const std::string&,const size_t =0) const;

  template<typename T>
  std::set<T> getComponents(const std::string&,const size_t) const;
  
  void setDesc(const std::string&,const std::string&);
  void setFlag(const std::string&);
  template<typename T>
  void setValue(const std::string&,const T&,const size_t =0);
  template<typename T>
  void setValue(const std::string&,const T&,const size_t,const size_t);
  void setMultiValue(const std::string&,const size_t,const std::string&);

  void processMainInput(std::vector<std::string>&);
  
  void writeDescription(std::ostream&) const;
  void write(std::ostream&) const;

};

}

#endif
 
