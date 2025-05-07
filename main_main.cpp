//
// Created by 赵子涵 on 25-5-4.
//
#include <iostream>

#include "src/NonManifoldMesh.h"
#include "src/SlabMesh.h"
#include <CLI11.hpp>

void LoadInputNMM(Mesh* input, SlabMesh* slabMesh, std::string maname) {
  std::ifstream mastream(maname.c_str());
  NonManifoldMesh newinputnmm;
  newinputnmm.numVertices = 0;
  newinputnmm.numEdges = 0;
  newinputnmm.numFaces = 0;
  int nv, ne, nf;
  mastream >> nv >> ne >> nf;

  // slab mesh
  slabMesh->numVertices = 0;
  slabMesh->numEdges = 0;
  slabMesh->numFaces = 0;
  slabMesh->bound_weight = 0.1;

  double len[4];
  len[0] = input->m_max[0] - input->m_min[0];
  len[1] = input->m_max[1] - input->m_min[1];
  len[2] = input->m_max[2] - input->m_min[2];
  len[3] = sqrt(len[0] * len[0] + len[1] * len[1] + len[2] * len[2]);
  newinputnmm.diameter = len[3];

  for (unsigned i = 0; i < input->pVertexList.size(); i++)
    newinputnmm.BoundaryPoints.push_back(SamplePoint(
        input->pVertexList[i]->point()[0], input->pVertexList[i]->point()[1],
        input->pVertexList[i]->point()[2]));

  for (unsigned i = 0; i < nv; i++) {
    char ch;
    double x, y, z, r;
    mastream >> ch >> x >> y >> z >> r;

    // handle the slab mesh
    Bool_SlabVertexPointer bsvp2;
    bsvp2.first = true;
    bsvp2.second = new SlabVertex;
    (*bsvp2.second).sphere.center[0] = x / input->bb_diagonal_length;
    (*bsvp2.second).sphere.center[1] = y / input->bb_diagonal_length;
    (*bsvp2.second).sphere.center[2] = z / input->bb_diagonal_length;
    (*bsvp2.second).sphere.radius = r / input->bb_diagonal_length;
    (*bsvp2.second).index = slabMesh->vertices.size();
    slabMesh->vertices.push_back(bsvp2);
    slabMesh->numVertices++;
  }

  for (unsigned i = 0; i < ne; i++) {
    char ch;
    unsigned ver[2];
    mastream >> ch;
    mastream >> ver[0];
    mastream >> ver[1];

    // handle the slab mesh
    Bool_SlabEdgePointer bsep2;
    bsep2.first = true;
    bsep2.second = new SlabEdge;
    (*bsep2.second).vertices_.first = ver[0];
    (*bsep2.second).vertices_.second = ver[1];
    (*slabMesh->vertices[(*bsep2.second).vertices_.first].second)
        .edges_.insert(slabMesh->edges.size());
    (*slabMesh->vertices[(*bsep2.second).vertices_.second].second)
        .edges_.insert(slabMesh->edges.size());
    (*bsep2.second).index = slabMesh->edges.size();
    slabMesh->edges.push_back(bsep2);
    slabMesh->numEdges++;
  }

  for (unsigned i = 0; i < nf; i++) {
    char ch;
    unsigned vid[3];
    unsigned eid[3];
    mastream >> ch >> vid[0] >> vid[1] >> vid[2];

    // handle the slab mesh
    Bool_SlabFacePointer bsfp2;
    bsfp2.first = true;
    bsfp2.second = new SlabFace;
    (*bsfp2.second).vertices_.insert(vid[0]);
    (*bsfp2.second).vertices_.insert(vid[1]);
    (*bsfp2.second).vertices_.insert(vid[2]);
    if (slabMesh->Edge(vid[0], vid[1], eid[0]))
      (*bsfp2.second).edges_.insert(eid[0]);
    if (slabMesh->Edge(vid[0], vid[2], eid[1]))
      (*bsfp2.second).edges_.insert(eid[1]);
    if (slabMesh->Edge(vid[1], vid[2], eid[2]))
      (*bsfp2.second).edges_.insert(eid[2]);
    (*bsfp2.second).index = slabMesh->faces.size();
    slabMesh->vertices[vid[0]].second->faces_.insert(slabMesh->faces.size());
    // slab_mesh.vertices[vid[0]].second->related_face += 2;
    slabMesh->vertices[vid[1]].second->faces_.insert(slabMesh->faces.size());
    // slab_mesh.vertices[vid[1]].second->related_face += 2;
    slabMesh->vertices[vid[2]].second->faces_.insert(slabMesh->faces.size());
    // slab_mesh.vertices[vid[2]].second->related_face += 2;
    slabMesh->edges[eid[0]].second->faces_.insert(slabMesh->faces.size());
    slabMesh->edges[eid[1]].second->faces_.insert(slabMesh->faces.size());
    slabMesh->edges[eid[2]].second->faces_.insert(slabMesh->faces.size());
    slabMesh->faces.push_back(bsfp2);
    slabMesh->numFaces++;
  }

  // newinputnmm.ComputeFacesNormal();
  // newinputnmm.ComputeFacesCentroid();
  // newinputnmm.ComputeFacesSimpleTriangles();
  // newinputnmm.ComputeEdgesCone();
  // input_nmm = newinputnmm;

  slabMesh->iniNumVertices = slabMesh->numVertices;
  slabMesh->iniNumEdges = slabMesh->numEdges;
  slabMesh->iniNumFaces = slabMesh->numFaces;

  slabMesh->CleanIsolatedVertices();
  slabMesh->computebb();
  slabMesh->ComputeFacesCentroid();
  slabMesh->ComputeFacesNormal();
  slabMesh->ComputeVerticesNormal();
  slabMesh->ComputeEdgesCone();
  slabMesh->ComputeFacesSimpleTriangles();
  slabMesh->DistinguishVertexType();
}

bool importMA(Mesh* input, SlabMesh* slabMesh, std::string maname) {
  // std::string filename = filename + ".ma";
  // if (!std::filesystem::exists(filename)) {
  //     std::cerr << "Related .ma file is missing." << std::endl;
  //     return false;
  // }

  std::cout << "Loading ma file " << maname << std::endl;
  // bool success = false;

  // m_pThreeDimensionalShape->input_nmm.meshname = filename;
  // m_pThreeDimensionalShape->input_nmm.domain =
  // m_pThreeDimensionalShape->input->domain;
  // m_pThreeDimensionalShape->input_nmm.pmesh =
  // &(m_pThreeDimensionalShape->input);
  // m_pThreeDimensionalShape->slab_mesh.pmesh =
  // &(m_pThreeDimensionalShape->input);
  slabMesh->type = 1;
  slabMesh->bound_weight = 1.0;
  // m_pThreeDimensionalShape->slab_mesh.type = 1;
  // m_pThreeDimensionalShape->slab_mesh.bound_weight = 1.0;
  // m_pThreeDimensionalShape->LoadInputNMM(filename);
  LoadInputNMM(input, slabMesh, maname);
  std::cout << "import MA done." << std::endl;
  // success = true;

  // if (success) {
  //     m_pGLWidget->set3DShape(m_pThreeDimensionalShape);
  // }

  return true;
}

//zihan
bool ComputeInputNMM(Mesh* input, SlabMesh* slab_mesh) {
    cout << "here! compute input NMM" << endl;
    NonManifoldMesh input_nmm;
    input_nmm.numVertices = 0;
    input_nmm.numEdges = 0;
    input_nmm.numFaces = 0;

    //
    input_nmm.vertices.clear();
    input_nmm.edges.clear();
    input_nmm.faces.clear();

    Triangulation * pt = &(input->dt);

    //loading
    slab_mesh->numVertices = 0;
    slab_mesh->numEdges = 0;
    slab_mesh->numFaces = 0;
    slab_mesh->type = 1;
    slab_mesh->bound_weight = 0.1; //1.0


    unsigned num_vor_v, num_vor_e, num_vor_f;
    num_vor_v = 0;
    num_vor_e = 0;
    num_vor_f = 0;

    double len[4];
    len[0] = input->m_max[0] - input->m_min[0];
    len[1] = input->m_max[1] - input->m_min[1];
    len[2] = input->m_max[2] - input->m_min[2];
    len[3] = sqrt(len[0]*len[0]+len[1]*len[1]+len[2]*len[2]);
    input_nmm.diameter = len[3];
    int mysample_counter = 0;
    for (Finite_vertices_iterator_t fvi = pt->finite_vertices_begin(); fvi != pt->finite_vertices_end(); fvi++) {
	    input_nmm.BoundaryPoints.push_back(SamplePoint(
			fvi->point()[0], fvi->point()[1], fvi->point()[2]));
    }
    cout << "boundary size: "<< input_nmm.BoundaryPoints.size() << endl;

    // Here is the key...


    int mas_vertex_count(0);
    //
    // slab_mesh->maxhausdorff_distance = 0;
    for(Finite_cells_iterator_t fci = pt->finite_cells_begin(); fci != pt->finite_cells_end(); fci ++)
    {
      if(fci->info().inside == false)
      {
        fci->info().tag = -1;
        continue;
      }
      fci->info().tag = mas_vertex_count ++;


      Bool_VertexPointer bvp;
      bvp.first = true;
      bvp.second = new NonManifoldMesh_Vertex;
      (*bvp.second).sphere.center = to_wm4(CGAL::circumcenter(pt->tetrahedron(fci)));
      (*bvp.second).is_pole = fci->info().is_pole;
      for(unsigned k = 0; k < 4; k ++)
        (*bvp.second).bplist.insert(fci->vertex(k)->info().id);
      (*bvp.second).sphere.radius = pt->TetCircumRadius(pt->tetrahedron(fci));
      //(*bvp.second).sphere.radius = fci->info().dist_center_to_boundary; // make sure that all the spheres are inside the domain
      //bvp.second->sphere.center = bvp.second->sphere.center;
      //bvp.second->sphere.radius = bvp.second->sphere.radius;
      input_nmm.vertices.push_back(bvp);
      input_nmm.numVertices ++;
      num_vor_v ++;



      // ����slab vertices
      Bool_SlabVertexPointer bsvp2;
      bsvp2.first = true;
      bsvp2.second = new SlabVertex;
      (*bsvp2.second).sphere.center[0] = (*bvp.second).sphere.center.X() / input->bb_diagonal_length;
      (*bsvp2.second).sphere.center[1] = (*bvp.second).sphere.center.Y() / input->bb_diagonal_length;
      (*bsvp2.second).sphere.center[2] = (*bvp.second).sphere.center.Z() / input->bb_diagonal_length;
      (*bsvp2.second).sphere.radius = (*bvp.second).sphere.radius / input->bb_diagonal_length;
      //(*bsvp2.second).sphere.radius = r * 1.2;
      (*bsvp2.second).index = slab_mesh->vertices.size();
      // (*bsvp2.second).bplist = (*bvp.second).bplist;
      slab_mesh->vertices.push_back(bsvp2);
      slab_mesh->numVertices ++;

      // double min_dis = DBL_MAX;
      // for (set<unsigned>::iterator si = (*bvp.second).bplist.begin(); si != (*bvp.second).bplist.end(); si++)
      // {
      // 	Vector3d bou_ver(input->pVertexList[*si]->point()[0], input->pVertexList[*si]->point()[1], input->pVertexList[*si]->point()[2]);
      // 	Sphere ma_ver = bsvp2.second->sphere;
      // 	double temp_length = abs((bou_ver - ma_ver.center).Length() - ma_ver.radius);
      // 	min_dis = min(min_dis, temp_length);
      // }
      // slab_mesh->maxhausdorff_distance = max(slab_mesh->maxhausdorff_distance, min_dis);
    }
    // slab_mesh->initialhausdorff_distance = slab_mesh->maxhausdorff_distance;


    for(Finite_facets_iterator_t ffi = pt->finite_facets_begin(); ffi != pt->finite_facets_end(); ffi ++)
    {
      Triangulation::Object o = pt->dual(*ffi);
      if(const Triangulation::Segment *s = CGAL::object_cast<Triangulation::Segment>(&o))
      {
        if( (ffi->first->info().inside == false) || (pt->mirror_facet(*ffi).first->info().inside == false) )
          continue;
        Bool_EdgePointer bep;
        bep.first = true;
        bep.second = new NonManifoldMesh_Edge;
        (*bep.second).vertices_.first = ffi->first->info().tag;
        (*bep.second).vertices_.second = pt->mirror_facet(*ffi).first->info().tag;
        (*input_nmm.vertices[ffi->first->info().tag].second).edges_.insert(input_nmm.edges.size());
        (*input_nmm.vertices[pt->mirror_facet(*ffi).first->info().tag].second).edges_.insert(input_nmm.edges.size());
        input_nmm.edges.push_back(bep);
        input_nmm.numEdges ++;
        num_vor_e ++;

        // ����slab edges
        Bool_SlabEdgePointer bsep2;
        bsep2.first = true;
        bsep2.second = new SlabEdge;
        (*bsep2.second).vertices_.first = (*bep.second).vertices_.first;
        (*bsep2.second).vertices_.second = (*bep.second).vertices_.second;
        (*slab_mesh->vertices[(*bsep2.second).vertices_.first].second).edges_.insert(slab_mesh->edges.size());
        (*slab_mesh->vertices[(*bsep2.second).vertices_.second].second).edges_.insert(slab_mesh->edges.size());
        (*bsep2.second).index = slab_mesh->edges.size();
        slab_mesh->edges.push_back(bsep2);
        slab_mesh->numEdges ++;
      }
    }

    for(Finite_edges_iterator_t fei = pt->finite_edges_begin(); fei != pt->finite_edges_end(); fei ++)
    {
      bool all_finite_inside = true;
      std::vector<Cell_handle_t> vec_ch;
      Cell_circulator_t cc = pt->incident_cells(*fei);
      do
      {
        if(pt->is_infinite(cc))
          all_finite_inside = false;
        else if(cc->info().inside == false)
          all_finite_inside = false;
        vec_ch.push_back(cc++);
      }while(cc != pt->incident_cells(*fei));
      if(!all_finite_inside)
        continue;

      for(unsigned k = 2; k < vec_ch.size() - 1; k ++)
      {
        Bool_EdgePointer bep;
        bep.first = true;
        bep.second = new NonManifoldMesh_Edge;
        (*bep.second).vertices_.first = vec_ch[0]->info().tag;
        (*bep.second).vertices_.second = vec_ch[k]->info().tag;
        (*input_nmm.vertices[vec_ch[0]->info().tag].second).edges_.insert(input_nmm.edges.size());
        (*input_nmm.vertices[vec_ch[k]->info().tag].second).edges_.insert(input_nmm.edges.size());
        input_nmm.edges.push_back(bep);
        input_nmm.numEdges ++;

        // ����slab edges
        Bool_SlabEdgePointer bsep2;
        bsep2.first = true;
        bsep2.second = new SlabEdge;
        (*bsep2.second).vertices_.first = (*bep.second).vertices_.first;
        (*bsep2.second).vertices_.second = (*bep.second).vertices_.second;
        (*slab_mesh->vertices[(*bsep2.second).vertices_.first].second).edges_.insert(slab_mesh->edges.size());
        (*slab_mesh->vertices[(*bsep2.second).vertices_.second].second).edges_.insert(slab_mesh->edges.size());
        (*bsep2.second).index = slab_mesh->edges.size();
        slab_mesh->edges.push_back(bsep2);
        slab_mesh->numEdges ++;
      }

      for(unsigned k = 1; k < vec_ch.size() - 1; k ++)
      {
        Bool_FacePointer bfp;
        bfp.first = true;
        bfp.second = new NonManifoldMesh_Face;
        unsigned vid[3];
        vid[0] = vec_ch[0]->info().tag;
        vid[1] = vec_ch[k]->info().tag;
        vid[2] = vec_ch[k+1]->info().tag;
        (*bfp.second).vertices_.insert(vec_ch[0]->info().tag);
        (*bfp.second).vertices_.insert(vec_ch[k]->info().tag);
        (*bfp.second).vertices_.insert(vec_ch[k+1]->info().tag);
        unsigned eid[3];
        if(input_nmm.Edge(vid[0],vid[1],eid[0]))
          (*bfp.second).edges_.insert(eid[0]);
        if(input_nmm.Edge(vid[0],vid[2],eid[1]))
          (*bfp.second).edges_.insert(eid[1]);
        if(input_nmm.Edge(vid[1],vid[2],eid[2]))
          (*bfp.second).edges_.insert(eid[2]);
        input_nmm.vertices[vid[0]].second->faces_.insert(input_nmm.faces.size());
        input_nmm.vertices[vid[1]].second->faces_.insert(input_nmm.faces.size());
        input_nmm.vertices[vid[2]].second->faces_.insert(input_nmm.faces.size());
        input_nmm.edges[eid[0]].second->faces_.insert(input_nmm.faces.size());
        input_nmm.edges[eid[1]].second->faces_.insert(input_nmm.faces.size());
        input_nmm.edges[eid[2]].second->faces_.insert(input_nmm.faces.size());
        input_nmm.faces.push_back(bfp);
        input_nmm.numFaces ++;
        num_vor_f ++;

        // ����slab face
        Bool_SlabFacePointer bsfp2;
        bsfp2.first = true;
        bsfp2.second = new SlabFace;
        (*bsfp2.second).vertices_.insert(vid[0]);
        (*bsfp2.second).vertices_.insert(vid[1]);
        (*bsfp2.second).vertices_.insert(vid[2]);
        if(slab_mesh->Edge(vid[0],vid[1],eid[0]))
        	(*bsfp2.second).edges_.insert(eid[0]);
        if(slab_mesh->Edge(vid[0],vid[2],eid[1]))
        	(*bsfp2.second).edges_.insert(eid[1]);
        if(slab_mesh->Edge(vid[1],vid[2],eid[2]))
        	(*bsfp2.second).edges_.insert(eid[2]);
        (*bsfp2.second).index = slab_mesh->faces.size();
        slab_mesh->vertices[vid[0]].second->faces_.insert(slab_mesh->faces.size());
        slab_mesh->vertices[vid[1]].second->faces_.insert(slab_mesh->faces.size());
        slab_mesh->vertices[vid[2]].second->faces_.insert(slab_mesh->faces.size());
        slab_mesh->edges[eid[0]].second->faces_.insert(slab_mesh->faces.size());
        slab_mesh->edges[eid[1]].second->faces_.insert(slab_mesh->faces.size());
        slab_mesh->edges[eid[2]].second->faces_.insert(slab_mesh->faces.size());
        slab_mesh->faces.push_back(bsfp2);
        slab_mesh->numFaces++;
      }
    }
  std::cout<<"export inputmm: " << input_nmm.meshname << std::endl;
	// input_nmm.Export("test.ma");
    // input_nmm.Export_vor_pts("01Ants-12_mesh.txt");

    input_nmm.numVertices = 0;
    input_nmm.numEdges = 0;
    input_nmm.numFaces = 0;

	slab_mesh->iniNumVertices = slab_mesh->numVertices;
	slab_mesh->iniNumEdges = slab_mesh->numEdges;
	slab_mesh->iniNumFaces = slab_mesh->numFaces;

	slab_mesh->CleanIsolatedVertices();
	slab_mesh->computebb();
	slab_mesh->ComputeFacesCentroid();
	slab_mesh->ComputeFacesNormal();
	slab_mesh->ComputeVerticesNormal();
	slab_mesh->ComputeEdgesCone();
	slab_mesh->ComputeFacesSimpleTriangles();
	slab_mesh->DistinguishVertexType();

    return true;
}

bool ComputeInputNMM3(Mesh* input, SlabMesh* slab_mesh) {
	cout << "here!" << endl;
	NonManifoldMesh input_nmm;
	input_nmm.numVertices = 0;
	input_nmm.numEdges = 0;
	input_nmm.numFaces = 0;

	//
	input_nmm.vertices.clear();
	input_nmm.edges.clear();
	input_nmm.faces.clear();

	Triangulation * pt = &(input->dt);

	//loading
	//slab_mesh.numVertices = 0;
	//slab_mesh.numEdges = 0;
	//slab_mesh.numFaces = 0;
	//loading
	slab_mesh->numVertices = 0;
	slab_mesh->numEdges = 0;
	slab_mesh->numFaces = 0;
	slab_mesh->type = 1;
	slab_mesh->bound_weight = 0.1;


	unsigned num_vor_v, num_vor_e, num_vor_f;
	num_vor_v = 0;
	num_vor_e = 0;
	num_vor_f = 0;

	double len[4];
	len[0] = input->m_max[0] - input->m_min[0];
	len[1] = input->m_max[1] - input->m_min[1];
	len[2] = input->m_max[2] - input->m_min[2];
	len[3] = sqrt(len[0]*len[0]+len[1]*len[1]+len[2]*len[2]);
	input_nmm.diameter = len[3];
	int mysample_counter = 0;
	for (Finite_vertices_iterator_t fvi = pt->finite_vertices_begin(); fvi != pt->finite_vertices_end(); fvi++) {
		input_nmm.BoundaryPoints.push_back(SamplePoint(fvi->point()[0], fvi->point()[1], fvi->point()[2]));
	}
	cout << "boundary size: "<< input_nmm.BoundaryPoints.size() << endl;

	// Here is the key...


	int mas_vertex_count(0);
	//

	for(Finite_cells_iterator_t fci = pt->finite_cells_begin(); fci != pt->finite_cells_end(); fci ++)
	{
		if(fci->info().inside == false)
		{
			fci->info().tag = -1;
			continue;
		}
		fci->info().tag = mas_vertex_count ++;


		Bool_VertexPointer bvp;
		bvp.first = true;
		bvp.second = new NonManifoldMesh_Vertex;
		(*bvp.second).sphere.center = to_wm4(CGAL::circumcenter(pt->tetrahedron(fci)));
		(*bvp.second).is_pole = fci->info().is_pole;
		for(unsigned k = 0; k < 4; k ++)
			(*bvp.second).bplist.insert(fci->vertex(k)->info().id);
		(*bvp.second).sphere.radius = pt->TetCircumRadius(pt->tetrahedron(fci));

		input_nmm.vertices.push_back(bvp);
		input_nmm.numVertices ++;
		num_vor_v ++;
	}


	for(Finite_facets_iterator_t ffi = pt->finite_facets_begin(); ffi != pt->finite_facets_end(); ffi ++)
	{
		Triangulation::Object o = pt->dual(*ffi);
		if(const Triangulation::Segment *s = CGAL::object_cast<Triangulation::Segment>(&o))
		{
			if( (ffi->first->info().inside == false) || (pt->mirror_facet(*ffi).first->info().inside == false) )
				continue;
			Bool_EdgePointer bep;
			bep.first = true;
			bep.second = new NonManifoldMesh_Edge;
			(*bep.second).vertices_.first = ffi->first->info().tag;
			(*bep.second).vertices_.second = pt->mirror_facet(*ffi).first->info().tag;
			(*input_nmm.vertices[ffi->first->info().tag].second).edges_.insert(input_nmm.edges.size());
			(*input_nmm.vertices[pt->mirror_facet(*ffi).first->info().tag].second).edges_.insert(input_nmm.edges.size());
			input_nmm.edges.push_back(bep);
			input_nmm.numEdges ++;
			num_vor_e ++;
		}
	}

	for(Finite_edges_iterator_t fei = pt->finite_edges_begin(); fei != pt->finite_edges_end(); fei ++)
	{
		bool all_finite_inside = true;
		std::vector<Cell_handle_t> vec_ch;
		Cell_circulator_t cc = pt->incident_cells(*fei);
		do
		{
			if(pt->is_infinite(cc))
				all_finite_inside = false;
			else if(cc->info().inside == false)
				all_finite_inside = false;
			vec_ch.push_back(cc++);
		}while(cc != pt->incident_cells(*fei));
		if(!all_finite_inside)
			continue;

		for(unsigned k = 2; k < vec_ch.size() - 1; k ++)
		{
			Bool_EdgePointer bep;
			bep.first = true;
			bep.second = new NonManifoldMesh_Edge;
			(*bep.second).vertices_.first = vec_ch[0]->info().tag;
			(*bep.second).vertices_.second = vec_ch[k]->info().tag;
			(*input_nmm.vertices[vec_ch[0]->info().tag].second).edges_.insert(input_nmm.edges.size());
			(*input_nmm.vertices[vec_ch[k]->info().tag].second).edges_.insert(input_nmm.edges.size());
			input_nmm.edges.push_back(bep);
			input_nmm.numEdges ++;
		}

		for(unsigned k = 1; k < vec_ch.size() - 1; k ++)
		{
			Bool_FacePointer bfp;
			bfp.first = true;
			bfp.second = new NonManifoldMesh_Face;
			unsigned vid[3];
			vid[0] = vec_ch[0]->info().tag;
			vid[1] = vec_ch[k]->info().tag;
			vid[2] = vec_ch[k+1]->info().tag;
			(*bfp.second).vertices_.insert(vec_ch[0]->info().tag);
			(*bfp.second).vertices_.insert(vec_ch[k]->info().tag);
			(*bfp.second).vertices_.insert(vec_ch[k+1]->info().tag);
			unsigned eid[3];
			if(input_nmm.Edge(vid[0],vid[1],eid[0]))
				(*bfp.second).edges_.insert(eid[0]);
			if(input_nmm.Edge(vid[0],vid[2],eid[1]))
				(*bfp.second).edges_.insert(eid[1]);
			if(input_nmm.Edge(vid[1],vid[2],eid[2]))
				(*bfp.second).edges_.insert(eid[2]);
			input_nmm.vertices[vid[0]].second->faces_.insert(input_nmm.faces.size());
			input_nmm.vertices[vid[1]].second->faces_.insert(input_nmm.faces.size());
			input_nmm.vertices[vid[2]].second->faces_.insert(input_nmm.faces.size());
			input_nmm.edges[eid[0]].second->faces_.insert(input_nmm.faces.size());
			input_nmm.edges[eid[1]].second->faces_.insert(input_nmm.faces.size());
			input_nmm.edges[eid[2]].second->faces_.insert(input_nmm.faces.size());
			input_nmm.faces.push_back(bfp);
			input_nmm.numFaces ++;
			num_vor_f ++;
		}
	}
	input_nmm.Export(input_nmm.meshname);

	input_nmm.numVertices = 0;
	input_nmm.numEdges = 0;
	input_nmm.numFaces = 0;
	//input_nmm.ComputeFacesNormal();
	//input_nmm.ComputeFacesCentroid();
	//input_nmm.ComputeFacesSimpleTriangles();
	//input_nmm.ComputeEdgesCone();

	//slab_mesh.ComputeFacesCentroid();
	//slab_mesh.ComputeFacesNormal();
	//slab_mesh.ComputeVerticesNormal();
}


void LoadSlabMesh(SlabMesh* slabMesh) {
  slabMesh->clear();
  // long startt = clock();
  // handle each face
  for (unsigned i = 0; i < slabMesh->vertices.size(); i++) {
    if (!slabMesh->vertices[i].first) continue;

    SlabVertex sv = *slabMesh->vertices[i].second;
    std::set<unsigned> fset = sv.faces_;
    Vector4d C1(sv.sphere.center.X(), sv.sphere.center.Y(),
                sv.sphere.center.Z(), sv.sphere.radius);

    for (set<unsigned>::iterator si = fset.begin(); si != fset.end(); si++) {
      SlabFace sf = *slabMesh->faces[*si].second;

      if (sf.valid_st == false || sf.st[0].normal == Vector3d(0., 0., 0.) ||
          sf.st[1].normal == Vector3d(0., 0., 0.))
        continue;

      Vector4d normal1(sf.st[0].normal.X(), sf.st[0].normal.Y(),
                       sf.st[0].normal.Z(), 1.0);
      Vector4d normal2(sf.st[1].normal.X(), sf.st[1].normal.Y(),
                       sf.st[1].normal.Z(), 1.0);

      // compute the matrix of A
      Matrix4d temp_A1, temp_A2;
      temp_A1.MakeTensorProduct(normal1, normal1);
      temp_A2.MakeTensorProduct(normal2, normal2);
      temp_A1 *= 2.0;
      temp_A2 *= 2.0;

      // compute the matrix of b
      double normal_mul_point1 = normal1.Dot(C1);
      double normal_mul_point2 = normal2.Dot(C1);
      Wm4::Vector4d temp_b1 = normal1 * 2 * normal_mul_point1;
      Wm4::Vector4d temp_b2 = normal2 * 2 * normal_mul_point2;

      // compute c
      double temp_c1 = normal_mul_point1 * normal_mul_point1;
      double temp_c2 = normal_mul_point2 * normal_mul_point2;

      slabMesh->vertices[i].second->slab_A += temp_A1;
      slabMesh->vertices[i].second->slab_A += temp_A2;
      slabMesh->vertices[i].second->slab_b += temp_b1;
      slabMesh->vertices[i].second->slab_b += temp_b2;
      slabMesh->vertices[i].second->slab_c += temp_c1;
      slabMesh->vertices[i].second->slab_c += temp_c2;

      slabMesh->vertices[i].second->related_face += 2;
    }
  }

  switch (slabMesh->preserve_boundary_method) {
    case 1:
      slabMesh->PreservBoundaryMethodOne();
      break;
    case 2:
      slabMesh->PreservBoundaryMethodTwo();
      break;
    case 3:
      slabMesh->PreservBoundaryMethodThree();
      break;
    default:
      slabMesh->PreservBoundaryMethodFour();
      break;
  }

  slabMesh->initCollapseQueue();
  // long endt = clock();
}

void openmeshfile(Mesh* input, SlabMesh* slabMesh, std::string filename) {
  // QString filename = QFileDialog::getOpenFileName(this, tr("Select a 3D model
  // to open"), NULL, tr("3D model(*.off)"));
  if (!filename.empty()) {
    // QDir qd(filename);
    std::string prefix = filename.substr(0, filename.size() - 4);
    // ThreeDimensionalShape * pThreeDimensionalShape = new
    // ThreeDimensionalShape;
    bool suc = false;
    std::ifstream stream(filename);
	std::cout << "Opening mesh file " << filename << std::endl;
    if (stream) {
	    std::cout<<"reading..."<<std::endl;
    	stream >> *input;
    	// compute the properties of the input mesh
    	input->computebb();            // bounding box
    	input->GenerateList();         // generate vertex and triangle list
    	input->GenerateRandomColor();  // color of vertex and triangle
    	input->compute_normals();      // normal of vertex and triangle
    	// pThreeDimensionalShape->input_nmm.meshname = filename.;

    	std::ifstream streampol(filename);
    	Polyhedron pol;
    	streampol >> pol;

    	// set the non manifold mesh
    	// Mesh_domain * pdom;
    	// pdom = new Mesh_domain(pol);
    	//    input->domain = pdom;

    	input->domain = new Mesh_domain(pol);
    	input->computedt();
    	// input->markpoles();
    	suc = true;
    }
    if (suc) {
      // m_pThreeDimensionalShape = pThreeDimensionalShape;
      // //ui.actionShow_Edge->setChecked(true);
      //
      // ui.actionShow_Face->setChecked(true);
      // ui.actionReverse_Orientation->setChecked(true);

      // importVP(prefix);

      std::cout<<"suc read, begin mm.."<<std::endl;
      // bool re = importMA(input, slabMesh, maname);
      bool re = ComputeInputNMM(input, slabMesh);
      if (re == false) return;

      float k = 0.00001;
      slabMesh->k = k;
      // initialize();
      slabMesh->preserve_boundary_method = 0;
      slabMesh->hyperbolic_weight_type = 3;
      slabMesh->compute_hausdorff = false;
      slabMesh->boundary_compute_scale = 0;
      slabMesh->prevent_inversion = false;

      LoadSlabMesh(slabMesh);
      // long ti = m_pThreeDimensionalShape->LoadSlabMesh();
      // slab_initial = true;

      // // set back
      // ui.actionSet_k_value->setChecked(false);
      //
      // // show the input mesh in the dialog.
      // m_pGLWidget->set3DShape(m_pThreeDimensionalShape);
      // statusBar()->showMessage(filename + tr(" is loaded successfully.") );
      // setWindowTitle( tr("Medial Axis Simplification 3D - ") + filename );

      // m_isSimplified = false;
      std::cout << "openmeshfile done." << std::endl;
    } else {
    	std::cout<<"openmeshfile failed."<<std::endl;
    }
  } else {
    std::cout << "Filename is empty !" << std::endl;
  }
}

void simplifySlab(Mesh* input, SlabMesh* slabMesh, std::string ptsFile, unsigned num_spheres) {
	slabMesh->CleanIsolatedVertices();
	int threhold = num_spheres;


	if (slabMesh->isCoverageAxis) {
		/*
	This is the core part of QMAT simplification...
	Zhiyang

	Here we make the following modification...
	- Get selected inside poles.
	- Remove the point to a better postion for further simplication.
	*/
		vector<vector<double> > selected_pole;
		cout << "Simplify_with_Selected_Pole" << endl;
		slabMesh->Simplify_with_Selected_Pole(
			slabMesh->numVertices - threhold, selected_pole, ptsFile, input->bb_diagonal_length);
		cout << "szie selected_pole " << selected_pole.size() << endl;
		// slabMesh->Export_OBJ("C://Users//frank//Desktop//export//sim_MA");
	}
	else
		slabMesh->Simplify(slabMesh->numVertices - threhold);  // original qmat


	slabMesh->ComputeFacesNormal();
	slabMesh->ComputeVerticesNormal();
	slabMesh->ComputeEdgesCone();
	slabMesh->ComputeFacesSimpleTriangles();

	std::cout << "Simplify done." << std::endl;
}


std::string get_filename_without_extension(const std::string& filepath) {
	size_t last_slash = filepath.find_last_of("/\\");
	std::string filename = (last_slash == std::string::npos) ? filepath : filepath.substr(last_slash + 1);

	size_t last_dot = filename.find_last_of('.');
	if (last_dot == std::string::npos) return filename;
	return filename.substr(0, last_dot);
}


int main(int argc, char** argv) {
	CLI::App app{"qmat & coverageAxis"};
	struct {
		std::string
			filename,
			coverageAxisPtsFile = "",
			out_dir = "./";
		unsigned num_spheres;
	} args;

	app.add_option("-f,--file", args.filename, ".off mesh file")->required();
	app.add_option("-n,--num-spheres", args.num_spheres, "number of vertices")->required();
	app.add_option("--cov", args.coverageAxisPtsFile, "run coverageAxis with selected points");
	app.add_option("--outdir", args.out_dir, "output directory. please end with /");
	CLI11_PARSE(app, argc, argv);

	std::string outfilename = args.out_dir +  get_filename_without_extension(args.filename) + ".ma";
	Mesh input;
	Mesh* pinput = &input;
	SlabMesh slabMesh;
	SlabMesh* pslabMesh = &slabMesh;
	pinput->isCoverageAxis = (args.coverageAxisPtsFile!="");
	pslabMesh->isCoverageAxis = (args.coverageAxisPtsFile!="");

	openmeshfile(pinput, pslabMesh, args.filename);
	printf("done openmeshfile\n");
	simplifySlab(pinput, pslabMesh, args.coverageAxisPtsFile, args.num_spheres);
	printf("done simplifyslab\n");
	pslabMesh->Export(outfilename, pinput);
	printf("done export\n");

	return 0;
}
