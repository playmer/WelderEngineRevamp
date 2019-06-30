// MIT Licensed (see LICENSE.md).
#pragma once

namespace Zero
{

namespace Events
{
DeclareEvent(Notify);
}

class NotifyEvent : public Event
{
public:
  ZilchDeclareType(NotifyEvent, TypeCopyMode::ReferenceType);
  NotifyType::Enum Type;
  String Name;
  String Message;
  String Icon;
};

} // namespace Zero
