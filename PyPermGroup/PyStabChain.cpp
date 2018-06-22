// PyStabChain.cpp

#include <Python.h>
#include "PyStabChain.h"
#include "PyPerm.h"
#include "PermutationStream.h"
#include <sstream>

struct PyStabChainObject
{
	PyObject_HEAD
	StabilizerChain* stabChain;
};

void PyStabChainObject_dealloc(PyStabChainObject* self);
int PyStabChainObject_init(PyStabChainObject* self, PyObject* args, PyObject* kwds);
PyObject* PyStabChainObject_to_json(PyStabChainObject* self, PyObject* args);
PyObject* PyStabChainObject_from_json(PyStabChainObject* self, PyObject* args);
PyObject* PyStabChainObject_clone(PyStabChainObject* self, PyObject* args);
PyObject* PyStabChainObject_depth(PyStabChainObject* self, PyObject* args);
PyObject* PyStabChainObject_generate(PyStabChainObject* self, PyObject* args);
PyObject* PyStabChainObject_order(PyStabChainObject* self, PyObject* args);
PyObject* PyStabChainObject_walk(PyStabChainObject* self, PyObject* args);
PyObject* PyStabChainObject_overload_str(PyObject* object);
PyObject* PyStabChainObject_overload_new(PyTypeObject* type, PyObject* args, PyObject* kwds);

PyMethodDef PyStabChainObject_methods[] =
{
	{"to_json", (PyCFunction)PyStabChainObject_to_json, METH_VARARGS, ""},
	{"from_json", (PyCFunction)PyStabChainObject_from_json, METH_VARARGS, ""},
	{"clone", (PyCFunction)PyStabChainObject_clone, METH_VARARGS, ""},
	{"depth", (PyCFunction)PyStabChainObject_depth, METH_VARARGS, ""},
	{"generate", (PyCFunction)PyStabChainObject_generate, METH_VARARGS, ""},
	{"order", (PyCFunction)PyStabChainObject_order, METH_VARARGS, ""},
	{"walk", (PyCFunction)PyStabChainObject_walk, METH_VARARGS, ""},
	{nullptr, nullptr, 0, nullptr}
};

PyTypeObject PyStabChainTypeObject =
{
	PyVarObject_HEAD_INIT(nullptr, 0)
	"Stabilizer Chain",									// tp_name
	sizeof(PyStabChainObject),							// tp_basicsize
	0,													// tp_itemsize
	(destructor)PyStabChainObject_dealloc,				// tp_dealloc
	0,													// tp_print
	0,													// tp_getattr
	0,													// tp_setattr
	0,													// tp_reserved
	0,													// tp_repr
	nullptr,											// tp_as_number
	0,													// tp_as_sequence
	0,													// tp_as_mapping
	0,													// tp_hash
	nullptr,											// tp_call
	PyStabChainObject_overload_str,						// tp_str
	0,													// tp_getattro
	0,													// tp_setattro
	0,													// tp_as_buffer
	Py_TPFLAGS_DEFAULT,									// tp_flags
	"Stabilizer Chain objects",							// tp_doc
	nullptr,											// tp_traverse
	nullptr,											// tp_clear
	nullptr,											// tp_richcompare
	0,													// tp_weaklistoffset
	nullptr,											// tp_iter
	nullptr,											// tp_iternext
	PyStabChainObject_methods,							// tp_methods
	nullptr,											// tp_members
	nullptr,											// tp_getset
	nullptr,											// tp_base
	nullptr,											// tp_dict
	nullptr,											// tp_descr_get
	nullptr,											// tp_descr_set
	0,													// tp_dictoffset
	(initproc)PyStabChainObject_init,					// tp_init
	nullptr,											// tp_alloc
	PyStabChainObject_overload_new,						// tp_new
	nullptr,											// tp_free
	nullptr,											// tp_is_gc
	nullptr,											// tp_bases
	nullptr,											// tp_mro
	nullptr,											// tp_cache
	nullptr,											// tp_subclasses
	nullptr,											// tp_weaklist
	nullptr,											// tp_del
	0,													// tp_version_tag
	nullptr,											// tp_finalize
};

StabilizerChain* StabChain_from_PyObject(PyObject* object)
{
	if(PyObject_TypeCheck(object, &PyStabChainTypeObject))
		return ((PyStabChainObject*)object)->stabChain;
	return nullptr;
}

PyObject* StabChain_to_PyObject(StabilizerChain* stabChain)
{
	PyStabChainObject* object = (PyStabChainObject*)PyStabChainObject_overload_new(&PyStabChainTypeObject, nullptr, nullptr);
	object->stabChain = stabChain;
	return (PyObject*)object;
}

static void PyStabChainObject_dealloc(PyStabChainObject* self)
{
	delete self->stabChain;
	Py_TYPE(self)->tp_free((PyObject*)self);
}

static int PyStabChainObject_init(PyStabChainObject* self, PyObject* args, PyObject* kwds)
{
	delete self->stabChain;
	self->stabChain = new StabilizerChain();
	return 0;
}

static PyObject* PyStabChainObject_to_json(PyStabChainObject* self, PyObject* args)
{
	std::string jsonString;

	if(!self->stabChain->SaveToJsonString(jsonString))
	{
		PyErr_SetString(PyExc_ValueError, "Failed to serialize stab-chain.");
		return nullptr;
	}

	PyObject* result = PyBytes_FromString(jsonString.c_str());
	return result;
}

static PyObject* PyStabChainObject_from_json(PyStabChainObject* self, PyObject* args)
{
	const char* jsonStr = nullptr;

	if(!PyArg_ParseTuple(args, "s", &jsonStr))
	{
		PyErr_SetString(PyExc_ValueError, "Expected string.");
		return nullptr;
	}

	if(!self->stabChain->LoadFromJsonString(jsonStr))
	{
		PyErr_SetString(PyExc_ValueError, "Failed to deserialize stab-chain.");
		return nullptr;
	}

	Py_RETURN_NONE;
}

static PyObject* PyStabChainObject_clone(PyStabChainObject* self, PyObject* args)
{
	if(self->stabChain == nullptr)
	{
		PyErr_SetString(PyExc_ValueError, "Can't clone null stab-chain.");
		return nullptr;
	}

	StabilizerChain* stabChain = self->stabChain->Clone();
	return StabChain_to_PyObject(stabChain);
}

static PyObject* PyStabChainObject_depth(PyStabChainObject* self, PyObject* args)
{
	uint depth = self->stabChain->Depth();
	PyObject* depth_obj = PyLong_FromSize_t((signed)depth);
	return depth_obj;
}

static PyObject* PyStabChainObject_generate(PyStabChainObject* self, PyObject* args)
{
	PyObject* generator_list_obj = nullptr;
	PyObject* base_array_obj = nullptr;

	if(!PyArg_ParseTuple(args, "OO", &generator_list_obj, &base_array_obj))
	{
		PyErr_SetString(PyExc_ValueError, "Failed to parse arguments.");
		return nullptr;
	}

	if(!PyList_Check(generator_list_obj))
	{
		PyErr_SetString(PyExc_TypeError, "Expected first argument to be a list of permutations (generators of the group.)");
		return nullptr;
	}

	if(!PyList_Check(base_array_obj))
	{
		PyErr_SetString(PyExc_TypeError, "Expected second argument to be a list of integers (the base array.)");
		return nullptr;
	}

	PermutationSet generatorSet;
	uint count = PyList_Size(generator_list_obj);
	for(uint i = 0; i < count; i++)
	{
		PyObject* perm_obj = PyList_GetItem(generator_list_obj, i);
		if(!PyObject_TypeCheck(perm_obj, &PyPermTypeObject))
		{
			PyErr_SetString(PyExc_TypeError, "Expected a permutation in the given list of generators.");
			return nullptr;
		}

		generatorSet.insert(*Permutation_from_PyObject(perm_obj));
	}

	UintArray baseArray;
	count = PyList_Size(base_array_obj);
	for(uint i = 0; i < count; i++)
	{
		PyObject* point_obj = PyList_GetItem(base_array_obj, i);
		if(!PyLong_Check(point_obj))
		{
			PyErr_SetString(PyExc_TypeError, "Expected list of integers in the given base array.");
			return nullptr;
		}

		baseArray.push_back((uint)PyLong_AsSize_t(point_obj));
	}

	if(!self->stabChain->Generate(generatorSet, baseArray))
	{
		PyErr_SetString(PyExc_ValueError, "Failed to generated stabilizer chain.");
		return nullptr;
	}

	Py_RETURN_NONE;
}

static PyObject* PyStabChainObject_order(PyStabChainObject* self, PyObject* args)
{
	if(!self->stabChain->group)
	{
		PyErr_SetString(PyExc_ValueError, "No group has been generated for the stab-chain.");
		return nullptr;
	}

	unsigned long long order = self->stabChain->group->Order();
	PyObject* order_obj = PyLong_FromLongLong((signed)order);
	return order_obj;
}

static PyObject* PyStabChainObject_walk(PyStabChainObject* self, PyObject* args)
{
	PyObject* callback_obj = nullptr;

	if(!PyArg_ParseTuple(args, "O", callback_obj))
	{
		PyErr_SetString(PyExc_TypeError, "Failed to parse arguments.");
		return nullptr;
	}

	if(!PyCallable_Check(callback_obj))
	{
		PyErr_SetString(PyExc_TypeError, "Expected callable to be given.");
		return nullptr;
	}

	PermutationProductStream productStream;
	productStream.Configure(self->stabChain);

	Permutation permutation;
	while(productStream.OutputPermutation(permutation) && !productStream.wrapped)
	{
		Permutation* newPermutation = new Permutation();
		newPermutation->SetCopy(permutation);
		PyObject* perm_obj = Permutation_to_PyObject(newPermutation);
		Py_INCREF(perm_obj);

		PyObject* args = PyTuple_New(1);
		PyTuple_SetItem(args, 0, perm_obj);

		PyObject* result = PyObject_Call(callback_obj, args, nullptr);
		int bail = PyObject_IsTrue(result);
		Py_DECREF(result);
		if(bail)
			break;
	}

	Py_RETURN_NONE;
}

static PyObject* PyStabChainObject_overload_str(PyObject* object)
{
	// This is probably unecessary since we're only bound to our stab-chain type.
	if(!PyObject_TypeCheck(object, &PyStabChainTypeObject))
	{
		PyErr_SetString(PyExc_TypeError, "Expected stab-chain object.");
		return nullptr;
	}

	std::stringstream stream;
	((PyStabChainObject*)object)->stabChain->Print(stream);

	PyObject* result = PyBytes_FromString(stream.str().c_str());
	return result;
}

static PyObject* PyStabChainObject_overload_new(PyTypeObject* type, PyObject* args, PyObject* kwds)
{
	PyStabChainObject* object = (PyStabChainObject*)type->tp_alloc(type, 0);
	object->stabChain = nullptr;
	return (PyObject*)object;
}