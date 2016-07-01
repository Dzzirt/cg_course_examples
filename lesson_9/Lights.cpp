#include "stdafx.h"
#include "Lights.h"
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

namespace
{
}

void CLightingSystem::Enable()
{
    glEnable(GL_LIGHTING);
}

void CLightingSystem::Disable()
{
    glDisable(GL_LIGHTING);
}

void CLightingSystem::SetGlobalAmbient(const glm::vec3 &color)
{
    float values[] = { color.x, color.y, color.z, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, values);
}

void CLightingSystem::SetTwoSideLightingEnabled(bool enabled)
{
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, enabled ? GL_TRUE : GL_FALSE);
}

CAbstractLightSource::CAbstractLightSource(unsigned index)
    : m_index(index)
{
    static_assert(sizeof(unsigned) == sizeof(GLenum),
                  "GLenum was trunckated before constructor called");
}

CAbstractLightSource::~CAbstractLightSource()
{
}

glm::vec4 CAbstractLightSource::GetAmbient() const
{
    return m_ambient;
}

glm::vec4 CAbstractLightSource::GetDiffuse() const
{
    return m_diffuse;
}

glm::vec4 CAbstractLightSource::GetSpecular() const
{
    return m_specular;
}

void CAbstractLightSource::SetAmbient(const glm::vec4 &color)
{
    m_ambient = color;
}

void CAbstractLightSource::SetDiffuse(const glm::vec4 &color)
{
    m_diffuse = color;
}

void CAbstractLightSource::SetSpecular(const glm::vec4 &color)
{
    m_specular = color;
}

void CAbstractLightSource::RenderImpl() const
{
    // Включаем источник света с заданным индексом.
    // Источник действует только когда включен режим GL_LIGHTING!
    glEnable(GLenum(m_index));
    glLightfv(m_index, GL_AMBIENT, glm::value_ptr(m_ambient));
    glLightfv(m_index, GL_DIFFUSE, glm::value_ptr(m_diffuse));
    glLightfv(m_index, GL_SPECULAR, glm::value_ptr(m_specular));
}

unsigned CAbstractLightSource::GetIndex() const
{
    return m_index;
}

CDirectedLightSource::CDirectedLightSource(unsigned index)
    : CAbstractLightSource(index)
    , m_direction(0, 0, 0, 0)
{
}

glm::vec3 CDirectedLightSource::GetDirection() const
{
    return {m_direction.x, m_direction.y, m_direction.z};
}

void CDirectedLightSource::SetDirection(const glm::vec3 &value)
{
    m_direction.x = value.x;
    m_direction.y = value.y;
    m_direction.z = value.z;
}

void CDirectedLightSource::Setup() const
{
    // Если GL_POSITION установить как (x, y, z, 0), т.е. как вектор
    // в однородных координатах, источник света будет направленным.
    RenderImpl();
    glLightfv(GetIndex(), GL_POSITION, glm::value_ptr(m_direction));
}

CPositionLightSource::CPositionLightSource(unsigned index)
    : CAbstractLightSource(index)
    , m_position(0, 0, 0, 1)
{
}

glm::vec3 CPositionLightSource::GetPosition() const
{
    return { m_position.x, m_position.y, m_position.z };
}

void CPositionLightSource::SetPosition(const glm::vec3 &value)
{
    m_position.x = value.x;
    m_position.y = value.y;
    m_position.z = value.z;
}

void CPositionLightSource::Setup() const
{
    // Если GL_POSITION установить как (x, y, z, 1), т.е. как точку
    // в однородных координатах, источник света будет точечным.
    RenderImpl();
    glLightfv(GetIndex(), GL_POSITION, glm::value_ptr(m_position));
}
