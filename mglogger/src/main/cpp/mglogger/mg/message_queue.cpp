/**
 * Description:
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

    MGMessage::MGMessage(int what, void *obj, int length) {
        this->what = what;
        this->obj = malloc(length);
        memcpy(this->obj, obj, length);
        this->freeFunc = [this]() {
            if (this->obj != nullptr) {
                free(this->obj);
            }
        };
    }

    MGMessage::~MGMessage() {
        if (obj != nullptr) {
            freeFunc();
        }
    }

    MessageQueue::MessageQueue() {
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

    void MessageQueue::sendMessage(int what, void *obj, int length) {
        std::shared_ptr<MGMessage> msg = std::make_shared<MGMessage>(what, obj, length);
        sendMessage(msg);
    }

    void MessageQueue::sendMessage(std::shared_ptr<MGMessage> msg) {
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
        SDL_LockMutex(mutex);
        abort_request = 0;
        SDL_CondSignal(cond);
        SDL_UnlockMutex(mutex);
    }

    void MessageQueue::abort() {
        SDL_LockMutex(mutex);
        abort_request = 1;
        SDL_CondSignal(cond);
        SDL_UnlockMutex(mutex);
    }

    void MessageQueue::sendMessageLocked(std::shared_ptr<MGMessage> msg) {
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

