/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   processInc/DivideGrid.h
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
#ifndef ModelSupport_DivideGrid_h
#define ModelSupport_DivideGrid_h


class FuncDataBase;
namespace XML
{
  class XMLcollect;
}

namespace ModelSupport
{

/*!
  \class DivideGrid
  \version 1.0
  \author S. Ansell
  \date November 2015
  \brief Material descriptor for divided components in 3D
*/

class DivideGrid  
{
 private:

  std::vector<std::string> IJKnames;           ///< names for divider

  std::map<size_t,std::string> MatMap;         ///< Hash key : mat type
  ///< Hash key : CornerPoints
  std::map<size_t,std::vector<Geometry::Vec3D>> PointMap;


  static size_t hash(const size_t,const size_t,
		     const size_t);

 public:

  DivideGrid(const std::string&);
  DivideGrid(const DivideGrid&);
  DivideGrid& operator=(const DivideGrid&);
  virtual ~DivideGrid();

  int getMaterial(const size_t,const size_t,const size_t) const;
  
  const std::string& getMatString(const size_t,const size_t,
				  const size_t,const size_t) const;
  void setMaterial(const size_t,const size_t,
		   const size_t,const std::string&);
  const std::string& getMatString
    (const size_t,const size_t,const size_t) const;

  void setPoints(const size_t,const size_t,
		 const size_t,const std::vector<Geometry::Vec3D>&);
  const std::vector<Geometry::Vec3D>&
    getPoints(const size_t,const size_t,const size_t) const;

  void setKeyNames(const std::string&,const std::string&,
		   const std::string&);


  int loadXML(const std::string&,const std::string&);
  
  void writeXML(const std::string&,const std::string&,
		const size_t,const size_t,const size_t) const ;
};

}  

#endif
