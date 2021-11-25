#include "Object.h"

namespace anari {
namespace rpr {

std::atomic<TimeStamp> g_timeStamp = []() { return 0; }();

TimeStamp newTimeStamp()
{
    return g_timeStamp++;
}

void Object::commit()
{
    // no-op
}

bool Object::getProperty(
        const std::string &, ANARIDataType, void *, ANARIWaitMask)
{
    return false;
}

void Object::setObjectType(ANARIDataType type)
{
    m_type = type;
}

ANARIDataType Object::type() const
{
    return m_type;
}

TimeStamp Object::lastUpdated() const
{
    return m_lastUpdated;
}

void Object::markUpdated()
{
    m_lastUpdated = newTimeStamp();
}

int Object::commitPriority() const
{
    return m_commitPriority;
}

void Object::setCommitPriority(int p)
{
    m_commitPriority = p;
}

} // namespace rpr

ANARI_TYPEFOR_DEFINITION(rpr::Object *);

} // namespace anari
