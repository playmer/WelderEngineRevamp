
#pragma once
#ifndef ZILCH_BINDING_INL
#  define ZILCH_BINDING_INL


namespace Zilch
{
  template <typename T>
  T Handle::Get(GetOptions::Enum options) const
  {
    if (this->StoredType == nullptr)
    {
      ErrorIf(options == GetOptions::AssertOnNull, "The value inside the Handle was null");
      return T();
    }

    // Check if we can directly convert the stored type into the requested type
    // This supports derived -> base class casting (but not visa versa), enums to
    // integers, etc
    BoundType* toType = ZilchTypeId(T);
    if (this->StoredType->IsRawCastableTo(toType) == false)
    {
      ErrorIf(options == GetOptions::AssertOnNull,
        "There was a value inside the Handle of type '%s' but it cannot be "
        "converted",
        this->StoredType->Name.c_str());
      return T();
    }

    return InternalReadRef<T>((byte*)this);
  }

  //#define P_ENABLE_IF(Condition) typename Zero::enable_if<(Condition)>::type* = nullptr

template <typename ZilchSelf, typename SetupFunction>
void SetupType(LibraryBuilder& builder,
               BoundType* type,
               SetupFunction setupType,
               typename Zero::enable_if<(HasZilchSetupType<ZilchSelf>::value)>::type*)
{
  builder.AddNativeBoundType(type, ZilchTypeId(typename ZilchSelf::ZilchBase), ZilchSelf::ZilchCopyMode);
  if (setupType != nullptr)
    setupType(nullptr, builder, type);
  ZilchSelf::ZilchSetupType(builder, type);
};


//#define P_DISABLE_IF(Condition) typename Zero::disable_if<(Condition)>::type* = nullptr
template <typename ZilchSelf, typename SetupFunction>
void SetupType(LibraryBuilder& builder,
                BoundType* type,
                SetupFunction setupType,
                typename Zero::disable_if<(HasZilchSetupType<ZilchSelf>::value)>::type*)
{
  ErrorIf(setupType == nullptr,
          "No setup function provided for externally BoundType %s. "
          "Be sure you are calling the correct initialize function: "
          "ZilchInitializeExternalType,"
          "ZilchInitializeRange, or ZilchInitializeEnum.",
          type->Name.c_str());
  setupType(nullptr, builder, type);
};

    

} // namespace Zilch

#endif