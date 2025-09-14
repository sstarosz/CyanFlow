#ifndef CF_UI_NODEEDITOR_HPP
#define CF_UI_NODEEDITOR_HPP

#include "Core/Scene.hpp"
#include "Core/Document.hpp"

#include <QAbstractGraphicsShapeItem>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsSceneMouseEvent>

#include <QMenuBar>
#include <QWidget>
#include <QPointer>

namespace cf::ui {

class NodeEditorModel : public QObject {
    Q_OBJECT
public:
    NodeEditorModel(std::shared_ptr<core::Scene> scene, QObject* parent = nullptr)
        : QObject(parent), m_scene(scene) {}

    std::shared_ptr<core::Scene> getScene() const { return m_scene; }
    void setScene(std::shared_ptr<core::Scene> scene) { m_scene = scene; }
private:
    std::shared_ptr<core::Scene> m_scene;
};

class QtAttribute {
    


};


//Model class over core::Node
class QtNode : public QObject{
    Q_OBJECT
public:
    explicit QtNode(std::shared_ptr<core::Node> node, QObject* parent = nullptr)
        : QObject(parent), m_node(node) {}

    QString getName() const { return QString::fromStdString(m_node->getName()); }
    void setName(const QString& name) { m_node->setName(name.toStdString()); }

private:
    std::shared_ptr<core::Node> m_node;
};


// #373B3E
constexpr QColor NodeColor = QColor(55, 59, 62);
// #5A676B
constexpr QColor NodeBorderColor = QColor(90, 103, 107);
// #87E5CF
constexpr QColor NodeHighlightBorderColor = QColor(135, 229, 207);

class NodeItem;
class NodeAttribute;
class ConnectionItem;

class NodePlug : public QAbstractGraphicsShapeItem
{
    private:
    static constexpr QColor nodeConnectionColor = QColor(85, 209, 208);
    static constexpr QColor nodeConnectionHover = QColor(175, 233, 233);

    static constexpr float plugWidth = 25.0f;
    static constexpr float plugHeight = 25.0f;

    public:
    NodePlug(QGraphicsItem* parent = nullptr);

    QRectF boundingRect() const override;

    void paint(QPainter* painter,
                const QStyleOptionGraphicsItem* option,
                QWidget* widget) override;

    void setConnected(bool state);
    bool isConnected() const;

    void beginConnection();
    void endConnection();

    void setHovered(bool state);
    bool isHovered() const;

    NodeAttribute* getParentAttribute() const;

    QPointF getPlugCenterPosition() const;

    void addConnection(ConnectionItem* connection);
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

    private:
    bool m_isHovered = false;
    bool m_isConnected = false;
    bool m_isConnecting = false;
    std::vector<ConnectionItem*> m_connections;

    QPainterPath createPlugShape();
};

/**
 * @brief Represent attribute in the node
 *
 * MARK: NodeAttribute
 */
class NodeAttribute : public QAbstractGraphicsShapeItem
{
    private:
    // Size
    // Main node size
    static constexpr int32_t nodeWidth = 320;
    static constexpr int32_t nodeHeight = 25;

    // Node Laber
    static constexpr int32_t nodeLabelMarginHorizontal = 25;
    static constexpr int32_t nodeLabelMarginVertical = 0;
    static constexpr int32_t nodeLabelWidth = 280;
    static constexpr int32_t nodeLabelHeight = 25;

    // Colors
    // #585F63
    static constexpr QColor nodeLabelColor = QColor(88, 95, 99);
    // #87E5CF
    static constexpr QColor nodeConnectionColor = QColor(85, 209, 208);
    ;

    public:
    NodeAttribute(std::shared_ptr<core::Attribute> attribute,
                  QGraphicsItem* parent = nullptr);

    QRectF boundingRect() const override;

    void paint(QPainter* painter,
                [[maybe_unused]] const QStyleOptionGraphicsItem* option,
                [[maybe_unused]] QWidget* widget) override;

    /*--------------------------------*/
    /*---------Getter/Setters---------*/
    /*--------------------------------*/
    NodePlug* getInputPlug() const;
    NodePlug* getOutputPlug() const;
    NodeItem* getParentNode() const;

    std::shared_ptr<core::Attribute> getAttribute() const;
    core::AttributeDescriptor getAttributeDescriptor() const;

    private:
    std::shared_ptr<core::Attribute> m_attribute; // Handler to Attribute
    NodePlug* m_pInputPlug;
    NodePlug* m_pOutputPlug;
};

class NodeItem : public QAbstractGraphicsShapeItem {
    static constexpr int32_t NodeWidth = 300;
    static constexpr int32_t NodeHeight = 400;

    static constexpr int32_t HeaderWidth = 300;
    static constexpr int32_t HeaderHeight = 45;
    static constexpr int32_t HeaderRadius = 20;

public:
    NodeItem(QtNode* m_qtNode,
             std::shared_ptr<core::Scene> scene, 
             std::shared_ptr<core::Node> node, 
             QGraphicsItem* parent = nullptr);


    virtual QRectF boundingRect() const override;

    virtual void paint(QPainter* painter,
                       const QStyleOptionGraphicsItem* option,
                       QWidget* widget) override;

    std::shared_ptr<core::Node> getNode() const;

private:
    std::shared_ptr<core::Scene> m_scene;
    std::shared_ptr<core::Node> m_node;
    std::vector<NodeAttribute*> m_attributes;

    QtNode* m_qtNode;
};

class ConnectionItem : public QGraphicsPathItem {

    static constexpr QColor connectionColor = QColor(135, 229, 207);
public:
    ConnectionItem(QGraphicsItem* parent = nullptr);
    ConnectionItem(NodePlug* startPlug, NodePlug* endPlug, QGraphicsItem* parent = nullptr);

    void updatePath();

    void setStartPlug(NodePlug* plug) { m_startPlug = plug; }
    void updateTempConnection(const QPointF& endPos);
    void setEndPlug(NodePlug* plug) { m_endPlug = plug; }

    NodePlug* getStartPlug() const;
    NodePlug* getEndPlug() const;

private:
    NodePlug* m_startPlug;
    NodePlug* m_endPlug;
    bool m_isSelected = false;
    bool m_isHovered = false;
    QPainterPath m_path;
};

class NodeScene : public QGraphicsScene {
    Q_OBJECT


    constexpr static int32_t sceneWidth = 10000;
    constexpr static int32_t sceneHeight = 10000;

    enum class DragMode {
        None,
        Panning,
        Connecting
    };

public:
    explicit NodeScene(std::shared_ptr<core::Scene> scene, QObject* parent = nullptr);

    void populateScene();


protected:
    void drawBackground(QPainter* painter, const QRectF& rect) override;

    /*--------------------------------*/
    /*---------Event Handlers---------*/
    /*--------------------------------*/
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

private:
    std::shared_ptr<core::Scene> m_scene;
    std::vector<NodeItem*> m_nodeItems;
    std::vector<ConnectionItem*> m_connectionItems;
    std::vector<QPointer<QtNode>> m_qtNodes;

    DragMode m_dragMode = DragMode::None;

    ConnectionItem* m_tempConnection = nullptr;
    NodePlug* m_startPlug = nullptr;
};

class NodeGraphView : public QGraphicsView {
    Q_OBJECT

public:
    explicit NodeGraphView(std::shared_ptr<core::Scene> scene, QWidget* parent = nullptr);


    void resetView();

protected:
    void showEvent(QShowEvent* event) override;

    /*--------------------------------*/
    /*---------Event Handlers---------*/
    /*--------------------------------*/
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    NodeScene* m_nodeScene;
    std::shared_ptr<core::Scene> m_scene;

    bool m_isPanning = false;
    QPoint m_panStartPoint;

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
    //std::shared_ptr<core::Document> m_
    NodeEditorModel* m_model;
};

} // namespace cf::ui

#endif // CF_UI_NODEEDITOR_HPP