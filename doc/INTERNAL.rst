=================
Pythran Internals
=================

This document describes some internals of Pytrhan compiler.

Pythran pass management is used throughout the document::

    >>> from pythran import passmanager, analysis, passes, optimizations, backend
    >>> pm = passmanager.PassManager('dummy')

To retrieve the code source from a function definition, the ``inspect`` module
is used::

    >>> from inspect import getsource

And to turn source code into an AST(Abstract Syntax tree), Python provides the
``ast`` module::

    >>> import ast
    >>> getast = lambda f: ast.parse(getsource(f))

Scoping
-------

There are only two scopes in Python: ``globals()`` and ``locals()``. When
generating C++ code, Pythran tries its best not to declare variables at the
function level, but using the deepest scope. This provides two benefits:

1. It makes writing OpenMP clauses easier, as local variables are automatically
   marked as private;
2. It avoids to build variables with the empty constructor then assigning them a
   value.

Let's illustrate this with two simple examples. In the following function,
variable ``a`` has to be declared outside of the ``if`` statement::

    >>> def foo(n):
    ...     if n:
    ...         a = 1
    ...     else:
    ...         a = 2
    ...     return n*a
        
When computing variable scope, one gets a dictionary binding variable names to
their defining statement::

    >>> foo_tree = getast(foo)
    >>> scopes = pm.gather(analysis.Scope, foo_tree)

``n`` is a formal parameter, so it has function scope::

    >>> print type(scopes['n'])
    <class '_ast.FunctionDef'>

``a`` is used at the function scope (in the ``return`` statement), so even if
it's declared in an ``if`` it has function scope too::

    >>> print type(scopes['a'])
    <class '_ast.FunctionDef'>

Now let's see what happen if we had a loop to the function::

    >>> def foo(n):
    ...     s = 0
    ...     for i in __builtin__.range(n):
    ...         if i:
    ...             a = 1
    ...         else:
    ...             a = 2
    ...         s *= a
    ...     return s
    >>> foo_tree = getast(foo)
    >>> scopes = pm.gather(analysis.Scope, foo_tree)

Variable ``a`` is only used in the loop body, so one can declare it inside the
loop::

    >>> print type(scopes['a'])
    <class '_ast.For'>

In a similar manner, the iteration variable ``i`` gets a new value at each
iteration step, and is declared at the loop level::

    >>> print type(scopes['i'])
    <class '_ast.For'>

OpenMP directives interacts a lot with scoping. In C or C++, variables declared
inside a parallel region are automatically marked as private. Pythran emulates
this whenever possible::

    >>> def foo(n):
    ...     s = 0
    ...     "omp parallel for reduction(*:s)"
    ...     for i in __builtin__.range(n):
    ...         if i:
    ...             a = 1
    ...         else:
    ...             a = 2
    ...         s *= a
    ...     return s

Without scoping directive, both ``i`` and ``a`` are private::

    >>> foo_tree = getast(foo)
    >>> scopes = pm.gather(analysis.Scope, foo_tree)
    >>> map(type, (scopes['a'], scopes['i']))
    [<class '_ast.For'>, <class '_ast.For'>]

But if one adds a
``lastprivate`` clause, as in::

    >>> def foo(n):
    ...     s = 0
    ...     "omp parallel for reduction(*:s) lastprivate(a)"
    ...     for i in __builtin__.range(n):
    ...         if i:
    ...             a = 1
    ...         else:
    ...             a = 2
    ...         s *= a
    ...     return s
    >>> foo_tree = getast(foo)

The scope information change. Pythran first needs to understand OpenMP
directives, using a dedicated pass::

    >>> from pythran import openmp
    >>> _ = pm.apply(openmp.GatherOMPData, foo_tree)

Then let's have a look to ::

    >>> scopes = pm.gather(analysis.Scope, foo_tree)
    >>> map(type, (scopes['a'], scopes['i']))
    [<class '_ast.FunctionDef'>, <class '_ast.For'>]

``a`` now has function scope, which makes the OpenMP directive legal.

When the scope can be attached to an assignment, Pythran uses this piece of information::

    >>> def foo(n):
    ...     s = 0
    ...     "omp parallel for reduction(*:s)"
    ...     for i in __builtin__.range(n):
    ...         a = 2
    ...         s *= a
    ...     return s
    >>> foo_tree = getast(foo)
    >>> _ = pm.apply(openmp.GatherOMPData, foo_tree)
    >>> scopes = pm.gather(analysis.Scope, foo_tree)
    >>> print type(scopes['a'])
    <class '_ast.Assign'>

Additionnaly, some OpenMP directives, when applied to a single statement, are
treated by Pythran as if they created a bloc, emulated by a dummy
conditionnal::

    >>> def foo(n):
    ...     "omp parallel"
    ...     "omp single"
    ...     s = 1
    ...     return s
    >>> foo_tree = getast(foo)
    >>> _ = pm.apply(openmp.GatherOMPData, foo_tree)
    >>> print pm.dump(backend.Python, foo_tree)
    def foo(n):
        if 1:
            s = 1
        return s

Note that the OpenMP directives are not pretty-printed by the Python backend!
However the additionnal if bloc makes it clear that ``s`` should have function
scope, and the scope is not attached to the first assignment::

    >>> scopes = pm.gather(analysis.Scope, foo_tree)
    >>> print type(scopes['s'])
    <class '_ast.FunctionDef'>
