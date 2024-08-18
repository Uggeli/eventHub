#ifndef EVENTHUB_HPP
#define EVENTHUB_HPP

#include <vector>
#include <unordered_map>
#include <map>
#include <memory>

#include "EventHubInterfaces.hpp"
#include "Event.hpp"

class EventHub {
    public:
        static EventHub& getInstance() {
            static EventHub instance;
            return instance;
        }
        virtual ~EventHub() = default;

        template<typename T, typename... Args>
        void createEvent(EventType type, uint8_t expirationTime, Args&&... args) {
            auto event = std::make_shared<T>(std::forward<Args>(args)...);
            event->setExpirationTime(expirationTime);
            events.push_back(event);
            dispatch(*event);
        }

        void addListener(IListener* listener, EventType type);
        void removeListener(IListener* listener, EventType type);
        void removeListener(IListener* listener);
        void dispatch(const Event& event) const;
        void update();
    private:
        EventHub() = default;
        std::unordered_map<EventType, std::vector<IListener*>> listeners;
        std::vector<std::shared_ptr<Event>> events;
};


// TODO: Implement ResponseHub
class ResponseHub {
    public:
        static ResponseHub& getInstance() {
            static ResponseHub instance;
            return instance;
        }
        virtual ~ResponseHub() = default;
        std::shared_ptr<Event> getResponse(IListener* recipient);
        void addResponse(IListener* recepient, std::shared_ptr<Event> response);
        void update();
    private:
        ResponseHub() = default;
        void removeResponse(IListener* recepient, const std::shared_ptr<Event>& response);
        void removeResponse(IListener* recepient);
        std::map<IListener*, std::vector<std::shared_ptr<Event>>> responses;  
};


#endif // EVENTHUB_HPP