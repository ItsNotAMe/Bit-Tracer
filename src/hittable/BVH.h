#pragma once
#include "Common.h"

#include <algorithm>

#include "AABB.h"
#include "Hittable.h"
#include "HittableList.h"

class BVHNode : public Hittable
{
public:
    BVHNode(HittableList list)
        : BVHNode(list.objects(), 0, list.objects().size())
    {
    }
    BVHNode(std::vector<std::shared_ptr<Hittable>>& objects, size_t start, size_t end)
    {
        // Build the bounding box of the span of source objects.
        m_bbox = AABB::empty;
        for (size_t i = start; i < end; i++)
            m_bbox = AABB(m_bbox, objects[i]->boundingBox());

        // Split along the longest axis
        int axis = m_bbox.longestAxis();

        auto comparator = (axis == 0) ? boxCompareX
            : (axis == 1) ? boxCompareY
            : boxCompareZ;

        size_t objectSpan = end - start;

        if (objectSpan == 1)
            m_left = m_right = objects[start];
        else if (objectSpan == 3)
        {
            std::sort(objects.begin() + start, objects.begin() + end, comparator);
            m_left = std::make_shared<BVHNode>(objects, start, start + 2);
            m_right = objects[start + 2];
        }
        else if (objectSpan == 2)
        {
            m_left = objects[start];
            m_right = objects[start + 1];
        }
        else
        {
            std::sort(objects.begin() + start, objects.begin() + end, comparator);

            size_t mid = start + objectSpan / 2;
            m_left = std::make_shared<BVHNode>(objects, start, mid);
            m_right = std::make_shared<BVHNode>(objects, mid, end);
        }

        // // Build the bounding box of the span of source objects.
        // m_bbox = AABB::empty;
        // for (size_t i = start; i < end; i++)
        //     m_bbox = AABB(m_bbox, objects[i]->boundingBox());

        // size_t objectSpan = end - start;

        // if (objectSpan == 1)
        // {
        //     m_left = m_right = objects[start];
        // }
        // else if (objectSpan == 2)
        // {
        //     m_left = objects[start];
        //     m_right = objects[start + 1];
        // }
        // else
        // {
        //     AABB leftBox, rightBox;
        //     size_t bestAxis = -1;
        //     size_t bestIndex = start;
        //     float bestCost = INF;

        //     // Try splitting along each axis
        //     for (int axis = 0; axis < 3; axis++)
        //     {
        //         std::sort(objects.begin() + start, objects.begin() + end,
        //             [axis](const std::shared_ptr<Hittable>& a, const std::shared_ptr<Hittable>& b) {
        //                 return boxCompare(a, b, axis);
        //             });

        //         AABB leftBox = AABB::empty;
        //         AABB rightBox = AABB::empty;

        //         // Evaluate the SAH cost at each possible split
        //         for (size_t i = start + 1; i < end; ++i)
        //         {
        //             leftBox = AABB(leftBox, objects[i - 1]->boundingBox());
        //             rightBox = AABB::empty;

        //             for (size_t j = i; j < end; ++j)
        //                 rightBox = AABB(rightBox, objects[j]->boundingBox());

        //             float cost = 1 + (leftBox.surfaceArea() * (i - start) + rightBox.surfaceArea() * (end - i)) / m_bbox.surfaceArea();

        //             if (cost < bestCost)
        //             {
        //                 bestCost = cost;
        //                 bestAxis = axis;
        //                 bestIndex = i;
        //             }
        //         }
        //     }

        //     std::sort(objects.begin() + start, objects.begin() + end,
        //         [bestAxis](const std::shared_ptr<Hittable>& a, const std::shared_ptr<Hittable>& b) {
        //             return boxCompare(a, b, bestAxis);
        //         });

        //     m_left = std::make_shared<BVHNode>(objects, start, bestIndex);
        //     m_right = std::make_shared<BVHNode>(objects, bestIndex, end);
        // }
    }

    bool hit(const Ray& r, HitRecord& rec, Interval tRange) const override
    {
        if (!m_bbox.hit(r, tRange))
            return false;

        bool hit_left = m_left->hit(r, rec, tRange);
        bool hit_right = m_right->hit(r, rec, Interval(tRange.min(), hit_left ? rec.t : tRange.max()));

        return hit_left || hit_right;
    }

    AABB boundingBox() const override { return m_bbox; }
private:
    static bool boxCompare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b, int axis_index)
    {
        return a->boundingBox().centroid()[axis_index] < b->boundingBox().centroid()[axis_index];
    }

    static bool boxCompareX(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b)
    {
        return boxCompare(a, b, 0);
    }

    static bool boxCompareY(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b)
    {
        return boxCompare(a, b, 1);
    }

    static bool boxCompareZ(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b)
    {
        return boxCompare(a, b, 2);
    }
private:
    std::shared_ptr<Hittable> m_left;
    std::shared_ptr<Hittable> m_right;
    AABB m_bbox;
};