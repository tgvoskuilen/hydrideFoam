if (finalIter)
{
    mesh.data::add("finalIteration", true);
}
Info<< "Solving hydride bed governing equations" << endl;

//#include "calculatekeff.H"
#include "FEqn.H"
#include "TEqn.H"
#include "pEqn.H"


if (finalIter)
{
    mesh.data::remove("finalIteration");
}
