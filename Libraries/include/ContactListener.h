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
        // Store the collision status
        UpdateCollisionStatus(contact, true);
    }

    void EndContact(b2Contact* contact) override {
        // Store the collision status
        UpdateCollisionStatus(contact, false);
    }

    bool AreBodiesColliding(b2Body* bodyA, b2Body* bodyB) const {
        auto key1 = std::make_pair(bodyA, bodyB);
        auto key2 = std::make_pair(bodyB, bodyA);
        return collisionMap.count(key1) > 0 || collisionMap.count(key2) > 0;
    }

private:
    void UpdateCollisionStatus(b2Contact* contact, bool isColliding) {
        b2Body* bodyA = contact->GetFixtureA()->GetBody();
        b2Body* bodyB = contact->GetFixtureB()->GetBody();
        auto key1 = std::make_pair(bodyA, bodyB);
        auto key2 = std::make_pair(bodyB, bodyA);

        if (isColliding) {
            collisionMap.insert(key1);
            collisionMap.insert(key2);
        }
        else {
            collisionMap.erase(key1);
            collisionMap.erase(key2);
        }
    }

    std::unordered_set<std::pair<b2Body*, b2Body*>, pair_hash> collisionMap;
};

#endif