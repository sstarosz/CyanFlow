#include "CollapsibleWidget.hpp"

namespace cf::ui {

CollapsibleWidget::CollapsibleWidget(const QString& title, QWidget* parent)
    : QWidget(parent)
    , m_animationDuration(150)
{
    m_toggleButton = new QToolButton(this);
    m_toggleButton->setText(title);
    m_toggleButton->setCheckable(true);
    m_toggleButton->setChecked(true);
    m_toggleButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    m_toggleButton->setArrowType(Qt::ArrowType::DownArrow);
    m_toggleButton->setStyleSheet("QToolButton { border: none; font-weight: bold; }");

    m_contentArea = new QFrame(this);
    m_contentArea->setFrameShape(QFrame::Box);
    m_contentArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    m_animation = new QPropertyAnimation(m_contentArea, "maximumHeight", this);
    m_animation->setDuration(m_animationDuration);

    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setSpacing(0);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->addWidget(m_toggleButton);
    m_mainLayout->addWidget(m_contentArea);

    connect(m_toggleButton, &QToolButton::toggled, this, &CollapsibleWidget::toggleExpansion);

    // Start expanded
    m_contentArea->setMaximumHeight(1000); // Large value to show all content
}

void CollapsibleWidget::setContentLayout(QLayout* contentLayout)
{
    delete m_contentArea->layout();
    m_contentArea->setLayout(contentLayout);

    // Update animation end value based on content height
    const auto collapsedHeight = 0;
    const auto contentHeight = contentLayout->sizeHint().height();

    m_animation->setStartValue(collapsedHeight);
    m_animation->setEndValue(contentHeight);

    if (!m_toggleButton->isChecked()) {
        m_contentArea->setMaximumHeight(collapsedHeight);
    }
}

void CollapsibleWidget::setExpanded(bool expanded)
{
    m_toggleButton->setChecked(expanded);
    toggleExpansion();
}

bool CollapsibleWidget::isExpanded() const
{
    return m_toggleButton->isChecked();
}

int CollapsibleWidget::animationDuration() const
{
    return m_animationDuration;
}

void CollapsibleWidget::setAnimationDuration(int duration)
{
    m_animationDuration = duration;
    m_animation->setDuration(duration);
}

void CollapsibleWidget::toggleExpansion()
{
    const bool expanding = m_toggleButton->isChecked();
    m_toggleButton->setArrowType(expanding ? Qt::ArrowType::DownArrow : Qt::ArrowType::RightArrow);

    m_animation->setDirection(expanding ? QAbstractAnimation::Forward : QAbstractAnimation::Backward);
    m_animation->start();

    emit expansionChanged(expanding);
}

}