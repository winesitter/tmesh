#ifndef tmesh_tests_h
#define tmesh_tests_h

/*************************************************************
* Unit test function to handle creation and 
* destruction of a mesh structure
*************************************************************/
char *test_mesh_create_destroy();

/************************************************************
* Unit test function to test if an object can be located
* within the domain
************************************************************/
char *test_tmMesh_objInside();

/*************************************************************
* Unit test function to the boundary refinement
************************************************************/
char *test_tmBdry_refine();

/*************************************************************
* Unit test function to handle the tmQtree structure
*
* Maximum number of qtree-objects must be set to 3 !
* in order to get this test running
************************************************************/
char *test_tmQtree();

/************************************************************
* Unit test function to handle the tmQtree structure
*
* Maximum number of qtree-objects must be set to 2  
* in order to get this test running
************************************************************/
char *test_tmQtree_2();

/*************************************************************
* Unit test function to handle the tmQtree performance
************************************************************/
char *test_tmQtree_performance();

/*************************************************************
* Unit test function to handle the tmQtree performance
************************************************************/
char *test_tmQtree_performance2();

/*************************************************************
* Unit test function for the triangle intersection 
************************************************************/
char *test_tmTri_intersection();

/*************************************************************
* Unit test function for the advancing front algorithm
************************************************************/
char *test_tmFront_init();
char *test_tmFront_advance();
char *test_tmFront_simpleMesh();
char *test_tmFront_simpleMesh2();
char *test_tmFront_innerOuterMesh();
char *test_tmFront_tMesh();

/************************************************************
* Unit test function to handle the automatic sizeFunction
* at boundary edges
************************************************************/ 
char *test_tmBdry_sizeFunction();

#endif
