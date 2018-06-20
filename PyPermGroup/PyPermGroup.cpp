// PyPermGroup.cpp : Defines the exported functions for the DLL application.
//

#include <Python.h>
#include "PyPermGroup.h"
#include "PyPerm.h"

static PyMethodDef methodDefTable[] =
{
	{nullptr, nullptr, 0, nullptr},
};

static PyModuleDef moduleDef =
{
	PyModuleDef_HEAD_INIT,
	PyDoc_STR("PermGroup"),
	PyDoc_STR("A module for working with permutation groups."),
	-1,
	methodDefTable,
	NULL,
	NULL,
	NULL,
	NULL
};

PyMODINIT_FUNC PyInit_PermGroup(void)
{
	if(PyType_Ready(&PyPermTypeObject) < 0)
		return nullptr;

	PyObject* moduleObj = PyModule_Create(&moduleDef);

	Py_INCREF(&PyPermTypeObject);
	PyModule_AddObject(moduleObj, "Perm", (PyObject*)&PyPermTypeObject);

	return moduleObj;
}