#include "ECS.h"
#include "../Logger/Logger.h"
#include <iostream>
#include <algorithm>

int IComponent::nextId = 0;

int Entity::GetId() const {
    return id;
}

void System::AddEntityToSystem(Entity entity) {
    entities.push_back(entity);
};

void System::RemoveEntityFromSystem(Entity entity) {
    // for (int i=0; i < entities.size(); ++i) {
    //     if (entities[i] == entity) {
    //         entities.erase(entities.begin() + i);
    //     }
    // }
    entities.erase(std::remove_if(entities.begin(), entities.end(), [&entity](Entity other) {
        return entity == other;
    }), entities.end());
};


std::vector<Entity> System::GetSystemEntities() const {
    return entities;
};

const Signature& System::GetComponentSignature() const {
    return componentSignature;
};

Entity Registry::CreateEntity() {
    int entityId;

    entityId = numEntitites++;

    Entity entity(entityId);
    entity.registry = this;
    entitiesTobeAdded.insert(entity);

    if (entityId >= static_cast<int>(entityComponentSignatures.size())) {
        entityComponentSignatures.resize(entityId + 1);
    }

    Logger::Log("Entity created with id = " + std::to_string(entityId));

    return entity;
};

void Registry::AddEntityToSystems(Entity entity) {
    const auto entityId = entity.GetId();

    const auto& entityComponentSignature = entityComponentSignatures[entityId];

    for (auto& system: systems) {
        const auto& systemComponentSignature = system.second->GetComponentSignature();

        bool isInterested = (entityComponentSignature & systemComponentSignature) == systemComponentSignature;

        if (isInterested) {
            system.second->AddEntityToSystem(entity);
        }
    }
};

void Registry::Update() {
    for (auto entity: entitiesTobeAdded) {
        AddEntityToSystems(entity);
    }

    entitiesTobeAdded.clear();

};
