set (geometrySources
    ArbPoly.cxx BasicMesh3D.cxx Circle.cxx 
    Cone.cxx Convex2D.cxx Convex.cxx 
    CylCan.cxx Cylinder.cxx DblLine.cxx 
    Edge.cxx Ellipse.cxx Ellipsoid.cxx 
    EllipticCyl.cxx Face.cxx General.cxx 
    geomSupport.cxx Intersect.cxx Line.cxx 
    localRotate.cxx M2.cxx MBrect.cxx Mesh3D.cxx 
    MeshGrid.cxx NullSurface.cxx Plane.cxx 
    Pnt.cxx Quadratic.cxx Quadrilateral.cxx 
    Quaternion.cxx RecTriangle.cxx SglLine.cxx 
    Sphere.cxx Surface.cxx surfaceFactory.cxx
    surfImplicates.cxx surfIndex.cxx surfProg.cxx 
    SurInter.cxx Torus.cxx Transform.cxx 
    Triangle.cxx Vec2D.cxx Vec3D.cxx 
    Vert2D.cxx Vertex.cxx 
)

add_library (geometry SHARED
  ${geometrySources}
)
target_include_directories (geometry PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../geomInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/ArbPoly.cxx
  ${tarDIR}/BasicMesh3D.cxx
  ${tarDIR}/Circle.cxx
  ${tarDIR}/Cone.cxx
  ${tarDIR}/Convex2D.cxx
  ${tarDIR}/Convex.cxx
  ${tarDIR}/CylCan.cxx
  ${tarDIR}/Cylinder.cxx
  ${tarDIR}/DblLine.cxx
  ${tarDIR}/Edge.cxx
  ${tarDIR}/Ellipse.cxx
  ${tarDIR}/Ellipsoid.cxx
  ${tarDIR}/EllipticCyl.cxx
  ${tarDIR}/Face.cxx
  ${tarDIR}/General.cxx
  ${tarDIR}/geomSupport.cxx
  ${tarDIR}/Intersect.cxx
  ${tarDIR}/Line.cxx
  ${tarDIR}/localRotate.cxx
  ${tarDIR}/MBrect.cxx
  ${tarDIR}/Mesh3D.cxx
  ${tarDIR}/MeshGrid.cxx
  ${tarDIR}/NullSurface.cxx
  ${tarDIR}/Plane.cxx
  ${tarDIR}/Pnt.cxx
  ${tarDIR}/Quadratic.cxx
  ${tarDIR}/Quadrilateral.cxx
  ${tarDIR}/Quaternion.cxx
  ${tarDIR}/RecTriangle.cxx
  ${tarDIR}/SglLine.cxx
  ${tarDIR}/Sphere.cxx
  ${tarDIR}/Surface.cxx
  ${tarDIR}/surfaceFactory.cxx
  ${tarDIR}/surfImplicates.cxx
  ${tarDIR}/surfIndex.cxx
  ${tarDIR}/surfProg.cxx
  ${tarDIR}/SurInter.cxx
  ${tarDIR}/Torus.cxx
  ${tarDIR}/Transform.cxx
  ${tarDIR}/Triangle.cxx
  ${tarDIR}/Vec2D.cxx
  ${tarDIR}/Vec3D.cxx
  ${tarDIR}/Vert2D.cxx
  ${tarDIR}/Vertex.cxx
  ${tarINC}/ArbPoly.h
  ${tarINC}/BasicMesh3D.h
  ${tarINC}/Circle.h
  ${tarINC}/Cone.h
  ${tarINC}/Convex2D.h
  ${tarINC}/Convex.h
  ${tarINC}/CylCan.h
  ${tarINC}/Cylinder.h
  ${tarINC}/DblLine.h
  ${tarINC}/Edge.h
  ${tarINC}/Ellipse.h
  ${tarINC}/Ellipsoid.h
  ${tarINC}/EllipticCyl.h
  ${tarINC}/Face.h
  ${tarINC}/General.h
  ${tarINC}/geomSupport.h
  ${tarINC}/Intersect.h
  ${tarINC}/Line.h
  ${tarINC}/localRotate.h
  ${tarINC}/MBrect.h
  ${tarINC}/Mesh3D.h
  ${tarINC}/MeshGrid.h
  ${tarINC}/NullSurface.h
  ${tarINC}/Plane.h
  ${tarINC}/Pnt.h
  ${tarINC}/Quadratic.h
  ${tarINC}/Quadrilateral.h
  ${tarINC}/Quaternion.h
  ${tarINC}/RecTriangle.h
  ${tarINC}/SglLine.h
  ${tarINC}/Sphere.h
  ${tarINC}/surfaceFactory.h
  ${tarINC}/Surface.h
  ${tarINC}/surfImplicates.h
  ${tarINC}/surfIndex.h
  ${tarINC}/surfProg.h
  ${tarINC}/SurInter.h
  ${tarINC}/Torus.h
  ${tarINC}/Transform.h
  ${tarINC}/Triangle.h
  ${tarINC}/Vec2D.h
  ${tarINC}/Vec3D.h
  ${tarINC}/vecOrder.h
  ${tarINC}/Vert2D.h
  ${tarINC}/Vertex.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

