/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   delftInc/FuelLoad.h
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
#ifndef delftSystem_FuelLoad_h
#define delftSystem_FuelLoad_h


class FuncDataBase;
namespace XML
{
  class XMLcollect;
}

namespace delftSystem
{

/*!
  \class FuelLoad
  \version 1.0
  \author S. Ansell
  \date April 2014
  \brief Creates fuel after burn for the fuel elements
*/

class FuelLoad  
{
 private:

  std::map<size_t,std::string> FuelMap;         ///< Hash key : fuel number
  XML::XMLcollect* activeXOut;                  ///< Memmory for XML file

  static size_t hash(const size_t,const size_t,
		     const size_t,const size_t);
  static size_t hash(const std::string&,
		     const size_t,const size_t);

  static std::string gridName(const size_t,const size_t);
  void createActive();

 public:

  FuelLoad();
  FuelLoad(const FuelLoad&);
  FuelLoad& operator=(const FuelLoad&);
  virtual ~FuelLoad();

  int getMaterial(const size_t,const size_t,const size_t,
		  const size_t,const int) const;
  const std::string& getMatString(const size_t,const size_t,
				  const size_t,const size_t) const;
  void setMaterial(const size_t,const size_t,
		    const size_t,const size_t,const std::string&);

  void addXML(const size_t,const size_t,const size_t,
	      const size_t, const std::set<size_t>&);
  int loadXML(const std::string&);
  void writeXML(const std::string&);
};

}  

#endif
