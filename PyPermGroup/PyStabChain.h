// PyStabChain.h

#pragma once

#include "StabilizerChain.h"

extern PyTypeObject PyStabChainTypeObject;

StabilizerChain* StabChain_from_PyObject(PyObject* object);
PyObject* StabChain_to_PyObject(StabilizerChain* stabChain);