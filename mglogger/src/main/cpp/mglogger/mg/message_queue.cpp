/**
 * Description:消息队列，线程间通信
 * Created by lantian
 * Date: 2025/7/14
 * Time: 15:56
 *
 */
#include "message_queue.h"

namespace MGLogger {

    MGMessage::MGMessage(int what) {
        this->what = what;
    }

    MGMessage::MGMessage(int what, const char *cmd) {
        this->what = what;
        this->msg = cmd ? std::string(cmd) : "";
    }

    MessageQueue::MessageQueue() {
        ALOGD("MessageQueue::MessageQueue - initializing message queue");
        mutex = SDL_CreateMutex();
        cond = SDL_CreateCond();
        abort_request = 1;
    }

    MessageQueue::~MessageQueue() {
        SDL_LockMutex(mutex);
        msgList.clear();
        SDL_UnlockMutex(mutex);

        SDL_DestroyMutex(mutex);
        SDL_DestroyCond(cond);
        mutex = nullptr;
        cond = nullptr;
    }

    void MessageQueue::sendMessage(int what) {
        std::shared_ptr<MGMessage> msg = std::make_shared<MGMessage>(what);
        sendMessage(msg);
    }

    void MessageQueue::sendMessage(int what, const char *cmd) {
        std::shared_ptr<MGMessage> msg = std::make_shared<MGMessage>(what,cmd);
        sendMessage(msg);
    }

    void MessageQueue::sendMessage(const std::shared_ptr<MGMessage>& msg) {
        SDL_LockMutex(mutex);
        sendMessageLocked(msg);
        SDL_UnlockMutex(mutex);
    }

    void MessageQueue::removeMessage(int what) {
        SDL_LockMutex(mutex);
        removeMessageLocked(what);
        SDL_UnlockMutex(mutex);
    }

    std::shared_ptr<MGMessage> MessageQueue::getMessage() {
        std::shared_ptr<MGMessage> msg;
        if (mutex == nullptr) {
            return nullptr;
        }
        SDL_LockMutex(mutex);
        msg = getMessageLocked();
        SDL_UnlockMutex(mutex);

        return msg;
    }

    void MessageQueue::start() {
        ALOGD("MessageQueue::start - starting message queue");
        if (mutex == nullptr || cond == nullptr) {
            ALOGE("MessageQueue::start - Mutex or Cond not initialized");
            return;
        }
        SDL_LockMutex(mutex);
        abort_request = 0;
        SDL_CondSignal(cond);
        SDL_UnlockMutex(mutex);
    }

    void MessageQueue::abort() {
        ALOGD("MessageQueue::abort - aborting message queue");
        SDL_LockMutex(mutex);
        abort_request = 1;
        SDL_CondSignal(cond);
        SDL_UnlockMutex(mutex);
        ALOGD("MessageQueue::abort - message queue aborted");
    }

    void MessageQueue::sendMessageLocked(const std::shared_ptr<MGMessage>& msg) {
        if (abort_request)
            return;

        msgList.push_back(msg);
        SDL_CondSignal(cond);
    }

    void MessageQueue::removeMessageLocked(int what) {
        if (abort_request)
            return;

        auto iter = msgList.begin();
        while (!msgList.empty() && iter != msgList.end()) {
            if (iter->get() == nullptr || iter->get()->what == what) {
                auto removeItem = iter;
                iter++;
                msgList.erase(removeItem);
            } else {
                iter++;
            }
        }
    }

    std::shared_ptr<MGMessage> MessageQueue::getMessageLocked() {
        std::shared_ptr<MGMessage> msg = nullptr;
        if (abort_request)
            return nullptr;

        do {
            if (!msgList.empty()) {
                msg = msgList.front();
                msgList.pop_front();
            } else {
                SDL_CondWait(cond, mutex);
            }
        } while (msg == nullptr && !abort_request);

        return msg;
    }

}

