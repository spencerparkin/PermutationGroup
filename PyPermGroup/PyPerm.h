// PyPerm.h

#pragma once

#include "Permutation.h"

extern PyTypeObject PyPermTypeObject;

Permutation* Permutation_from_PyObject(PyObject* object);
PyObject* Permutation_to_PyObject(Permutation* permutation);
