/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/GuideBay.h
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
#ifndef essSystem_GuideBay_h
#define essSystem_GuideBay_h

class Simulation;

namespace essSystem
{
  class GuideItem;

/*!
  \class GuideBay
  \version 1.0
  \author S. Ansell
  \date November 2013
  \brief Group of guide objects sharing a common sector
*/


class GuideBay :
    public attachSystem::FixedRotate,
    public attachSystem::ContainedGroup,
    public attachSystem::ExternalCut,
    public attachSystem::CellMap
{
 private:
  
  const std::string baseKey;    ///< Base key
  const size_t bayNumber;       ///< Bay number [id]
  
  double viewAngle;     ///< Angle of guide
  double innerHeight;   ///< height
  double innerDepth;    ///< Depth 
  double height;        ///< height
  double depth;         ///< Depth 
  double midRadius;     ///< Mid divide
  int mat;              ///< Material
  size_t nItems;        ///< Number of guide items

  double RInner;
  double ROuter;
  
  /// Guide units
  std::vector<std::shared_ptr<GuideItem> > GUnit;
  void calcRadii();
  
  void populate(const FuncDataBase&) override;
  void createSurfaces();
  void createLinks();
  void createObjects(Simulation&);

  
 public:

  GuideBay(const std::string&,const size_t);
  GuideBay(const GuideBay&);
  GuideBay& operator=(const GuideBay&);
  ~GuideBay() override;

  /// accessor to guid units
  const std::vector<std::shared_ptr<GuideItem>>& getGuideItems() const
     {return GUnit;}
  
  void outerMerge(Simulation&,GuideBay&);
  void createGuideItems(Simulation&,const std::string&,const std::string&);

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int) override;

};

}

#endif
 

