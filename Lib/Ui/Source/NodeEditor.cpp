#include "NodeEditor.hpp"
#include "Core/TypeRegistry.hpp"

#include <QHBoxLayout>
#include <QLabel>
#include <QMenuBar>
#include <QTimer>
#include <QToolBar>
#include <QMouseEvent>


#include <algorithm>

#include <spdlog/spdlog.h>
namespace cf::ui {
/*-------------------------------------------*/
/*-----------MARK: NodeItem------------------*/
/*-------------------------------------------*/
NodeItem::NodeItem(std::shared_ptr<core::Scene> scene,
    std::shared_ptr<core::Node> node,
    QGraphicsItem* parent)
    : QAbstractGraphicsShapeItem(parent)
    , m_scene(scene)
    , m_node(node)
    , m_isSelected(false)
{
    setAcceptHoverEvents(true);
    setFlags(flags() | QGraphicsItem::ItemIsSelectable);
    setBrush(NodeColor);
    setPen(Qt::NoPen);

    core::NodeDescriptor typeDesc = core::TypeRegistry::getNodeDescriptor(node->getType());

    uint32_t inputYOffset = 55;
    const auto& attributes = scene->getNodeAttributes(node);
    spdlog::info("NodeItem::NodeItem - Found {} attributes for node '{}'", attributes.size(), node->getName());

    for (const auto& attr : attributes) {
        core::AttributeDescriptor attrDesc = scene->getAttributeDescriptor(attr->getHandle());

        NodeAttribute* attributeItem = new NodeAttribute(attr, attrDesc, this);
            attributeItem->setZValue(1);
            attributeItem->setPos(-10, inputYOffset);
            m_attributes.push_back(attributeItem);
            inputYOffset += 29;
    }
}

QRectF NodeItem::boundingRect() const
{
    return QRectF(0, 0, NodeWidth, NodeHeight);
}

void NodeItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QColor borderColor = m_isSelected || m_isHovered ? NodeHighlightBorderColor : NodeBorderColor;
    constexpr qreal borderWidth = 5.0;

    // 1. Draw the main body background (excluding header area)
    painter->setPen(Qt::NoPen);
    painter->setBrush(borderColor);
    painter->drawRoundedRect(QRectF(0, 0, NodeWidth, NodeHeight), 20, 20);

    // 2. Draw the node body as a slightly smaller filled rounded rect
    painter->setBrush(NodeColor);
    painter->drawRoundedRect(QRectF(borderWidth, borderWidth, NodeWidth - 2 * borderWidth, NodeHeight - 2 * borderWidth), 20 - borderWidth, 20 - borderWidth);

    // 3. Draw the header as a filled rounded rect (same as before, but inset)
    painter->setBrush(borderColor);
    painter->drawRoundedRect(QRectF(0, 0, NodeWidth, HeaderHeight), 20, 20);
    painter->drawRect(QRectF(0, 20, NodeWidth, 25)); // Bottom part of header to cover rounded corners

    // 4. Draw the node name
    painter->setPen(Qt::white);
    painter->setFont(QFont("Inter", 24));
    QRectF textRect(20, 10, 280, 35);
    painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter,
        QString::fromStdString(m_node->getName()));
}

void NodeItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    QAbstractGraphicsShapeItem::hoverEnterEvent(event);

    if (m_isSelected) {
        return;
    }

    m_isHovered = true;
    update();
}

void NodeItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    QAbstractGraphicsShapeItem::hoverLeaveEvent(event);

    if (m_isSelected) {
        return;
    }

    m_isHovered = false;
    update();
}

void NodeItem::setSelected(bool state)
{
    m_isSelected = state;
    update();
}

std::shared_ptr<core::Node> NodeItem::getNode() const
{
    return m_node;
}

/*-------------------------------------------*/
/*-----------MARK: NodeItem------------------*/
/*-------------------------------------------*/

NodeScene::NodeScene(std::shared_ptr<core::Scene> scene, QObject* parent)
    : QGraphicsScene(parent)
    , m_scene(scene)
{
    setSceneRect(-sceneWidth, -sceneHeight, 2 * sceneWidth, 2 * sceneHeight);
}

void NodeScene::populateScene()
{
    clear();

    if (!m_scene) {
        spdlog::error("NodeScene::populateScene - Scene is null");
        return;
    }

    //Draw Nodes
    const auto& nodes = m_scene->getNodes();
    for (const auto& [nodeHandler, node] : nodes) {
        static qreal yOffset = -50;
        static qreal xOffset = 50;

        auto nodeItem = new NodeItem(m_scene, node);
        nodeItem->setPos(xOffset, yOffset);
        addItem(nodeItem);
        m_nodeItems.push_back(nodeItem);

        xOffset += 500;
    }

    // Draw Connections
    const auto& connections = m_scene->getConnections();
    for (const auto& connection : connections) {
        auto fromNodeIt = nodes.find(connection.nodeSource);
        auto toNodeIt = nodes.find(connection.nodeTarget);

        if (fromNodeIt == nodes.end() || toNodeIt == nodes.end()) {
            spdlog::error("NodeScene::populateScene - Connection references invalid node handle");
            continue;
        }

        // Find the NodeItems in the scene
        NodeItem* fromNodeItem = nullptr;
        NodeItem* toNodeItem = nullptr;

        for (auto item : items()) {
            if (auto nodeItem = dynamic_cast<NodeItem*>(item)) {
                if (nodeItem->getNode() == fromNodeIt->second) {
                    fromNodeItem = nodeItem;
                }
                if (nodeItem->getNode() == toNodeIt->second) {
                    toNodeItem = nodeItem;
                }
            }
        }

        if (!fromNodeItem || !toNodeItem) {
            spdlog::error("NodeScene::populateScene - Could not find NodeItems for connection");
            continue;
        }

        // Find the AttributeItems in the NodeItems
        NodeAttribute* fromAttrItem = nullptr;
        NodeAttribute* toAttrItem = nullptr;

        spdlog::info("Finding attributes for connection: {} -> {}", connection.attributeSource, connection.attributeTarget);
        spdlog::info("Number of child items in fromNodeItem: {}", fromNodeItem->childItems().size());

        for (auto attrItem : fromNodeItem->childItems()) {
            if (auto nodeAttr = dynamic_cast<NodeAttribute*>(attrItem)) {

                spdlog::info("Checking attribute: {}, {}",  static_cast<void*>(nodeAttr->getAttribute().get()), nodeAttr->getAttribute()->getHandle());
                spdlog::info("Attribute name: {}", nodeAttr->getAttributeDescriptor().name);         
            }
        }

        for (auto attrItem : fromNodeItem->childItems()) {
            if (auto nodeAttr = dynamic_cast<NodeAttribute*>(attrItem)) {
                spdlog::info("Checking attribute: {}, {}",  static_cast<void*>(nodeAttr->getAttribute().get()), nodeAttr->getAttribute()->getHandle());
                spdlog::info("Looking for attribute: {}, {}", static_cast<void*>(m_scene->getAttribute(connection.attributeSource).get()), m_scene->getAttribute(connection.attributeSource)->getHandle());
                if (nodeAttr->getAttribute() == m_scene->getAttribute(connection.attributeSource)) {
                    fromAttrItem = nodeAttr;
                    break;
                }
            }
        }

        for(auto attrItem : toNodeItem->childItems()) {
            if (auto nodeAttr = dynamic_cast<NodeAttribute*>(attrItem)) {
                if (nodeAttr->getAttribute() == m_scene->getAttribute(connection.attributeTarget)) {
                    toAttrItem = nodeAttr;
                    break;
                }
            }
        }

        if (!fromAttrItem || !toAttrItem) {
            spdlog::error("NodeScene::populateScene - Could not find AttributeItems for connection");
            continue;
        }

        //Get Plug positions
        // Create and add the connection line

        ConnectionItem* connectionItem = new ConnectionItem(fromAttrItem->getOutputPlug(), toAttrItem->getInputPlug());
        connectionItem->setZValue(-1); // Ensure connections are drawn behind nodes
        addItem(connectionItem);
        m_connectionItems.push_back(connectionItem);
        connectionItem->updatePath();
        fromAttrItem->getOutputPlug()->setConnected(true);
        toAttrItem->getInputPlug()->setConnected(true);
    }
}

void NodeScene::drawBackground(QPainter* painter, const QRectF& rect)
{
    Q_UNUSED(rect);

    // Draw grid lines
    painter->setPen(Qt::lightGray);
    for (int32_t x = -sceneWidth; x <= sceneWidth; x += 10) {
        painter->drawLine(x * 10, -sceneHeight, x * 10, sceneHeight);
    }

    for (int32_t y = -sceneHeight; y <= sceneHeight; y += 10) {
        painter->drawLine(-sceneWidth, y * 10, sceneWidth, y * 10);
    }

    // Draw axis
    painter->setPen(QPen(Qt::black, 4));
    painter->drawLine(-sceneWidth, 0, sceneWidth, 0);
    painter->drawLine(0, -sceneHeight, 0, sceneHeight);
}

void NodeScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    Q_UNUSED(event);
}

void NodeScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    Q_UNUSED(event);

}

void NodeScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    Q_UNUSED(event);
}

NodeGraphView::NodeGraphView(std::shared_ptr<core::Scene> scene, QWidget* parent)
    : QGraphicsView(parent)
    , m_nodeScene(new NodeScene(scene, this))
    , m_scene(scene)
{

    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::TextAntialiasing);
    setRenderHint(QPainter::SmoothPixmapTransform);

    setDragMode(QGraphicsView::ScrollHandDrag);
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    setOptimizationFlags(QGraphicsView::DontSavePainterState | QGraphicsView::DontAdjustForAntialiasing);

    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    scale(1.0, 1.0);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setScene(m_nodeScene);
}

void NodeGraphView::showEvent(QShowEvent* event)
{
    QGraphicsView::showEvent(event);
    QTimer::singleShot(0, this, [this]() { centerOn(0, 0); });
    m_nodeScene->populateScene();
}

NodeEditor::NodeEditor(std::shared_ptr<core::Scene> scene, QWidget* parent)
    : QWidget(parent)
    , m_scene(scene)
{
}

void NodeEditor::showEvent([[maybe_unused]] QShowEvent* event)
{
    m_menuBar = new QMenuBar(this);

    QMenu* fileMenu = m_menuBar->addMenu("Menu");
    fileMenu->addAction("Action 1");
    fileMenu->addAction("Action 2");
    fileMenu->addAction("Action 3");

    QToolBar* toolBar = new QToolBar("Toolbar", this);
    toolBar->addAction("Tool 1");
    toolBar->addAction("Tool 2");

    m_graphView = new NodeGraphView(m_scene, this);

    QVBoxLayout* nodeEditorLayout = new QVBoxLayout(this);
    nodeEditorLayout->setMenuBar(m_menuBar);
    nodeEditorLayout->addWidget(toolBar);
    nodeEditorLayout->addWidget(m_graphView);

    setLayout(nodeEditorLayout);
}

NodeAttribute::NodeAttribute(std::shared_ptr<core::Attribute> attribute,
    core::AttributeDescriptor m_attributeDesc,
    QGraphicsItem* parent)
    : QAbstractGraphicsShapeItem(parent)
    , m_attribute(attribute)
    , m_attributeDesc(m_attributeDesc)
{
    setAcceptHoverEvents(true);

    setBrush(nodeLabelColor);
    setPen(Qt::NoPen);

    // Draw connection Input
    if (m_attributeDesc.role == core::AttributeRole::eInput || m_attributeDesc.role == core::AttributeRole::eInOut)
    {
        NodePlug* nodePlug = new NodePlug(this);
        nodePlug->setPos(0, 0);
        m_pInputPlug = nodePlug;
    }

    // Draw Connection Output
    if (m_attributeDesc.role == core::AttributeRole::eOutput || m_attributeDesc.role == core::AttributeRole::eInOut)
    {
        //TODO validate transform
        NodePlug* nodePlug = new NodePlug(this);
        QTransform transform;
        transform.translate(320, 0);
        transform.scale(-1, 1);
        nodePlug->setTransform(transform);
        m_pOutputPlug = nodePlug;
    }
}

QRectF NodeAttribute::boundingRect() const
{
    return QRectF(0, 0, nodeWidth, nodeHeight);
}

void NodeAttribute::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    // TODO color of connection that depend on type of the handler
    // TODO connection and disconnection animation

    // Draw Label
    painter->setPen(pen());
    painter->setBrush(brush());
    painter->drawRect(20,
        0,
        nodeLabelWidth,
        nodeLabelHeight);

    // Draw connection end
    painter->setBrush(nodeConnectionColor);

    // Draw connection input
    if (m_attributeDesc.role == core::AttributeRole::eInput || m_attributeDesc.role == core::AttributeRole::eInOut) {
        painter->drawRect(20, 0, 5, 25);
    }

    // Draw connection output
    if (m_attributeDesc.role == core::AttributeRole::eOutput || m_attributeDesc.role == core::AttributeRole::eInOut) {
        painter->drawRect(295, 0, 5, 25);
    }

    // Draw text
    painter->setPen(Qt::white);
    painter->setFont(QFont("Inter", 12));
    QRectF textRect { 45, 5, 230, 15 };
    painter->drawText(textRect,
        Qt::AlignLeft | Qt::AlignVCenter,
        QString::fromStdString(m_attributeDesc.name));

}

void NodeAttribute::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    update();
    QAbstractGraphicsShapeItem::hoverEnterEvent(event);
}

void NodeAttribute::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    update();
    QAbstractGraphicsShapeItem::hoverLeaveEvent(event);
}

NodeItem* NodeAttribute::getParentNode() const
{
    return dynamic_cast<NodeItem*>(parentItem());
}

std::shared_ptr<core::Attribute> NodeAttribute::getAttribute() const
{
    return m_attribute;
}

core::AttributeDescriptor NodeAttribute::getAttributeDescriptor() const
{
    return m_attributeDesc;
}   

NodePlug* NodeAttribute::getInputPlug() const
{
    return m_pInputPlug;
}

NodePlug* NodeAttribute::getOutputPlug() const
{
    return m_pOutputPlug;
}

/*-------------------------------------------*/
/*-----------MARK: NodePlug------------------*/
/*-------------------------------------------*/
NodePlug::NodePlug(QGraphicsItem* parent)
    : QAbstractGraphicsShapeItem(parent),
    m_isConnected(false),
    m_isConnecting(false),
    m_connections()
{
    setAcceptHoverEvents(true);
}

QRectF NodePlug::boundingRect() const
{
    return QRectF(0.0F, 0.0F, 25.0F, 25.0F);
}

QPainterPath NodePlug::createConnectionPath()
{
    QPainterPath path;

    // Start at the first point (matches SVG M0 5)
    path.moveTo(0, 5);

    // Create the top rounded corner (matches SVG C0 2.23858 2.23858 0 5 0)
    path.cubicTo(QPointF(0, 2.23858), QPointF(2.23858, 0), QPointF(5, 0));

    // Determine the right extent based on connection state
    float rightExtent = 15.0f;
    if (isConnected() || isUnderMouse() || m_isConnecting) {
        rightExtent = 25.0f;
    }

    // Draw the top horizontal line
    path.lineTo(rightExtent, 0);

    // Draw the right vertical line
    path.lineTo(rightExtent, 25);

    // Draw the bottom horizontal line
    path.lineTo(5, 25);

    // Create the bottom rounded corner (matches SVG C2.23858 25 0 22.7614 0 20)
    path.cubicTo(QPointF(2.23858, 25), QPointF(0, 22.7614), QPointF(0, 20));

    // Draw the left vertical line
    path.lineTo(0, 5);

    // Close the path
    path.closeSubpath();

    return path;
}

void NodePlug::paint(QPainter* painter,
    const QStyleOptionGraphicsItem* option,
    QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(Qt::NoPen);
    painter->setBrush(nodeConnectionColor);

    QPainterPath path = createConnectionPath();
    painter->drawPath(path);
}

void NodePlug::setConnected(bool state)
{
    m_isConnected = state;
}

bool NodePlug::isConnected() const
{
    return m_isConnected;
}

void NodePlug::beginConnection()
{
    m_isConnecting = true;
}

void NodePlug::endConnection()
{
    m_isConnecting = false;
}

NodeAttribute* NodePlug::getParentAttribute() const
{
    return dynamic_cast<NodeAttribute*>(parentItem());
}

QPointF NodePlug::getPlugCenterPosition() const
{
    // TODO refactor
    return scenePos() + QPointF(0, 12.5);
}

void NodePlug::addConnection(ConnectionItem* connection)
{
    m_connections.push_back(connection);
}

QVariant NodePlug::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == QGraphicsItem::ItemPositionChange) {
        // Update all connected connections
        for (auto connection : m_connections) {
            connection->updatePath();
        }
    }
    return QAbstractGraphicsShapeItem::itemChange(change, value);
}


/*-------------------------------------------*/
/*-----------MARK: ConnectionItem------------------*/
/*-------------------------------------------*/

ConnectionItem::ConnectionItem(NodePlug* startPlug, NodePlug* endPlug, QGraphicsItem* parent):
    QGraphicsPathItem(parent)
    , m_startPlug(startPlug)
    , m_endPlug(endPlug)
{
    setPen(QPen(connectionColor, 3));
    setBrush(Qt::NoBrush);
    setZValue(-1); // Ensure connections are drawn behind nodes
    updatePath();
}

void ConnectionItem::updatePath()
{
    if (!m_startPlug || !m_endPlug) {
        return;
    }

    QPointF startPos = m_startPlug->getPlugCenterPosition();
    QPointF endPos = m_endPlug->getPlugCenterPosition();

    QPainterPath path;
    path.moveTo(startPos);

    // Control points for cubic Bezier curve
    QPointF controlPoint1 = startPos + QPointF(100, 0);
    QPointF controlPoint2 = endPos - QPointF(100, 0);

    path.cubicTo(controlPoint1, controlPoint2, endPos);
    setPath(path);
}

NodePlug* ConnectionItem::getStartPlug() const
{
    return m_startPlug;
}

NodePlug* ConnectionItem::getEndPlug() const
{
    return m_endPlug;
}

void ConnectionItem::setHovered(bool state)
{
    m_isHovered = state;
    update();
}

void ConnectionItem::setSelected(bool state)
{
    m_isSelected = state;
    update();
    if (m_isSelected) {
        setPen(QPen(Qt::yellow, 4));
    } else if (m_isHovered) {
        setPen(QPen(Qt::cyan, 4));
    } else {
        setPen(QPen(connectionColor, 3));
    }
}




} // namespace cf::ui