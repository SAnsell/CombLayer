/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essBuildInc/H2FlowGuide.h
 *
 * Copyright (c) 2004-2017 by Konstantin Batkov
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
  \author Konstantin Batkov
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
  double baseOffset;  ///< Base offset
  double len1L; ///< Left length of 1st blade
  double len1R; ///< Right length of 1st blade
  double angle1; ///< Opening angle of the 1st blade
  double radius1; ///< Curvature radius of the 1st segment
  double len1Foot; ///< Length of the foot segment of the 1st blade
  double dist12; ///< Distance between 1st and 2nd blades
  double len2L; ///< Left length of 2nd blade
  double len2R; ///< Right length of 2nd blade
  double angle2; ///< Opening angle of the 2nd blade
  double len2Foot; ///< Length of the foot segment of the 2nd blade
  double radius2; ///< Curvature radius of the 2nd blade
  double dist23; ///< Distance betwen 2nd and 3rd blades
  double len3L; ///< Left length of 3rd blade
  double len3R; ///< Right length of 3rd blade
  double angle3; ///< Inclination of 3rd blade
  double dist14; ///< parameter F of the central SQ surfaces
  double len4L; ///< Left length of 4th blade
  double len4R; ///< Right length of 4th blade
  double angle4; ///< Inclination of 4rd blade

  int wallMat;                   ///< inner material
  double wallTemp;               ///< LH2 temperature [K]

  Geometry::Vec3D midNorm(const Geometry::Vec3D&,const Geometry::Vec3D&,
			  const Geometry::Vec3D&) const;
  void createCurvedBladeSurf(const int,const double&,const double&,
			     const double&,const double&,const double&,
			     const double&,const double&,const int&);

  void createStraightBladeSurf(const int,const double&,const double&,
			       const double&,const double&);

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&);

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

