// PyPermGroup.cpp

#include <Python.h>
#include "PyPermGroup.h"
#include "PyPerm.h"
#include "PyStabChain.h"

static PyMethodDef methodDefTable[] =
{
	{nullptr, nullptr, 0, nullptr},
};

static PyModuleDef moduleDef =
{
	PyModuleDef_HEAD_INIT,
	PyDoc_STR("PyPermGroup"),
	PyDoc_STR("A module for working with permutation groups."),
	-1,
	methodDefTable,
	NULL,
	NULL,
	NULL,
	NULL
};

PyMODINIT_FUNC PyInit_PyPermGroup(void)
{
	if(PyType_Ready(&PyPermTypeObject) < 0)
		return nullptr;

	if(PyType_Ready(&PyStabChainTypeObject) < 0)
		return nullptr;

	PyObject* moduleObj = PyModule_Create(&moduleDef);

	Py_INCREF(&PyPermTypeObject);
	PyModule_AddObject(moduleObj, "Perm", (PyObject*)&PyPermTypeObject);

	Py_INCREF(&PyStabChainTypeObject);
	PyModule_AddObject(moduleObj, "StabChain", (PyObject*)&PyStabChainTypeObject);

	return moduleObj;
}