#pragma once
#include "Common.h"

#include <vector>

#include "Hittable.h"

class HittableList : public Hittable
{
public:
    HittableList() {}
    HittableList(std::shared_ptr<Hittable> obj) { add(obj); }

    void clear() { m_objects.clear(); }

    void add(std::shared_ptr<Hittable> obj)
    {
        m_objects.push_back(obj);
        m_bbox = AABB(m_bbox, obj->boundingBox());
    }

    std::vector<std::shared_ptr<Hittable>>& objects() { return m_objects; }

    bool hit(const Ray& r, HitRecord& rec, Interval tRange) const override
    {
        HitRecord tempRec;
        bool hitAnything = false;
        auto closest = tRange.max();

        for (const auto& object : m_objects)
        {
            if (object->hit(r, tempRec, Interval(tRange.min(), closest)))
            {
                hitAnything = true;
                closest = tempRec.t;
                rec = tempRec;
            }
        }

        return hitAnything;
    }

    AABB boundingBox() const override { return m_bbox; }
private:
    std::vector<std::shared_ptr<Hittable>> m_objects;
    AABB m_bbox;
};