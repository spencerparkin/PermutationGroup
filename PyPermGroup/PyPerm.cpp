// PyPerm.cpp

#include <Python.h>
#include "PyPerm.h"
#include <sstream>

struct PyPermObject
{
	PyObject_HEAD
	Permutation* permutation;
};

static void PyPermObject_dealloc(PyPermObject* self);
static int PyPermObject_init(PyPermObject* self, PyObject* args, PyObject* kwds);
static PyObject* PyPermObject_define(PyPermObject* self, PyObject* args);
static PyObject* PyPermObject_define_cycle(PyPermObject* self, PyObject* args);
static PyObject* PyPermObject_define_identity(PyPermObject* self, PyObject* args);
static PyObject* PyPermObject_is_valid(PyPermObject* self, PyObject* args);
static PyObject* PyPermObject_is_identity(PyPermObject* self, PyObject* args);
static PyObject* PyPermObject_to_array(PyPermObject* self, PyObject* args);
static PyObject* PyPermObject_from_array(PyPermObject* self, PyObject* args);
static PyObject* PyPermObject_to_json(PyPermObject* self, PyObject* args);
static PyObject* PyPermObject_from_json(PyPermObject* self, PyObject* args);
static PyObject* PyPermObject_clone(PyPermObject* self, PyObject* args);
static PyObject* PyPermObject_overload_multiply(PyObject* leftObject, PyObject* rightObject);
static PyObject* PyPermObject_overload_invert(PyObject* object);
static PyObject* PyPermObject_overload_str(PyObject* object);
static PyObject* PyPermObject_overload_repr(PyObject* object);
static PyObject* PyPermObject_overload_new(PyTypeObject* type, PyObject* args, PyObject* kwds);
static PyObject* PyPermObject_overload_call(PyObject* callable_object, PyObject* args, PyObject* kw);
static bool _PyPermObject_populate(PyPermObject* self, PyObject* perm_array_obj);

PyMethodDef PyPermObject_methods[] =
{
	{"define", (PyCFunction)PyPermObject_define, METH_VARARGS, ""},
	{"define_cycle", (PyCFunction)PyPermObject_define_cycle, METH_VARARGS, ""},
	{"define_identity", (PyCFunction)PyPermObject_define_identity, METH_VARARGS, ""},
	{"is_valid", (PyCFunction)PyPermObject_is_valid, METH_VARARGS, ""},
	{"is_identity", (PyCFunction)PyPermObject_is_identity, METH_VARARGS, ""},
	{"to_array", (PyCFunction)PyPermObject_to_array, METH_VARARGS, ""},
	{"from_array", (PyCFunction)PyPermObject_from_array, METH_VARARGS, ""},
	{"to_json", (PyCFunction)PyPermObject_to_json, METH_VARARGS, ""},
	{"from_json", (PyCFunction)PyPermObject_from_json, METH_VARARGS, ""},
	{"clone", (PyCFunction)PyPermObject_clone, METH_VARARGS, ""},
	{nullptr, nullptr, 0, nullptr}
};

PyNumberMethods PyPermObject_numberMethods =
{
	nullptr,											// nb_add
	nullptr,											// nb_subtract
	PyPermObject_overload_multiply,						// nb_multiply
	nullptr,											// nb_remainder
	nullptr,											// nb_divmod
	nullptr,											// nb_power
	nullptr,											// nb_negative
	nullptr,											// nb_positive
	nullptr,											// nb_absolute
	nullptr,											// nb_bool
	PyPermObject_overload_invert,						// nb_invert
	nullptr,											// nb_lshift
	nullptr,											// nb_rshift
	nullptr,											// nb_and
	nullptr,											// nb_xor
	nullptr,											// nb_or
	nullptr,											// nb_int
	nullptr,											// nb_reserved
	nullptr,											// nb_float
	nullptr,											// nb_inplace_add
	nullptr,											// nb_inplace_subtract
	nullptr,											// nb_inplace_multiply
	nullptr,											// nb_inplace_remainder
	nullptr,											// nb_inplace_power
	nullptr,											// nb_inplace_lshift
	nullptr,											// nb_inplace_rshift
	nullptr,											// nb_inplace_and
	nullptr,											// nb_inplace_xor
	nullptr,											// nb_inplace_or
	nullptr,											// nb_floor_divide
	nullptr,											// nb_true_divide
	nullptr,											// nb_inplace_floor_divide
	nullptr,											// nb_inplace_true_divide
	nullptr,											// nb_index
	nullptr,											// nb_matrix_multiply
	nullptr,											// nb_inplace_matrix_multiply
};

PyTypeObject PyPermTypeObject =
{
	PyVarObject_HEAD_INIT(nullptr, 0)
	"Permutation",										// tp_name
	sizeof(PyPermObject),								// tp_basicsize
	0,													// tp_itemsize
	(destructor)PyPermObject_dealloc,					// tp_dealloc
	0,													// tp_print
	0,													// tp_getattr
	0,													// tp_setattr
	0,													// tp_reserved
	PyPermObject_overload_repr,							// tp_repr
	&PyPermObject_numberMethods,						// tp_as_number
	0,													// tp_as_sequence
	0,													// tp_as_mapping
	0,													// tp_hash
	PyPermObject_overload_call,							// tp_call
	PyPermObject_overload_str,							// tp_str
	0,													// tp_getattro
	0,													// tp_setattro
	0,													// tp_as_buffer
	Py_TPFLAGS_DEFAULT,									// tp_flags
	"Permutations objects",								// tp_doc
	nullptr,											// tp_traverse
	nullptr,											// tp_clear
	nullptr,											// tp_richcompare
	0,													// tp_weaklistoffset
	nullptr,											// tp_iter
	nullptr,											// tp_iternext
	PyPermObject_methods,								// tp_methods
	nullptr,											// tp_members
	nullptr,											// tp_getset
	nullptr,											// tp_base
	nullptr,											// tp_dict
	nullptr,											// tp_descr_get
	nullptr,											// tp_descr_set
	0,													// tp_dictoffset
	(initproc)PyPermObject_init,						// tp_init
	nullptr,											// tp_alloc
	PyPermObject_overload_new,							// tp_new
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

Permutation* Permutation_from_PyObject(PyObject* object)
{
	if(PyObject_TypeCheck(object, &PyPermTypeObject))
		return ((PyPermObject*)object)->permutation;
	return nullptr;
}

PyObject* Permutation_to_PyObject(Permutation* permutation)
{
	PyPermObject* object = (PyPermObject*)PyPermObject_overload_new(&PyPermTypeObject, nullptr, nullptr);
	object->permutation = permutation;
	return (PyObject*)object;
}

static void PyPermObject_dealloc(PyPermObject* self)
{
	delete self->permutation;
	Py_TYPE(self)->tp_free((PyObject*)self);
}

static bool _PyPermObject_populate(PyPermObject* self, PyObject* perm_array_obj)
{
	if(!PyList_Check(perm_array_obj))
	{
		PyErr_SetString(PyExc_TypeError, "Expected an array object.");
		return false;
	}

	unsigned int count = (unsigned int)PyList_Size(perm_array_obj);
	if(count > MAX_MAP_SIZE)
	{
		PyErr_Format(PyExc_ValueError, "Hard limit (%d) on permutation size reached.", MAX_MAP_SIZE);
		return false;
	}

	for(unsigned int input = 0; input < count; input++)
	{
		PyObject* obj = PyList_GetItem(perm_array_obj, input);
		if(!PyLong_Check(obj))
		{
			PyErr_SetString(PyExc_TypeError, "Expected integer element in array.");
			return false;
		}

		unsigned int output = (unsigned int)PyLong_AsSize_t(obj);
		if(!self->permutation->Define(input, output))
		{
			PyErr_Format(PyExc_ValueError, "Can't map from %d to %d.", input, output);
			return false;
		}
	}

	return true;
}

static int PyPermObject_init(PyPermObject* self, PyObject* args, PyObject* kwds)
{
	delete self->permutation;
	self->permutation = new Permutation();

	PyObject* perm_array_obj = nullptr;
	PyObject* perm_obj = nullptr;

	static const char* kwlist[] = {"perm_obj", "perm_array", nullptr};

	if(!PyArg_ParseTupleAndKeywords(args, kwds, "|OO", (char**)kwlist, &perm_array_obj, &perm_obj))
	{
		PyErr_SetString(PyExc_ValueError, "Failed to parse keyword arguments.");
		return -1;
	}

	if(perm_obj)
	{
		if(!PyObject_TypeCheck(perm_obj, &PyPermTypeObject))
		{
			PyErr_SetString(PyExc_TypeError, "Expected a permutation object.");
			return -1;
		}

		self->permutation->SetCopy(*((PyPermObject*)perm_obj)->permutation);
	}
	else if(perm_array_obj)
	{
		if(!_PyPermObject_populate(self, perm_array_obj))
			return -1;
	}
	
	if(!self->permutation->IsValid())
	{
		PyErr_SetString(PyExc_ValueError, "Invalid permutation.");
		return -1;
	}

	return 0;
}

static PyObject* PyPermObject_define(PyPermObject* self, PyObject* args)
{
	unsigned int input, output;

	if(!PyArg_ParseTuple(args, "II", &input, &output))
		return nullptr;

	if(!self->permutation->Define(input, output))
	{
		PyErr_Format(PyExc_ValueError, "Can't map from %d to %d.", input, output);
		return nullptr;
	}

	Py_INCREF(self);
	return (PyObject*)self;
}

static PyObject* PyPermObject_define_cycle(PyPermObject* self, PyObject* args)
{
	PyObject* cycle_list_obj = nullptr;

	if(!PyArg_ParseTuple(args, "O", &cycle_list_obj))
	{
		PyErr_SetString(PyExc_TypeError, "Failed to parse arguments.");
		return nullptr;
	}

	if(!PyList_Check(cycle_list_obj))
	{
		PyErr_SetString(PyExc_TypeError, "Expected list object as argument.");
		return nullptr;
	}

	unsigned int count = (unsigned int)PyList_Size(cycle_list_obj);
	for(unsigned int i = 0; i < count; i++)
	{
		PyObject* obj = PyList_GetItem(cycle_list_obj, i);
		unsigned int input = (unsigned int)PyLong_AsSize_t(obj);
		unsigned int j = (i + 1) % count;
		obj = PyList_GetItem(cycle_list_obj, j);
		unsigned int output = (unsigned int)PyLong_AsSize_t(obj);

		if(!self->permutation->Define(input, output))
		{
			PyErr_Format(PyExc_ValueError, "Can't map from %d to %d.", input, output);
			return nullptr;
		}
	}

	Py_INCREF(self);
	return (PyObject*)self;
}

static PyObject* PyPermObject_define_identity(PyPermObject* self, PyObject* args)
{
	self->permutation->DefineIdentity();
	Py_INCREF(self);
	return (PyObject*)self;
}

static PyObject* PyPermObject_is_valid(PyPermObject* self, PyObject* args)
{
	if(self->permutation->IsValid())
		Py_RETURN_TRUE;
	Py_RETURN_FALSE;
}

static PyObject* PyPermObject_is_identity(PyPermObject* self, PyObject* args)
{
	if(self->permutation->IsIdentity())
		Py_RETURN_TRUE;
	Py_RETURN_FALSE;
}

static PyObject* PyPermObject_to_array(PyPermObject* self, PyObject* args)
{
	PyObject* perm_array = PyList_New(MAX_MAP_SIZE);

	for(unsigned int input = 0; input < MAX_MAP_SIZE; input++)
	{
		unsigned int output = self->permutation->map[input];
		PyObject* obj = PyLong_FromSize_t(output);
		PyList_SetItem(perm_array, input, obj);
	}

	return perm_array;
}

static PyObject* PyPermObject_from_array(PyPermObject* self, PyObject* args)
{
	PyObject* perm_list_obj = nullptr;

	if(!PyArg_ParseTuple(args, "O", &perm_list_obj))
	{
		PyErr_SetString(PyExc_TypeError, "Failed to parse arguments.");
		return nullptr;
	}

	if(!_PyPermObject_populate(self, perm_list_obj))
		return nullptr;

	Py_INCREF(self);
	return (PyObject*)self;
}

static PyObject* PyPermObject_to_json(PyPermObject* self, PyObject* args)
{
	std::string jsonString;
	if(!self->permutation->SaveToJsonString(jsonString))
	{
		PyErr_SetString(PyExc_ValueError, "Failed to generate json text from permutation.");
		return nullptr;
	}

	PyObject* json_obj = PyUnicode_FromString(jsonString.c_str());
	return json_obj;
}

static PyObject* PyPermObject_from_json(PyPermObject* self, PyObject* args)
{
	const char* json_c_str = nullptr;

	if(!PyArg_ParseTuple(args, "s", &json_c_str))
	{
		PyErr_SetString(PyExc_ValueError, "Expected string argument.");
		return nullptr;
	}

	std::string jsonString(json_c_str);
	if(!self->permutation->LoadFromJsonString(jsonString))
	{
		PyErr_SetString(PyExc_ValueError, "Failed to generate permutation from json text.");
		return nullptr;
	}

	Py_RETURN_NONE;
}

static PyObject* PyPermObject_clone(PyPermObject* self, PyObject* args)
{
	if(self->permutation == nullptr)
	{
		PyErr_SetString(PyExc_ValueError, "Can't clone null permutation.");
		return nullptr;
	}

	Permutation* permutation = new Permutation();
	self->permutation->GetCopy(*permutation);
	return Permutation_to_PyObject(permutation);
}

static PyObject* PyPermObject_overload_multiply(PyObject* leftObject, PyObject* rightObject)
{
	if(!PyObject_TypeCheck(leftObject, &PyPermTypeObject) || !PyObject_TypeCheck(rightObject, &PyPermTypeObject))
	{
		PyErr_SetString(PyExc_TypeError, "Can only multiply permutation objects.");
		return nullptr;
	}

	const Permutation* leftPerm = ((PyPermObject*)leftObject)->permutation;
	const Permutation* rightPerm = ((PyPermObject*)rightObject)->permutation;

	Permutation* product = new Permutation();
	product->Multiply(*leftPerm, *rightPerm);

	PyObject* product_obj = Permutation_to_PyObject(product);
	Py_INCREF(product_obj);
	return product_obj;
}

static PyObject* PyPermObject_overload_invert(PyObject* object)
{
	Permutation* permutation = Permutation_from_PyObject(object);
	if(permutation == nullptr)
	{
		PyErr_SetString(PyExc_TypeError, "Failed to cast object to permutation.");
		return nullptr;
	}

	Permutation* invPermutation = new Permutation();
	if(!permutation->GetInverse(*invPermutation))
	{
		delete invPermutation;
		PyErr_SetString(PyExc_ValueError, "Failed to find permutation inverse; the permuation must be invalid.");
		return nullptr;
	}

	return Permutation_to_PyObject(invPermutation);
}

static PyObject* PyPermObject_overload_str(PyObject* object)
{
	if(!PyObject_TypeCheck(object, &PyPermTypeObject))
	{
		PyErr_SetString(PyExc_TypeError, "Expected a permutation object.");
		return nullptr;
	}

	std::stringstream stream;
	((PyPermObject*)object)->permutation->Print(stream);

	PyObject* result = PyUnicode_FromString(stream.str().c_str());
	return result;
}

static PyObject* PyPermObject_overload_repr(PyObject* object)
{
	return PyPermObject_overload_str(object);
}

static PyObject* PyPermObject_overload_call(PyObject* callable_object, PyObject* args, PyObject* kw)
{
	unsigned int input;

	if(!PyArg_ParseTuple(args, "I", &input))
	{
		PyErr_SetString(PyExc_ValueError, "Failed to parse arguments.");
		return nullptr;
	}

	if(!PyObject_TypeCheck(callable_object, &PyPermTypeObject))
	{
		PyErr_SetString(PyExc_TypeError, "Expected a permutation object.");
		return nullptr;
	}

	unsigned int output = ((PyPermObject*)callable_object)->permutation->Evaluate(input);

	PyObject* result = PyLong_FromSize_t(output);
	return result;
}

static PyObject* PyPermObject_overload_new(PyTypeObject* type, PyObject* args, PyObject* kwds)
{
	PyPermObject* object = (PyPermObject*)type->tp_alloc(type, 0);
	object->permutation = nullptr;
	return (PyObject*)object;
}
