#include "NodeEditor.hpp"

#include <QHBoxLayout>
#include <QLabel>
#include <QMenuBar>
#include <QTimer>
#include <QToolBar>

#include <spdlog/spdlog.h>
namespace cf::ui {
/*-------------------------------------------*/
/*-----------MARK: NodeItem------------------*/
/*-------------------------------------------*/
NodeItem::NodeItem(std::shared_ptr<core::Node> node,
    QGraphicsItem* parent)
    : QAbstractGraphicsShapeItem(parent)
    , m_node(node)
    , m_isSelected(false)
{
    setAcceptHoverEvents(true);
    setFlags(flags() | QGraphicsItem::ItemIsSelectable);
    setBrush(NodeColor);
    setPen(QPen(NodeBorderColor, 4));
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
    QPen borderPen(borderColor, 4);

    // 1. Draw the main body background (excluding header area)
    painter->setPen(Qt::NoPen);
    painter->setBrush(NodeColor);
    QPainterPath bodyPath;
    bodyPath.addRoundedRect(QRectF(0, HeaderHeight - 20, NodeWidth, NodeHeight - HeaderHeight + 20), 20, 20);
    painter->drawPath(bodyPath);

    // 2. Draw the header background
    painter->setBrush(borderColor);
    painter->drawRoundedRect(QRectF(0, 0, NodeWidth, HeaderHeight), 20, 20);
    painter->drawRect(QRectF(0, 20, NodeWidth, 25)); // Bottom part of header

    // 3. Draw the outline for the entire node
    painter->setPen(borderPen);
    painter->setBrush(Qt::NoBrush);
    QPainterPath outlinePath;
    outlinePath.addRoundedRect(QRectF(0, 0, NodeWidth, NodeHeight), 20, 20);
    painter->drawPath(outlinePath);

    // 4. Draw the node name last so it's always on top
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

    const auto& nodes = m_scene->getNodes();
    for (const auto& [nodeHandler, node] : nodes) {
        static qreal yOffset = 0;
        static qreal xOffset = 0;

        auto nodeItem = new NodeItem(node);
        nodeItem->setPos(xOffset, yOffset);
        addItem(nodeItem);

        yOffset += 150;
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

void NodeGraphView::resizeEvent(QResizeEvent* event)
{
    QGraphicsView::resizeEvent(event);
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

} // namespace cf::ui