/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   inputInc/IItemBase.h
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
#ifndef mainSystem_IItemBase_h
#define mainSystem_IItemBase_h

namespace mainSystem
{
  /*!
    \class IItemBase
    \version 1.0
    \author S. Ansell
    \date March 2011
    \brief Holds a base Item
  */
class IItemBase
{
 private:

  std::string Key;   ///< Key
  std::string Long;  ///< Long Key
  std::string Desc;  ///< Description

 public:

  explicit IItemBase(const std::string&);
  IItemBase(const std::string&,const std::string&);

  IItemBase(const IItemBase&);
  IItemBase& operator=(const IItemBase&);
  virtual ~IItemBase() {}  ///< Destructor
  ///\cond ABSTRACT
  virtual IItemBase* clone() const = 0;

  virtual size_t getNData() const =0;
  virtual size_t getNReqData() const =0;
  virtual size_t dataCnt() const =0;
  /// Number of components
  virtual size_t getNComp() const { return (flag()) ? 1 : 0; }
  /// Number of data in a given component
  virtual size_t getNCompData(const size_t) const { return getNData(); }
  virtual bool flag() const =0;

  virtual void addNewSection() =0;
  virtual size_t convert(const size_t,const size_t,
		      const std::vector<std::string>&)=0;
  virtual void write(std::ostream&) const =0;
  ///\endcond ABSTRACT

  /// Set Description
  void setDesc(const std::string& D) {Desc=D;}
  /// Get Short key
  const std::string& getKey() const { return Key; }
  /// Get Long key
  const std::string& getLong() const { return Long; }
  /// Get Description
  const std::string& getDesc() const { return Desc; }  
  
};

/*!
  \class IItemFlag
  \version 1.0
  \author S. Ansell
  \date April 20011
  \brief Simple Flag 
*/

class IItemFlag : public IItemBase
{
 private:

  size_t active;          ///< is object active [binary flag]

 public:
 
  explicit IItemFlag(const std::string&);
  IItemFlag(const std::string&,const std::string&);
  IItemFlag(const IItemFlag&);
  IItemFlag& operator=(const IItemFlag&);
  IItemBase* clone() const;
  virtual ~IItemFlag() {}

  /// Access number of data
  virtual size_t getNData() const { return 0; }
  /// Access min data read
  virtual size_t getNReqData() const { return 0; }
  /// Number of data items
  virtual size_t dataCnt() const { return 0; }
  /// access flag state
  virtual bool flag() const { return active; }
  virtual void addNewSection();
  virtual size_t convert(const size_t,const size_t,
			 const std::vector<std::string>&);
  /// Access write string
  virtual void write(std::ostream&) const { return; }
  /// Access to flags
  void setObj() { active=1; }

};

/*!
  \class IItemObj
  \version 1.0
  \author S. Ansell
  \date April 20011
  \brief Store of a set of base object
  \tparam T :: Object Type [required copy+def]
*/

template<typename T>
class IItemObj: public IItemBase
{
 private:

  const size_t N;         ///< Full Number of data items
  const size_t NReq;      ///< Default min
  size_t active;          ///< [binary flag + bin-key on items]
  T* ObjPtr;           ///< Object to store

 public:

  IItemObj(const size_t,const std::string&);
  IItemObj(const size_t,const std::string&,const std::string&);


  IItemObj(const size_t,const size_t,const std::string&);
  IItemObj(const size_t,const size_t,const std::string&,const std::string&);

  IItemObj(const IItemObj<T>&);
  IItemObj& operator=(const IItemObj<T>&);
  IItemBase* clone() const;
  virtual ~IItemObj();

  /// Number of data items
  virtual size_t getNData() const { return N; }
  /// Access min data read
  virtual size_t getNReqData() const { return NReq; }
  virtual size_t dataCnt() const;
  virtual size_t getNCompData(const size_t) const;
  /// flag set/not set
  virtual bool flag() const;

  const T& getObj(const size_t =0) const;

  void setObj(const size_t,const T&);
  void setDefObj(const size_t,const T&);
  void setObj(const T&);

  virtual void addNewSection();
  virtual size_t convert(const size_t,const size_t,
		      const std::vector<std::string>&);
  virtual void write(std::ostream&) const;
};


/*!
  \class IItemMulti
  \version 1.0
  \author S. Ansell
  \date April 20011
  \brief Unknown number of objects
  \tparam T :: Object Type [required copy+def]
*/

template<typename T>
class IItemMulti: public IItemBase
{
 private:

  /// Vector multi array type
  typedef std::vector<std::vector<T*> > VTYPE;
  /// Internal
  typedef std::vector<T*> ITYPE;

  const size_t nRead;                  ///< Number to be read per item
  const size_t nReq;                   ///< Number Req [per read]
  size_t NGrp;                         ///< Number of groups
  size_t active;                       ///< [binary flag + bin-key on items]
  VTYPE ObjVec;                       ///< Object to store

  void deleteVec();

 public:

  IItemMulti(const std::string&,const size_t,const size_t);
  IItemMulti(const std::string&,const std::string&,const size_t,const size_t);

  IItemMulti(const IItemMulti<T>&);
  IItemMulti& operator=(const IItemMulti<T>&);
  IItemBase* clone() const;
  virtual ~IItemMulti();

  /// Number of data items
  virtual size_t getNData() const { return nRead; }
  /// Access min data read
  virtual size_t getNReqData() const { return nReq; }
  virtual size_t getNComp() const { return NGrp; }
  virtual size_t getNCompData(const size_t) const;
  virtual size_t dataCnt() const;
  virtual bool flag() const;

  const T& getObj(const size_t,const size_t) const;
  const T& getObj(const size_t) const;

  void setObj(const size_t,const size_t,const T&);
  void setObj(const T&);

  virtual void addNewSection();
  virtual size_t convert(const size_t,const size_t,
		      const std::vector<std::string>&);
  virtual void write(std::ostream&) const;
};

std::ostream&
operator<<(std::ostream&,const IItemBase&);

}

#endif
 
