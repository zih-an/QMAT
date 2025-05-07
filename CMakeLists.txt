cmake_minimum_required(VERSION 3.16)
project(QMAT)

set(CMAKE_CXX_STANDARD 14)

#if(APPLE)
#    if(CMAKE_C_COMPILER_ID MATCHES "Clang")
#        set(OpenMP_C "${CMAKE_C_COMPILER}" CACHE STRING "" FORCE)
#        set(OpenMP_C_FLAGS "-fopenmp=libomp -Wno-unused-command-line-argument" CACHE STRING "" FORCE)
#        set(OpenMP_C_LIB_NAMES "libomp" "libgomp" "libiomp5" CACHE STRING "" FORCE)
#        set(OpenMP_libomp_LIBRARY ${OpenMP_C_LIB_NAMES} CACHE STRING "" FORCE)
#        set(OpenMP_libgomp_LIBRARY ${OpenMP_C_LIB_NAMES} CACHE STRING "" FORCE)
#        set(OpenMP_libiomp5_LIBRARY ${OpenMP_C_LIB_NAMES} CACHE STRING "" FORCE)
#    endif()
#    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
#        set(OpenMP_CXX "${CMAKE_CXX_COMPILER}" CACHE STRING "" FORCE)
#        set(OpenMP_CXX_FLAGS "-fopenmp=libomp -Wno-unused-command-line-argument" CACHE STRING "" FORCE)
#        set(OpenMP_CXX_LIB_NAMES "libomp" "libgomp" "libiomp5" CACHE STRING "" FORCE)
#        set(OpenMP_libomp_LIBRARY ${OpenMP_CXX_LIB_NAMES} CACHE STRING "" FORCE)
#        set(OpenMP_libgomp_LIBRARY ${OpenMP_CXX_LIB_NAMES} CACHE STRING "" FORCE)
#        set(OpenMP_libiomp5_LIBRARY ${OpenMP_CXX_LIB_NAMES} CACHE STRING "" FORCE)
#    endif()
#
#    find_package(OpenMP REQUIRED)
#    if (OPENMP_FOUND)
#        set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
#        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
#    endif()
if(APPLE)
    # Make sure to specify correct OpenMP flags and library paths on macOS
    set(OpenMP_C_FLAGS "-Xpreprocessor -fopenmp -I/opt/homebrew/opt/libomp/include")
    set(OpenMP_C_LIB_NAMES "omp")
    set(OpenMP_omp_LIBRARY "/opt/homebrew/opt/libomp/lib/libomp.dylib")
    set(OpenMP_CXX_FLAGS "${OpenMP_C_FLAGS}")
    set(OpenMP_CXX_LIB_NAMES "${OpenMP_C_LIB_NAMES}")
    set(OpenMP_CXX_LIBRARIES "${OpenMP_omp_LIBRARY}")
    find_package(OpenMP REQUIRED)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
elseif(WIN32)
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-O3")
endif()

include_directories(include)


add_executable(QMAT
#        main.cpp
        src/SlabMesh.h
        src/SlabMesh.cpp
        src/Wm4Math.cpp
        src/Wm4Math.h
        src/Wm4Matrix.cpp
        src/Wm4Matrix.h
        src/Wm4Vector.cpp
        src/Wm4Vector.h
        src/PrimMesh.cpp
        src/PrimMesh.h
        src/Mesh.cpp
        src/Mesh.h
        src/GeometryObjects.cpp
        src/GeometryObjects.h
        src/NonManifoldMesh.cpp
        src/NonManifoldMesh.h
        main_main.cpp
)

target_link_libraries(QMAT OpenMP::OpenMP_CXX)
#find_package(CGAL REQUIRED)
#find_package(Eigen3 CONFIG REQUIRED)
#target_link_libraries(QMAT CGAL::CGAL Eigen3::Eigen OpenMP::OpenMP_CXX)