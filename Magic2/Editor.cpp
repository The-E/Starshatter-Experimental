/*  Project MAGIC
    John DiCamillo Software Consulting
    Copyright © 1994-1997. All Rights Reserved.

    SUBSYSTEM:    Magic.exe Application
    FILE:         Editor.cpp
    AUTHOR:       John DiCamillo


    OVERVIEW
    ========
    Source file for implementation of Selector
*/


#include "stdafx.h"
#include "Editor.h"
#include "MagicDoc.h"
#include "ModelView.h"
#include "Selection.h"

// +----------------------------------------------------------------------+


static float project_u(Vec3& v, int style)
{
   switch (style) {
   case 0:  return v.x;    // PLAN
   case 1:  return v.x;    // FRONT
   case 2:  return v.z;    // SIDE
   }
   
   return v.x;
}

static float project_v(Vec3& v, int style)
{
   switch (style) {
   case 0:  return -v.y;    // PLAN
   case 1:  return -v.z;    // FRONT
   case 2:  return -v.y;    // SIDE
   }
   
   return -v.y;
}

static float project_u_cylindrical(Vec3& v, int axis)
{
   float t = 0.0f;

   switch (axis) {
   // PLAN
   case 0:  if (v.x == 0)
               return 0.0f;
            t = v.y/v.x;
            return (float) atan(t);

   // FRONT
   case 1:  if (v.x == 0)
               return 0.0f;
            t = v.z/v.x;
            return (float) atan(t);

   // SIDE
   case 2:  return (float) atan2(v.z, v.y);    // SIDE
   }
   
   return project_u(v, axis);
}

static float project_v_cylindrical(Vec3& v, int axis)
{
   switch (axis) {
   case 0:  return v.z;    // PLAN
   case 1:  return v.y;    // FRONT
   case 2:  return v.x;    // SIDE
   }
   
   return project_v(v, axis);
}

void
Editor::ApplyMaterial(Material* material, std::vector<Poly>& polys,
                    int mapping, int axis, float scale_u, float scale_v,
                    int flip, int mirror, int rotate)
{
   // save state:
   EditCommand* command = new EditCommand("ApplyMaterial", document);
   document->Exec(command);

   // do the job:
   if (mapping == MAP_CYLINDRICAL) {
      ApplyMaterialCylindrical(material, polys, axis, scale_u, scale_v, flip, mirror, rotate);
      return;
   }

   if (mapping == MAP_SPHERICAL) {
      ApplyMaterialSpherical(material, polys, axis, scale_u, scale_v, flip, mirror, rotate);
      return;
   }

   VertexSet*  vset = polys.begin()->vertex_set;

   Vec3*    loc   = vset->loc;
   float    min_u = 100000.0f, max_u = -100000.0f;
   float    min_v = 100000.0f, max_v = -100000.0f;

   auto iter = polys.begin();

   // compute range and scale:
   if (mapping == MAP_PLANAR) {
      while (iter != polys.end()) {
         for (int i = 0; i < iter->nverts; i++) {
            int v = iter->verts[i];
            
            float u0 = project_u(loc[v], axis);
            float v0 = project_v(loc[v], axis);

            if (u0 < min_u) min_u = u0;
            if (v0 < min_v) min_v = v0;
            if (u0 > max_u) max_u = u0;
            if (v0 > max_v) max_v = v0;
         }
		 ++iter;
      }
   }

   float base_u = 0.0f;
   float base_v = 0.0f;
   
   if (max_u != min_u) base_u = 1.0f / (max_u - min_u);
   if (max_v != min_v) base_v = 1.0f / (max_v - min_v);

   iter = polys.begin();

   // assign texture id and coordinates:
   while (iter != polys.end()) {
      iter->material = material;

      if (mapping == MAP_NONE)
         continue;

      for (int i = 0; i < iter->nverts; i++) {
         int v = iter->verts[i];

         // planar projection
         if (mapping == MAP_PLANAR) {
            if (!rotate) {
               if (mirror)
                  vset->tu[v] = (1.0f - base_u * (project_u(loc[v], axis) - min_u)) * scale_u;
               else
                  vset->tu[v] = (project_u(loc[v], axis) - min_u) * scale_u * base_u;

               if (flip)
                  vset->tv[v] = (1.0f - base_v * (project_v(loc[v], axis) - min_v)) * scale_v;
               else   
                  vset->tv[v] = (project_v(loc[v], axis) - min_v) * scale_v * base_v;
            }
            else {
               if (!mirror)
                  vset->tv[v] = (1.0f - base_u * (project_u(loc[v], axis) - min_u)) * scale_u;
               else
                  vset->tv[v] = (project_u(loc[v], axis) - min_u) * scale_u * base_u;

               if (flip)
                  vset->tu[v] = (1.0f - base_v * (project_v(loc[v], axis) - min_v)) * scale_v;
               else   
                  vset->tu[v] = (project_v(loc[v], axis) - min_v) * scale_v * base_v;
            }
         }

         // stretch to fit
         else if (mapping == MAP_STRETCH) {
            if (scale_u < 0.001) scale_u = 1;
            if (scale_v < 0.001) scale_v = 1;

            if (!rotate) {
               if (mirror)
                  vset->tu[v] = scale_u * (float) (i < 1 || i > 2);
               else
                  vset->tu[v] = scale_u * (float) (i > 0 && i < 3);

               if (flip)
                  vset->tv[v] = scale_v * (float) (i <= 1);
               else   
                  vset->tv[v] = scale_v * (float) (i >  1);
            }
            else {
               if (!mirror)
                  vset->tv[v] = scale_v * (float) (i < 1 || i > 2);
               else
                  vset->tv[v] = scale_v * (float) (i > 0 && i < 3);

               if (flip)
                  vset->tu[v] = scale_u * (float) (i <= 1);
               else   
                  vset->tu[v] = scale_u * (float) (i >  1);
            }
         }
      }
	  ++iter;
   }

   Resegment();
}

void
Editor::ApplyMaterialCylindrical(Material* material, std::vector<Poly>& polys,
                    int axis, float scale_u, float scale_v,
                    int flip, int mirror, int rotate)
{
   VertexSet*  vset = polys.begin()->vertex_set;

   Vec3*    loc   = vset->loc;
   float    min_u = 100000.0f, max_u = -100000.0f;
   float    min_v = 100000.0f, max_v = -100000.0f;

   auto iter = polys.begin();

   // compute range and scale:
   while (iter != polys.end()) {
      for (int i = 0; i < iter->nverts; i++) {
         int v = iter->verts[i];
         
         float u0 = project_u_cylindrical(loc[v], axis);
         float v0 = project_v_cylindrical(loc[v], axis);

         if (u0 < min_u) min_u = u0;
         if (v0 < min_v) min_v = v0;
         if (u0 > max_u) max_u = u0;
         if (v0 > max_v) max_v = v0;
      }
	  ++iter;
   }

   float base_u = 0.0f;
   float base_v = 0.0f;
   
   if (max_u != min_u) base_u = 1.0f / (max_u - min_u);
   if (max_v != min_v) base_v = 1.0f / (max_v - min_v);

   iter = polys.begin();

   // assign texture id and coordinates:
   while (iter != polys.end()) {
      iter->material = material;
      
      for (int i = 0; i < iter->nverts; i++) {
         int   v  = iter->verts[i];
         float u0 = project_u_cylindrical(loc[v], axis);
         float v0 = project_v_cylindrical(loc[v], axis);

         if (!rotate) {
            if (mirror)
               vset->tu[v] = (1.0f - base_u * (u0 - min_u)) * scale_u;
            else
               vset->tu[v] = (u0 - min_u) * scale_u * base_u;

            if (flip)
               vset->tv[v] = (1.0f - base_v * (v0 - min_v)) * scale_v;
            else   
               vset->tv[v] = (v0 - min_v) * scale_v * base_v;
         }
         else {
            if (!mirror)
               vset->tv[v] = (1.0f - base_u * (u0 - min_u)) * scale_u;
            else
               vset->tv[v] = (u0 - min_u) * scale_u * base_u;

            if (flip)
               vset->tu[v] = (1.0f - base_v * (v0 - min_v)) * scale_v;
            else   
               vset->tu[v] = (v0 - min_v) * scale_v * base_v;
         }
      }
	  ++iter;
   }

   Resegment();
}

void
Editor::ApplyMaterialSpherical(Material* material, std::vector<Poly>& polys,
                    int axis, float scale_u, float scale_v,
                    int flip, int mirror, int rotate)
{
}

// +----------------------------------------------------------------------+

static int mcomp(const void* a, const void* b)
{
   Poly* pa = (Poly*) a;
   Poly* pb = (Poly*) b;

   if (pa->sortval == pb->sortval)
      return 0;

   if (pa->sortval < pb->sortval)
      return -1;

   return 1;
}

void
Editor::Resegment()
{
   if (model) {
      for (auto iter = model->GetSurfaces().begin(); iter != model->GetSurfaces().end(); ++iter) {
         Poly*    polys   = iter->GetPolys();
         int      npolys  = iter->NumPolys();

         for (int n = 0; n < npolys; n++) {
            Poly*     p = polys + n;
            Material* m = p->material;
			int sortval = 0;
			for (auto mat_iter = model->GetMaterials().begin(); mat_iter != model->GetMaterials().end(); ++mat_iter) {
				if (*m == *mat_iter)
					sortval = (mat_iter - model->GetMaterials().begin()) + 1;
			}

            if (p->sortval != sortval)
               p->sortval = sortval;
         }

         // destroy the old segments and video data:
         VideoPrivateData* video_data = iter->GetVideoPrivateData();
         iter->SetVideoPrivateData(0);
         iter->GetSegments().clear();

         delete video_data;

         // sort the polys by material index:
         qsort((void*) polys, npolys, sizeof(Poly), mcomp);

         // create new cohesive segments:
         Segment* segment = 0;

         for (int n = 0; n < npolys; n++) {
            if (segment && segment->material == polys[n].material) {
               segment->npolys++;
            }
            else {
               segment = 0;
            }

            if (!segment) {
               segment = new Segment;

               segment->npolys   = 1;
               segment->polys    = polys + n;
               segment->material = segment->polys->material;

			   iter->GetSegments().push_back(*segment);
            }
         }
      }
   }
}


// +----------------------------------------------------------------------+
// +----------------------------------------------------------------------+
// +----------------------------------------------------------------------+

EditCommand::EditCommand(const char* n, MagicDoc* d)
   : Command(n, d), model1(0), model2(0)
{
}

EditCommand::~EditCommand()
{
   delete model1;
   delete model2;
}

// +----------------------------------------------------------------------+

void
EditCommand::Do()
{
   if (document) {
      Solid* solid = document->GetSolid();

      // first application:
      if (!model2) {
         if (!model1)
            model1 = new Model(*solid->GetModel());
      }
      // re-do:
      else {
         solid->GetModel()->operator=(*model2);
      }
   }
}

// +----------------------------------------------------------------------+

void
EditCommand::Undo()
{
   if (document && model1) {
      Solid* solid = document->GetSolid();

      // save current state for later re-do:
      if (!model2)
         model2 = new Model(*solid->GetModel());

      solid->GetModel()->operator=(*model1);
   }
}

