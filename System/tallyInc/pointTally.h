/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   tallyInc/pointTally.h
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
#ifndef tallySystem_pointTally_h
#define tallySystem_pointTally_h

namespace tallySystem
{
/*!
  \class pointTally
  \version 1.0
  \date June 2006
  \author S. Ansell
  \brief Hold a point tally
*/
class pointTally : public Tally
{
 private:
  
  Geometry::Vec3D Centre;        ///< Point for point tally
  double Radius;                 ///< Effective radius
  int fuFlag;                    ///< Special active
  NRange fuCard;                 ///< Fu card

  DTriple<int,double,int> ddFlag; ///< ddFlag
  std::vector<Geometry::Vec3D> Window;     ///< Window (if applied)
  int secondDFlag;               ///< Use the second distance number
  double secondDist;             ///< second distance
  bool mcnp6Out;                 ///< MCNP6 output 

  void orderWindow();
  void writeMCNP6(std::ostream&) const;

 public:
  
  explicit pointTally(const int);
  pointTally(const pointTally&);
  virtual pointTally* clone() const; 
  pointTally& operator=(const pointTally&);
  virtual ~pointTally();
  /// ClassName
  virtual std::string className() const 
      { return "pointTally"; }

  /// Set MCNP6 output flag
  void setMCNP6(const bool B) { mcnp6Out=B; }

  void setWindow(const std::vector<Geometry::Vec3D>&);
  void setFUflag(const int);
  void setDDFlag(const double,const int);
  void setSecondDist(const double);
  void setCentre(const Geometry::Vec3D&);
  void setCentre(const double);
  void setCentreAngle(const Geometry::Vec3D&,const double,const double);
  void setCells(const std::vector<int>&);
  virtual int addLine(const std::string&); 
  /// Reqiure Rdum
  int hasRdum() const { return (Window.size()==4); }
  /// back step distance 
  double getSecondDist() const 
  { return (secondDFlag) ? secondDist : 0.0; }
 
  /// Access centre
  const Geometry::Vec3D& getCentre() const { return Centre; } 
  double calcArea() const;
  Geometry::Vec3D calcMid() const;
  Geometry::Vec3D calcNormal() const;
  std::pair<Geometry::Vec3D,Geometry::Vec3D> calcDirection() const;
  std::pair<Geometry::Vec3D,Geometry::Vec3D> calcEdge() const;
  const Geometry::Vec3D& getWindowPt(const size_t) const;
  Geometry::Plane calcPlane() const;
  void setProjectionCentre(const Geometry::Vec3D&,const Geometry::Vec3D&,
				      const double);

  void widenWindow(const int,const double);
  void slideWindow(const int,const double);
  void shiftWindow(const double);
  void divideWindow(const int,const int);
  void removeWindow();

  void printWindow() const;  

  void rotateMaster();
  //  void writeRDUM(std::ostream&) const;
  virtual void write(std::ostream&) const;
};


}  // NAMESPACE tallySystem

#endif
