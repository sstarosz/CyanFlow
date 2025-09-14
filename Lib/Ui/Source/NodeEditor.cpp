#include "NodeEditor.hpp"
#include "Core/TypeRegistry.hpp"
#include "SelectionManager.hpp"

#include <QHBoxLayout>
#include <QLabel>
#include <QMenuBar>
#include <QTimer>
#include <QToolBar>
#include <QMouseEvent>
#include <QScrollBar>

#include <algorithm>

#include <spdlog/spdlog.h>
namespace cf::ui {

/*-------------------------------------------*/
/*-----------MARK: NodeItem------------------*/
/*-------------------------------------------*/
NodeItem::NodeItem(QtNode* m_qtNode,
    std::shared_ptr<core::Scene> scene,
    std::shared_ptr<core::Node> node,
    QGraphicsItem* parent)
    : QAbstractGraphicsShapeItem(parent)
    , m_qtNode(m_qtNode)
    , m_scene(scene)
    , m_node(node)
{
    setAcceptHoverEvents(true);
    setFlags(flags() | QGraphicsItem::ItemIsSelectable);

    uint32_t inputYOffset = 55;
    const auto& attributes = scene->getNodeAttributes(node);
    for (const auto& attr : attributes) {
        core::AttributeDescriptor attrDesc = attr->getAttributeDescriptor();

        NodeAttribute* attributeItem = new NodeAttribute(attr, attrDesc, this);
            attributeItem->setZValue(zValue() + 1);
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

    QColor borderColor = isSelected() || isUnderMouse() ? NodeHighlightBorderColor : NodeBorderColor;
    constexpr qreal borderWidth = 5.0;

    // 1. Draw the main body background (border)
    painter->setPen(Qt::NoPen);
    painter->setBrush(borderColor);
    painter->drawRoundedRect(QRectF(0, 0, NodeWidth, NodeHeight), 20, 20);

    // 2. Draw the node body as a slightly smaller rounded rect (inset to show border)
    painter->setBrush(NodeColor);
    painter->drawRoundedRect(QRectF(borderWidth, borderWidth, NodeWidth - 2 * borderWidth, NodeHeight - 2 * borderWidth), 20 - borderWidth, 20 - borderWidth);

    // 3. Draw the header as a filled rounded rect (same as before, but inset)
    painter->setBrush(borderColor);
    painter->drawRoundedRect(QRectF(0, 0, NodeWidth, HeaderHeight), HeaderRadius, HeaderRadius);
    painter->drawRect(QRectF(0, 20, NodeWidth, 25)); // Bottom part of header to cover rounded corners

    // 4. Draw the node name
    painter->setPen(Qt::white);
    painter->setFont(QFont("Inter", 24));
    QRectF textRect(30, 0, NodeWidth, HeaderHeight);
    painter->drawText(textRect,
                      Qt::AlignLeft | Qt::AlignVCenter,
                      m_qtNode->getName());
}

std::shared_ptr<core::Node> NodeItem::getNode() const
{
    return m_node;
}

/*-------------------------------------------*/
/*-----------MARK: NodeScene------------------*/
/*-------------------------------------------*/

NodeScene::NodeScene(std::shared_ptr<core::Scene> scene, QObject* parent)
    : QGraphicsScene(parent)
    , m_scene(scene)
{
    setSceneRect(-sceneWidth, -sceneHeight, 2 * sceneWidth, 2 * sceneHeight);

    connect(this, &QGraphicsScene::selectionChanged, [this]() {
        std::vector<std::shared_ptr<core::Node>> selectedNodes;

        for (auto item : selectedItems()) {
            if (auto nodeItem = dynamic_cast<NodeItem*>(item)) {
                selectedNodes.push_back(nodeItem->getNode());
            }
        }

        SelectionManager::getInstance().setSelection(selectedNodes);
    });
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

        QtNode* qtNode = new QtNode(node, this);
        m_qtNodes.push_back(qtNode);

        auto nodeItem = new NodeItem(qtNode, m_scene, node);
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
    if (event->button() == Qt::LeftButton) {
        QGraphicsItem* item = itemAt(event->scenePos(), QTransform());
        
        if (NodePlug* plug = dynamic_cast<NodePlug*>(item)) {
            m_dragMode = DragMode::Connecting;
            
            //TODO: check if this is necessary
            if(m_tempConnection) {
                delete m_tempConnection;
                m_tempConnection = nullptr;
            }

            m_tempConnection = new ConnectionItem();
            m_tempConnection->setAcceptedMouseButtons(Qt::NoButton);
            m_tempConnection->setAcceptHoverEvents(false);
            m_tempConnection->setZValue(-1);

            m_tempConnection->setStartPlug(plug);
            m_tempConnection->updateTempConnection(event->scenePos());
            addItem(m_tempConnection);

            m_startPlug = plug;
            m_startPlug->beginConnection();
            event->accept();

        } else {
            QGraphicsScene::mousePressEvent(event);
        }
    } else {
        QGraphicsScene::mousePressEvent(event);
    }
}

void NodeScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
   if(m_dragMode == DragMode::Connecting && m_tempConnection) {
        m_tempConnection->updateTempConnection(event->scenePos());

        QList<QGraphicsItem*> itemsAtPos = items(event->scenePos());

        NodePlug* hoveredPlug = nullptr;
        for (QGraphicsItem* item : itemsAtPos) {
            // Check if the item is a NodePlug
            if (NodePlug* plug = dynamic_cast<NodePlug*>(item)) {
                hoveredPlug = plug;
                break;
            }
        }
        
        for(QGraphicsItem* item : items()) {
            if(NodePlug* plug = dynamic_cast<NodePlug*>(item)) {
                plug->setHovered(plug == hoveredPlug);
            }
        }

        event->accept();
    } else {
        QGraphicsScene::mouseMoveEvent(event);
    }

}

void NodeScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    if(m_dragMode == DragMode::Connecting && m_tempConnection) {

        QList<QGraphicsItem*> itemsAtPos = items(event->scenePos());

        NodePlug* plug = nullptr;
        for(auto& item : itemsAtPos) {
            plug = dynamic_cast<NodePlug*>(item);

            if(plug && plug != m_startPlug) {
                break;
            } 
        }


        if (plug) {
            
            m_tempConnection->setEndPlug(plug);
            m_tempConnection->updatePath();
            m_connectionItems.push_back(m_tempConnection);


            m_startPlug->addConnection(m_tempConnection);
            m_startPlug->endConnection();
            m_startPlug->setConnected(true);

            plug->addConnection(m_tempConnection);
            plug->endConnection();
            plug->setConnected(true);

            m_tempConnection = nullptr;
        } else {
            spdlog::info("NodeScene::mouseReleaseEvent - Not released on a plug, removing temp connection");
            removeItem(m_tempConnection);
            delete m_tempConnection;
            m_tempConnection = nullptr;

            if(m_startPlug) {
                m_startPlug->endConnection();
            }
        }

        m_dragMode = DragMode::None;
        event->accept();
    } else {
        spdlog::info("NodeScene::mouseReleaseEvent - Not in connecting mode");
        QGraphicsScene::mouseReleaseEvent(event);
    }
}

/*-------------------------------------------*/
/*-----------MARK: NodeGraphView-------------*/
/*-------------------------------------------*/

NodeGraphView::NodeGraphView(std::shared_ptr<core::Scene> scene, QWidget* parent)
    : QGraphicsView(parent)
    , m_nodeScene(new NodeScene(scene, this))
    , m_scene(scene)
{

    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::TextAntialiasing);
    setRenderHint(QPainter::SmoothPixmapTransform);

    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    setOptimizationFlags(QGraphicsView::DontSavePainterState | QGraphicsView::DontAdjustForAntialiasing);

    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    scale(1.0, 1.0);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setScene(m_nodeScene);
}

void NodeGraphView::resetView()
{
    resetTransform();
    scale(1.0, 1.0);
    centerOn(0, 0);
}

void NodeGraphView::showEvent(QShowEvent* event)
{
    QGraphicsView::showEvent(event);
    QTimer::singleShot(0, this, [this]() { centerOn(0, 0); });
    m_nodeScene->populateScene();
}

void NodeGraphView::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::MiddleButton) {
            m_isPanning = true;
            m_panStartPoint = event->pos();
            setCursor(Qt::ClosedHandCursor);
            event->accept();
            return;
    }

    QGraphicsView::mousePressEvent(event);
}

void NodeGraphView::mouseMoveEvent(QMouseEvent* event) {
    if (m_isPanning) {
        // Calculate pan distance and update scrollbars
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - (event->position().x() - m_panStartPoint.x()));
        verticalScrollBar()->setValue(verticalScrollBar()->value() - (event->position().y() - m_panStartPoint.y()));
        m_panStartPoint = event->pos();
        event->accept();
        return;
    }
    
    QGraphicsView::mouseMoveEvent(event);
}

void NodeGraphView::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::MiddleButton) {
        m_isPanning = false;
        setCursor(Qt::ArrowCursor);
        event->accept();
        return;
    }
    
    QGraphicsView::mouseReleaseEvent(event);
}

/*-------------------------------------------*/
/*-------------MARK: NodeEditor--------------*/
/*-------------------------------------------*/

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

    m_graphView = new NodeGraphView(m_scene, this);

    QToolBar* toolBar = new QToolBar("Toolbar", this);
    toolBar->addAction("Tool 1");
    toolBar->addAction("Tool 2");
    toolBar->addAction("Reset View", m_graphView, &NodeGraphView::resetView);


    QVBoxLayout* nodeEditorLayout = new QVBoxLayout(this);
    nodeEditorLayout->setMenuBar(m_menuBar);
    nodeEditorLayout->addWidget(toolBar);
    nodeEditorLayout->addWidget(m_graphView);

    setLayout(nodeEditorLayout);
}

/*-------------------------------------------*/
/*-------------MARK: NodeAttribute-----------*/
/*-------------------------------------------*/
NodeAttribute::NodeAttribute(std::shared_ptr<core::Attribute> attribute,
    core::AttributeDescriptor m_attributeDesc,
    QGraphicsItem* parent)
    : QAbstractGraphicsShapeItem(parent)
    , m_attribute(attribute)
    , m_attributeDesc(m_attributeDesc)
{
    // Draw connection Input
    if (m_attributeDesc.role == core::AttributeRole::eInput || m_attributeDesc.role == core::AttributeRole::eInOut)
    {
        NodePlug* nodePlug = new NodePlug(this);
        nodePlug->setPos(0, 0);
        nodePlug->setZValue(zValue() + 1);
        m_pInputPlug = nodePlug;
    }

    // Draw Connection Output
    if (m_attributeDesc.role == core::AttributeRole::eOutput || m_attributeDesc.role == core::AttributeRole::eInOut)
    {
        //TODO validate transform
        NodePlug* nodePlug = new NodePlug(this);
        nodePlug->setZValue(zValue() + 1);

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
    painter->setPen(Qt::NoPen);
    painter->setBrush(nodeLabelColor);
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
    setZValue(10);
}

QRectF NodePlug::boundingRect() const
{
    return QRectF(0.0F, 0.0F, plugWidth, plugHeight);
}

QPainterPath NodePlug::createPlugShape()
{
    QPainterPath path;

    // Start at the first point (matches SVG M0 5)
    path.moveTo(0, 5);

    // Create the top rounded corner (matches SVG C0 2.23858 2.23858 0 5 0)
    path.cubicTo(QPointF(0, 2.23858), QPointF(2.23858, 0), QPointF(5, 0));

    // Determine the right extent based on connection state
    float rightExtent = 15.0f;
    if (isConnected() || isUnderMouse() || isHovered() || m_isConnecting) {
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

    QPainterPath path = createPlugShape();
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

void NodePlug::setHovered(bool state)
{
    m_isHovered = state;
    update();
}

bool NodePlug::isHovered() const
{
    return m_isHovered;
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

ConnectionItem::ConnectionItem(QGraphicsItem* parent):
    ConnectionItem(nullptr, nullptr, parent)
{
}

ConnectionItem::ConnectionItem(NodePlug* startPlug, NodePlug* endPlug, QGraphicsItem* parent)
    : QGraphicsPathItem(parent)
    , m_startPlug(startPlug)
    , m_endPlug(endPlug)
    , m_isHovered(false)
    , m_isSelected(false)
{
    setPen(QPen(connectionColor, 3));
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

void ConnectionItem::updateTempConnection(const QPointF& endPos)
{
    if (!m_startPlug) {
        return;
    }

    QPointF startPos = m_startPlug->getPlugCenterPosition();

    QPainterPath path;
    path.moveTo(startPos);

    // Control points for cubic Bezier curve
    QPointF controlPoint1 = startPos + QPointF(100, 0);
    QPointF controlPoint2 = endPos - QPointF(100, 0);

    path.cubicTo(controlPoint1, controlPoint2, endPos);
    setPath(path);
}

} // namespace cf::ui