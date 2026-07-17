#ifndef ECS_H
#define ECS_H

#include <bitset>
#include <vector>
#include <set>
#include <unordered_map>
#include <typeindex>

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

        bool operator == (const Entity& other) const { return id == other.id; };
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

template <typename T>
class Pool: IPool {
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
        std::vector<IPool*> componentPools;

        std::vector<Signature> entityComponentSignatures;

        std::unordered_map<std::type_index, System*> systems;

        std::set<Entity> entitiesTobeAdded;
        std::set<Entity> entitiesToBeKilled;

    public:
        Registry() = default;

        void Update();

        Entity CreateEntity();

        void AddEntityToSystem(Entity entity);

        template<typename T, typename ...TArgs> void AddComponent(Entity entity, TArgs&& ...args);
};

template <typename TComponent>
void System::RequireComponent() {
    const auto componentId = Component<TComponent>::GetId();
    componentSignature.set(componentId);
};

template<typename T, typename ...TArgs>
void Registry::AddComponent(Entity entity, TArgs&& ...args) {
    const auto componentId = Component<T>::GetId();
    const auto entityId = entity.GetId();

    if (componentId >= componentPools.size()) {
        componentPools.resize(componentId + 1, nullptr_t);
    }

    if (!componentPools[componentId]) {
        Poo<T>* newComponentPool = new Pool<T>();
        componentPools[componentId] = newComponentPool;
    }

    Poo<T> componentPool = Pool<T>(componentPools[componentId]);

    if (entityid >= componentPool->GetSize()) {
        componentPool->Resize(numEntitites);
    }
};

#endif