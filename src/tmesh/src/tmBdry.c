#include "tmesh/tmTypedefs.h"
#include "tmesh/tmMesh.h"
#include "tmesh/tmEdge.h"
#include "tmesh/tmNode.h"
#include "tmesh/tmBdry.h"
#include "tmesh/tmQtree.h"
#include "tmesh/tmFront.h"
#include "tmesh/tmTri.h"


/**********************************************************
* Function: tmBdry_create()
*----------------------------------------------------------
* Create a new tmBdry structure and returns a pointer
* to it.
*----------------------------------------------------------
* @return: Pointer to a new tmBdry structure
**********************************************************/
tmBdry *tmBdry_create(tmMesh *mesh, 
                      tmBool  is_interior,
                      int     index)
{
  tmBdry *bdry = (tmBdry*) calloc( 1, sizeof(tmBdry) );
  check_mem(bdry);

  /*-------------------------------------------------------
  | Properties of this boundary
  -------------------------------------------------------*/
  bdry->mesh        = mesh;
  bdry->no_edges    =  0;
  bdry->is_interior = is_interior;
  bdry->index       = index;
  bdry->mesh_pos    = NULL;
  bdry->area        = 0.0;


  /*-------------------------------------------------------
  | Boundary edges
  -------------------------------------------------------*/
  bdry->edges_head   = NULL;
  bdry->edges_stack  = List_create();
  bdry->edges_qtree  = tmQtree_create(mesh, TM_EDGE);
  tmQtree_init(bdry->edges_qtree, NULL, 0, 
               mesh->xy_min, mesh->xy_max);

  return bdry;

error:
  return NULL;

} /* tmBdry_create() */

/**********************************************************
* Function: tmBdry_destroy()
*----------------------------------------------------------
* Destroys a tmBdry structure and frees all its
* memory.
*----------------------------------------------------------
* @param *mesh: pointer to a tmBdry to destroy
*
**********************************************************/
void tmBdry_destroy(tmBdry *bdry)
{
  ListNode *cur, *nxt;

  /*-------------------------------------------------------
  | Free all boundary edges on the stack
  -------------------------------------------------------*/
  cur = nxt = bdry->edges_stack->first;
  while (nxt != NULL)
  {
    nxt = cur->next;
    tmEdge_destroy(cur->value);
    cur = nxt;
  }

  /*-------------------------------------------------------
  | Destroy edges qtree and edges stack
  -------------------------------------------------------*/
  tmQtree_destroy(bdry->edges_qtree);
  List_destroy(bdry->edges_stack);

  /*-------------------------------------------------------
  | Remove boundary from the mesh structure 
  -------------------------------------------------------*/
  tmMesh_remBdry(bdry->mesh, bdry);

  /*-------------------------------------------------------
  | Finally free the structure
  -------------------------------------------------------*/
  free(bdry);

} /* tmBdry_destroy() */


/**********************************************************
* Function: tmBdry_edgeCreate()
*----------------------------------------------------------
* Function to create a new boundary edge 
*----------------------------------------------------------
* 
**********************************************************/
tmEdge *tmBdry_edgeCreate(tmBdry *bdry, 
                          tmNode *n1, 
                          tmNode *n2,
                          tmIndex marker)
{
  tmEdge *edge = tmEdge_create( bdry->mesh, n1, n2, bdry, 0);
  edge->bdry_marker = marker;

} /*tmBdry_edgeCreate() */


/**********************************************************
* Function: tmBdry_addEdge()
*----------------------------------------------------------
* Function to add an edge to a tmBdry structure
* This edge is the new head of the boundary structure
*----------------------------------------------------------
* @return: ListNode to tmEdge on the mesh's edge stack
**********************************************************/
ListNode *tmBdry_addEdge(tmBdry *bdry, tmEdge *edge)
{
  ListNode *edge_pos;
  bdry->no_edges += 1;
  bdry->edges_head  = edge;

  List_push(bdry->edges_stack, edge);
  tmQtree_addObj(bdry->edges_qtree, edge);
  edge_pos = List_last_node(bdry->edges_stack);
  
  return edge_pos;

} /* tmBdry_addEdge() */

/**********************************************************
* Function: tmBdry_remEdge()
*----------------------------------------------------------
* Function to remove an edge from a tmBdry structure
*----------------------------------------------------------
*
**********************************************************/
void tmBdry_remEdge(tmBdry *bdry, tmEdge *edge)
{
  tmBool    qtree_rem;

  /*-------------------------------------------------------
  | Check if object is in this boundary
  -------------------------------------------------------*/
  if ( edge->bdry != bdry )
    log_warn("Can not remove edge from mesh. Edge not found.");

  /*-------------------------------------------------------
  | Remove edge from qtree
  -------------------------------------------------------*/
  qtree_rem = tmQtree_remObj(bdry->edges_qtree, edge);
  bdry->no_edges -= 1;

  /*-------------------------------------------------------
  | Remove edge from stack
  -------------------------------------------------------*/
  List_remove(bdry->edges_stack, edge->stack_pos);

  /*-------------------------------------------------------
  | Destroy edge
  -------------------------------------------------------*/
  //tmEdge_destroy(edge);

} /* tmBdry_remEdge() */


/**********************************************************
* Function: tmBdry_isLeft()
*----------------------------------------------------------
* Check if an object is left of all boundary edges
*----------------------------------------------------------
* @param *edge: pointer to a tmEdge 
* @param *obj:  pointer to object to check for
* @param  obj_type: object type specifier
* @return boolean if object is located on the left of bdry
**********************************************************/
tmBool tmBdry_isLeft(tmBdry *bdry, void *obj, int obj_type)
{
  ListNode *cur;
  tmBool is_left = TRUE;

  for (cur = bdry->edges_stack->first; 
       cur != NULL; cur = cur->next)
  {
    is_left &= tmEdge_isLeft( cur->value, obj, obj_type);
  }

  return is_left;

} /* tmBdry_isLeft() */

/**********************************************************
* Function: tmBdry_isLeftOn()
*----------------------------------------------------------
* Check if an object is left of all boundary edges
*----------------------------------------------------------
* @param *edge: pointer to a tmEdge 
* @param *obj:  pointer to object to check for
* @param  obj_type: object type specifier
* @return boolean if object is located on the left of bdry
**********************************************************/
tmBool tmBdry_isLeftOn(tmBdry *bdry, void *obj, int obj_type)
{
  ListNode *cur;
  tmBool is_left = TRUE;

  for (cur = bdry->edges_stack->first; 
       cur != NULL; cur = cur->next)
  {
    is_left &= tmEdge_isLeftOn( cur->value, obj, obj_type);
  }

  return is_left;

} /* tmBdry_isLeftOn() */

/**********************************************************
* Function: tmBdry_isRight()
*----------------------------------------------------------
* Check if an object is right of all boundary edges
*----------------------------------------------------------
* @param *edge: pointer to a tmEdge 
* @param *obj:  pointer to object to check for
* @param  obj_type: object type specifier
* @return boolean if object is located on the left of bdry
**********************************************************/
tmBool tmBdry_isRight(tmBdry *bdry, void *obj, int obj_type)
{
  ListNode *cur;
  tmBool is_right = TRUE;

  for (cur = bdry->edges_stack->first; 
       cur != NULL; cur = cur->next)
  {
    is_right &= tmEdge_isRight( cur->value, obj, obj_type);
  }

  return is_right;

} /* tmBdry_isRight() */

/**********************************************************
* Function: tmBdry_isRightOn()
*----------------------------------------------------------
* Check if an object is right of all boundary edges
*----------------------------------------------------------
* @param *bdry: pointer to a tmBdry 
* @param *obj:  pointer to object to check for
* @param  obj_type: object type specifier
* @return boolean if object is located on the left of bdry
**********************************************************/
tmBool tmBdry_isRightOn(tmBdry *bdry, void *obj, int obj_type)
{
  ListNode *cur;
  tmBool is_right = TRUE;

  for (cur = bdry->edges_stack->first; 
       cur != NULL; cur = cur->next)
  {
    is_right &= tmEdge_isRightOn( cur->value, obj, obj_type);
  }

  return is_right;

} /* tmBdry_isRightOn() */

/**********************************************************
* Function: tmBdry_isInside()
*----------------------------------------------------------
* Check if an object with position xy is inside of a 
* boundary which is enclosed by at least three edges,
* using the Ray-Method.
* If the object is located on the boundary edges,
* it is treated as lying inside.
* 
* Check out this source:
* http://alienryderflex.com/polygon/
*----------------------------------------------------------
* @param *bdry: pointer to a tmBdry 
* @param  xy:   location to check for
* @return boolean if object is located on the left of bdry
**********************************************************/
tmBool tmBdry_isInside(tmBdry *bdry, tmDouble xy[2])
{
  ListNode *cur;
  tmBool    is_inside = TRUE;
  tmMesh   *mesh      = bdry->mesh;
  int      count      = 0;

  if (bdry->no_edges < 3)
    log_err("tmBdry_isInside() can not be called for boundaries with less than three segments.");

  /*-------------------------------------------------------
  | Loop over all boundary edges
  -------------------------------------------------------*/
  for (cur = bdry->edges_stack->first; 
       cur != NULL; cur = cur->next)
  {
    tmDouble *e0 = ((tmEdge*)cur->value)->n1->xy;
    tmDouble *e1 = ((tmEdge*)cur->value)->n2->xy;

    /*-----------------------------------------------------
    | Lines cross
    -----------------------------------------------------*/
    if (  (xy[1]>e1[1] && xy[1]<=e0[1])
       || (xy[1]>e0[1] && xy[1]<=e1[1]) )
    {
      if (e1[0] + (xy[1]-e1[1])/(e0[1]-e1[1])*(e0[0]-e1[0]) < xy[0])
        count++;
    }

    /*-----------------------------------------------------
    | Point is on line
    -----------------------------------------------------*/
    if ( EQ(xy[1],e1[1]) && EQ(xy[1],e0[1]) )
    {
      if (IN_ON_SEGMENT(e0, e1, xy))
        return TRUE;
    }

  }

  return count&1; /* Same as (count%2 == 1) */

} /* tmBdry_isInside() */

/**********************************************************
* Function: tmBdry_splitEdge()
*----------------------------------------------------------
* Split an edge by inserting a new node on its centroid
* and splitting it into two new edges
* This edge will be pointing from its node n1 to the new
* node and a new edge will be created, which points
* from the new node to n2.
*----------------------------------------------------------
* @param *bdry: pointer to bdry
* @param *edge: pointer to a tmEdge 
* @return: pointer to newly created first edge
**********************************************************/
tmEdge *tmBdry_splitEdge(tmBdry *bdry, tmEdge *edge)
{
  tmSizeFun sizeFun = bdry->mesh->sizeFun;

  tmNode *n1 = edge->n1;
  tmNode *n2 = edge->n2;

  /*-------------------------------------------------------
  | Create new node and edges
  -------------------------------------------------------*/
  tmNode *nn = tmNode_create(edge->mesh, edge->xy);

  tmIndex marker = edge->bdry_marker;

  tmEdge_destroy(edge);

  tmEdge *ne1 = tmBdry_edgeCreate(bdry, n1, nn, marker);
  tmEdge *ne2 = tmBdry_edgeCreate(bdry, nn, n2, marker);

  return ne1;

} /* tmEdge_split() */

/**********************************************************
* Function: tmBdry_refine()
*----------------------------------------------------------
* Refine the edges of a boundary structure according to
* a size function
*----------------------------------------------------------
* @param *bdry: pointer to bdry
* @return: 
**********************************************************/
void tmBdry_refine(tmBdry *bdry)
{

  ListNode *cur, *nxt;
  int counter = 0;

  cur = nxt = bdry->edges_stack->first;

  tmSizeFun sizeFun = bdry->mesh->sizeFun;

  while ( counter < bdry->no_edges )
  {
    nxt = cur->next;

    tmDouble rho_1 = sizeFun( ((tmEdge*)cur->value)->n1->xy );
    tmDouble rho_m = sizeFun( ((tmEdge*)cur->value)->xy );
    tmDouble rho   = TM_BDRY_REFINE_FAC * (rho_1 + rho_m);
    check( rho > TM_MIN_SIZE,
        "Size function return value lower than defined minimum scale.");

    if ( ((tmEdge*)cur->value)->len > rho )
      tmBdry_splitEdge(bdry, cur->value);
    else
      counter += 1;

    cur = nxt;

    if (cur == NULL)
      cur = bdry->edges_stack->first;
  }

error:
  return;

} /* tmBdry_refine() */

/**********************************************************
* Function: tmBdry_calcArea()
*----------------------------------------------------------
* Computes the area enclosed by the boundary and sets
* it in the boundaries properties
*
* Reference:
* https://www.wikihow.com/Calculate-the-Area-of-a-Polygon
*----------------------------------------------------------
* @param *bdry: pointer to bdry
* @return: 
**********************************************************/
void tmBdry_calcArea(tmBdry *bdry)
{
  ListNode *cur;

  tmDouble p1 = 0.0;
  tmDouble p2 = 0.0;

  /*-------------------------------------------------------
  | Loop over all boundary edges
  -------------------------------------------------------*/
  for (cur = bdry->edges_stack->first; 
       cur != NULL; cur = cur->next)
  {
    tmNode *n1 = ((tmEdge*)cur->value)->n1;
    tmNode *n2 = ((tmEdge*)cur->value)->n2;

    p1 += n1->xy[0] * n2->xy[1];
    p2 += n1->xy[1] * n2->xy[0];
  }

  bdry->area = 0.5 * (p1 - p2);

  if (bdry->is_interior == TRUE)
  {
    check(bdry->area <= 0.0, "Interior boundary seems to have a counter-clockwise orientation.");
  }
  else
  {
    check(bdry->area >= 0.0, "Exterior boundary seems to have a clockwise orientation.");
  }

error:
  return;

} /* tmBdry_calcArea() */