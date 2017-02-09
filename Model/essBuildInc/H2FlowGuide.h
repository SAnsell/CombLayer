/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essBuildInc/H2FlowGuide.h
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#ifndef essSystem_H2FlowGuide_h
#define essSystem_H2FlowGuide_h

class Simulation;


namespace essSystem
{

/*!
  \class H2FlowGuide
  \version 2.0
  \author S. Ansell / Konstantin Batkov
  \date Feb 2017
  \brief H2FlowGuide component in the butterfly moderator
*/

class H2FlowGuide :
  public attachSystem::FixedComp
{
 private:

  const std::string baseName; ///< Base name
  const std::string midName; ///< Mid Name
  const std::string endName; ///< End Name
  const int flowIndex;       ///< Index of surface offset
  int cellIndex;             ///< Cell index

  double wallThick;            ///< Wal thickness
  double baseLen;              ///< Base length
  Geometry::Vec3D baseOffset;  ///< Base offset
  double angle; ///< sq xy angle
  double sqOffsetY; ///< sq offset along the y-axis
  double sqSideA; ///< parameter A of the side SQ surfaces
  double sqSideE; ///< parameter E of the side SQ surfaces
  double sqSideF; ///< parameter F of the side SQ surfaces
  double sqCenterA; ///< parameter A of the central SQ surfaces
  double sqCenterE; ///< parameter E of the central SQ surfaces
  double sqCenterF; ///< parameter F of the central SQ surfaces

  int wallMat;                   ///< inner material
  double wallTemp;               ///< LH2 temperature [K]

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&);

  std::string getSQSurface(const double&,const double&,const double&,const double&);

  void createSurfaces();
  void createObjects(Simulation&,const attachSystem::FixedComp&);

 public:

  H2FlowGuide(const std::string&,const std::string&,const std::string&);
  H2FlowGuide(const H2FlowGuide&);
  H2FlowGuide& operator=(const H2FlowGuide&);
  virtual H2FlowGuide* clone() const;
  virtual ~H2FlowGuide();

  void createAll(Simulation&,const attachSystem::FixedComp&);
};

}

#endif

