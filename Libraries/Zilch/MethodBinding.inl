// MIT Licensed (see LICENSE.md).


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Bound Static
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename FunctionType, FunctionType function, typename... Args>
static void BoundStatic(Call& call, ExceptionReport& report) 
{
  constexpr std::size_t numberOfArguments = sizeof...(Args);

  if constexpr (0 != numberOfArguments)
  {
    std::size_t i = 0;
    
    byte* byteArgs[numberOfArguments]{call.GetArgumentPointer<Details::GetBindingType<Args>>(i++)...};
    
    if (report.HasThrownExceptions())
    {
      return;
    }
    
    i = 0;
    
    function(call.CastArgumentPointer<Details::GetBindingType<Args>>(byteArgs[i++])...);
  }
  else
  {
    function();
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Bound Static Return
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename FunctionType, FunctionType function, typename Return, typename... Args>
static void BoundStaticReturn(Call& call, ExceptionReport& report)
{
  constexpr std::size_t numberOfArguments = sizeof...(Args);

  if constexpr (0 != numberOfArguments)
  {
    std::size_t i = 0;

    byte* byteArgs[numberOfArguments]{call.GetArgumentPointer<Details::GetBindingType<Args>>(i++)...};

    if (report.HasThrownExceptions())
    {
      return;
    }

    i = 0;

    Return result = function(call.CastArgumentPointer<Details::GetBindingType<Args>>(byteArgs[i++])...);

    if (report.HasThrownExceptions())
    {
      return;
    }

    call.Set<Return>(Call::Return, result);
  }
  else
  {
    Return result = function();

    if (report.HasThrownExceptions())
    {
      return;
    }

    call.Set<Return>(Call::Return, result);
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Bound Instance
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename FunctionType, FunctionType function, typename Class, typename... Args>
static void BoundInstance(Call& call, ExceptionReport& report)
{
  constexpr std::size_t numberOfArguments = sizeof...(Args);

  if constexpr (0 != numberOfArguments)
  {
    std::size_t i = 0;

    byte* byteArgs[numberOfArguments]{call.GetArgumentPointer<Details::GetBindingType<Args>>(i++)...};

    if (report.HasThrownExceptions())
    {
      return;
    }

    Class* self = (Class*)call.GetHandle(Call::This).Dereference();

    i = 0;

    (self->*function)(call.CastArgumentPointer<Details::GetBindingType<Args>>(byteArgs[i++])...);
  }
  else
  {
    Class* self = (Class*)call.GetHandle(Call::This).Dereference();

    (self->*function)();
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Bound Instance Return
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename FunctionType, FunctionType function, typename Class, typename Return, typename... Args>
static void BoundInstanceReturn(Call& call, ExceptionReport& report)
{
  constexpr std::size_t numberOfArguments = sizeof...(Args);

  if constexpr (0 != numberOfArguments)
  {
    std::size_t i = 0;

    byte* byteArgs[numberOfArguments]{call.GetArgumentPointer<Details::GetBindingType<Args>>(i++)...};

    if (report.HasThrownExceptions())
    {
      return;
    }

    Class* self = (Class*)call.GetHandle(Call::This).Dereference();

    i = 0;

    Return result = (self->*function)(call.CastArgumentPointer<Details::GetBindingType<Args>>(byteArgs[i++])...);

    if (report.HasThrownExceptions())
    {
      return;
    }

    call.Set<Return>(Call::Return, result);
  }
  else
  {
    Class* self = (Class*)call.GetHandle(Call::This).Dereference();

    Return result = (self->*function)();

    if (report.HasThrownExceptions())
    {
      return;
    }

    call.Set<Return>(Call::Return, result);
  }
}




















































/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Bound Static
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename FunctionType, FunctionType function>
static Function* FromMethod(
    LibraryBuilder& builder, BoundType* classBoundType, StringRange name, StringRange spaceDelimitedNames, void (*)())
{
  BoundFn boundFunction = FunctionBinding<FunctionType>::template GetFunctionInvoker<function>();

  ParameterArray parameters;
  ParseParameterArrays(parameters, spaceDelimitedNames);
  return builder.AddBoundFunction(
      classBoundType, name, boundFunction, parameters, ZilchTypeId(void), FunctionOptions::Static);
}

template <typename FunctionType, FunctionType function, typename Arg0>
static Function* FromMethod(LibraryBuilder& builder,
                            BoundType* classBoundType,
                            StringRange name,
                            StringRange spaceDelimitedNames,
                            void (*)(Arg0))
{
  BoundFn boundFunction = FunctionBinding<FunctionType>::template GetFunctionInvoker<function>();
  ParameterArray parameters;
  DelegateParameter& p0 = parameters.PushBack();
  p0.ParameterType = ZilchTypeId(Arg0);
  ParseParameterArrays(parameters, spaceDelimitedNames);
  return builder.AddBoundFunction(
      classBoundType, name, boundFunction, parameters, ZilchTypeId(void), FunctionOptions::Static);
}

template <typename FunctionType, FunctionType function, typename Arg0, typename Arg1>
static Function* FromMethod(LibraryBuilder& builder,
                            BoundType* classBoundType,
                            StringRange name,
                            StringRange spaceDelimitedNames,
                            void (*)(Arg0, Arg1))
{
  BoundFn boundFunction = FunctionBinding<FunctionType>::template GetFunctionInvoker<function>();
  ParameterArray parameters;
  DelegateParameter& p0 = parameters.PushBack();
  p0.ParameterType = ZilchTypeId(Arg0);
  DelegateParameter& p1 = parameters.PushBack();
  p1.ParameterType = ZilchTypeId(Arg1);
  ParseParameterArrays(parameters, spaceDelimitedNames);
  return builder.AddBoundFunction(
      classBoundType, name, boundFunction, parameters, ZilchTypeId(void), FunctionOptions::Static);
}

template <typename FunctionType, FunctionType function, typename Arg0, typename Arg1, typename Arg2>
static Function* FromMethod(LibraryBuilder& builder,
                            BoundType* classBoundType,
                            StringRange name,
                            StringRange spaceDelimitedNames,
                            void (*)(Arg0, Arg1, Arg2))
{
  BoundFn boundFunction = FunctionBinding<FunctionType>::template GetFunctionInvoker<function>();
  ParameterArray parameters;
  DelegateParameter& p0 = parameters.PushBack();
  p0.ParameterType = ZilchTypeId(Arg0);
  DelegateParameter& p1 = parameters.PushBack();
  p1.ParameterType = ZilchTypeId(Arg1);
  DelegateParameter& p2 = parameters.PushBack();
  p2.ParameterType = ZilchTypeId(Arg2);
  ParseParameterArrays(parameters, spaceDelimitedNames);
  return builder.AddBoundFunction(
      classBoundType, name, boundFunction, parameters, ZilchTypeId(void), FunctionOptions::Static);
}

template <typename FunctionType, FunctionType function, typename Arg0, typename Arg1, typename Arg2, typename Arg3>
static Function* FromMethod(LibraryBuilder& builder,
                            BoundType* classBoundType,
                            StringRange name,
                            StringRange spaceDelimitedNames,
                            void (*)(Arg0, Arg1, Arg2, Arg3))
{
  BoundFn boundFunction = FunctionBinding<FunctionType>::template GetFunctionInvoker<function>();
  ParameterArray parameters;
  DelegateParameter& p0 = parameters.PushBack();
  p0.ParameterType = ZilchTypeId(Arg0);
  DelegateParameter& p1 = parameters.PushBack();
  p1.ParameterType = ZilchTypeId(Arg1);
  DelegateParameter& p2 = parameters.PushBack();
  p2.ParameterType = ZilchTypeId(Arg2);
  DelegateParameter& p3 = parameters.PushBack();
  p3.ParameterType = ZilchTypeId(Arg3);
  ParseParameterArrays(parameters, spaceDelimitedNames);
  return builder.AddBoundFunction(
      classBoundType, name, boundFunction, parameters, ZilchTypeId(void), FunctionOptions::Static);
}

template <typename FunctionType,
          FunctionType function,
          typename Arg0,
          typename Arg1,
          typename Arg2,
          typename Arg3,
          typename Arg4>
static Function* FromMethod(LibraryBuilder& builder,
                            BoundType* classBoundType,
                            StringRange name,
                            StringRange spaceDelimitedNames,
                            void (*)(Arg0, Arg1, Arg2, Arg3, Arg4))
{
  BoundFn boundFunction = FunctionBinding<FunctionType>::template GetFunctionInvoker<function>();
  ParameterArray parameters;
  DelegateParameter& p0 = parameters.PushBack();
  p0.ParameterType = ZilchTypeId(Arg0);
  DelegateParameter& p1 = parameters.PushBack();
  p1.ParameterType = ZilchTypeId(Arg1);
  DelegateParameter& p2 = parameters.PushBack();
  p2.ParameterType = ZilchTypeId(Arg2);
  DelegateParameter& p3 = parameters.PushBack();
  p3.ParameterType = ZilchTypeId(Arg3);
  DelegateParameter& p4 = parameters.PushBack();
  p4.ParameterType = ZilchTypeId(Arg4);
  ParseParameterArrays(parameters, spaceDelimitedNames);
  return builder.AddBoundFunction(
      classBoundType, name, boundFunction, parameters, ZilchTypeId(void), FunctionOptions::Static);
}

template <typename FunctionType,
          FunctionType function,
          typename Arg0,
          typename Arg1,
          typename Arg2,
          typename Arg3,
          typename Arg4,
          typename Arg5>
static Function* FromMethod(LibraryBuilder& builder,
                            BoundType* classBoundType,
                            StringRange name,
                            StringRange spaceDelimitedNames,
                            void (*)(Arg0, Arg1, Arg2, Arg3, Arg4, Arg5))
{
  BoundFn boundFunction = FunctionBinding<FunctionType>::template GetFunctionInvoker<function>();
  ParameterArray parameters;
  DelegateParameter& p0 = parameters.PushBack();
  p0.ParameterType = ZilchTypeId(Arg0);
  DelegateParameter& p1 = parameters.PushBack();
  p1.ParameterType = ZilchTypeId(Arg1);
  DelegateParameter& p2 = parameters.PushBack();
  p2.ParameterType = ZilchTypeId(Arg2);
  DelegateParameter& p3 = parameters.PushBack();
  p3.ParameterType = ZilchTypeId(Arg3);
  DelegateParameter& p4 = parameters.PushBack();
  p4.ParameterType = ZilchTypeId(Arg4);
  DelegateParameter& p5 = parameters.PushBack();
  p5.ParameterType = ZilchTypeId(Arg5);
  ParseParameterArrays(parameters, spaceDelimitedNames);
  return builder.AddBoundFunction(
      classBoundType, name, boundFunction, parameters, ZilchTypeId(void), FunctionOptions::Static);
}

template <typename FunctionType,
          FunctionType function,
          typename Arg0,
          typename Arg1,
          typename Arg2,
          typename Arg3,
          typename Arg4,
          typename Arg5,
          typename Arg6>
static Function* FromMethod(LibraryBuilder& builder,
                            BoundType* classBoundType,
                            StringRange name,
                            StringRange spaceDelimitedNames,
                            void (*)(Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6))
{
  BoundFn boundFunction = FunctionBinding<FunctionType>::template GetFunctionInvoker<function>();
  ParameterArray parameters;
  DelegateParameter& p0 = parameters.PushBack();
  p0.ParameterType = ZilchTypeId(Arg0);
  DelegateParameter& p1 = parameters.PushBack();
  p1.ParameterType = ZilchTypeId(Arg1);
  DelegateParameter& p2 = parameters.PushBack();
  p2.ParameterType = ZilchTypeId(Arg2);
  DelegateParameter& p3 = parameters.PushBack();
  p3.ParameterType = ZilchTypeId(Arg3);
  DelegateParameter& p4 = parameters.PushBack();
  p4.ParameterType = ZilchTypeId(Arg4);
  DelegateParameter& p5 = parameters.PushBack();
  p5.ParameterType = ZilchTypeId(Arg5);
  DelegateParameter& p6 = parameters.PushBack();
  p6.ParameterType = ZilchTypeId(Arg6);
  ParseParameterArrays(parameters, spaceDelimitedNames);
  return builder.AddBoundFunction(
      classBoundType, name, boundFunction, parameters, ZilchTypeId(void), FunctionOptions::Static);
}


















/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Bound Static Return
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename FunctionType, FunctionType function, typename Return>
static Function* FromMethod(
    LibraryBuilder& builder, BoundType* classBoundType, StringRange name, StringRange spaceDelimitedNames, Return (*)())
{
  BoundFn boundFunction = FunctionBinding<FunctionType>::template GetFunctionInvoker<function>();
  ParameterArray parameters;
  ParseParameterArrays(parameters, spaceDelimitedNames);
  return builder.AddBoundFunction(
      classBoundType, name, boundFunction, parameters, ZilchTypeId(Return), FunctionOptions::Static);
}

template <typename FunctionType, FunctionType function, typename Return, typename Arg0>
static Function* FromMethod(LibraryBuilder& builder,
                            BoundType* classBoundType,
                            StringRange name,
                            StringRange spaceDelimitedNames,
                            Return (*)(Arg0))
{
  BoundFn boundFunction = FunctionBinding<FunctionType>::template GetFunctionInvoker<function>();
  ParameterArray parameters;
  DelegateParameter& p0 = parameters.PushBack();
  p0.ParameterType = ZilchTypeId(Arg0);
  ParseParameterArrays(parameters, spaceDelimitedNames);
  return builder.AddBoundFunction(
      classBoundType, name, boundFunction, parameters, ZilchTypeId(Return), FunctionOptions::Static);
}

template <typename FunctionType, FunctionType function, typename Return, typename Arg0, typename Arg1>
static Function* FromMethod(LibraryBuilder& builder,
                            BoundType* classBoundType,
                            StringRange name,
                            StringRange spaceDelimitedNames,
                            Return (*)(Arg0, Arg1))
{
  BoundFn boundFunction = FunctionBinding<FunctionType>::template GetFunctionInvoker<function>();
  ParameterArray parameters;
  DelegateParameter& p0 = parameters.PushBack();
  p0.ParameterType = ZilchTypeId(Arg0);
  DelegateParameter& p1 = parameters.PushBack();
  p1.ParameterType = ZilchTypeId(Arg1);
  ParseParameterArrays(parameters, spaceDelimitedNames);
  return builder.AddBoundFunction(
      classBoundType, name, boundFunction, parameters, ZilchTypeId(Return), FunctionOptions::Static);
}

template <typename FunctionType, FunctionType function, typename Return, typename Arg0, typename Arg1, typename Arg2>
static Function* FromMethod(LibraryBuilder& builder,
                            BoundType* classBoundType,
                            StringRange name,
                            StringRange spaceDelimitedNames,
                            Return (*)(Arg0, Arg1, Arg2))
{
  BoundFn boundFunction = FunctionBinding<FunctionType>::template GetFunctionInvoker<function>();
  ParameterArray parameters;
  DelegateParameter& p0 = parameters.PushBack();
  p0.ParameterType = ZilchTypeId(Arg0);
  DelegateParameter& p1 = parameters.PushBack();
  p1.ParameterType = ZilchTypeId(Arg1);
  DelegateParameter& p2 = parameters.PushBack();
  p2.ParameterType = ZilchTypeId(Arg2);
  ParseParameterArrays(parameters, spaceDelimitedNames);
  return builder.AddBoundFunction(
      classBoundType, name, boundFunction, parameters, ZilchTypeId(Return), FunctionOptions::Static);
}

template <typename FunctionType,
          FunctionType function,
          typename Return,
          typename Arg0,
          typename Arg1,
          typename Arg2,
          typename Arg3>
static Function* FromMethod(LibraryBuilder& builder,
                            BoundType* classBoundType,
                            StringRange name,
                            StringRange spaceDelimitedNames,
                            Return (*)(Arg0, Arg1, Arg2, Arg3))
{
  BoundFn boundFunction = FunctionBinding<FunctionType>::template GetFunctionInvoker<function>();
  ParameterArray parameters;
  DelegateParameter& p0 = parameters.PushBack();
  p0.ParameterType = ZilchTypeId(Arg0);
  DelegateParameter& p1 = parameters.PushBack();
  p1.ParameterType = ZilchTypeId(Arg1);
  DelegateParameter& p2 = parameters.PushBack();
  p2.ParameterType = ZilchTypeId(Arg2);
  DelegateParameter& p3 = parameters.PushBack();
  p3.ParameterType = ZilchTypeId(Arg3);
  ParseParameterArrays(parameters, spaceDelimitedNames);
  return builder.AddBoundFunction(
      classBoundType, name, boundFunction, parameters, ZilchTypeId(Return), FunctionOptions::Static);
}

template <typename FunctionType,
          FunctionType function,
          typename Return,
          typename Arg0,
          typename Arg1,
          typename Arg2,
          typename Arg3,
          typename Arg4>
static Function* FromMethod(LibraryBuilder& builder,
                            BoundType* classBoundType,
                            StringRange name,
                            StringRange spaceDelimitedNames,
                            Return (*)(Arg0, Arg1, Arg2, Arg3, Arg4))
{
  BoundFn boundFunction = FunctionBinding<FunctionType>::template GetFunctionInvoker<function>();
  ParameterArray parameters;
  DelegateParameter& p0 = parameters.PushBack();
  p0.ParameterType = ZilchTypeId(Arg0);
  DelegateParameter& p1 = parameters.PushBack();
  p1.ParameterType = ZilchTypeId(Arg1);
  DelegateParameter& p2 = parameters.PushBack();
  p2.ParameterType = ZilchTypeId(Arg2);
  DelegateParameter& p3 = parameters.PushBack();
  p3.ParameterType = ZilchTypeId(Arg3);
  DelegateParameter& p4 = parameters.PushBack();
  p4.ParameterType = ZilchTypeId(Arg4);
  ParseParameterArrays(parameters, spaceDelimitedNames);
  return builder.AddBoundFunction(
      classBoundType, name, boundFunction, parameters, ZilchTypeId(Return), FunctionOptions::Static);
}

template <typename FunctionType,
          FunctionType function,
          typename Return,
          typename Arg0,
          typename Arg1,
          typename Arg2,
          typename Arg3,
          typename Arg4,
          typename Arg5>
static Function* FromMethod(LibraryBuilder& builder,
                            BoundType* classBoundType,
                            StringRange name,
                            StringRange spaceDelimitedNames,
                            Return (*)(Arg0, Arg1, Arg2, Arg3, Arg4, Arg5))
{
  BoundFn boundFunction = FunctionBinding<FunctionType>::template GetFunctionInvoker<function>();
  ParameterArray parameters;
  DelegateParameter& p0 = parameters.PushBack();
  p0.ParameterType = ZilchTypeId(Arg0);
  DelegateParameter& p1 = parameters.PushBack();
  p1.ParameterType = ZilchTypeId(Arg1);
  DelegateParameter& p2 = parameters.PushBack();
  p2.ParameterType = ZilchTypeId(Arg2);
  DelegateParameter& p3 = parameters.PushBack();
  p3.ParameterType = ZilchTypeId(Arg3);
  DelegateParameter& p4 = parameters.PushBack();
  p4.ParameterType = ZilchTypeId(Arg4);
  DelegateParameter& p5 = parameters.PushBack();
  p5.ParameterType = ZilchTypeId(Arg5);
  ParseParameterArrays(parameters, spaceDelimitedNames);
  return builder.AddBoundFunction(
      classBoundType, name, boundFunction, parameters, ZilchTypeId(Return), FunctionOptions::Static);
}

template <typename FunctionType,
          FunctionType function,
          typename Return,
          typename Arg0,
          typename Arg1,
          typename Arg2,
          typename Arg3,
          typename Arg4,
          typename Arg5,
          typename Arg6>
static Function* FromMethod(LibraryBuilder& builder,
                            BoundType* classBoundType,
                            StringRange name,
                            StringRange spaceDelimitedNames,
                            Return (*)(Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6))
{
  BoundFn boundFunction = FunctionBinding<FunctionType>::template GetFunctionInvoker<function>();
  ParameterArray parameters;
  DelegateParameter& p0 = parameters.PushBack();
  p0.ParameterType = ZilchTypeId(Arg0);
  DelegateParameter& p1 = parameters.PushBack();
  p1.ParameterType = ZilchTypeId(Arg1);
  DelegateParameter& p2 = parameters.PushBack();
  p2.ParameterType = ZilchTypeId(Arg2);
  DelegateParameter& p3 = parameters.PushBack();
  p3.ParameterType = ZilchTypeId(Arg3);
  DelegateParameter& p4 = parameters.PushBack();
  p4.ParameterType = ZilchTypeId(Arg4);
  DelegateParameter& p5 = parameters.PushBack();
  p5.ParameterType = ZilchTypeId(Arg5);
  DelegateParameter& p6 = parameters.PushBack();
  p6.ParameterType = ZilchTypeId(Arg6);
  ParseParameterArrays(parameters, spaceDelimitedNames);
  return builder.AddBoundFunction(
      classBoundType, name, boundFunction, parameters, ZilchTypeId(Return), FunctionOptions::Static);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Bound Instance
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename FunctionType, FunctionType function, typename Class>
static Function* FromMethod(LibraryBuilder& builder,
                            BoundType* classBoundType,
                            StringRange name,
                            StringRange spaceDelimitedNames,
                            void (Class::*)())
{
  BoundFn boundFunction = FunctionBinding<FunctionType>::template GetFunctionInvoker<function>();
  ParameterArray parameters;
  ParseParameterArrays(parameters, spaceDelimitedNames);
  return builder.AddBoundFunction(
      classBoundType, name, boundFunction, parameters, ZilchTypeId(void), FunctionOptions::None);
}

template <typename FunctionType, FunctionType function, typename Class, typename Arg0>
static Function* FromMethod(LibraryBuilder& builder,
                            BoundType* classBoundType,
                            StringRange name,
                            StringRange spaceDelimitedNames,
                            void (Class::*)(Arg0))
{
  BoundFn boundFunction = FunctionBinding<FunctionType>::template GetFunctionInvoker<function>();
  ParameterArray parameters;
  DelegateParameter& p0 = parameters.PushBack();
  p0.ParameterType = ZilchTypeId(Arg0);
  ParseParameterArrays(parameters, spaceDelimitedNames);
  return builder.AddBoundFunction(
      classBoundType, name, boundFunction, parameters, ZilchTypeId(void), FunctionOptions::None);
}

template <typename FunctionType, FunctionType function, typename Class, typename Arg0, typename Arg1>
static Function* FromMethod(LibraryBuilder& builder,
                            BoundType* classBoundType,
                            StringRange name,
                            StringRange spaceDelimitedNames,
                            void (Class::*)(Arg0, Arg1))
{
  BoundFn boundFunction = FunctionBinding<FunctionType>::template GetFunctionInvoker<function>();
  ParameterArray parameters;
  DelegateParameter& p0 = parameters.PushBack();
  p0.ParameterType = ZilchTypeId(Arg0);
  DelegateParameter& p1 = parameters.PushBack();
  p1.ParameterType = ZilchTypeId(Arg1);
  ParseParameterArrays(parameters, spaceDelimitedNames);
  return builder.AddBoundFunction(
      classBoundType, name, boundFunction, parameters, ZilchTypeId(void), FunctionOptions::None);
}

template <typename FunctionType, FunctionType function, typename Class, typename Arg0, typename Arg1, typename Arg2>
static Function* FromMethod(LibraryBuilder& builder,
                            BoundType* classBoundType,
                            StringRange name,
                            StringRange spaceDelimitedNames,
                            void (Class::*)(Arg0, Arg1, Arg2))
{
  BoundFn boundFunction = FunctionBinding<FunctionType>::template GetFunctionInvoker<function>();
  ParameterArray parameters;
  DelegateParameter& p0 = parameters.PushBack();
  p0.ParameterType = ZilchTypeId(Arg0);
  DelegateParameter& p1 = parameters.PushBack();
  p1.ParameterType = ZilchTypeId(Arg1);
  DelegateParameter& p2 = parameters.PushBack();
  p2.ParameterType = ZilchTypeId(Arg2);
  ParseParameterArrays(parameters, spaceDelimitedNames);
  return builder.AddBoundFunction(
      classBoundType, name, boundFunction, parameters, ZilchTypeId(void), FunctionOptions::None);
}

template <typename FunctionType,
          FunctionType function,
          typename Class,
          typename Arg0,
          typename Arg1,
          typename Arg2,
          typename Arg3>
static Function* FromMethod(LibraryBuilder& builder,
                            BoundType* classBoundType,
                            StringRange name,
                            StringRange spaceDelimitedNames,
                            void (Class::*)(Arg0, Arg1, Arg2, Arg3))
{
  BoundFn boundFunction = FunctionBinding<FunctionType>::template GetFunctionInvoker<function>();
  ParameterArray parameters;
  DelegateParameter& p0 = parameters.PushBack();
  p0.ParameterType = ZilchTypeId(Arg0);
  DelegateParameter& p1 = parameters.PushBack();
  p1.ParameterType = ZilchTypeId(Arg1);
  DelegateParameter& p2 = parameters.PushBack();
  p2.ParameterType = ZilchTypeId(Arg2);
  DelegateParameter& p3 = parameters.PushBack();
  p3.ParameterType = ZilchTypeId(Arg3);
  ParseParameterArrays(parameters, spaceDelimitedNames);
  return builder.AddBoundFunction(
      classBoundType, name, boundFunction, parameters, ZilchTypeId(void), FunctionOptions::None);
}

template <typename FunctionType,
          FunctionType function,
          typename Class,
          typename Arg0,
          typename Arg1,
          typename Arg2,
          typename Arg3,
          typename Arg4>
static Function* FromMethod(LibraryBuilder& builder,
                            BoundType* classBoundType,
                            StringRange name,
                            StringRange spaceDelimitedNames,
                            void (Class::*)(Arg0, Arg1, Arg2, Arg3, Arg4))
{
  BoundFn boundFunction = FunctionBinding<FunctionType>::template GetFunctionInvoker<function>();
  ParameterArray parameters;
  DelegateParameter& p0 = parameters.PushBack();
  p0.ParameterType = ZilchTypeId(Arg0);
  DelegateParameter& p1 = parameters.PushBack();
  p1.ParameterType = ZilchTypeId(Arg1);
  DelegateParameter& p2 = parameters.PushBack();
  p2.ParameterType = ZilchTypeId(Arg2);
  DelegateParameter& p3 = parameters.PushBack();
  p3.ParameterType = ZilchTypeId(Arg3);
  DelegateParameter& p4 = parameters.PushBack();
  p4.ParameterType = ZilchTypeId(Arg4);
  ParseParameterArrays(parameters, spaceDelimitedNames);
  return builder.AddBoundFunction(
      classBoundType, name, boundFunction, parameters, ZilchTypeId(void), FunctionOptions::None);
}

template <typename FunctionType,
          FunctionType function,
          typename Class,
          typename Arg0,
          typename Arg1,
          typename Arg2,
          typename Arg3,
          typename Arg4,
          typename Arg5>
static Function* FromMethod(LibraryBuilder& builder,
                            BoundType* classBoundType,
                            StringRange name,
                            StringRange spaceDelimitedNames,
                            void (Class::*)(Arg0, Arg1, Arg2, Arg3, Arg4, Arg5))
{
  BoundFn boundFunction = FunctionBinding<FunctionType>::template GetFunctionInvoker<function>();
  ParameterArray parameters;
  DelegateParameter& p0 = parameters.PushBack();
  p0.ParameterType = ZilchTypeId(Arg0);
  DelegateParameter& p1 = parameters.PushBack();
  p1.ParameterType = ZilchTypeId(Arg1);
  DelegateParameter& p2 = parameters.PushBack();
  p2.ParameterType = ZilchTypeId(Arg2);
  DelegateParameter& p3 = parameters.PushBack();
  p3.ParameterType = ZilchTypeId(Arg3);
  DelegateParameter& p4 = parameters.PushBack();
  p4.ParameterType = ZilchTypeId(Arg4);
  DelegateParameter& p5 = parameters.PushBack();
  p5.ParameterType = ZilchTypeId(Arg5);
  ParseParameterArrays(parameters, spaceDelimitedNames);
  return builder.AddBoundFunction(
      classBoundType, name, boundFunction, parameters, ZilchTypeId(void), FunctionOptions::None);
}

template <typename FunctionType,
          FunctionType function,
          typename Class,
          typename Arg0,
          typename Arg1,
          typename Arg2,
          typename Arg3,
          typename Arg4,
          typename Arg5,
          typename Arg6>
static Function* FromMethod(LibraryBuilder& builder,
                            BoundType* classBoundType,
                            StringRange name,
                            StringRange spaceDelimitedNames,
                            void (Class::*)(Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6))
{
  BoundFn boundFunction = FunctionBinding<FunctionType>::template GetFunctionInvoker<function>();
  ParameterArray parameters;
  DelegateParameter& p0 = parameters.PushBack();
  p0.ParameterType = ZilchTypeId(Arg0);
  DelegateParameter& p1 = parameters.PushBack();
  p1.ParameterType = ZilchTypeId(Arg1);
  DelegateParameter& p2 = parameters.PushBack();
  p2.ParameterType = ZilchTypeId(Arg2);
  DelegateParameter& p3 = parameters.PushBack();
  p3.ParameterType = ZilchTypeId(Arg3);
  DelegateParameter& p4 = parameters.PushBack();
  p4.ParameterType = ZilchTypeId(Arg4);
  DelegateParameter& p5 = parameters.PushBack();
  p5.ParameterType = ZilchTypeId(Arg5);
  DelegateParameter& p6 = parameters.PushBack();
  p6.ParameterType = ZilchTypeId(Arg6);
  ParseParameterArrays(parameters, spaceDelimitedNames);
  return builder.AddBoundFunction(
      classBoundType, name, boundFunction, parameters, ZilchTypeId(void), FunctionOptions::None);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Bound Instance Const
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename FunctionType, FunctionType function, typename Class>
static Function* FromMethod(LibraryBuilder& builder,
                            BoundType* classBoundType,
                            StringRange name,
                            StringRange spaceDelimitedNames,
                            void (Class::*)() const)
{
  BoundFn boundFunction = FunctionBinding<FunctionType>::template GetFunctionInvoker<function>();
  ParameterArray parameters;
  ParseParameterArrays(parameters, spaceDelimitedNames);
  return builder.AddBoundFunction(
      classBoundType, name, boundFunction, parameters, ZilchTypeId(void), FunctionOptions::None);
}

template <typename FunctionType, FunctionType function, typename Class, typename Arg0>
static Function* FromMethod(LibraryBuilder& builder,
                            BoundType* classBoundType,
                            StringRange name,
                            StringRange spaceDelimitedNames,
                            void (Class::*)(Arg0) const)
{
  BoundFn boundFunction = FunctionBinding<FunctionType>::template GetFunctionInvoker<function>();
  ParameterArray parameters;
  DelegateParameter& p0 = parameters.PushBack();
  p0.ParameterType = ZilchTypeId(Arg0);
  ParseParameterArrays(parameters, spaceDelimitedNames);
  return builder.AddBoundFunction(
      classBoundType, name, boundFunction, parameters, ZilchTypeId(void), FunctionOptions::None);
}

template <typename FunctionType, FunctionType function, typename Class, typename Arg0, typename Arg1>
static Function* FromMethod(LibraryBuilder& builder,
                            BoundType* classBoundType,
                            StringRange name,
                            StringRange spaceDelimitedNames,
                            void (Class::*)(Arg0, Arg1) const)
{
  BoundFn boundFunction = FunctionBinding<FunctionType>::template GetFunctionInvoker<function>();
  ParameterArray parameters;
  DelegateParameter& p0 = parameters.PushBack();
  p0.ParameterType = ZilchTypeId(Arg0);
  DelegateParameter& p1 = parameters.PushBack();
  p1.ParameterType = ZilchTypeId(Arg1);
  ParseParameterArrays(parameters, spaceDelimitedNames);
  return builder.AddBoundFunction(
      classBoundType, name, boundFunction, parameters, ZilchTypeId(void), FunctionOptions::None);
}

template <typename FunctionType, FunctionType function, typename Class, typename Arg0, typename Arg1, typename Arg2>
static Function* FromMethod(LibraryBuilder& builder,
                            BoundType* classBoundType,
                            StringRange name,
                            StringRange spaceDelimitedNames,
                            void (Class::*)(Arg0, Arg1, Arg2) const)
{
  BoundFn boundFunction = FunctionBinding<FunctionType>::template GetFunctionInvoker<function>();
  ParameterArray parameters;
  DelegateParameter& p0 = parameters.PushBack();
  p0.ParameterType = ZilchTypeId(Arg0);
  DelegateParameter& p1 = parameters.PushBack();
  p1.ParameterType = ZilchTypeId(Arg1);
  DelegateParameter& p2 = parameters.PushBack();
  p2.ParameterType = ZilchTypeId(Arg2);
  ParseParameterArrays(parameters, spaceDelimitedNames);
  return builder.AddBoundFunction(
      classBoundType, name, boundFunction, parameters, ZilchTypeId(void), FunctionOptions::None);
}

template <typename FunctionType,
          FunctionType function,
          typename Class,
          typename Arg0,
          typename Arg1,
          typename Arg2,
          typename Arg3>
static Function* FromMethod(LibraryBuilder& builder,
                            BoundType* classBoundType,
                            StringRange name,
                            StringRange spaceDelimitedNames,
                            void (Class::*)(Arg0, Arg1, Arg2, Arg3) const)
{
  BoundFn boundFunction = FunctionBinding<FunctionType>::template GetFunctionInvoker<function>();
  ParameterArray parameters;
  DelegateParameter& p0 = parameters.PushBack();
  p0.ParameterType = ZilchTypeId(Arg0);
  DelegateParameter& p1 = parameters.PushBack();
  p1.ParameterType = ZilchTypeId(Arg1);
  DelegateParameter& p2 = parameters.PushBack();
  p2.ParameterType = ZilchTypeId(Arg2);
  DelegateParameter& p3 = parameters.PushBack();
  p3.ParameterType = ZilchTypeId(Arg3);
  ParseParameterArrays(parameters, spaceDelimitedNames);
  return builder.AddBoundFunction(
      classBoundType, name, boundFunction, parameters, ZilchTypeId(void), FunctionOptions::None);
}

template <typename FunctionType,
          FunctionType function,
          typename Class,
          typename Arg0,
          typename Arg1,
          typename Arg2,
          typename Arg3,
          typename Arg4>
static Function* FromMethod(LibraryBuilder& builder,
                            BoundType* classBoundType,
                            StringRange name,
                            StringRange spaceDelimitedNames,
                            void (Class::*)(Arg0, Arg1, Arg2, Arg3, Arg4) const)
{
  BoundFn boundFunction = FunctionBinding<FunctionType>::template GetFunctionInvoker<function>();
  ParameterArray parameters;
  DelegateParameter& p0 = parameters.PushBack();
  p0.ParameterType = ZilchTypeId(Arg0);
  DelegateParameter& p1 = parameters.PushBack();
  p1.ParameterType = ZilchTypeId(Arg1);
  DelegateParameter& p2 = parameters.PushBack();
  p2.ParameterType = ZilchTypeId(Arg2);
  DelegateParameter& p3 = parameters.PushBack();
  p3.ParameterType = ZilchTypeId(Arg3);
  DelegateParameter& p4 = parameters.PushBack();
  p4.ParameterType = ZilchTypeId(Arg4);
  ParseParameterArrays(parameters, spaceDelimitedNames);
  return builder.AddBoundFunction(
      classBoundType, name, boundFunction, parameters, ZilchTypeId(void), FunctionOptions::None);
}

template <typename FunctionType,
          FunctionType function,
          typename Class,
          typename Arg0,
          typename Arg1,
          typename Arg2,
          typename Arg3,
          typename Arg4,
          typename Arg5>
static Function* FromMethod(LibraryBuilder& builder,
                            BoundType* classBoundType,
                            StringRange name,
                            StringRange spaceDelimitedNames,
                            void (Class::*)(Arg0, Arg1, Arg2, Arg3, Arg4, Arg5) const)
{
  BoundFn boundFunction = FunctionBinding<FunctionType>::template GetFunctionInvoker<function>();
  ParameterArray parameters;
  DelegateParameter& p0 = parameters.PushBack();
  p0.ParameterType = ZilchTypeId(Arg0);
  DelegateParameter& p1 = parameters.PushBack();
  p1.ParameterType = ZilchTypeId(Arg1);
  DelegateParameter& p2 = parameters.PushBack();
  p2.ParameterType = ZilchTypeId(Arg2);
  DelegateParameter& p3 = parameters.PushBack();
  p3.ParameterType = ZilchTypeId(Arg3);
  DelegateParameter& p4 = parameters.PushBack();
  p4.ParameterType = ZilchTypeId(Arg4);
  DelegateParameter& p5 = parameters.PushBack();
  p5.ParameterType = ZilchTypeId(Arg5);
  ParseParameterArrays(parameters, spaceDelimitedNames);
  return builder.AddBoundFunction(
      classBoundType, name, boundFunction, parameters, ZilchTypeId(void), FunctionOptions::None);
}

template <typename FunctionType,
          FunctionType function,
          typename Class,
          typename Arg0,
          typename Arg1,
          typename Arg2,
          typename Arg3,
          typename Arg4,
          typename Arg5,
          typename Arg6>
static Function* FromMethod(LibraryBuilder& builder,
                            BoundType* classBoundType,
                            StringRange name,
                            StringRange spaceDelimitedNames,
                            void (Class::*)(Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6) const)
{
  BoundFn boundFunction = FunctionBinding<FunctionType>::template GetFunctionInvoker<function>();
  ParameterArray parameters;
  DelegateParameter& p0 = parameters.PushBack();
  p0.ParameterType = ZilchTypeId(Arg0);
  DelegateParameter& p1 = parameters.PushBack();
  p1.ParameterType = ZilchTypeId(Arg1);
  DelegateParameter& p2 = parameters.PushBack();
  p2.ParameterType = ZilchTypeId(Arg2);
  DelegateParameter& p3 = parameters.PushBack();
  p3.ParameterType = ZilchTypeId(Arg3);
  DelegateParameter& p4 = parameters.PushBack();
  p4.ParameterType = ZilchTypeId(Arg4);
  DelegateParameter& p5 = parameters.PushBack();
  p5.ParameterType = ZilchTypeId(Arg5);
  DelegateParameter& p6 = parameters.PushBack();
  p6.ParameterType = ZilchTypeId(Arg6);
  ParseParameterArrays(parameters, spaceDelimitedNames);
  return builder.AddBoundFunction(
      classBoundType, name, boundFunction, parameters, ZilchTypeId(void), FunctionOptions::None);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Bound Instance Return
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// EndBound
template <typename FunctionType, FunctionType function, typename Class, typename Return>
static Function* FromMethod(LibraryBuilder& builder,
                            BoundType* classBoundType,
                            StringRange name,
                            StringRange spaceDelimitedNames,
                            Return (Class::*)())
{
  BoundFn boundFunction = FunctionBinding<FunctionType>::template GetFunctionInvoker<function>();
  ParameterArray parameters;
  ParseParameterArrays(parameters, spaceDelimitedNames);
  return builder.AddBoundFunction(
      classBoundType, name, boundFunction, parameters, ZilchTypeId(Return), FunctionOptions::None);
}
// BeginBound
// EndBound
template <typename FunctionType, FunctionType function, typename Class, typename Return, typename Arg0>
static Function* FromMethod(LibraryBuilder& builder,
                            BoundType* classBoundType,
                            StringRange name,
                            StringRange spaceDelimitedNames,
                            Return (Class::*)(Arg0))
{
  BoundFn boundFunction = FunctionBinding<FunctionType>::template GetFunctionInvoker<function>();
  ParameterArray parameters;
  DelegateParameter& p0 = parameters.PushBack();
  p0.ParameterType = ZilchTypeId(Arg0);
  ParseParameterArrays(parameters, spaceDelimitedNames);
  return builder.AddBoundFunction(
      classBoundType, name, boundFunction, parameters, ZilchTypeId(Return), FunctionOptions::None);
}
// BeginBound
// EndBound
template <typename FunctionType, FunctionType function, typename Class, typename Return, typename Arg0, typename Arg1>
static Function* FromMethod(LibraryBuilder& builder,
                            BoundType* classBoundType,
                            StringRange name,
                            StringRange spaceDelimitedNames,
                            Return (Class::*)(Arg0, Arg1))
{
  BoundFn boundFunction = FunctionBinding<FunctionType>::template GetFunctionInvoker<function>();
  ParameterArray parameters;
  DelegateParameter& p0 = parameters.PushBack();
  p0.ParameterType = ZilchTypeId(Arg0);
  DelegateParameter& p1 = parameters.PushBack();
  p1.ParameterType = ZilchTypeId(Arg1);
  ParseParameterArrays(parameters, spaceDelimitedNames);
  return builder.AddBoundFunction(
      classBoundType, name, boundFunction, parameters, ZilchTypeId(Return), FunctionOptions::None);
}
// BeginBound

// EndBound
template <typename FunctionType,
          FunctionType function,
          typename Class,
          typename Return,
          typename Arg0,
          typename Arg1,
          typename Arg2>
static Function* FromMethod(LibraryBuilder& builder,
                            BoundType* classBoundType,
                            StringRange name,
                            StringRange spaceDelimitedNames,
                            Return (Class::*)(Arg0, Arg1, Arg2))
{
  BoundFn boundFunction = FunctionBinding<FunctionType>::template GetFunctionInvoker<function>();
  ParameterArray parameters;
  DelegateParameter& p0 = parameters.PushBack();
  p0.ParameterType = ZilchTypeId(Arg0);
  DelegateParameter& p1 = parameters.PushBack();
  p1.ParameterType = ZilchTypeId(Arg1);
  DelegateParameter& p2 = parameters.PushBack();
  p2.ParameterType = ZilchTypeId(Arg2);
  ParseParameterArrays(parameters, spaceDelimitedNames);
  return builder.AddBoundFunction(
      classBoundType, name, boundFunction, parameters, ZilchTypeId(Return), FunctionOptions::None);
}

// EndBound
template <typename FunctionType,
          FunctionType function,
          typename Class,
          typename Return,
          typename Arg0,
          typename Arg1,
          typename Arg2,
          typename Arg3>
static Function* FromMethod(LibraryBuilder& builder,
                            BoundType* classBoundType,
                            StringRange name,
                            StringRange spaceDelimitedNames,
                            Return (Class::*)(Arg0, Arg1, Arg2, Arg3))
{
  BoundFn boundFunction = FunctionBinding<FunctionType>::template GetFunctionInvoker<function>();
  ParameterArray parameters;
  DelegateParameter& p0 = parameters.PushBack();
  p0.ParameterType = ZilchTypeId(Arg0);
  DelegateParameter& p1 = parameters.PushBack();
  p1.ParameterType = ZilchTypeId(Arg1);
  DelegateParameter& p2 = parameters.PushBack();
  p2.ParameterType = ZilchTypeId(Arg2);
  DelegateParameter& p3 = parameters.PushBack();
  p3.ParameterType = ZilchTypeId(Arg3);
  ParseParameterArrays(parameters, spaceDelimitedNames);
  return builder.AddBoundFunction(
      classBoundType, name, boundFunction, parameters, ZilchTypeId(Return), FunctionOptions::None);
}
// BeginBound
// EndBound
template <typename FunctionType,
          FunctionType function,
          typename Class,
          typename Return,
          typename Arg0,
          typename Arg1,
          typename Arg2,
          typename Arg3,
          typename Arg4>
static Function* FromMethod(LibraryBuilder& builder,
                            BoundType* classBoundType,
                            StringRange name,
                            StringRange spaceDelimitedNames,
                            Return (Class::*)(Arg0, Arg1, Arg2, Arg3, Arg4))
{
  BoundFn boundFunction = FunctionBinding<FunctionType>::template GetFunctionInvoker<function>();
  ParameterArray parameters;
  DelegateParameter& p0 = parameters.PushBack();
  p0.ParameterType = ZilchTypeId(Arg0);
  DelegateParameter& p1 = parameters.PushBack();
  p1.ParameterType = ZilchTypeId(Arg1);
  DelegateParameter& p2 = parameters.PushBack();
  p2.ParameterType = ZilchTypeId(Arg2);
  DelegateParameter& p3 = parameters.PushBack();
  p3.ParameterType = ZilchTypeId(Arg3);
  DelegateParameter& p4 = parameters.PushBack();
  p4.ParameterType = ZilchTypeId(Arg4);
  ParseParameterArrays(parameters, spaceDelimitedNames);
  return builder.AddBoundFunction(
      classBoundType, name, boundFunction, parameters, ZilchTypeId(Return), FunctionOptions::None);
}
// BeginBound
// EndBound
template <typename FunctionType,
          FunctionType function,
          typename Class,
          typename Return,
          typename Arg0,
          typename Arg1,
          typename Arg2,
          typename Arg3,
          typename Arg4,
          typename Arg5>
static Function* FromMethod(LibraryBuilder& builder,
                            BoundType* classBoundType,
                            StringRange name,
                            StringRange spaceDelimitedNames,
                            Return (Class::*)(Arg0, Arg1, Arg2, Arg3, Arg4, Arg5))
{
  BoundFn boundFunction = FunctionBinding<FunctionType>::template GetFunctionInvoker<function>();
  ParameterArray parameters;
  DelegateParameter& p0 = parameters.PushBack();
  p0.ParameterType = ZilchTypeId(Arg0);
  DelegateParameter& p1 = parameters.PushBack();
  p1.ParameterType = ZilchTypeId(Arg1);
  DelegateParameter& p2 = parameters.PushBack();
  p2.ParameterType = ZilchTypeId(Arg2);
  DelegateParameter& p3 = parameters.PushBack();
  p3.ParameterType = ZilchTypeId(Arg3);
  DelegateParameter& p4 = parameters.PushBack();
  p4.ParameterType = ZilchTypeId(Arg4);
  DelegateParameter& p5 = parameters.PushBack();
  p5.ParameterType = ZilchTypeId(Arg5);
  ParseParameterArrays(parameters, spaceDelimitedNames);
  return builder.AddBoundFunction(
      classBoundType, name, boundFunction, parameters, ZilchTypeId(Return), FunctionOptions::None);
}
// BeginBound
// EndBound
template <typename FunctionType,
          FunctionType function,
          typename Class,
          typename Return,
          typename Arg0,
          typename Arg1,
          typename Arg2,
          typename Arg3,
          typename Arg4,
          typename Arg5,
          typename Arg6>
static Function* FromMethod(LibraryBuilder& builder,
                            BoundType* classBoundType,
                            StringRange name,
                            StringRange spaceDelimitedNames,
                            Return (Class::*)(Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6))
{
  BoundFn boundFunction = FunctionBinding<FunctionType>::template GetFunctionInvoker<function>();
  ParameterArray parameters;
  DelegateParameter& p0 = parameters.PushBack();
  p0.ParameterType = ZilchTypeId(Arg0);
  DelegateParameter& p1 = parameters.PushBack();
  p1.ParameterType = ZilchTypeId(Arg1);
  DelegateParameter& p2 = parameters.PushBack();
  p2.ParameterType = ZilchTypeId(Arg2);
  DelegateParameter& p3 = parameters.PushBack();
  p3.ParameterType = ZilchTypeId(Arg3);
  DelegateParameter& p4 = parameters.PushBack();
  p4.ParameterType = ZilchTypeId(Arg4);
  DelegateParameter& p5 = parameters.PushBack();
  p5.ParameterType = ZilchTypeId(Arg5);
  DelegateParameter& p6 = parameters.PushBack();
  p6.ParameterType = ZilchTypeId(Arg6);
  ParseParameterArrays(parameters, spaceDelimitedNames);
  return builder.AddBoundFunction(
      classBoundType, name, boundFunction, parameters, ZilchTypeId(Return), FunctionOptions::None);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Bound Instance Return Const
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename FunctionType, FunctionType function, typename Class, typename Return>
static Function* FromMethod(LibraryBuilder& builder,
                            BoundType* classBoundType,
                            StringRange name,
                            StringRange spaceDelimitedNames,
                            Return (Class::*)() const)
{
  BoundFn boundFunction = FunctionBinding<FunctionType>::template GetFunctionInvoker<function>();
  ParameterArray parameters;
  ParseParameterArrays(parameters, spaceDelimitedNames);
  return builder.AddBoundFunction(
      classBoundType, name, boundFunction, parameters, ZilchTypeId(Return), FunctionOptions::None);
}

template <typename FunctionType, FunctionType function, typename Class, typename Return, typename Arg0>
static Function* FromMethod(LibraryBuilder& builder,
                            BoundType* classBoundType,
                            StringRange name,
                            StringRange spaceDelimitedNames,
                            Return (Class::*)(Arg0) const)
{
  BoundFn boundFunction = FunctionBinding<FunctionType>::template GetFunctionInvoker<function>();
  ParameterArray parameters;
  DelegateParameter& p0 = parameters.PushBack();
  p0.ParameterType = ZilchTypeId(Arg0);
  ParseParameterArrays(parameters, spaceDelimitedNames);
  return builder.AddBoundFunction(
      classBoundType, name, boundFunction, parameters, ZilchTypeId(Return), FunctionOptions::None);
}

template <typename FunctionType, FunctionType function, typename Class, typename Return, typename Arg0, typename Arg1>
static Function* FromMethod(LibraryBuilder& builder,
                            BoundType* classBoundType,
                            StringRange name,
                            StringRange spaceDelimitedNames,
                            Return (Class::*)(Arg0, Arg1) const)
{
  BoundFn boundFunction = FunctionBinding<FunctionType>::template GetFunctionInvoker<function>();
  ParameterArray parameters;
  DelegateParameter& p0 = parameters.PushBack();
  p0.ParameterType = ZilchTypeId(Arg0);
  DelegateParameter& p1 = parameters.PushBack();
  p1.ParameterType = ZilchTypeId(Arg1);
  ParseParameterArrays(parameters, spaceDelimitedNames);
  return builder.AddBoundFunction(
      classBoundType, name, boundFunction, parameters, ZilchTypeId(Return), FunctionOptions::None);
}

template <typename FunctionType,
          FunctionType function,
          typename Class,
          typename Return,
          typename Arg0,
          typename Arg1,
          typename Arg2>
static Function* FromMethod(LibraryBuilder& builder,
                            BoundType* classBoundType,
                            StringRange name,
                            StringRange spaceDelimitedNames,
                            Return (Class::*)(Arg0, Arg1, Arg2) const)
{
  BoundFn boundFunction = FunctionBinding<FunctionType>::template GetFunctionInvoker<function>();
  ParameterArray parameters;
  DelegateParameter& p0 = parameters.PushBack();
  p0.ParameterType = ZilchTypeId(Arg0);
  DelegateParameter& p1 = parameters.PushBack();
  p1.ParameterType = ZilchTypeId(Arg1);
  DelegateParameter& p2 = parameters.PushBack();
  p2.ParameterType = ZilchTypeId(Arg2);
  ParseParameterArrays(parameters, spaceDelimitedNames);
  return builder.AddBoundFunction(
      classBoundType, name, boundFunction, parameters, ZilchTypeId(Return), FunctionOptions::None);
}

template <typename FunctionType,
          FunctionType function,
          typename Class,
          typename Return,
          typename Arg0,
          typename Arg1,
          typename Arg2,
          typename Arg3>
static Function* FromMethod(LibraryBuilder& builder,
                            BoundType* classBoundType,
                            StringRange name,
                            StringRange spaceDelimitedNames,
                            Return (Class::*)(Arg0, Arg1, Arg2, Arg3) const)
{
  BoundFn boundFunction = FunctionBinding<FunctionType>::template GetFunctionInvoker<function>();
  ParameterArray parameters;
  DelegateParameter& p0 = parameters.PushBack();
  p0.ParameterType = ZilchTypeId(Arg0);
  DelegateParameter& p1 = parameters.PushBack();
  p1.ParameterType = ZilchTypeId(Arg1);
  DelegateParameter& p2 = parameters.PushBack();
  p2.ParameterType = ZilchTypeId(Arg2);
  DelegateParameter& p3 = parameters.PushBack();
  p3.ParameterType = ZilchTypeId(Arg3);
  ParseParameterArrays(parameters, spaceDelimitedNames);
  return builder.AddBoundFunction(
      classBoundType, name, boundFunction, parameters, ZilchTypeId(Return), FunctionOptions::None);
}

template <typename FunctionType,
          FunctionType function,
          typename Class,
          typename Return,
          typename Arg0,
          typename Arg1,
          typename Arg2,
          typename Arg3,
          typename Arg4>
static Function* FromMethod(LibraryBuilder& builder,
                            BoundType* classBoundType,
                            StringRange name,
                            StringRange spaceDelimitedNames,
                            Return (Class::*)(Arg0, Arg1, Arg2, Arg3, Arg4) const)
{
  BoundFn boundFunction = FunctionBinding<FunctionType>::template GetFunctionInvoker<function>();
  ParameterArray parameters;
  DelegateParameter& p0 = parameters.PushBack();
  p0.ParameterType = ZilchTypeId(Arg0);
  DelegateParameter& p1 = parameters.PushBack();
  p1.ParameterType = ZilchTypeId(Arg1);
  DelegateParameter& p2 = parameters.PushBack();
  p2.ParameterType = ZilchTypeId(Arg2);
  DelegateParameter& p3 = parameters.PushBack();
  p3.ParameterType = ZilchTypeId(Arg3);
  DelegateParameter& p4 = parameters.PushBack();
  p4.ParameterType = ZilchTypeId(Arg4);
  ParseParameterArrays(parameters, spaceDelimitedNames);
  return builder.AddBoundFunction(
      classBoundType, name, boundFunction, parameters, ZilchTypeId(Return), FunctionOptions::None);
}

template <typename FunctionType,
          FunctionType function,
          typename Class,
          typename Return,
          typename Arg0,
          typename Arg1,
          typename Arg2,
          typename Arg3,
          typename Arg4,
          typename Arg5>
static Function* FromMethod(LibraryBuilder& builder,
                            BoundType* classBoundType,
                            StringRange name,
                            StringRange spaceDelimitedNames,
                            Return (Class::*)(Arg0, Arg1, Arg2, Arg3, Arg4, Arg5) const)
{
  BoundFn boundFunction = FunctionBinding<FunctionType>::template GetFunctionInvoker<function>();
  ParameterArray parameters;
  DelegateParameter& p0 = parameters.PushBack();
  p0.ParameterType = ZilchTypeId(Arg0);
  DelegateParameter& p1 = parameters.PushBack();
  p1.ParameterType = ZilchTypeId(Arg1);
  DelegateParameter& p2 = parameters.PushBack();
  p2.ParameterType = ZilchTypeId(Arg2);
  DelegateParameter& p3 = parameters.PushBack();
  p3.ParameterType = ZilchTypeId(Arg3);
  DelegateParameter& p4 = parameters.PushBack();
  p4.ParameterType = ZilchTypeId(Arg4);
  DelegateParameter& p5 = parameters.PushBack();
  p5.ParameterType = ZilchTypeId(Arg5);
  ParseParameterArrays(parameters, spaceDelimitedNames);
  return builder.AddBoundFunction(
      classBoundType, name, boundFunction, parameters, ZilchTypeId(Return), FunctionOptions::None);
}

template <typename FunctionType,
          FunctionType function,
          typename Class,
          typename Return,
          typename Arg0,
          typename Arg1,
          typename Arg2,
          typename Arg3,
          typename Arg4,
          typename Arg5,
          typename Arg6>
static Function* FromMethod(LibraryBuilder& builder,
                            BoundType* classBoundType,
                            StringRange name,
                            StringRange spaceDelimitedNames,
                            Return (Class::*)(Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6) const)
{
  BoundFn boundFunction = FunctionBinding<FunctionType>::template GetFunctionInvoker<function>();
  ParameterArray parameters;
  DelegateParameter& p0 = parameters.PushBack();
  p0.ParameterType = ZilchTypeId(Arg0);
  DelegateParameter& p1 = parameters.PushBack();
  p1.ParameterType = ZilchTypeId(Arg1);
  DelegateParameter& p2 = parameters.PushBack();
  p2.ParameterType = ZilchTypeId(Arg2);
  DelegateParameter& p3 = parameters.PushBack();
  p3.ParameterType = ZilchTypeId(Arg3);
  DelegateParameter& p4 = parameters.PushBack();
  p4.ParameterType = ZilchTypeId(Arg4);
  DelegateParameter& p5 = parameters.PushBack();
  p5.ParameterType = ZilchTypeId(Arg5);
  DelegateParameter& p6 = parameters.PushBack();
  p6.ParameterType = ZilchTypeId(Arg6);
  ParseParameterArrays(parameters, spaceDelimitedNames);
  return builder.AddBoundFunction(
      classBoundType, name, boundFunction, parameters, ZilchTypeId(Return), FunctionOptions::None);
}

