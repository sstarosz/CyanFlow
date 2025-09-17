#ifndef CF_UI_QTAPPLICATIONCONTEXT_HPP
#define CF_UI_QTAPPLICATIONCONTEXT_HPP

#include "Framework/ApplicationContext.hpp"
#include "Core/Events/ConnectionAddedEvent.hpp"

#include <QObject>

namespace cf::ui {

class QtApplicationContext : public QObject {
Q_OBJECT
    public:
        QtApplicationContext(framework::ApplicationContext& appContext)
            : m_appContext(appContext) 
        {
            setupEventSubscriptions();
        }

        void setupEventSubscriptions();


        std::shared_ptr<core::Document> getCurrentDocument() const {
            return m_appContext.getCurrentDocument();
        }

        std::shared_ptr<core::Scene> getActiveScene() const {
                return m_appContext.getCurrentDocument()->getScene();
        }

        void setCurrentDocument(std::shared_ptr<core::Document> document) {
            m_appContext.m_currentDocument = document;
        }

    template<typename CommandType, typename... Args>
    void execute(Args&&... args) {
        m_appContext.execute<CommandType>(std::forward<Args>(args)...);
    }

    signals:
        void connectionAdded(const core::ConnectionAddedEvent& event);


    private:
        framework::ApplicationContext& m_appContext;
        std::vector<core::EventBus::SubscriptionId> m_subscriptions;
};
        

} // namespace cf::ui

#endif // CF_UI_QTAPPLICATIONCONTEXT_HPP