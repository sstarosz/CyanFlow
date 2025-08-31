#ifndef CF_UI_NODEEDITOR_HPP
#define CF_UI_NODEEDITOR_HPP

#include "Core/Scene.hpp"

#include <QAbstractGraphicsShapeItem>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMenuBar>
#include <QWidget>

namespace cf::ui {

// #373B3E
constexpr QColor NodeColor = QColor(55, 59, 62);
// #5A676B
constexpr QColor NodeBorderColor = QColor(90, 103, 107);
// #87E5CF
constexpr QColor NodeHighlightBorderColor = QColor(135, 229, 207);

class NodeItem : public QAbstractGraphicsShapeItem {
    static constexpr int32_t NodeWidth = 300;
    static constexpr int32_t NodeHeight = 400;

    static constexpr int32_t HeaderHeight = 45;
    static constexpr int32_t HeaderWidth = 300;
    static constexpr int32_t HeaderRadius = 20;

public:
    NodeItem(std::shared_ptr<core::Node> node, QGraphicsItem* parent = nullptr);

    virtual QRectF boundingRect() const override;

    virtual void paint(QPainter* painter,
        const QStyleOptionGraphicsItem* option,
        QWidget* widget) override;

    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

    std::shared_ptr<core::Node> getNode() const;

    void setSelected(bool state);

private:
    std::shared_ptr<core::Node> m_node;
    bool m_isSelected = false;
    bool m_isHovered = false;
};

class NodeScene : public QGraphicsScene {
    Q_OBJECT

    constexpr static int32_t sceneWidth = 10000;
    constexpr static int32_t sceneHeight = 10000;

public:
    explicit NodeScene(std::shared_ptr<core::Scene> scene, QObject* parent = nullptr);

    void populateScene();

protected:
    void drawBackground(QPainter* painter, const QRectF& rect) override;

private:
    std::shared_ptr<core::Scene> m_scene;
};

class NodeGraphView : public QGraphicsView {
    Q_OBJECT

public:
    explicit NodeGraphView(std::shared_ptr<core::Scene> scene, QWidget* parent = nullptr);

protected:
    void resizeEvent(QResizeEvent* event) override;
    void showEvent(QShowEvent* event) override;

private:
    NodeScene* m_nodeScene;
    std::shared_ptr<core::Scene> m_scene;
};

class NodeEditor : public QWidget {
    Q_OBJECT
public:
    NodeEditor(std::shared_ptr<core::Scene> scene, QWidget* parent = nullptr);

protected:
    void showEvent(QShowEvent* event) override;

private:
    QMenuBar* m_menuBar;
    NodeGraphView* m_graphView;
    std::shared_ptr<core::Scene> m_scene;
};

} // namespace cf::ui

#endif // CF_UI_NODEEDITOR_HPP