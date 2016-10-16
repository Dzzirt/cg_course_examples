#include "stdafx.h"
#include "Tesselator.h"
#include <algorithm>

namespace
{

struct CSphereTesselator : SMeshDataP3NT2
{
public:
    void Tesselate(unsigned slices, unsigned stacks)
    {
        MakeVertexAttributes(slices, stacks);
        MakeTriangleStripIndicies(slices, stacks);
    }

private:
    void MakeVertexAttributes(unsigned columnCount, unsigned rowCount)
    {
        // вычисляем позиции вершин.
        for (unsigned ci = 0; ci < columnCount; ++ci)
        {
            const float u = float(ci) / float(columnCount - 1);
            for (unsigned ri = 0; ri < rowCount; ++ri)
            {
                const float v = float(ri) / float(rowCount - 1);

                SVertexP3NT2 vertex;
                vertex.position = GetPositionOnSphere(u, v);

                // Нормаль к сфере - это нормализованный вектор радиуса к данной точке
                // Поскольку координаты центра равны 0, координаты вектора радиуса
                // будут равны координатам вершины.
                // Благодаря радиусу, равному 1, нормализация не требуется.
                vertex.normal = vertex.position;

                // Обе текстурные координаты должны плавно изменяться от 0 до 1,
                // натягивая прямоугольную картинку на тело вращения.
                // При UV-параметризации текстурными координатами будут u и v.
                vertex.texCoord = {1.f - u, v};

                vertices.push_back(vertex);
            }
        }
    }

    void MakeTriangleStripIndicies(unsigned columnCount, unsigned rowCount)
    {
        indicies.clear();
        indicies.reserve((columnCount - 1) * rowCount * 2);
        // вычисляем индексы вершин.
        for (unsigned ci = 0; ci < columnCount - 1; ++ci)
        {
            if (ci % 2 == 0)
            {
                for (unsigned ri = 0; ri < rowCount; ++ri)
                {
                    unsigned index = ci * rowCount + ri;
                    indicies.push_back(index + rowCount);
                    indicies.push_back(index);
                }
            }
            else
            {
                for (unsigned ri = rowCount - 1; ri < rowCount; --ri)
                {
                    unsigned index = ci * rowCount + ri;
                    indicies.push_back(index);
                    indicies.push_back(index + rowCount);
                }
            }
        }
    }

    glm::vec3 GetPositionOnSphere(float u, float v)const
    {
        const float radius = 1.f;
        const float latitude = float(M_PI) * (1.f - v); // 𝝅∙(𝟎.𝟓-𝒗)
        const float longitude = float(2.0 * M_PI) * u; // 𝟐𝝅∙𝒖
        const float latitudeRadius = radius * sinf(latitude);

        return { cosf(longitude) * latitudeRadius,
                 cosf(latitude) * radius,
                 sinf(longitude) * latitudeRadius };
    }
};
}

std::unique_ptr<CMeshP3NT2> CTesselator::TesselateSphere(unsigned precision)
{
    assert(precision >= MIN_PRECISION);

    CSphereTesselator tesselator;
    tesselator.Tesselate(precision, precision);

    auto pMesh = std::make_unique<CMeshP3NT2>(MeshType::TriangleStrip);
    pMesh->Copy(tesselator);
    return pMesh;
}