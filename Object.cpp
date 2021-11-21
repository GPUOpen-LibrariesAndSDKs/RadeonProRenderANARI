#include "Object.h"

namespace anari {
namespace rpr {

std::atomic<TimeStamp> g_timeStamp = []() { return 0; }();

TimeStamp newTimeStamp()
{
    return g_timeStamp++;
}

box3 Object::bounds() const
{
    return m_bounds;
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

TimeStamp Object::lastAttached() const
{
    return m_lastAttached;
}

void Object::markAttached()
{
    m_lastAttached = newTimeStamp();
}

int Object::commitPriority() const
{
    return m_commitPriority;
}

void Object::setCommitPriority(int p)
{
    m_commitPriority = p;
}

void Object::extendBounds(box3 bounds) {
    m_bounds.extend(bounds);
}

void Object::resetBounds() {
    m_bounds = {vec3(std::numeric_limits<float>::max()),
                -vec3(std::numeric_limits<float>::max())};
}

} // namespace rpr

ANARI_TYPEFOR_DEFINITION(rpr::Object *);

} // namespace anari
