#ifndef CONTACT_LISTENER_H
#define CONTACT_LISTENER_H

#include <Box2D/box2d/box2d.h>
#include <unordered_map>
#include <unordered_set>

// A custom hash function for std::pair to be used in unordered_set
struct pair_hash {
    template <class T1, class T2>
    std::size_t operator()(const std::pair<T1, T2>& p) const {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);
        return h1 ^ h2;
    }
};

class CollisionListener : public b2ContactListener {
public:
    void BeginContact(b2Contact* contact) override {
        UpdateCollisionStatus(contact, true);
    }

    void EndContact(b2Contact* contact) override {
        UpdateCollisionStatus(contact, false);
    }

    bool AreBodiesColliding(b2Body* bodyA, b2Body* bodyB) const {
        auto itA = collisionMap.find(bodyA);
        if (itA != collisionMap.end() && itA->second.count(bodyB) > 0) {
            return true;
        }
        auto itB = collisionMap.find(bodyB);
        return (itB != collisionMap.end() && itB->second.count(bodyA) > 0);
    }

    bool OnCollisionEnter(b2Body* body) const {
        auto it = collisionMap.find(body);
        return (it != collisionMap.end() && !it->second.empty());
    }

private:
    void UpdateCollisionStatus(b2Contact* contact, bool isColliding) {
        b2Body* bodyA = contact->GetFixtureA()->GetBody();
        b2Body* bodyB = contact->GetFixtureB()->GetBody();

        if (isColliding) {
            collisionMap[bodyA].insert(bodyB);
            collisionMap[bodyB].insert(bodyA);
        }
        else {
            collisionMap[bodyA].erase(bodyB);
            collisionMap[bodyB].erase(bodyA);

            // Clean up if the sets are empty
            if (collisionMap[bodyA].empty()) collisionMap.erase(bodyA);
            if (collisionMap[bodyB].empty()) collisionMap.erase(bodyB);
        }
    }

    std::unordered_map<b2Body*, std::unordered_set<b2Body*>> collisionMap;
};

#endif
