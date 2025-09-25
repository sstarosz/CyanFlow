#ifndef CF_UI_COLLAPSIBLEWIDGET_HPP
#define CF_UI_COLLAPSIBLEWIDGET_HPP

#include <QWidget>
#include <QToolButton>
#include <QFrame>
#include <QVBoxLayout>
#include <QPropertyAnimation>

namespace cf::ui {

class CollapsibleWidget : public QWidget {
    Q_OBJECT
    Q_PROPERTY(int animationDuration READ animationDuration WRITE setAnimationDuration)
    
public:
    explicit CollapsibleWidget(const QString& title, QWidget* parent = nullptr);
    
    void setContentLayout(QLayout* contentLayout);
    void setExpanded(bool expanded);
    bool isExpanded() const;
    
    int animationDuration() const;
    void setAnimationDuration(int duration);
    
signals:
    void expansionChanged(bool expanded);
    
protected:
    void toggleExpansion();
    
private:
    QToolButton* m_toggleButton;
    QFrame* m_contentArea;
    QVBoxLayout* m_mainLayout;
    QPropertyAnimation* m_animation;
    int m_animationDuration;
};

} // namespace cf::ui

#endif // CF_UI_COLLAPSIBLEWIDGET_HPP