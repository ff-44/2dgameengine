#include "ECS.h"
#include <iostream>

int Entity::GetId() const {
    return id;
}

void System::AddEntityToSystem(Entity entity) {
    entities.push_back(entity);
};

void System::RemoveEntityFromSystem(Entity entity) {
    for (int i=0; i < entities.size(); ++i) {
        if (entities[i].GetId() == entity.GetId()) {
            entities.erase(entities.begin() + i);
        }
    }
    // entities.erase(std::remove_if(entitites.being(), entities.end(), [&entity](Entity other) { return entity.GetId() == other.GetId(); }), entity.end());
};


std::vector<Entity> System::GetSystemEntities() const {
    return entities;
};

const Signature& System::GetComponentSignature() const {
    return componentSignature;
};
