#include <gtest/gtest.h>
#include "EventHub.hpp"
#include "Event.hpp"
#include "EventHubInterfaces.hpp"
#include "Logger.hpp"
#include <sstream>

// Test fixture
class EventSystemTest : public ::testing::Test {
protected:
    void SetUp() override {
        eventHub = &EventHub::getInstance();
        eventTypeManager = &EventTypeManager::getInstance();
        
        // Add custom event types
        messageSentType = eventTypeManager->addEventType("MESSAGE_SENT");
        userJoinedType = eventTypeManager->addEventType("USER_JOINED");
        userLeftType = eventTypeManager->addEventType("USER_LEFT");
    }

    EventHub* eventHub;
    EventTypeManager* eventTypeManager;
    EventType messageSentType;
    EventType userJoinedType;
    EventType userLeftType;
};

// Test EventTypeManager
TEST_F(EventSystemTest, EventTypeManager) {
    EXPECT_EQ(eventTypeManager->getEventType("UNKNOWN"), EventType::UNKNOWN);
    
    EXPECT_NE(eventTypeManager->getEventType("MESSAGE_SENT"), EventType::UNKNOWN);
    EXPECT_NE(eventTypeManager->getEventType("USER_JOINED"), EventType::UNKNOWN);
    EXPECT_NE(eventTypeManager->getEventType("USER_LEFT"), EventType::UNKNOWN);
    
    EXPECT_EQ(eventTypeManager->getEventType("NON_EXISTENT"), EventType::UNKNOWN);
}


// Test Logger
class TestLogger : public Logger {
public:
    TestLogger(EventType minLevelLog) : Logger(minLevelLog) {}

    std::string getLastLogMessage() const {
        return lastLogMessage;
    }

protected:
    void HandleEvent(const Event* event) override {
        if (event->getType() >= minLevelLog) {
            const LogEvent* logEvent = dynamic_cast<const LogEvent*>(event);
            if (logEvent) {
                lastLogMessage = logEvent->message;
            }
        }
    }

private:
    std::string lastLogMessage;
};


// Test Listener
class TestListener : public IListener {
public:
    TestListener(EventTypeManager* etm) : IListener({
        etm->getEventType("MESSAGE_SENT"),
        etm->getEventType("USER_JOINED")
    }) {}
    
    void HandleEvent(const Event* event) override {
        handledEvents.push_back(event->getType());
    }

    std::vector<EventType> handledEvents;
};


// Custom events
class MessageSentEvent : public Event {
public:
    MessageSentEvent(EventType type, const std::string& message, const std::string& sender, const std::string& recipient) {
        this->type = type;
        this->message = message;
        this->sender = sender;
        this->recipient = recipient;
    }

    std::string message;
    std::string sender;
    std::string recipient;
};

class UserJoinedEvent : public Event {
public:
    UserJoinedEvent(EventType type, const std::string& username) {
        this->type = type;
        this->username = username;
    }

    std::string username;
};


// Test creating and dispatching log events
TEST_F(EventSystemTest, LoggerHandling) {
    TestLogger debugLogger(EventType::LOG_DEBUG);
    TestLogger infoLogger(EventType::LOG_INFO);
    TestLogger warningLogger(EventType::LOG_WARNING);

    eventHub->createEvent<LogEvent>(EventType::LOG_DEBUG, 1, "Debug message", EventType::LOG_DEBUG);

    debugLogger.Update();
    infoLogger.Update();
    warningLogger.Update();

    EXPECT_EQ(debugLogger.getLastLogMessage(), "Debug message");
    EXPECT_EQ(infoLogger.getLastLogMessage(), "");
    EXPECT_EQ(warningLogger.getLastLogMessage(), "");

    eventHub->createEvent<LogEvent>(EventType::LOG_INFO, 1, "Info message", EventType::LOG_INFO);

    debugLogger.Update();
    infoLogger.Update();
    warningLogger.Update();

    EXPECT_EQ(debugLogger.getLastLogMessage(), "Info message");
    EXPECT_EQ(infoLogger.getLastLogMessage(), "Info message");
    EXPECT_EQ(warningLogger.getLastLogMessage(), "");

    eventHub->createEvent<LogEvent>(EventType::LOG_WARNING, 1, "Warning message", EventType::LOG_WARNING);

    debugLogger.Update();
    infoLogger.Update();
    warningLogger.Update();

    EXPECT_EQ(debugLogger.getLastLogMessage(), "Warning message");
    EXPECT_EQ(infoLogger.getLastLogMessage(), "Warning message");
    EXPECT_EQ(warningLogger.getLastLogMessage(), "Warning message");
}


// Test creating and dispatching an event
TEST_F(EventSystemTest, CreateAndDispatchEvent) {
    TestListener listener(eventTypeManager);
    
    eventHub->createEvent<MessageSentEvent>(messageSentType, 1, messageSentType, "Hello", "Sender", "Recipient");
    listener.Update();

    ASSERT_EQ(listener.handledEvents.size(), 1);
    EXPECT_EQ(listener.handledEvents[0], messageSentType);
}


// Test listener receiving multiple events
TEST_F(EventSystemTest, ListenerReceivesMultipleEvents) {
    TestListener listener(eventTypeManager);
    
    eventHub->createEvent<MessageSentEvent>(messageSentType, 1, messageSentType, "Hello", "Sender", "Recipient");
    eventHub->createEvent<UserJoinedEvent>(userJoinedType, 1, userJoinedType, "NewUser");
    listener.Update();

    ASSERT_EQ(listener.handledEvents.size(), 2);
    EXPECT_EQ(listener.handledEvents[0], messageSentType);
    EXPECT_EQ(listener.handledEvents[1], userJoinedType);
}

// Test event expiration
TEST_F(EventSystemTest, EventExpiration) {
    TestListener listener(eventTypeManager);
    
    eventHub->createEvent<MessageSentEvent>(messageSentType, 2, messageSentType, "Hello", "Sender", "Recipient");
    
    eventHub->update(); // Decrement expiration time
    listener.Update();
    ASSERT_EQ(listener.handledEvents.size(), 1);
    
    eventHub->update(); // This should remove the event
    listener.handledEvents.clear();
    listener.Update();
    EXPECT_EQ(listener.handledEvents.size(), 0);
}


// Test adding and removing listeners
TEST_F(EventSystemTest, AddRemoveListener) {
    TestListener listener1(eventTypeManager);
    TestListener listener2(eventTypeManager);
    
    eventHub->createEvent<MessageSentEvent>(messageSentType, 1, messageSentType, "Hello", "Sender", "Recipient");
    
    listener1.Update();
    listener2.Update();
    ASSERT_EQ(listener1.handledEvents.size(), 1);
    ASSERT_EQ(listener2.handledEvents.size(), 1);
    
    eventHub->removeListener(&listener2, messageSentType);
    
    listener1.handledEvents.clear();
    listener2.handledEvents.clear();
    eventHub->createEvent<MessageSentEvent>(messageSentType, 1, messageSentType, "Hello again", "Sender", "Recipient");
    
    listener1.Update();
    listener2.Update();
    EXPECT_EQ(listener1.handledEvents.size(), 1);
    EXPECT_EQ(listener2.handledEvents.size(), 0);
}


int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}