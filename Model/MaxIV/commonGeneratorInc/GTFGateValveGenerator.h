/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonGeneratorInc/GTFGateValveGenerator.h
 *
 * Copyright (c) 2004-2023 by Stuart Ansell and Konstantin Batkov
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
#ifndef setVariable_GTFGateValveGenerator_h
#define setVariable_GTFGateValveGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class GTFGateValveGenerator
  \version 1.0
  \author S. Ansell and K. Batkov
  \date Oct 2023
  \brief GTFGateValveGenerator for the GTFGateValve variables
*/

class GTFGateValveGenerator
{
 private:

  double length;                ///< Void length
  double radius;                ///< Radius if round
  double width;                 ///< Void width (full)
  double height;                ///< height
  double depth;                 ///< depth

  double wallThick;             ///< Wall thickness

  double portARadius;            ///< Port inner radius (opening)
  double portAThick;             ///< Port outer ring
  double portALen;               ///< Forward step of port

  double portBRadius;            ///< Port inner radius (opening)
  double portBThick;             ///< Port outer ring
  double portBLen;               ///< Forward step of port

  double bladeLift;             ///< Height of blade up
  double bladeThick;            ///< moving blade thickness
  double bladeRadius;           ///< moving blade radius
  double bladeCutThick;         ///< Thickness of the central part cut
  double bladeScrewHousingRadius; ///< Radius of the central steel cylinder for the screw

  int clampWidth;              ///< Clamp width
  double clampDepth;           ///< Clamp depth
  double clampHeight;          ///< Clamp height
  double clampBulkHeight;       ///< Clamp height before the truncated section
  double clampBaseThick;       ///< Clamp base plate thickness
  double clampBaseWidth;       ///< Clamp base plate width
  double clampTopWidth;        ///< Clamp top truncated part width

  double lsFlangeWidth;         ///< Lifting structure square flange width
  double lsFlangeDepth;         ///< Lifting structure square flange depth
  double lsFlangeHeight;        ///< Lifting structure square flange height
  double lsFlangeHoleRadius;    ///< Lifting structure square flange hole radius
  double lsShaftRadius;         ///< Lifting structure shaft inner radius
  double lsShaftThick;          ///< Lifting structure shaft tube thickness
  double lsShaftLength;         ///< Lifting structure shaft tube total length
  double lsShaftFlangeRadius;   ///< Lifting structure shaft flange outer radius
  double lsShaftFlangeThick;    ///< Lifting structure shaft flange thickness

  double liftWidth;             ///< Lifting width
  double liftHeight;            ///< Height of lifting space

  std::string voidMat;          ///< Void material
  std::string bladeMat;         ///< Void material
  std::string wallMat;          ///< Pipe material
  std::string clampMat;        ///< Clamp material
  std::string lsFlangeMat;              ///< Lifting structure square flange material

 public:

  GTFGateValveGenerator();
  GTFGateValveGenerator(const GTFGateValveGenerator&);
  GTFGateValveGenerator& operator=(const GTFGateValveGenerator&);
  ~GTFGateValveGenerator();

  template<typename CF> void setCylCF();
  template<typename CF> void setCubeCF();
  template<typename CF> void setAPortCF();
  template<typename CF> void setBPortCF();

  template<typename innerCF,typename outerCF> void setPortPairCF();


  /// set wall thickness
  void setWallThick(const double T) { wallThick=T; }
  /// set blade thickness
  void setBladeThick(const double T) { bladeThick=T; }

  void setBladeLift(const double L) { bladeLift=L; }
  void setBladeRadius(const double R) { bladeRadius=R; }

  /// make a radius system
  void setRadius(const double R) { radius=R; }
  /// set total thickness
  void setLength(const double L) { length=L; }
  void setOuter(const double,const double,
		const double,const double);
  void setPort(const double,const double,const double);
  void setAPort(const double,const double,const double);
  void setBPort(const double,const double,const double);

  /// set void material
  void setVoidMat(const std::string& M) { voidMat=M; }
  /// set wall material
  void setWallMat(const std::string& M) { wallMat=M; }
  /// set wall material
  void setBladeMat(const std::string& M) { bladeMat=M; }

  void generateValve(FuncDataBase&,const std::string&,
		     const double,const int) const;

};

}

#endif
