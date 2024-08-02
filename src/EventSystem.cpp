#include "EventSystem.h"

void EventSystem::RegisterCallback(const std::string& eventName, EventCallback callback) {
    eventCallbacks_[eventName].push_back(callback);
}

void EventSystem::UnregisterCallback(const std::string& eventName, EventCallback callback) {
    auto& callbacks = eventCallbacks_[eventName];
    auto it = std::remove(callbacks.begin(), callbacks.end(), callback);
    if (it != callbacks.end()) {
        callbacks.erase(it, callbacks.end());
    }
}

void EventSystem::TriggerEvent(const std::string& eventName) const {
    auto it = eventCallbacks_.find(eventName);
    if (it != eventCallbacks_.end()) {
        for (EventCallback callback : it->second) {
            callback();
        }
    }
}