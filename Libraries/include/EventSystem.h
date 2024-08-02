#ifndef EVENT_SYSTEM_H
#define EVENT_SYSTEM_H

#include <vector>
#include <string>
#include <unordered_map>

typedef void (*EventCallback)();

class EventSystem {
public:
    // Register a callback for a specific event
    void RegisterCallback(const std::string& eventName, EventCallback callback);

    // Unregister a callback for a specific event
    void UnregisterCallback(const std::string& eventName, EventCallback callback);

    // Trigger all callbacks for a specific event
    void TriggerEvent(const std::string& eventName) const;

private:
    // Map from event names to a list of callbacks
    std::unordered_map<std::string, std::vector<EventCallback>> eventCallbacks_;
};

#endif // EVENT_SYSTEM_H
