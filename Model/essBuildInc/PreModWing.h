/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/PreModWing.h
 *
 * Copyright (c) 2015-2017 by Konstantin Batkov/Stuart Ansell
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
#ifndef essSystem_PreModWing_h
#define essSystem_PreModWing_h

class Simulation;

namespace essSystem
{

/*!
  \class PreModWing
  \author S. Ansell / K. Batkov
  \version 2.0
  \date August 2016
  \brief Premoderator wing :fills space below pre-mod and H2 start.
*/

class PreModWing : public attachSystem::ContainedComp,
  public attachSystem::FixedOffset,
  public attachSystem::CellMap
{
 private:
  
  const int modIndex;             ///< Index of surface offset
  int cellIndex;                  ///< Cell index

  double innerHeight;             ///< Mid step [up]
  double outerHeight;             ///< outer step [up]
  double innerDepth;              ///< Mid set [down]
  double outerDepth;              ///< Outer step [down]
  double wallThick;               ///< wall thickness

  double innerRadius;            ///< Start of down curve [-ve if not used]
  double outerRadius;            ///< End of down curve [-ve if not used]
  double innerYCut;              ///< Start from inner shape [makeing overstep]
  
  int mat;                        ///< (water) material
  int wallMat;                    ///< wall material

  size_t nLayers;                 ///< Number of layers
  std::vector<double> layerRadii; ///< Radii for change in material
  std::vector<int> innerMat;      ///< Inner materials [water]
  std::vector<int> surfMat;       ///< Surface material [al]
  
  HeadRule topSurf;              ///< Top cut surface
  HeadRule baseSurf;             ///< Base cut surface
  HeadRule innerSurf;            ///< Inner surface(s)
  HeadRule midSurf;              ///< Extended Inner surface(s)
  HeadRule outerSurf;            ///< Outer surface(s)
  HeadRule mainDivider;          ///< Seperatue unit for divider [to simpify boundary]

  std::string getLayerZone(const size_t) const;
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  
 public:

  PreModWing(const std::string&);
  PreModWing(const PreModWing&);
  PreModWing& operator=(const PreModWing&);
  virtual PreModWing* clone() const;
  virtual ~PreModWing();

  /// assignment of main string
  void setInnerExclude(const std::string& HRStr)
  { innerSurf.procString(HRStr); }
  /// assignment of oute radial divider
  void setMidExclude(const std::string& HRStr)
  { midSurf.procString(HRStr); }
  /// assignment of base rule
  void setBaseCut(const HeadRule& HR) { baseSurf=HR; }
  /// assignement of top rule
  void setTopCut(const HeadRule& HR) { topSurf=HR; }
  /// assignment of divider rule
  void setDivider(const HeadRule& HR) { mainDivider=HR; }
  /// assignment of outer surf
  void setOuter(const HeadRule& HR) { outerSurf=HR; }

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
