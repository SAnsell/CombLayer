/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   saxsModelInc/makeSAXS.h
*
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#ifndef saxsSystem_makeSAXS_h
#define saxsSystem_makeSAXS_h

namespace insertSystem
{
  class insertPlate;
  class insertCylinder;
}

namespace instrumentSystem
{
  class CylSample;
}

namespace d4cSystem
{
  class BellJar;
  class DetectorArray;
  class DetectorBank;
}

namespace saxsSystem
{
  class AreaPlate;
  class Capillary;
  /*!
    \class makeSAXS
    \version 1.0
    \author S. Ansell
    \date October 2012
    \brief main builder for saxs model
  */

class makeSAXS
{
 private:

  /// Outer bell jar
  std::shared_ptr<d4cSystem::BellJar> BellObj;

  /// Capillary
  std::shared_ptr<Capillary> sampleObj;
  /// Capillary
  std::shared_ptr<Capillary> waterObj;
  /// Capillary
  std::shared_ptr<Capillary> energyObj;

  /// Area detector plate
  std::shared_ptr<AreaPlate> areaPlate;

  /// beam stop
  std::shared_ptr<insertSystem::insertCylinder> eCutDisk;  

 public:
  
  makeSAXS();
  makeSAXS(const makeSAXS&);
  makeSAXS& operator=(const makeSAXS&);
  ~makeSAXS();
  
  void build(Simulation&,const mainSystem::inputParam&);
  
};

}

#endif
