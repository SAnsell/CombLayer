/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   t1BuildInc/t1Reflector.h
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#ifndef moderatorSystem_Reflector_h
#define moderatorSystem_Reflector_h

class Simulation;

namespace constructSystem
{
  class LinkWrapper;
  class LWInner;  
  class LWOuter;
}

namespace ts1System
{

  class refPlate;
  class ReflectRods;
  
/*!
  \class t1Reflector
  \version 1.0
  \author S. Ansell
  \date May 2012
  \brief TS1 t1Reflector [insert object]
*/

class t1Reflector : public attachSystem::ContainedComp,
    public attachSystem::FixedComp
{
 private:
  
  const int refIndex;           ///< Index of surface offset
  int cellIndex;                ///< Cell index
  int populated;                ///< 1:var,2:axis,4:cent,8:face,16:cell
  
  double xStep;                 ///< Offset on X to Target
  double yStep;                 ///< Offset on Y to Target [+ve forward]
  double zStep;                 ///< Offset on Z top Target

  double xyAngle;               ///< Angle [degrees]
  double xSize;                 ///< Left/Right size
  double ySize;                 ///< length down target
  double ySizeColdCut;          ///< Cold Mods different thickness  
  double zSize;                 ///< Vertical size
  double cutLen;                ///< Distance cut back (on axis)

  int defMat;                   ///< Default material
  
  /// Boxes in the reflector
  std::vector< boost::shared_ptr<constructSystem::LinkWrapper> > Boxes;
  /// Rods in the reflector
  std::vector< boost::shared_ptr<ReflectRods> > Rods;
  /// Extra plates
  std::vector< boost::shared_ptr<refPlate> > Plates;
  double baseZCut;              ///< Distance down to base cut


  void populate(const Simulation&);
  void createUnitVector(const attachSystem::FixedComp&);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  t1Reflector(const std::string&);
  t1Reflector(const t1Reflector&);
  t1Reflector& operator=(const t1Reflector&);
  virtual ~t1Reflector();

  std::string getComposite(const std::string&) const;
  virtual void addToInsertChain(attachSystem::ContainedComp&) const;
  void createAll(Simulation&,const attachSystem::FixedComp&);

  void createBoxes(Simulation&,const std::string&);
};

}

#endif
 
