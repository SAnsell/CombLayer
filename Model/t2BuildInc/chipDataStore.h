/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   buildInc/chipDataStore.h
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
#ifndef chipIRDatum_h
#define chipIRDatum_h

class Simulation;

namespace chipIRDatum
{
  /// Unit enumeration
  enum Units
  {
    none=0,mm=1,cm=2
  };

  /// Enumeration of C points
  enum CPENUM 
  {
    targetCentre=0,voidCentre=1,chipStartPt=2,
    shutterAxis=3,tubeAxis=4,secScatImpact=5,
    secScatCentre=6,shinePt1=7,shinePt2=8,shinePt3=9,shinePt4=10,
    shinePt5=11,shinePt6=12,shinePt7=13,shinePt8=14,
    collVOrigin=15,collVAxis=16,collHOrigin=17,collHAxis=18,
    preCollOrigin=19,preCollAxis=20,
    preCollHoleCent,insertStart,insertEnd,insertAxis,
    guidePt1,guidePt2,guidePt3,guidePt4,
    guidePt5,guidePt6,guidePt7,guidePt8,
    filterPos,blockStart,collimator,
    table1Cent,table2Cent,beamStop,
    CTotal
  };
  /// Enumeration of D points
  enum DPENUM 
  {
    shutterPt1=0,shutterPt2=1,shutterPt3=2,shutterPt4=3,
    shutterPt5=4,shutterPt6=5,shutterPt7=6,shutterPt8=7,
    shutterMidPtA=8,shutterMidPtB=9,tab1Inter=10,tab2Inter=11,
    guideExit=12,hutRApex=13,floodC,flood1,flood2,flood3,flood4,
    DTotal
  };
  /// Enumeration of E points
  enum EPENUM 
  {
    zoomBendShutter=0,zoomBendStart=1,
    zoomBendCentre=2,zoomBendExit=3,
    zoomBendStartNorm=4,zoomBendExitNorm=5,
    zoomBoxExit=6,zoomGuideExit=7,zoomShutterExit=8,
    ETotal
  };
  /// Enumeration of M points [Moderator]
  enum MPENUM 
  {
    grooveCentre=0,
    MTotal
  };
  /// Enumeration of V values
  enum VPENUM
  {
    torpedoRing=0,shutterRing=1,innerRing=2,outerRing=3,
    zoomBendRadius=4,zoomBendLength=5,insertRadius=6,
    VTotal
  };

/*!
  \class chipDataStore
  \version 1.0
  \author S. Ansell
  \date September 2009
  \brief Sets the Datum points for ChipIR
*/

class chipDataStore
{
 private:

  Units U;                               ///< Units for output
  
  std::vector<int> CUnits;               ///< Set ChipIR Transport References
  std::vector<int> DUnits;               ///< Set ChipIR Hutch References
  std::vector<int> EUnits;               ///< Set Zoom References
  std::vector<int> MUnits;               ///< Set Moderator References
  std::vector<int> VUnits;               ///< Set Value References

  std::vector<int> Cflag;               ///< Set ChipIR Transport References
  std::vector<int> Dflag;               ///< Set ChipIR Hutch References
  std::vector<int> Eflag;               ///< Set Zoom References
  std::vector<int> Mflag;               ///< Set Moderator References
  std::vector<int> Vflag;               ///< Set Value References

  std::vector<Geometry::Vec3D> CRef;   ///< Transport reference Values
  std::vector<Geometry::Vec3D> DRef;   ///< Hutch reference Values
  std::vector<Geometry::Vec3D> ERef;   ///< Zoom reference Values
  std::vector<Geometry::Vec3D> MRef;   ///< Moderator reference Values
  std::vector<double> VRef;            ///< Reference values

  std::vector<std::string> CName;      ///< ChipIR Transport Values
  std::vector<std::string> DName;      ///< ChipIR Hutch Values
  std::vector<std::string> EName;      ///< Zoom Values
  std::vector<std::string> MName;      ///< Zoom Values
  std::vector<std::string> VName;      ///< Reference Values
  
  chipDataStore();
  ///\cond SINGLETON
  chipDataStore(const chipDataStore&);
  chipDataStore& operator=(const chipDataStore&);
  ///\endcond SINGLETON

  void postProcess();

 public:

  static chipDataStore& Instance();

  /// Destructor
  ~chipDataStore() {}

  /// Set the units
  void setUnits(const Units ux) { U=ux; }

  void setCNum(const CPENUM,const Geometry::Vec3D&);
  void setDNum(const DPENUM,const Geometry::Vec3D&);
  void setENum(const EPENUM,const Geometry::Vec3D&);
  void setMNum(const MPENUM,const Geometry::Vec3D&);
  void setVNum(const VPENUM,const double&);

  void setCNum(const size_t,const Geometry::Vec3D&);
  void setDNum(const size_t,const Geometry::Vec3D&);
  void setENum(const size_t,const Geometry::Vec3D&);
  void setMNum(const size_t,const Geometry::Vec3D&);
  void setVNum(const size_t,const double&);

  Geometry::Vec3D getCValue(const size_t) const;
  Geometry::Vec3D getDValue(const size_t) const;
  Geometry::Vec3D getEValue(const size_t) const;
  Geometry::Vec3D getMValue(const size_t) const;


  void writeMasterTable(const std::string&);

};

}

#endif
 
