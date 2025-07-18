#ifndef MGLOGGER_MESSAGE_QUEUE_H
#define MGLOGGER_MESSAGE_QUEUE_H

#include <functional>
#include <memory>
#include "string"
#include <list>
#ifdef __cplusplus
extern "C" {
#endif
#include "sdl_mutex.h"
#ifdef __cplusplus
}
#endif

/**
 * Description:
 * Created by lantian
 * Date： 2025/7/11
 * Time： 15:16
 */

namespace MGLogger {

    class MGMessage {
    public:
        MGMessage(int what);

        MGMessage(int what, void *obj,int length);

        ~MGMessage();

        int what{0};
        std::string msg;
        void *obj{nullptr};
        std::function<void()> freeFunc;
    };

    class MessageQueue {
    public:
        MessageQueue();

        ~MessageQueue();

        void sendMessage(int what);

        void sendMessage(int what, void *obj,int length);

        void sendMessage(std::shared_ptr<MGMessage> msg);

        void removeMessage(int what);

        std::shared_ptr<MGMessage> getMessage();

        void start();

        void abort();

    private:
        void sendMessageLocked(std::shared_ptr<MGMessage> msg);

        void removeMessageLocked(int what);

        std::shared_ptr<MGMessage> getMessageLocked();

        std::list<std::shared_ptr<MGMessage>> msgList{nullptr};
        int abort_request{0};
        SDL_mutex *mutex{nullptr};
        SDL_cond *cond{nullptr};
    };
}

#endif // MGLOGGER_MESSAGE_QUEUE_H
