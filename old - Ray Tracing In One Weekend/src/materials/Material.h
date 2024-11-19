#pragma once
#include "Common.h"

#include "pdf/PDF.h"

struct HitRecord;

struct ScatterRecord
{
    Color Attenuation;
    std::shared_ptr<PDF> PDF;
    bool SkipPDF;
    Ray SkipPDFRay;
};

class Material
{
public:
    virtual ~Material() = default;

    virtual bool scatter(const Ray& rayIn, const HitRecord& rec, ScatterRecord& srec) const { return false; }

    virtual float scatteringPDF(const Ray& rayIn, const HitRecord& rec, const Ray& scattered) const { return 0; }

    virtual Color emitted(const Ray& rayIn, const HitRecord& rec, double u, double v, const Point3& p) const { return Color(0, 0, 0); }
};