#include "GameObject.h"

template <typename T>
T *GameObject::GetComponent()
{
    return m_Components[ComponentTraits<T>::id];//TODO cast
}