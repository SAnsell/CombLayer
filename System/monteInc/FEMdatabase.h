/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   monteInc/FEMdatabase.h
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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
#ifndef ModelSupport_FEMdatabase_h
#define ModelSupport_FEMdatabase_h

struct FEMinfo
{
  double RhoCp;     ///< heat capacity * density
  double K;     ///< conductivity
};


namespace ModelSupport
{
  
/*!
  \class FEMdatabase 
  \version 1.0
  \author S. Ansell
  \date May 2023
  \brief Storage for finite element heat info
*/

class FEMdatabase
{  
 private:
  
  /// String to id-number
  typedef std::map<std::string,int> SCTYPE;
  /// Storage type for Materials (based on id number)
  typedef std::map<int,FEMinfo> MTYPE;
  
  SCTYPE IndexMap;   ///< Map of indexes
  MTYPE MStore;      ///< Store of materials

  FEMdatabase();


  FEMdatabase(const FEMdatabase&) =delete;
  FEMdatabase(const FEMdatabase&&) =delete;
  FEMdatabase& operator=(const FEMdatabase&) =delete;
  FEMdatabase& operator=(const FEMdatabase&&) =delete;

  void initMaterial();
  void checkNameIndex(const int,const std::string&) const;
  
 public:

  static FEMdatabase& Instance();
  
  ~FEMdatabase() {}  ///< Destructor
  
  /// get data store
  const MTYPE& getStore() const { return MStore; }

  void addMaterial(const std::string&,const double,const double);
  
  bool hasKey(const std::string&) const;
  bool hasKey(const int) const;
  int getIndex(const std::string&) const;
  const std::string& getKey(const int) const;

  double getK(const int) const;
  double getK(const std::string&) const;
  double getRhoCp(const int) const;
  double getRhoCp(const std::string&) const;
};

}

#endif
