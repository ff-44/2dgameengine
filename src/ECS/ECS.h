#ifndef ECS_H
#define ECS_H

#include "../Logger/Logger.h"
#include <vector>
#include <bitset>
#include <set>
#include <unordered_map>
#include <typeindex>
#include <memory>

const unsigned int MAX_COMPONENTS = 32;

// We use a bitset (1 and 0) to kee track of which componenets an entity has
// and also helps keep track of which entities the system is interesting
typedef std::bitset<MAX_COMPONENTS> Signature;

struct IComponent {
    protected:
        static int nextId;
};

template <typename T>
class Component: public IComponent {
    public:
        static int GetId() {
            static auto id = nextId++;
            return id;
        }
};

class Entity {
    private:
        int id;

    public:
        Entity(int id): id(id) {};
        int GetId() const;

        Entity& operator =(const Entity& other) = default;
        bool operator ==(const Entity& other) const { return id == other.id; }
        bool operator !=(const Entity& other) const { return id != other.id; }
        bool operator >(const Entity& other) const { return id > other.id; }
        bool operator <(const Entity& other) const { return id < other.id; }

        template <typename TComponent, typename ...TArgs> void AddComponent(TArgs&& ...args);
        template <typename TComponent> void RemoveComponent();
        template <typename TComponent> bool HasComponent() const;
        template <typename TComponent> TComponent& GetComponent() const;

        // A pointer to the entity's owner registry
        class Registry* registry;
};

class System {
    private:
        Signature componentSignature;
        std::vector<Entity> entities;
    
    public:
        System() = default;
        ~System() = default;

        void AddEntityToSystem(Entity entity);
        void RemoveEntityFromSystem(Entity entity);
        std::vector<Entity> GetSystemEntities() const;
        const Signature& GetComponentSignature() const;

        template <typename TComponent> void RequireComponent();
};

class IPool {
    public:
        virtual ~IPool() {}
};

/**
 * Template of IPool class with the following method to manage pools
 */
template <typename T>
class Pool: public IPool {
    private:
        std::vector<T> data;
    
    public:
        Pool(int size = 100) {
            data.resize(size);
        }
        virtual ~Pool() = default;

        bool isEmpty() const {
            return data.empty();
        }

        int GetSize() const {
            return data.size();
        }

        void Resize(int n) {
            data.resize(n);
        }

        void Clear() {
            data.clear();
        }

        void Add(T object) {
            data.push_back(object);
        }

        void Set(int index, T object) {
            data[index] = object;
        }

        T& Get(int index) {
            return static_cast<T&>(data[index]);
        }

        T& operator [](unsigned int index) {
            return data[index];
        }
};

class Registry {
    private:
        int numEntitites = 0;
        
        // Vector index is component type id
        // Pool index = entity id
        std::vector<std::shared_ptr<IPool>> componentPools;

        std::vector<Signature> entityComponentSignatures;

        std::unordered_map<std::type_index, std::shared_ptr<System>> systems;

        std::set<Entity> entitiesTobeAdded;
        std::set<Entity> entitiesToBeKilled;

    public:
        Registry() { Logger::Log("Registry constructor called"); }
        ~Registry() { Logger::Log("Registry desctructor called"); }

        void Update();

        Entity CreateEntity();

        void AddEntityToSystem(Entity entity);

        template <typename T, typename ...TArgs> void AddComponent(Entity entity, TArgs&& ...args);

        template <typename T> void RemoveComponent(Entity entity);
        template <typename T> bool HasComponent(Entity entity);
        template <typename T> T& GetComponent(Entity entity) const;

        template <typename TSystem, typename ...TArgs> void AddSystem(TArgs&& ...args);
        template <typename TSystem> void RemoveSystem();
        template <typename TSystem> bool HasSystem() const;
        template <typename TSystem> TSystem& GetSystem() const;

        void AddEntityToSystems(Entity entity);
};


/**
 * Systems method of the registry to
 * add system
 * remove system
 * check if the system exists
 * get a system
 */
template <typename TSystem, typename ...TArgs>
void Registry::AddSystem(TArgs&& ...args) {
    std::shared_ptr<TSystem> newSystem = std::make_shared<TSystem>(std::forward<TArgs>(args)...);
    systems.insert(std::make_pair(std::type_index(typeid(TSystem)), newSystem));
};

template <typename TSystem>
void Registry::RemoveSystem() {
    auto system = systems.find(std::type_index(typeid(TSystem)));
    systems.erase(system);
};

template <typename TSystem>
bool Registry::HasSystem() const {
    return systems.find(std::type_index(typeid(TSystem))) != systems.end();
};

template <typename TSystem>
TSystem& Registry::GetSystem() const {
    auto system = systems.find(std::type_index(typeid(TSystem)));
    return *(std::static_pointer_cast<TSystem>(system->second));
};

template <typename TComponent>
void System::RequireComponent() {
    const auto componentId = Component<TComponent>::GetId();
    componentSignature.set(componentId);
};

/**
 * Methods for the registry to
 * add component to the entity
 * remove component from the entity
 * check if the entity has a component
 * get a component
 * 
 * This methods will be reused by the Entity class
 */
template<typename T, typename ...TArgs>
void Registry::AddComponent(Entity entity, TArgs&& ...args) {
    const auto componentId = Component<T>::GetId();
    const auto entityId = entity.GetId();

    if (componentId >= static_cast<int>(componentPools.size())) {
        componentPools.resize(componentId + 1, nullptr);
    }

    if (!componentPools[componentId]) {
        std::shared_ptr<Pool<T>> newComponentPool = std::make_shared<Pool<T>>();
        componentPools[componentId] = newComponentPool;
    }

    std::shared_ptr<Pool<T>> componentPool = std::static_pointer_cast<Pool<T>>(componentPools[componentId]);

    if (entityId >= componentPool->GetSize()) {
        componentPool->Resize(numEntitites);
    }

    T newComponent(std::forward<TArgs>(args)...);

    componentPool->Set(entityId, newComponent);
    entityComponentSignatures[entityId].set(componentId);

    Logger::Log("Component id = " + std::to_string(componentId) + " was added to entity id " + std::to_string(entityId));
};

template<typename T>
void Registry::RemoveComponent(Entity entity) {
    const auto componentId = Component<T>::GetId();
    const auto entityId = entity.GetId();

    entityComponentSignatures[entityId].set(componentId, false);

    Logger::Log("Component id " + std::to_string(componentId) + " was removed from the entity id " + std::to_string(entityId));
};

template<typename T>
bool Registry::HasComponent(Entity entity) {
    const auto componentId = Component<T>::GetId();
    const auto entityId = entity.GetId();

    return entityComponentSignatures[entityId].test(componentId);
};

template<typename T>
T& Registry::GetComponent(Entity entity) const {
    const auto componentId = Component<T>::GetId();
    const auto entityId = entity.GetId();
    auto componentPool = std::static_pointer_cast<Pool<T>>(componentPools[componentId]);
    return componentPool->Get(entityId);
};

/**
 * Entities mothod to
 * add component to an entity
 * remove component from the entity
 * check if the entity has a component
 * get component of the entity
 */
template<typename TComponent, typename ...TArgs>
void Entity::AddComponent(TArgs&& ...args) {
    registry->AddComponent<TComponent>(*this, std::forward<TArgs>(args)...);
};

template<typename TComponent>
void Entity::RemoveComponent() {
    registry->RemoveComponent<TComponent>(*this);
};

template<typename TComponent>
bool Entity::HasComponent() const {
    return registry->HasComponent<TComponent>(*this);
};

template<typename TComponent>
TComponent& Entity::GetComponent() const {
    return registry->GetComponent<TComponent>(*this);
};

#endif
