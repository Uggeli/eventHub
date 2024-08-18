#ifndef EVENTHUBINTERFACES_HPP
#define EVENTHUBINTERFACES_HPP

#include <vector>
#include "Event.hpp"

// Forward declare Eventhub
class EventHub;

class IListener {
public:
    IListener(std::vector<EventType> types);
    virtual ~IListener();
    virtual void Update() {
        for (const Event* event : messageQueue) {
            HandleEvent(event);
        }
        messageQueue.clear();
    }

    virtual void HandleEvent(const Event* event) = 0;
    virtual void onEvent(const Event& event) {
        messageQueue.push_back(&event);
    }
protected:
    std::vector<EventType> types;
    std::vector<const Event*> messageQueue;
};

#endif // EVENTHUBINTERFACES_HPP