Example of Usage
=================

1. GJK - Intersection Test
---------------------------
This section describes how to use **libccddbl** for testing if two convex objects intersects (i.e., 'yes/no' test) using Gilbert-Johnson-Keerthi (GJK) algorithm.

Procedure is very simple (and similar to the usage of the rest of the
library):

#. Include ``<ccddbl/ccddbl.h>`` file.
#. Implement support function for specific shapes. Support function is
   function that returns furthest point from object (shape) in specified
   direction.
#. Set up ``ccddbl_t`` structure.
#. Run ``ccddblGJKIntersect()`` function on desired objects.


Here is a skeleton of simple program:

.. code-block:: c

    #include <ccddbl/ccddbl.h>
    #include <ccddbl/quat.h> // for work with quaternions

    /** Support function for box */
    void support(const void *obj, const ccddbl_vec3_t *dir,
                 ccddbl_vec3_t *vec)
    {
        // assume that obj_t is user-defined structure that holds info about
        // object (in this case box: x, y, z, pos, quat - dimensions of box,
        // position and rotation)
        obj_t *obj = (obj_t *)_obj;
        ccddbl_vec3_t dir;
        ccddbl_quat_t qinv;

        // apply rotation on direction vector
        ccddblVec3Copy(&dir, _dir);
        ccddblQuatInvert2(&qinv, &obj->quat);
        ccddblQuatRotVec(&dir, &qinv);

        // compute support point in specified direction
        ccddblVec3Set(v, ccddblSign(ccddblVec3X(&dir)) * box->x * CCDDBL_REAL(0.5),
                      ccddblSign(ccddblVec3Y(&dir)) * box->y * CCDDBL_REAL(0.5),
                      ccddblSign(ccddblVec3Z(&dir)) * box->z * CCDDBL_REAL(0.5));

        // transform support point according to position and rotation of object
        ccddblQuatRotVec(v, &obj->quat);
        ccddblVec3Add(v, &obj->pos);
    }


    int main(int argc, char *argv[])
    {
        ...

        ccddbl_t ccddbl;
        CCDDBL_INIT(&ccddbl); // initialize ccddbl_t struct
    
        // set up ccddbl_t struct
        ccddbl.support1       = support; // support function for first object
        ccddbl.support2       = support; // support function for second object
        ccddbl.max_iterations = 100;     // maximal number of iterations

        int intersect = ccddblGJKIntersect(obj1, obj2, &ccddbl);
        // now intersect holds true if obj1 and obj2 intersect, false otherwise
    }




2. GJK + EPA - Penetration Of Two Objects
------------------------------------------

If you want to obtain also penetration info about two intersection objects
``ccddblGJKPenetration()`` function can be used. 

Procedure is almost the same as for the previous case:

.. code-block:: c

    #include <ccddbl/ccddbl.h>
    #include <ccddbl/quat.h> // for work with quaternions

    /** Support function is same as in previous case */

    int main(int argc, char *argv[])
    {
        ...
        ccddbl_t ccddbl;
        CCDDBL_INIT(&ccddbl); // initialize ccddbl_t struct

        // set up ccddbl_t struct
        ccddbl.support1       = support; // support function for first object
        ccddbl.support2       = support; // support function for second object
        ccddbl.max_iterations = 100;     // maximal number of iterations
        ccddbl.epa_tolerance  = 0.0001;  // maximal tolerance fro EPA part

        ccddbl_real_t depth;
        ccddbl_vec3_t dir, pos;
        int intersect = ccddblGJKPenetration(obj1, obj2, &ccddbl, &depth, &dir, &pos);
        // now intersect holds true if obj1 and obj2 intersect, false otherwise
        // in depth, dir and pos is stored penetration depth, direction of
        // separation vector and position in global coordinate system
    }


3. MPR - Intersection Test
---------------------------

**libccddbl** also provides *MPR* - Minkowski Portal Refinement algorithm that
can be used for testing if two objects intersects.

Procedure is similar to the one used for GJK algorithm. Support function is
the same but also function that returns a center (or any point near center)
of a given object must be implemented:

.. code-block:: c

    #include <ccddbl/ccddbl.h>
    #include <ccddbl/quat.h> // for work with quaternions

    /** Support function is same as in previous case */

    /** Center function - returns center of object */
    void center(const void *_obj, ccddbl_vec3_t *center)
    {
        obj_t *obj = (obj_t *)_obj;
        ccddblVec3Copy(center, &obj->pos);
    }

    int main(int argc, char *argv[])
    {
        ...
        ccddbl_t ccddbl;
        CCDDBL_INIT(&ccddbl); // initialize ccddbl_t struct

        // set up ccddbl_t struct
        ccddbl.support1       = support; // support function for first object
        ccddbl.support2       = support; // support function for second object
        ccddbl.center1        = center;  // center function for first object
        ccddbl.center2        = center;  // center function for second object
        ccddbl.mpr_tolerance  = 0.0001;  // maximal tolerance

        int intersect = ccddblMPRIntersect(obj1, obj2, &ccddbl);
        // now intersect holds true if obj1 and obj2 intersect, false otherwise
    }


4. MPR - Penetration Of Two Objects
------------------------------------

Using MPR algorithm for obtaining penetration info about two intersection
objects is equally easy as in the previous case instead but
``ccddblMPRPenetration()`` function is used:

.. code-block:: c

    #include <ccddbl/ccddbl.h>
    #include <ccddbl/quat.h> // for work with quaternions

    /** Support function is same as in previous case */
    /** Center function is same as in prevous case */

    int main(int argc, char *argv[])
    {
        ...
        ccddbl_t ccddbl;
        CCDDBL_INIT(&ccddbl); // initialize ccddbl_t struct

        // set up ccddbl_t struct
        ccddbl.support1       = support; // support function for first object
        ccddbl.support2       = support; // support function for second object
        ccddbl.center1        = center;  // center function for first object
        ccddbl.center2        = center;  // center function for second object
        ccddbl.mpr_tolerance  = 0.0001;  // maximal tolerance

        ccddbl_real_t depth;
        ccddbl_vec3_t dir, pos;
        int intersect = ccddblMPRPenetration(obj1, obj2, &ccddbl, &depth, &dir, &pos);
        // now intersect holds true if obj1 and obj2 intersect, false otherwise
        // in depth, dir and pos is stored penetration depth, direction of
        // separation vector and position in global coordinate system
    }

