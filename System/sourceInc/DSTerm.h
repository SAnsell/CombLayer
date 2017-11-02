/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   include/DSTerm.h
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
#ifndef SDef_DSTerm_h
#define SDef_DSTerm_h

namespace SDef
{

/*!
  \class DSBase
  \version 1.0
  \date August 2010
  \author S.Ansell
  \brief Base class for DS  
*/

class DSBase
{
 protected:

  char option;                           ///< Option letter 
  std::string Key;                       ///< Item key

public:
  
  DSBase();
  DSBase(const std::string&,const char);
  DSBase(const DSBase&);
  virtual DSBase* clone() const =0;   ///< Clone [virtual]
  DSBase& operator=(const DSBase&);
  virtual ~DSBase() {}         ///< Destructor

  /// Access Key
  const std::string& getKey() const { return Key; }

  /// Access option
  char getOption() const { return option; }

  virtual void setType(const std::string&,const char);

  ///\cond TEMPLATE
  virtual void write(const size_t,std::ostream&) const =0;
  ///\endcond TEMPLATE

};


/*!
  \class DSTerm
  \version 1.0
  \date July 2009
  \author S.Ansell
  \brief Comprehensive holder for sdef dx option
  \tparam T :: Storage type (int/char/string/double)
  This is a basic form (d1 d2). Not the FPOS etc
*/

template<typename T>
class DSTerm : public DSBase
{
 private:

  std::vector<T> Values;            ///< dependent values

 public:
  
  DSTerm();
  DSTerm(const std::string&,const char);
  DSTerm(const DSTerm&);
  DSTerm* clone() const;
  DSTerm& operator=(const DSTerm&);
  virtual ~DSTerm();
  
  void setData(const std::vector<T>&);
  void addData(const T&);
  /// Access data
  std::vector<T>& getData() { return Values; }

  /// Specialized versions only
  void write(const size_t,std::ostream&) const;

};

 /*!
   \class DSIndex 
   \author S. Ansell
   \date August 2010
   \brief depended variable index object (ds)
   \version 1.0
  */
class DSIndex : public DSBase
{
 private:

  std::vector<SrcData> PMesh;            ///< probability mesh

 public:
  
  DSIndex();
  DSIndex(const std::string&);
  DSIndex(const DSIndex&);
  DSIndex* clone() const;
  DSIndex& operator=(const DSIndex&);
  virtual ~DSIndex();
  
  void addData(const size_t,const SrcInfo*,const SrcBias*,const SrcProb*);

  void write(const size_t,std::ostream&) const;
};

}

#endif
